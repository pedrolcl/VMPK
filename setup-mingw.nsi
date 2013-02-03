Name "Virtual MIDI Piano Keyboard"
SetCompressor /SOLID lzma

# Defines
!define QTFILES "C:\Qt\4.8.4\mingw\bin"
!define QTLANG "C:\Qt\4.8.4\mingw\translations"
!define MINGWFILES "C:\QtSDK\mingw\bin"
!define VMPKSRC "C:\Users\pedro\Projects\vmpk-desktop"
!define VMPKBLD "C:\Users\pedro\Projects\vmpk-build-desktop-Release"

!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.5.1
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
#!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "Czech"
#!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"

# Installer attributes
OutFile vmpk-${VERSION}-win32-setup.exe
InstallDir $PROGRAMFILES\vmpk
CRCCheck on
XPStyle on
ShowInstDetails show
VIProductVersion 0.5.1.0
VIAddVersionKey ProductName VMPK
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription "Virtual MIDI Piano Keyboard"
VIAddVersionKey LegalCopyright "Copyright (C) 2008-2013 Pedro Lopez-Cabanillas and others"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
Icon src/vmpk.ico

# Installer sections
Section -Main SEC0000
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
    File ${VMPKSRC}\data\help_de.html
    File ${VMPKSRC}\data\help_es.html
    File ${VMPKSRC}\data\help_nl.html
    File ${VMPKSRC}\data\help_ru.html
    File ${VMPKBLD}\translations\vmpk_cs.qm
    File ${VMPKBLD}\translations\vmpk_de.qm
    File ${VMPKBLD}\translations\vmpk_es.qm
    File ${VMPKBLD}\translations\vmpk_fr.qm
#   File ${VMPKBLD}\translations\vmpk_nl.qm
    File ${VMPKBLD}\translations\vmpk_ru.qm
    File ${VMPKBLD}\translations\vmpk_sv.qm
#   File ${VMPKBLD}\translations\vmpk_zh_CN.qm
    File ${QTLANG}\qt_cs.qm
    File ${QTLANG}\qt_de.qm
    File ${QTLANG}\qt_es.qm
    File ${QTLANG}\qt_fr.qm
    File ${QTLANG}\qt_ru.qm
    File ${QTLANG}\qt_sv.qm
#   File ${QTLANG}\qt_zh_CN.qm

    # Installing library mingwm10.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MINGWFILES}\mingwm10.dll $INSTDIR\mingwm10.dll $INSTDIR

    # Installing library libgcc_s_dw2-1.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${MINGWFILES}\libgcc_s_dw2-1.dll $INSTDIR\libgcc_s_dw2-1.dll $INSTDIR

    # Installing library QtCore4.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\QtCore4.dll $INSTDIR\QtCore4.dll $INSTDIR

    # Installing library QtGui4.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\QtGui4.dll $INSTDIR\QtGui4.dll $INSTDIR

    # Installing library QtXml4.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\QtXml4.dll $INSTDIR\QtXml4.dll $INSTDIR

    # Installing library QtSvg4.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\QtSvg4.dll $INSTDIR\QtSvg4.dll $INSTDIR

    # Installing library QtNetwork4.dll
    !insertmacro InstallLib DLL NOTSHARED REBOOT_PROTECTED ${QTFILES}\QtNetwork4.dll $INSTDIR\QtNetwork4.dll $INSTDIR

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
    Delete /REBOOTOK $INSTDIR\qt_ru.qm
    Delete /REBOOTOK $INSTDIR\qt_sv.qm
#   Delete /REBOOTOK $INSTDIR\qt_zh_CN.qm
    Delete /REBOOTOK $INSTDIR\vmpk_cs.qm
    Delete /REBOOTOK $INSTDIR\vmpk_de.qm
    Delete /REBOOTOK $INSTDIR\vmpk_es.qm
    Delete /REBOOTOK $INSTDIR\vmpk_fr.qm
#   Delete /REBOOTOK $INSTDIR\vmpk_nl.qm
    Delete /REBOOTOK $INSTDIR\vmpk_ru.qm
    Delete /REBOOTOK $INSTDIR\vmpk_sv.qm
#   Delete /REBOOTOK $INSTDIR\vmpk_zh_CN.qm
    Delete /REBOOTOK $INSTDIR\vmpk.exe
    Delete /REBOOTOK $INSTDIR\spanish.xml
    Delete /REBOOTOK $INSTDIR\german.xml
    Delete /REBOOTOK $INSTDIR\azerty.xml
    Delete /REBOOTOK $INSTDIR\it-qwerty.xml
    Delete /REBOOTOK $INSTDIR\vkeybd-default.xml
    Delete /REBOOTOK $INSTDIR\pc102win.xml
    Delete /REBOOTOK $INSTDIR\gmgsxg.ins
    Delete /REBOOTOK $INSTDIR\help.html
    Delete /REBOOTOK $INSTDIR\help_de.html
    Delete /REBOOTOK $INSTDIR\help_es.html
    Delete /REBOOTOK $INSTDIR\help_nl.html
    Delete /REBOOTOK $INSTDIR\help_ru.html

    # Uninstalling library $INSTDIR\mingwm10.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\mingwm10.dll

    # Uninstalling library $INSTDIR\libgcc_s_dw2-1.dll
    !insertmacro UnInstallLib DLL SHARED REBOOT_PROTECTED $INSTDIR\libgcc_s_dw2-1.dll

    # Uninstalling library $INSTDIR\QtCore4.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\QtCore4.dll

    # Uninstalling library $INSTDIR\QtGui4.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\QtGui4.dll

    # Uninstalling library $INSTDIR\QtXml4.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\QtXml4.dll

    # Uninstalling library $INSTDIR\QtSvg4.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\QtSvg4.dll

    # Uninstalling library $INSTDIR\QtNetwork4.dll
    !insertmacro UnInstallLib DLL NOTSHARED REBOOT_PROTECTED $INSTDIR\QtNetwork4.dll

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
