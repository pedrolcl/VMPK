Name "Virtual MIDI Piano Keyboard"
SetCompressor /SOLID lzma

# Defines
!define QTFILES "C:\Qt\5.3\mingw482_32\bin"
!define QTLANG "C:\Qt\5.3\mingw482_32\translations"
!define QTPLUGINS "C:\Qt\5.3\mingw482_32\plugins"
!define MINGWFILES "C:\Qt\5.3\mingw482_32\bin"
!define MOREFILES "C:\freesw\bin"
!define DRUMSTICK "C:\freesw\lib\drumstick"
!define VMPKSRC "C:\Users\pedro\Projects\vmpk-desktop"
!define VMPKBLD "C:\Users\pedro\Projects\vmpk-build"

!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.6.0
!define COMPANY VMPK
!define URL http://vmpk.sourceforge.net/

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER vmpk
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_WELCOMEPAGE_TITLE_3LINES

# Included files
!include Sections.nsh
!include MUI2.nsh
!include Library.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE gpl.rtf
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Galician"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"
#!insertmacro MUI_LANGUAGE "Dutch"
#!insertmacro MUI_LANGUAGE "Russian"
#!insertmacro MUI_LANGUAGE "SimpChinese"

# Installer attributes
OutFile vmpk-${VERSION}-win32-setup.exe
InstallDir $PROGRAMFILES\vmpk
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.6.0.0
VIAddVersionKey ProductName VMPK
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription "Virtual MIDI Piano Keyboard"
VIAddVersionKey LegalCopyright "Copyright (C) 2008-2014 Pedro Lopez-Cabanillas and others"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
Icon src\vmpk.ico

# Installer sections
Section -Main SEC0000
	CreateDirectory $INSTDIR\drumstick
	CreateDirectory $INSTDIR\platforms
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
    File ${VMPKSRC}\data\gmgsxg.ins
    File ${VMPKSRC}\data\help.html
    File ${VMPKSRC}\data\help_es.html
    File ${VMPKBLD}\translations\vmpk_cs.qm
    File ${VMPKBLD}\translations\vmpk_de.qm
    File ${VMPKBLD}\translations\vmpk_es.qm
    File ${VMPKBLD}\translations\vmpk_fr.qm
    File ${VMPKBLD}\translations\vmpk_gl.qm
    File ${VMPKBLD}\translations\vmpk_sr.qm
    File ${VMPKBLD}\translations\vmpk_sv.qm
#   File ${VMPKBLD}\translations\vmpk_nl.qm
#   File ${VMPKBLD}\translations\vmpk_ru.qm
#   File ${VMPKBLD}\translations\vmpk_zh_CN.qm
    File ${QTLANG}\qt_cs.qm
    File ${QTLANG}\qt_de.qm
    File ${QTLANG}\qt_es.qm
    File ${QTLANG}\qt_fr.qm
    File ${QTLANG}\qt_gl.qm
    File ${QTLANG}\qt_sv.qm
#   File ${QTLANG}\qt_ru.qm
#   File ${QTLANG}\qt_zh_CN.qm

    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MINGWFILES}\libstdc++-6.dll $INSTDIR\libstdc++-6.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MINGWFILES}\libwinpthread-1.dll $INSTDIR\libwinpthread-1.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MINGWFILES}\libgcc_s_dw2-1.dll $INSTDIR\libgcc_s_dw2-1.dll $INSTDIR

    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Core.dll $INSTDIR\Qt5Core.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Gui.dll $INSTDIR\Qt5Gui.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Xml.dll $INSTDIR\Qt5Xml.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Svg.dll $INSTDIR\Qt5Svg.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Network.dll $INSTDIR\Qt5Network.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Widgets.dll $INSTDIR\Qt5Widgets.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\icudt52.dll $INSTDIR\icudt52.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\icuin52.dll $INSTDIR\icuin52.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\icuuc52.dll $INSTDIR\icuuc52.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTPLUGINS}\platforms\qwindows.dll $INSTDIR\platforms\qwindows.dll $INSTDIR
	
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MOREFILES}\intl.dll $INSTDIR\intl.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MOREFILES}\libdrumstick-rt.dll $INSTDIR\libdrumstick-rt.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MOREFILES}\libfluidsynth.dll $INSTDIR\libfluidsynth.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MOREFILES}\libglib-2.0-0.dll $INSTDIR\libglib-2.0-0.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MOREFILES}\libgthread-2.0-0.dll $INSTDIR\libgthread-2.0-0.dll $INSTDIR

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
    Delete /REBOOTOK $INSTDIR\qt_sv.qm
    Delete /REBOOTOK $INSTDIR\vmpk_cs.qm
    Delete /REBOOTOK $INSTDIR\vmpk_de.qm
    Delete /REBOOTOK $INSTDIR\vmpk_es.qm
    Delete /REBOOTOK $INSTDIR\vmpk_fr.qm
    Delete /REBOOTOK $INSTDIR\vmpk_gl.qm
    Delete /REBOOTOK $INSTDIR\vmpk_sr.qm
    Delete /REBOOTOK $INSTDIR\vmpk_sv.qm
    Delete /REBOOTOK $INSTDIR\vmpk.exe
    Delete /REBOOTOK $INSTDIR\spanish.xml
    Delete /REBOOTOK $INSTDIR\german.xml
    Delete /REBOOTOK $INSTDIR\azerty.xml
    Delete /REBOOTOK $INSTDIR\it-qwerty.xml
    Delete /REBOOTOK $INSTDIR\vkeybd-default.xml
    Delete /REBOOTOK $INSTDIR\pc102win.xml
    Delete /REBOOTOK $INSTDIR\gmgsxg.ins
    Delete /REBOOTOK $INSTDIR\help.html
    Delete /REBOOTOK $INSTDIR\help_es.html

    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libstdc++-6.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libwinpthread-1.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libgcc_s_dw2-1.dll

    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Core.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Gui.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Xml.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Svg.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Network.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Widgets.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icudt52.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icuin52.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\icuuc52.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\platforms\qwindows.dll

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
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /REBOOTOK $INSTDIR
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
