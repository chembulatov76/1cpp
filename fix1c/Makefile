# nmake (VC6). Run through build.cmd, which sets VS6, SDK and generates Out\gen\names_ru_cp1251.inc.
!IFNDEF VS6
VS6=C:\VS6
!ENDIF
!IFNDEF SDK
SDK=C:\Install\vc6\1c-sdk
!ENDIF

CPP=$(VS6)\VC98\BIN\cl.exe
LINK32=$(VS6)\VC98\BIN\link.exe

RESULT=OdbcFix1C.dll
SRC=.\src
OUTDIR=.\Out
GENDIR=$(OUTDIR)\gen

# Same code generation as Json1C: /EHa so that catch(...) in the wrapper also sees hardware faults
# raised inside the original method (the flag is restored and the fault passes on unchanged).
CPP_PROJ=/nologo /I"$(VS6)\VC98\INCLUDE" /I"$(VS6)\VC98\MFC\INCLUDE" /I"$(SDK)" /I"$(GENDIR)" /I"$(SRC)" \
	/MD /W3 /GR /EHa /O2 /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_MBCS" \
	/Fo"$(OUTDIR)\\" /c

DEF_FILE=.\OdbcFix1C.def
LINK32_FLAGS=/nologo /libpath:"$(VS6)\VC98\LIB" /libpath:"$(VS6)\VC98\MFC\LIB" /libpath:"$(SDK)\1CHEADERS\libs" \
	/subsystem:windows /dll /incremental:no /machine:I386 /def:"$(DEF_FILE)" /out:"$(OUTDIR)\$(RESULT)"
LINK32_OBJS="$(OUTDIR)\stdafx.obj" "$(OUTDIR)\hook.obj" "$(OUTDIR)\fix.obj" "$(OUTDIR)\dllmain.obj"

all: "$(OUTDIR)\$(RESULT)"

"$(OUTDIR)\$(RESULT)" : $(DEF_FILE) $(LINK32_OBJS)
	$(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

{$(SRC)}.cpp{$(OUTDIR)}.obj::
	$(CPP) @<<
   $(CPP_PROJ) $<
<<

clean:
	-del /q "$(OUTDIR)\*.obj" "$(OUTDIR)\*.dll" "$(OUTDIR)\*.lib" "$(OUTDIR)\*.exp" 2>nul
