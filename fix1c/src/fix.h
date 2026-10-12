// 1C object "OdbcFix" (Russian name RU_TYPE_NAME). Turns the ODBCRecordset fix on and off and
// reports its state. Methods never raise a 1C exception: failures go to the result (0/-1) and
// to the property ErrorText.
#pragma once

class COdbcFix : public CContextImpl<COdbcFix>
{
public:
	COdbcFix();
	virtual ~COdbcFix();

	// Each BL_* line must stay on its own line: the macros chain descriptors through __LINE__.
	BL_BEGIN_CONTEXT("OdbcFix", RU_TYPE_NAME);
	BL_FUNC(Enable,      RU_ENABLE,       0);   // () -> 1 fix is on, 0 not (ErrorText)
	BL_FUNC(Disable,     RU_DISABLE,      0);   // () -> 1 fix is off, 0 not (ErrorText)
	BL_FUNC(Enabled,     RU_ENABLED,      0);   // () -> 1/0
	BL_FUNC(Version,     RU_VERSION,      0);   // () -> "OdbcFix1C <version>"
	BL_FUNC(Info,        RU_INFO,         0);   // () -> one line: state, addresses, counters
	BL_FUNC(CheckObject, RU_CHECK_OBJECT, 1);   // (Object) -> 1 ODBCRecordset goes through the fix, 0 not, -1 not an object
	BL_FUNC(RefreshOn,   RU_REFRESH_ON,   1);   // (ValueTable) -> 1/0 refresh flag, -1 not a value table
	BL_PROP_RO(ErrorText, RU_ERROR_TEXT);
	BL_END_CONTEXT();

private:
	CString m_errorText;
};
