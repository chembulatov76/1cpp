// Fix for 1C++ ODBCRecordset (1cpp.dll 3.2.4.1, unchanged on disk): ExecuteStatement /
// ВыполнитьИнструкцию into an existing value table keeps the table's refresh on, and a value table
// shown on a form pays ~300 us per added row (100 000 rows: 34 s instead of 0.2 s, measured
// 2026-10-12, plan C:\projects\1cpp\docs\plans\odbc-refactor.md).
//
// The fix replaces CallAsFunc in the ODBCRecordset method table (vtable of its CBLContext part,
// shared by all ODBCRecordset objects) with a wrapper. For ExecuteStatement with an existing
// value table as the receiver the wrapper turns the table's refresh off (CValueTable::m_bEnableRefresh,
// offset 0x3C, the same flag 1C++ itself uses in IndexedTable.Unload), calls the original method,
// turns the refresh back and refreshes once (NewRow + DeleteRow, as 1C++ does). Every other call
// goes to the original unchanged. 1C runtime errors (C++ exceptions) restore the flag and pass on.
#pragma once

// CBLContext virtual slots: 5 CObject slots, then CBLContext in declaration order (1CHEADERS
// bkend.h). The same numbering as 1C++ TurboBL (FindProp 19, FindMethod 26).
enum { SLOT_FINDPROP = 19, SLOT_FINDMETHOD = 26, SLOT_CALLASPROC = 31, SLOT_CALLASFUNC = 32 };

BOOL OdbcFixEnable(CString& err);
BOOL OdbcFixDisable(CString& err);
BOOL OdbcFixEnabled();
CString OdbcFixInfo();
int OdbcFixCheckObject(const CValue& obj);     // 1 our wrapper, 0 original/other, -1 not an object
int OdbcFixRefreshFlag(const CValue& table);   // 1/0 refresh flag of a value table, -1 not a value table
void OdbcFixShutdown();                         // on component unload: original back, no messages
const char* OdbcFixVersion();
