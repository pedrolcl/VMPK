Name "Virtual MIDI Piano Keyboard"
SetCompressor /SOLID lzma
# Request application privileges for Windows Vista
RequestExecutionLevel admin

# Defines
!define QTFILES    "C:\Qt\Qt5.5.0\5.5\mingw492_32"
!define BINFILES   "C:\freesw\bin"
!define DRUMSTICK  "C:\freesw\lib\drumstick"
!define VMPKSRC    "C:\Users\pedro\Projects\vmpk-desktop"
!define VMPKBLD    "C:\Users\pedro\Projects\vmpk-build-release"

!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.6.2
!define COMPANY VMPK
!define URL http://vmpk.sourceforge.net/

# Included files
!include Sections.nsh
!include MUI2.nsh
!include Library.nsh

# Variables
Var StartMenuGroup

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER vmpk

# Installer pages
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE gpl.rtf
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $INSTDIR\vmpk.exe
!define MUI_FINISHPAGE_LINK $(FinishLinkText) 
!define MUI_FINISHPAGE_LINK_LOCATION "http://play.google.com/store/apps/details?id=net.sourceforge.vmpk.free"
!define MUI_PAGE_CUSTOMFUNCTION_SHOW CustomFinishShow
!insertmacro MUI_PAGE_FINISH

!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TITLE_3LINES
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

# Installer languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Galician"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"
#!insertmacro MUI_LANGUAGE "Dutch"
#!insertmacro MUI_LANGUAGE "SimpChinese"

;Language strings
LangString FinishLinkText ${LANG_ENGLISH} "Android Application on Google Play"
LangString FinishLinkText ${LANG_CZECH} "Aplikace pro Android ve službě Google Play"
LangString FinishLinkText ${LANG_FRENCH} "Application Android sur Google Play"
LangString FinishLinkText ${LANG_GALICIAN} "Aplicación para Android en Google Play"
LangString FinishLinkText ${LANG_GERMAN} "Applikation für Android auf Google Play"
LangString FinishLinkText ${LANG_RUSSIAN} "Приложения для Android на Google Play"
LangString FinishLinkText ${LANG_SERBIAN} "Андроид апликација на Гоогле Плаи"
LangString FinishLinkText ${LANG_SPANISH} "Aplicación para Android en Google Play"
LangString FinishLinkText ${LANG_SWEDISH} "Applikationer på Google Play"

# Installer attributes
OutFile vmpk-${VERSION}-win32-setup.exe
InstallDir $PROGRAMFILES\vmpk
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.6.2.0
VIAddVersionKey ProductName VMPK
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription "Virtual MIDI Piano Keyboard"
VIAddVersionKey LegalCopyright "Copyright (C) 2008-2024 Pedro Lopez-Cabanillas and others"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
Icon src\vmpk.ico

# Installer sections
Section -Main SEC0000
	CreateDirectory $INSTDIR\drumstick
	CreateDirectory $INSTDIR\platforms
	CreateDirectory $INSTDIR\iconengines
	CreateDirectory $APPDATA\SoundFonts
    SetOutPath $INSTDIR
    SetOverwrite on
    File ${VMPKSRC}\qt.conf
    File ${VMPKBLD}\src\vmpk.exe
    File ${VMPKSRC}\data\spanish.xml
    File ${VMPKSRC}\data\german.xml
    File ${VMPKSRC}\data\azerty.xml
    File ${VMPKSRC}\data\it-qwerty.xml
    File ${VMPKSRC}\data\vkeybd-default.xml
    File ${VMPKSRC}\data\pc102win.xml
    File ${VMPKSRC}\data\Serbian-lat.xml
    File ${VMPKSRC}\data\Serbian-cyr.xml
    File ${VMPKSRC}\data\gmgsxg.ins
    File ${VMPKSRC}\data\help.html
    File ${VMPKSRC}\data\help_es.html
    File ${VMPKSRC}\data\help_sr.html
    File ${VMPKSRC}\data\help_ru.html
    File ${VMPKBLD}\translations\vmpk_cs.qm
    File ${VMPKBLD}\translations\vmpk_de.qm
    File ${VMPKBLD}\translations\vmpk_es.qm
    File ${VMPKBLD}\translations\vmpk_fr.qm
    File ${VMPKBLD}\translations\vmpk_gl.qm
    File ${VMPKBLD}\translations\vmpk_ru.qm
    File ${VMPKBLD}\translations\vmpk_sr.qm
    File ${VMPKBLD}\translations\vmpk_sv.qm
#   File ${VMPKBLD}\translations\vmpk_nl.qm
#   File ${VMPKBLD}\translations\vmpk_zh_CN.qm
    File ${QTFILES}\translations\qt_cs.qm
    File ${QTFILES}\translations\qt_de.qm
    File ${QTFILES}\translations\qt_es.qm
    File ${QTFILES}\translations\qt_fr.qm
    File ${QTFILES}\translations\qt_gl.qm
    File ${QTFILES}\translations\qt_sv.qm
    File ${QTFILES}\translations\qt_ru.qm
	File ${QTFILES}\translations\qtbase_cs.qm
	File ${QTFILES}\translations\qtscript_cs.qm
	File ${QTFILES}\translations\qtquick1_cs.qm
	File ${QTFILES}\translations\qtmultimedia_cs.qm
	File ${QTFILES}\translations\qtxmlpatterns_cs.qm
	File ${QTFILES}\translations\qtbase_de.qm
	File ${QTFILES}\translations\qtscript_de.qm
	File ${QTFILES}\translations\qtquick1_de.qm
	File ${QTFILES}\translations\qtmultimedia_de.qm
	File ${QTFILES}\translations\qtxmlpatterns_de.qm
	File ${QTFILES}\translations\qtbase_ru.qm
	File ${QTFILES}\translations\qtscript_ru.qm
	File ${QTFILES}\translations\qtquick1_ru.qm
	File ${QTFILES}\translations\qtmultimedia_ru.qm
	File ${QTFILES}\translations\qtxmlpatterns_ru.qm
#   File ${QTFILES}\translations\qt_zh_CN.qm
	File "/oname=$APPDATA\SoundFonts\GeneralUser GS FluidSynth v1.44.sf2" "C:\ProgramData\SoundFonts\GeneralUser GS FluidSynth v1.44.sf2"
	
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\libstdc++-6.dll $INSTDIR\libstdc++-6.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\libwinpthread-1.dll $INSTDIR\libwinpthread-1.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\libgcc_s_dw2-1.dll $INSTDIR\libgcc_s_dw2-1.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\Qt5Core.dll $INSTDIR\Qt5Core.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\Qt5Gui.dll $INSTDIR\Qt5Gui.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\Qt5Svg.dll $INSTDIR\Qt5Svg.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\Qt5Network.dll $INSTDIR\Qt5Network.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\Qt5Widgets.dll $INSTDIR\Qt5Widgets.dll $INSTDIR
	; !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\icudt52.dll $INSTDIR\icudt52.dll $INSTDIR
	; !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\icuin52.dll $INSTDIR\icuin52.dll $INSTDIR
	; !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bin\icuuc52.dll $INSTDIR\icuuc52.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\plugins\platforms\qwindows.dll $INSTDIR\platforms\qwindows.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\plugins\iconengines\qsvgicon.dll $INSTDIR\iconengines\qsvgicon.dll $INSTDIR
	
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${BINFILES}\intl.dll $INSTDIR\intl.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${BINFILES}\libdrumstick-rt.dll $INSTDIR\libdrumstick-rt.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${BINFILES}\libfluidsynth.dll $INSTDIR\libfluidsynth.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${BINFILES}\libglib-2.0-0.dll $INSTDIR\libglib-2.0-0.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${BINFILES}\libgthread-2.0-0.dll $INSTDIR\libgthread-2.0-0.dll $INSTDIR

	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\libdrumstick-rt-net-in.dll $INSTDIR\drumstick\libdrumstick-rt-net-in.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\libdrumstick-rt-net-out.dll $INSTDIR\drumstick\libdrumstick-rt-net-out.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\libdrumstick-rt-synth.dll $INSTDIR\drumstick\libdrumstick-rt-synth.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\libdrumstick-rt-win-in.dll $INSTDIR\drumstick\libdrumstick-rt-win-in.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\libdrumstick-rt-win-out.dll $INSTDIR\drumstick\libdrumstick-rt-win-out.dll $INSTDIR
	
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
SectionEnd

Section -post SEC0001
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
	
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory $SMPROGRAMS\$StartMenuGroup
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall VMPK.lnk" $INSTDIR\uninstall.exe
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\VMPK.lnk" $INSTDIR\vmpk.exe
    !insertmacro MUI_STARTMENU_WRITE_END
	
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
	
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" InputDriver "Network"
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" OutputDriver "FluidSynth"
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" InPort "21928"
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" OutPort "FluidSynth"
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" InEnabled "true"
	WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\Connections" ThruEnabled "true"
    WriteRegStr HKCU "Software\vmpk.sourceforge.net\VMPK-$(^Name)\FluidSynth" InstrumentsDefinition "$APPDATA\SoundFonts\GeneralUser GS FluidSynth v1.44.sf2"
	
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
    Delete /REBOOTOK $INSTDIR\qt.conf
    Delete /REBOOTOK $INSTDIR\qt_cs.qm
    Delete /REBOOTOK $INSTDIR\qt_de.qm
    Delete /REBOOTOK $INSTDIR\qt_es.qm
    Delete /REBOOTOK $INSTDIR\qt_fr.qm
    Delete /REBOOTOK $INSTDIR\qt_gl.qm
    Delete /REBOOTOK $INSTDIR\qt_ru.qm
    Delete /REBOOTOK $INSTDIR\qt_sv.qm
    Delete /REBOOTOK $INSTDIR\vmpk_cs.qm
    Delete /REBOOTOK $INSTDIR\vmpk_de.qm
    Delete /REBOOTOK $INSTDIR\vmpk_es.qm
    Delete /REBOOTOK $INSTDIR\vmpk_fr.qm
    Delete /REBOOTOK $INSTDIR\vmpk_gl.qm
    Delete /REBOOTOK $INSTDIR\vmpk_ru.qm
    Delete /REBOOTOK $INSTDIR\vmpk_sr.qm
    Delete /REBOOTOK $INSTDIR\vmpk_sv.qm
	Delete /REBOOTOK $INSTDIR\qtbase_cs.qm
	Delete /REBOOTOK $INSTDIR\qtscript_cs.qm
	Delete /REBOOTOK $INSTDIR\qtquick1_cs.qm
	Delete /REBOOTOK $INSTDIR\qtmultimedia_cs.qm
	Delete /REBOOTOK $INSTDIR\qtxmlpatterns_cs.qm
	Delete /REBOOTOK $INSTDIR\qtbase_de.qm
	Delete /REBOOTOK $INSTDIR\qtscript_de.qm
	Delete /REBOOTOK $INSTDIR\qtquick1_de.qm
	Delete /REBOOTOK $INSTDIR\qtmultimedia_de.qm
	Delete /REBOOTOK $INSTDIR\qtxmlpatterns_de.qm
	Delete /REBOOTOK $INSTDIR\qtbase_ru.qm
	Delete /REBOOTOK $INSTDIR\qtscript_ru.qm
	Delete /REBOOTOK $INSTDIR\qtquick1_ru.qm
	Delete /REBOOTOK $INSTDIR\qtmultimedia_ru.qm
	Delete /REBOOTOK $INSTDIR\qtxmlpatterns_ru.qm
    Delete /REBOOTOK $INSTDIR\vmpk.exe
    Delete /REBOOTOK $INSTDIR\spanish.xml
    Delete /REBOOTOK $INSTDIR\german.xml
    Delete /REBOOTOK $INSTDIR\azerty.xml
    Delete /REBOOTOK $INSTDIR\it-qwerty.xml
    Delete /REBOOTOK $INSTDIR\vkeybd-default.xml
    Delete /REBOOTOK $INSTDIR\pc102win.xml
    Delete /REBOOTOK $INSTDIR\Serbian-lat.xml
    Delete /REBOOTOK $INSTDIR\Serbian-cyr.xml
    Delete /REBOOTOK $INSTDIR\gmgsxg.ins
    Delete /REBOOTOK $INSTDIR\help.html
    Delete /REBOOTOK $INSTDIR\help_es.html
    Delete /REBOOTOK $INSTDIR\help_ru.html
    Delete /REBOOTOK $INSTDIR\help_sr.html
	Delete /REBOOTOK "$APPDATA\SoundFonts\GeneralUser GS FluidSynth v1.44.sf2"
	
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libstdc++-6.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libwinpthread-1.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libgcc_s_dw2-1.dll

    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Core.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Gui.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Svg.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Network.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Widgets.dll
	; !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icudt52.dll 
	; !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icuin52.dll 
	; !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icuuc52.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\platforms\qwindows.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\iconengines\qsvgicon.dll

	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\intl.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libdrumstick-rt.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libfluidsynth.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libglib-2.0-0.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libgthread-2.0-0.dll

	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\libdrumstick-rt-net-in.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\libdrumstick-rt-net-out.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\libdrumstick-rt-synth.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\libdrumstick-rt-win-in.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\libdrumstick-rt-win-out.dll
	
	RMDir /REBOOTOK $INSTDIR\drumstick
	RMDir /REBOOTOK $INSTDIR\platforms
	RMDir /REBOOTOK $INSTDIR\iconengines
	RMDir /REBOOTOK $APPDATA\SoundFonts
	
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall VMPK.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\VMPK.lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RMDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RMDir /REBOOTOK $INSTDIR
SectionEnd

#Installer Functions
Function .onInit
    !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

# Uninstaller functions
Function un.onInit
    !insertmacro MUI_UNGETLANGUAGE
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd

Function OpenLink
	ExecShell open "http://play.google.com/store/apps/details?id=net.sourceforge.vmpk.free"
FunctionEnd

Var hCtl_Finish_Bitmap1
Var hCtl_Finish_Bitmap1_hImage

Function CustomFinishShow
  ${NSD_CreateBitmap} 120u 130u 90u 30u ""
  Pop $hCtl_Finish_Bitmap1
  ${NSD_OnClick} $hCtl_Finish_Bitmap1 OpenLink
  ${Switch} $LANGUAGE
  ${Case} ${LANG_ENGLISH}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\en_app.bmp"
  ${Break}
  ${Case} ${LANG_CZECH}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\cs_app.bmp"
  ${Break}
  ${Case} ${LANG_FRENCH}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\fr_app.bmp"
  ${Break}
  ${Case} ${LANG_GALICIAN}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\es_app.bmp"
  ${Break}
  ${Case} ${LANG_GERMAN}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\de_app.bmp"
  ${Break}
  ${Case} ${LANG_RUSSIAN}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\ru_app.bmp"
  ${Break}
  ${Case} ${LANG_SERBIAN}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\sr_app.bmp"
  ${Break}
  ${Case} ${LANG_SPANISH}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\es_app.bmp"
  ${Break}
  ${Case} ${LANG_SWEDISH}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\sv_app.bmp"
  ${Break}
  ${Default}
  File "/oname=$PLUGINSDIR\banner.bmp" "${VMPKSRC}\data\en_app.bmp"
  ${Break}
  ${EndSwitch}
  ${NSD_SetImage} $hCtl_Finish_Bitmap1 "$PLUGINSDIR\banner.bmp" $hCtl_Finish_Bitmap1_hImage
FunctionEnd
