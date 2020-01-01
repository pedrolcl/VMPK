Name "Virtual MIDI Piano Keyboard"
SetCompressor /SOLID lzma
Unicode true
# BrandingText " "
# Request application privileges for Windows Vista
RequestExecutionLevel admin

# Defines
!define QTFILES "..\vmpk-0.7.2-win-x86"
!define QTLANG  "..\vmpk-0.7.2-win-x86"
!define VMPKSRC "."
!define VMPKBLD "..\vmpk-0.7.2-win-x86"
!define DRUMSTICK  "..\vmpk-0.7.2-win-x86"
!define PROGNAME "vmpk"
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.7.2
!define COMPANY VMPK
!define URL http://vmpk.sourceforge.net/

# Included files
!include Sections.nsh
!include MUI2.nsh
!include Library.nsh
!include x64.nsh

# MUI defines
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\nsis3-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\nsis3-uninstall.ico"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER vmpk

# Variables
Var StartMenuGroup

# Installer pages
!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE gpl.rtf
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_NOAUTOCLOSE
!insertmacro MUI_PAGE_FINISH

!define MUI_WELCOMEPAGE_TITLE_3LINES
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!define MUI_FINISHPAGE_TITLE_3LINES
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

# Installer attributes
OutFile vmpk-${VERSION}-win-x86-setup.exe
#InstallDir $PROGRAMFILES\vmpk
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.7.2.0
VIAddVersionKey ProductName VMPK
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription "Virtual MIDI Piano Keyboard"
VIAddVersionKey LegalCopyright "Copyright (C) 2008-2020 Pedro Lopez-Cabanillas and others"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
Icon ${VMPKSRC}\src\vmpk.ico
UninstallIcon ${VMPKSRC}\src\vmpk.ico

# Installer sections
Section -Main SEC0000
	CreateDirectory $INSTDIR\bearer
	CreateDirectory $INSTDIR\drumstick
	CreateDirectory $INSTDIR\iconengines
	CreateDirectory $INSTDIR\imageformats
	CreateDirectory $INSTDIR\platforms
	CreateDirectory $INSTDIR\styles
	CreateDirectory $INSTDIR\translations
    SetOverwrite on
	SetOutPath $INSTDIR\translations	
    File ${VMPKBLD}\translations\vmpk_cs.qm
    File ${VMPKBLD}\translations\vmpk_de.qm
    File ${VMPKBLD}\translations\vmpk_es.qm
    File ${VMPKBLD}\translations\vmpk_fr.qm
    File ${VMPKBLD}\translations\vmpk_gl.qm
    File ${VMPKBLD}\translations\vmpk_ru.qm
    File ${VMPKBLD}\translations\vmpk_sr.qm
    File ${VMPKBLD}\translations\vmpk_sv.qm
    File ${QTLANG}\translations\qt_cs.qm
    File ${QTLANG}\translations\qt_de.qm
    File ${QTLANG}\translations\qt_es.qm
    File ${QTLANG}\translations\qt_fr.qm
    File ${QTLANG}\translations\qt_gl.qm
    File ${QTLANG}\translations\qt_ru.qm
	#File ${QTLANG}\translations\qt_sr.qm
    File ${QTLANG}\translations\qt_sv.qm

    SetOutPath $INSTDIR
	File ${VMPKBLD}\vcredist_x86.exe
	File ${VMPKBLD}\vmpk.exe
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
    File ${VMPKSRC}\data\help_de.html
    File ${VMPKSRC}\data\help_es.html
    File ${VMPKSRC}\data\help_fr.html
    File ${VMPKSRC}\data\help_sr.html
    File ${VMPKSRC}\data\help_ru.html
	
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Core.dll $INSTDIR\Qt5Core.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Gui.dll $INSTDIR\Qt5Gui.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Network.dll $INSTDIR\Qt5Network.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Svg.dll $INSTDIR\Qt5Svg.dll $INSTDIR
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\Qt5Widgets.dll $INSTDIR\Qt5Widgets.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\libEGL.dll $INSTDIR\libEGL.dll $INSTDIR	
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\libGLESV2.dll $INSTDIR\libGLESV2.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\opengl32sw.dll $INSTDIR\opengl32sw.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\d3dcompiler_47.dll $INSTDIR\d3dcompiler_47.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\platforms\qwindows.dll $INSTDIR\platforms\qwindows.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\iconengines\qsvgicon.dll $INSTDIR\iconengines\qsvgicon.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bearer\qgenericbearer.dll $INSTDIR\bearer\qgenericbearer.dll $INSTDIR
	#insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\bearer\qnativewifibearer.dll $INSTDIR\bearer\qnativewifibearer.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\imageformats\qgif.dll $INSTDIR\imageformats\qgif.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\imageformats\qicns.dll $INSTDIR\imageformats\qicns.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\imageformats\qico.dll $INSTDIR\imageformats\qico.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\imageformats\qjpeg.dll $INSTDIR\imageformats\qjpeg.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\imageformats\qsvg.dll $INSTDIR\imageformats\qsvg.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\styles\qwindowsvistastyle.dll $INSTDIR\styles\qwindowsvistastyle.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\drumstick-rt1.dll $INSTDIR\drumstick-rt1.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\drumstick\drumstick-rt-net-in1.dll $INSTDIR\drumstick\drumstick-rt-net-in1.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\drumstick\drumstick-rt-net-out1.dll $INSTDIR\drumstick\drumstick-rt-net-out1.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\drumstick\drumstick-rt-win-in1.dll $INSTDIR\drumstick\drumstick-rt-win-in1.dll $INSTDIR
	!insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${DRUMSTICK}\drumstick\drumstick-rt-win-out1.dll $INSTDIR\drumstick\drumstick-rt-win-out1.dll $INSTDIR
	
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
	;VS2015 Runtime
	ReadRegStr $1 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"
	StrCmp $1 1 installed
	;not installed, so run the installer
	ExecWait '"$INSTDIR\vcredist_x86.exe" /install /quiet /norestart'
installed:	
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
    Delete /REBOOTOK $INSTDIR\translations\qt_cs.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_de.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_es.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_fr.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_gl.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_ru.qm
    Delete /REBOOTOK $INSTDIR\translations\qt_sv.qm
#   Delete /REBOOTOK $INSTDIR\translations\qt_sr.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_cs.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_de.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_es.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_fr.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_gl.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_ru.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_sv.qm
    Delete /REBOOTOK $INSTDIR\translations\vmpk_sr.qm

    Delete /REBOOTOK $INSTDIR\vmpk.exe
	Delete /REBOOTOK $INSTDIR\vcredist_x86.exe
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
    Delete /REBOOTOK $INSTDIR\help_de.html
    Delete /REBOOTOK $INSTDIR\help_es.html
    Delete /REBOOTOK $INSTDIR\help_fr.html
    Delete /REBOOTOK $INSTDIR\help_ru.html
    Delete /REBOOTOK $INSTDIR\help_sr.html

    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Core.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Gui.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Network.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Svg.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\Qt5Widgets.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libEGL.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\libGLESV2.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\opengl32sw.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\d3dcompiler_47.dll 
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\platforms\qwindows.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\iconengines\qsvgicon.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\bearer\qgenericbearer.dll
	#insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\bearer\qnativewifibearer.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\imageformats\qgif.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\imageformats\qicns.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\imageformats\qico.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\imageformats\qjpeg.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\imageformats\qsvg.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\styles\qwindowsvistastyle.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick-rt1.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\drumstick-rt-net-in1.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\drumstick-rt-net-out1.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\drumstick-rt-win-in1.dll
	!insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\drumstick\drumstick-rt-win-out1.dll
	RMDir /REBOOTOK $INSTDIR\translations
	RMDir /REBOOTOK $INSTDIR\styles
	RMDir /REBOOTOK $INSTDIR\platforms
	RMDir /REBOOTOK $INSTDIR\imageformats
	RMDir /REBOOTOK $INSTDIR\iconengines
	RMDir /REBOOTOK $INSTDIR\drumstick
	RMDir /REBOOTOK $INSTDIR\bearer
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
	${If} ${RunningX64}
		StrCpy $INSTDIR "$PROGRAMFILES32\${PROGNAME}"
	${Else}
		StrCpy $INSTDIR "$PROGRAMFILES\${PROGNAME}"
	${EndIf}	
FunctionEnd

# Uninstaller functions
Function un.onInit
    !insertmacro MUI_UNGETLANGUAGE
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd
