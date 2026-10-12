#include "stdafx.h"

BL_INIT_CONTEXT(COdbcFix);

// Every 1C-visible method body goes between METHOD_BEGIN and METHOD_END: nothing escapes to 1C,
// any exception becomes result `fail` and ErrorText.
#define METHOD_BEGIN(fail) \
	m_errorText.Empty(); \
	try {

#define METHOD_END(name, fail) \
	} \
	catch (CException* e_) { e_->Delete(); m_errorText.Format(MSG_EXCEPTION, name); fail; } \
	catch (...) { m_errorText.Format(MSG_EXCEPTION, name); fail; } \
	return TRUE;

COdbcFix::COdbcFix()
{
}

COdbcFix::~COdbcFix()
{
}

int COdbcFix::Enable(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = 0L)
	CString err;
	BOOL ok = OdbcFixEnable(err);
	m_errorText = err;
	retVal = ok ? 1L : 0L;
	METHOD_END(RU_ENABLE, retVal = 0L)
}

int COdbcFix::Disable(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = 0L)
	CString err;
	BOOL ok = OdbcFixDisable(err);
	m_errorText = err;
	retVal = ok ? 1L : 0L;
	METHOD_END(RU_DISABLE, retVal = 0L)
}

int COdbcFix::Enabled(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = 0L)
	retVal = OdbcFixEnabled() ? 1L : 0L;
	METHOD_END(RU_ENABLED, retVal = 0L)
}

int COdbcFix::Version(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = "")
	retVal = OdbcFixVersion();
	METHOD_END(RU_VERSION, retVal = "")
}

int COdbcFix::Info(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = "")
	CString s = OdbcFixInfo();
	retVal = (LPCSTR)s;
	METHOD_END(RU_INFO, retVal = "")
}

int COdbcFix::CheckObject(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = -1L)
	retVal = (long)OdbcFixCheckObject(*ppParams[0]);
	METHOD_END(RU_CHECK_OBJECT, retVal = -1L)
}

int COdbcFix::RefreshOn(CValue& retVal, CValue** ppParams)
{
	METHOD_BEGIN(retVal = -1L)
	retVal = (long)OdbcFixRefreshFlag(*ppParams[0]);
	METHOD_END(RU_REFRESH_ON, retVal = -1L)
}

int COdbcFix::Get_ErrorText(CValue& value) const
{
	value = (LPCSTR)m_errorText;
	return TRUE;
}
