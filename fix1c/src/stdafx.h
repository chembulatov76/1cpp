// Common include for OdbcFix1C. Build: VC6 only (binary compatibility with 1C 7.7 and MFC42).
// String and char literals in C++ sources are ASCII only; Russian names live in names_ru.inc.
#pragma once

// WIN32, _WINDOWS, _WINDLL, _AFXDLL, _AFXEXT, _MBCS come from the Makefile (/D).
#define VC_EXTRALEAN

#include <afxwin.h>
#include <afxdb.h>     // ODBC types used by 1C's bkend.h

// 1C internals and the "simplified" add-in scheme by A. Orefkov (SDK outside git, /I<SDK>).
#include "1CHEADERS/1cheaders.h"
#include "addin/addin.hpp"
#include "addin/contextimpl.hpp"

// Russian names in CP1251, generated from src/names_ru.inc into Out\gen (/I).
#include "names_ru_cp1251.inc"

#include "hook.h"
#include "fix.h"

extern HINSTANCE g_hInstance;
