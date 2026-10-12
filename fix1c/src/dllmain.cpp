// Entry point. Same loading scheme as Json1C, Http1C and WebView2Bridge:
// LoadExternalComponent (1C) -> LoadLibrary -> DllMain installs the addin.hpp trap,
// no COM registration needed.
#include "stdafx.h"
#include <afxdllx.h>

HINSTANCE g_hInstance = NULL;

class CAddin
{
protected:
	CAddin() {}
	BOOL InitAddin(IDispatch* pConn)
	{
		m_pDisp = pConn;
		context_obj::CContextBase::InitAllContextClasses();   // registers RU_TYPE_NAME for CreateObject (1C)
		return TRUE;
	}
	void DoneAddin()
	{
		OdbcFixShutdown();   // put the original CallAsFunc back before 1cpp.dll can go away
		context_obj::CContextBase::DoneAllContextClasses();
	}
	IDispatchPtr m_pDisp;
};

// CLSID of this component (generated 2026-10-12, unique to OdbcFix1C).
class __declspec(uuid("4d0b7c2e-5a1f-4c39-9e6d-2b8f1a7c3e51")) CAddin;

static AFX_EXTENSION_MODULE OdbcFix1CDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInstance;
		if (!AfxInitExtensionModule(OdbcFix1CDLL, hInstance))
			return 0;
		new CDynLinkLibrary(OdbcFix1CDLL);
		addin::COneAddinImpl<CAddin>::OnProcessAttach();
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(OdbcFix1CDLL);
		addin::COneAddinImpl<CAddin>::OnProcessDetach();
	}
	return 1;
}
