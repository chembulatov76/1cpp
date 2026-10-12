#include "stdafx.h"

#define ODBCFIX_VERSION "OdbcFix1C 1.0.0"

static DWORD* g_vtbl = NULL;      // method table of ODBCRecordset (CBLContext part), in 1cpp.dll
static DWORD  g_orig = 0;         // original CallAsFunc of ODBCRecordset
static int    g_nExec = -1;       // method number of ExecuteStatement / ВыполнитьИнструкцию
static int    g_slotByCode = -1;  // CallAsFunc slot as compiled here (from the vcall thunk)
static DWORD  g_lo = 0, g_hi = 0; // image range of 1cpp.dll
static LONG   g_calls = 0;        // ExecuteStatement calls seen
static LONG   g_guarded = 0;      // ... with an existing value table as the receiver
static LONG   g_failed = 0;       // ... of them ended with an exception (flag restored, passed on)

// The wrapper is a member of a class without virtual functions or bases, so a pointer to it is
// a plain 4-byte code address and it is called with `this` in ECX (thiscall), as 1C calls
// CBLContext::CallAsFunc. `this` is the CBLContext part of the ODBCRecordset object.
class CRSHook
{
public:
	int CallAsFunc(int iMethNum, CValue& rValue, CValue** ppValue);
};
typedef int (CRSHook::*PFN_CALLASFUNC)(int, CValue&, CValue**);
typedef int (CBLContext::*PFN_BL_CALLASFUNC)(int, CValue&, CValue**);

static DWORD HookAddress()
{
	PFN_CALLASFUNC p = &CRSHook::CallAsFunc;
	return *(DWORD*)&p;
}

static BOOL InModule(DWORD addr)
{
	return addr >= g_lo && addr < g_hi;
}

static BOOL ModuleRange(HMODULE h, DWORD& lo, DWORD& hi)
{
	BYTE* base = (BYTE*)h;
	IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)base;
	if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		return FALSE;
	IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
	if (nt->Signature != IMAGE_NT_SIGNATURE)
		return FALSE;
	lo = (DWORD)base;
	hi = lo + nt->OptionalHeader.SizeOfImage;
	return TRUE;
}

// Slot of CBLContext::CallAsFunc as this compiler laid it out: a pointer to a virtual member
// is the address of a vcall thunk "mov eax,[ecx]; jmp [eax+disp]". -1 if the thunk is not recognized.
static int SlotByCode()
{
	PFN_BL_CALLASFUNC p = &CBLContext::CallAsFunc;
	BYTE* c = *(BYTE**)&p;
	for (int hop = 0; hop < 2; ++hop)
	{
		if (c[0] == 0x8B && c[1] == 0x01 && c[2] == 0xFF)
		{
			if (c[3] == 0x60)
				return c[4] / 4;
			if (c[3] == 0xA0)
				return (int)(*(DWORD*)(c + 4) / 4);
			return -1;
		}
		if (c[0] == 0xE9)   // incremental-link jump stub
		{
			c = c + 5 + *(LONG*)(c + 1);
			continue;
		}
		break;
	}
	return -1;
}

// Existing value table given as the receiver: plain ТаблицаЗначений context. Other receivers
// (new table, value list, IndexedTable, classes based on a value table) keep the original path.
static CValueTable* ReceiverTable(CValue* v)
{
	if (v == NULL || v->GetTypeCode() != 100)
		return NULL;
	CBLContext* c = v->GetContext();
	if (c == NULL)
		return NULL;
	CRuntimeClass* rc = c->GetRuntimeClass();
	if (rc == NULL || rc->m_lpszClassName == NULL || strcmp(rc->m_lpszClassName, "CValueTableContext") != 0)
		return NULL;
	CValueTableContextData* d = (CValueTableContextData*)c->GetInternalData();
	return d ? d->GetValueTable() : NULL;
}

// One refresh after the flag is back: add and remove an empty row, as 1C++ does after loading.
static void RefreshTable(CValueTable* t)
{
	t->DeleteRow(t->NewRow(-1));
}

int CRSHook::CallAsFunc(int iMethNum, CValue& rValue, CValue** ppValue)
{
	PFN_CALLASFUNC orig;
	*(DWORD*)&orig = g_orig;
	if (iMethNum != g_nExec || ppValue == NULL)
		return (this->*orig)(iMethNum, rValue, ppValue);

	::InterlockedIncrement(&g_calls);
	CValueTable* t = ReceiverTable(ppValue[1]);
	if (t == NULL || !t->m_bEnableRefresh)   // no table, or refresh already off: nothing to do
		return (this->*orig)(iMethNum, rValue, ppValue);

	::InterlockedIncrement(&g_guarded);
	t->m_bEnableRefresh = FALSE;
	int ret;
	try
	{
		ret = (this->*orig)(iMethNum, rValue, ppValue);
	}
	catch (...)
	{
		// 1C runtime error (or any other exception) inside the method: the table gets its refresh
		// back and shows what was loaded, the exception goes on to 1C unchanged.
		::InterlockedIncrement(&g_failed);
		t->m_bEnableRefresh = TRUE;
		try { RefreshTable(t); } catch (...) {}
		throw;
	}
	t->m_bEnableRefresh = TRUE;
	RefreshTable(t);
	return ret;
}

static BOOL WriteSlot(DWORD* slot, DWORD value, CString& err)
{
	DWORD old = 0;
	if (!::VirtualProtect(slot, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &old))
	{
		err.Format(MSG_PROTECT, ::GetLastError());
		return FALSE;
	}
	*slot = value;
	DWORD dummy = 0;
	::VirtualProtect(slot, sizeof(DWORD), old, &dummy);
	return TRUE;
}

BOOL OdbcFixEnable(CString& err)
{
	if (g_vtbl != NULL && g_vtbl[SLOT_CALLASFUNC] == HookAddress())
		return TRUE;   // already on

	HMODULE h = ::GetModuleHandleA("1cpp.dll");
	if (h == NULL || !ModuleRange(h, g_lo, g_hi))
	{
		err = MSG_NO_1CPP;
		return FALSE;
	}

	g_slotByCode = SlotByCode();
	if (g_slotByCode != SLOT_CALLASFUNC)
	{
		err.Format(MSG_BAD_SLOT, g_slotByCode, (int)SLOT_CALLASFUNC);
		return FALSE;
	}

	CValue rs;
	if (!rs.CreateObject("ODBCRecordset") || rs.GetContext() == NULL)
	{
		err = MSG_NO_RS;
		return FALSE;
	}
	CBLContext* c = rs.GetContext();
	DWORD* vt = *(DWORD**)c;

	// The table and the methods ODBCRecordset overrides must all be inside 1cpp.dll; otherwise the
	// object is not what we expect (or someone else has patched it) and we do not touch anything.
	if (!InModule((DWORD)vt) || !InModule(vt[SLOT_CALLASPROC]) || !InModule(vt[SLOT_FINDMETHOD])
		|| !InModule(vt[SLOT_FINDPROP]))
	{
		err = MSG_BAD_VTBL;
		return FALSE;
	}
	if (!InModule(vt[SLOT_CALLASFUNC]))
	{
		err = MSG_FOREIGN;
		return FALSE;
	}

	int n = c->FindMethod("ExecuteStatement");
	if (n < 0)
	{
		err = MSG_NO_METHOD;
		return FALSE;
	}

	g_nExec = n;
	g_orig = vt[SLOT_CALLASFUNC];
	if (!WriteSlot(&vt[SLOT_CALLASFUNC], HookAddress(), err))
	{
		g_orig = 0;
		return FALSE;
	}
	g_vtbl = vt;
	return TRUE;
}

BOOL OdbcFixDisable(CString& err)
{
	if (g_vtbl == NULL)
		return TRUE;   // was never on
	if (g_vtbl[SLOT_CALLASFUNC] != HookAddress())
	{
		err = MSG_NOT_OURS;
		return FALSE;
	}
	if (!WriteSlot(&g_vtbl[SLOT_CALLASFUNC], g_orig, err))
		return FALSE;
	g_vtbl = NULL;
	return TRUE;
}

BOOL OdbcFixEnabled()
{
	return g_vtbl != NULL && g_vtbl[SLOT_CALLASFUNC] == HookAddress();
}

void OdbcFixShutdown()
{
	if (OdbcFixEnabled() && ::GetModuleHandleA("1cpp.dll") != NULL)
	{
		CString ignored;
		OdbcFixDisable(ignored);
	}
}

CString OdbcFixInfo()
{
	CString s;
	s.Format(INFO_FORMAT, ODBCFIX_VERSION, OdbcFixEnabled() ? INFO_YES : INFO_NO, g_lo, g_hi,
		(DWORD)g_vtbl, (int)SLOT_CALLASFUNC, g_slotByCode, g_orig, g_nExec, g_calls, g_guarded, g_failed);
	return s;
}

int OdbcFixCheckObject(const CValue& obj)
{
	if (obj.GetTypeCode() != 100 || obj.GetContext() == NULL)
		return -1;
	DWORD* vt = *(DWORD**)obj.GetContext();
	return vt[SLOT_CALLASFUNC] == HookAddress() ? 1 : 0;
}

int OdbcFixRefreshFlag(const CValue& table)
{
	CValueTable* t = ReceiverTable(const_cast<CValue*>(&table));
	if (t == NULL)
		return -1;
	return t->m_bEnableRefresh ? 1 : 0;
}

const char* OdbcFixVersion()
{
	return ODBCFIX_VERSION;
}
