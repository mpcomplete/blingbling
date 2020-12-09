; blingbling.nsi - 

!define VERSION 1.1
!define BLINGDIR "$PROGRAMFILES\BlingBling"

Name "BlingBling ${VERSION}"
OutFile "blingbling-${VERSION}.exe"

InstallDir "${BLINGDIR}"

; The stuff to install
Section "Install"
    SectionIn RO

    MessageBox MB_YESNO|MB_ICONINFORMATION \
	"This will install BlingBling.  Do you want to proceed?" IDYES yesinstall
      Quit
    yesinstall:

    call AutoInstallLibs

    SetOutPath $INSTDIR
    File "..\src\blingbling.exe"
    File /r "..\data"

    ; Write the path to the data directory
    WriteRegStr HKCU "Software\BlingBling\${VERSION}\" "datadir" "$INSTDIR\data"

    ; Write the uninstall keys for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingBling" "DisplayName" "BlingBling (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingBling" "UninstallString" '"$INSTDIR\uninstall.exe"'
    WriteUninstaller "uninstall.exe"

    CreateDirectory "$SMPROGRAMS\BlingBling"
    CreateShortCut "$SMPROGRAMS\BlingBling\BlingBling.lnk" \
	"$INSTDIR\blingbling.exe"
    CreateShortCut "$SMPROGRAMS\BlingBling\Uninstall BlingBling.lnk" \
	"$INSTDIR\uninstall.exe"

    MessageBox MB_YESNO|MB_ICONINFORMATION \
	"Create Desktop shortcut?" IDYES yesshortcut
      Quit
    yesshortcut:
    CreateShortCut "$DESKTOP\BlingBling.lnk" "$INSTDIR\blingbling.exe"
SectionEnd


Function AutoInstallLibs
    ReadRegStr $0 HKLM \
	"Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingLibs" \
	"DisplayName"
    StrCmp $0 "" getlibs
      Return
    getlibs:

    MessageBox MB_YESNO|MB_ICONINFORMATION "You are missing some required libraries.  Do you want me to download and install them for you?" IDYES yesgetlibs
      Quit
    yesgetlibs:

    StrCpy $2 "$TEMP\blinglibs-installer.exe"
    NSISdl::download http://somewhere.fscked.org/blingbling/blinglibs-${VERSION}.exe $2
    Pop $0
    StrCmp $0 success success
      SetDetailsView show
      DetailPrint "download failed: $0"
      Abort
    success:
      ExecWait '"$2" /S'
      Delete $2
FunctionEnd

;--------------------------------
; Uninstaller

UninstallText "This will uninstall BlingBling. Hit next to continue."

Section "Uninstall"
    ; remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingBling"
    DeleteRegKey HKCU "Software\BlingBling"

    ; remove files and uninstaller
    Delete "$INSTDIR\blingbling.exe"
    RMDir /r "$INSTDIR\data"
    RMDir $INSTDIR

    Delete "$DESKTOP\BlingBling.lnk"
    Delete "$SMPROGRAMS\BlingBling\BlingBling.lnk"
    Delete "$SMPROGRAMS\BlingBling\Uninstall BlingBling.lnk"
    RMDir "$SMPROGRAMS\BlingBling"
SectionEnd
