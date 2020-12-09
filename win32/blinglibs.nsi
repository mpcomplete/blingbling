; blinglibs.nsi - 

!define VERSION 1.1
!define BLINGDIR "$PROGRAMFILES\BlingBling"

Name "BlingLibs ${VERSION}"
OutFile "blinglibs-${VERSION}.exe"

InstallDir $WINDIR


; The stuff to install
Section "Install"
    SectionIn RO

    MessageBox MB_YESNO|MB_ICONINFORMATION "This will install the following libraries into your $WINDIR directory: SDL, SDL_image, SDL_mixer, smpeg, and libpng.  If you already have versions of these libraries, they will be overwritten.  (If you've never heard of them, you probably don't have them).  Do you want to proceed?" IDYES yesinstall
	Quit
    yesinstall:
    SetOutPath $INSTDIR
    File "*.dll"


    ; Write the uninstall keys for Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingLibs" "DisplayName" "BlingLibs (remove only)"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingLibs" "UninstallString" '"${BLINGDIR}\uninstall-libs.exe"'

    CreateDirectory "${BLINGDIR}"
    WriteUninstaller "${BLINGDIR}\uninstall-libs.exe"

    CreateShortCut "$SMPROGRAMS\BlingBling\Uninstall BlingBling libraries.lnk" \
	"${BLINGDIR}\uninstall-libs.exe"
SectionEnd

;--------------------------------
; Uninstaller

UninstallText "This will uninstall BlingLibs. Hit next to continue."

Section "Uninstall"
    ; remove registry keys
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\BlingLibs"

    ; remove files and uninstaller
    Delete "$WINDIR\SDL.dll"
    Delete "$WINDIR\SDL_image.dll"
    Delete "$WINDIR\SDL_mixer.dll"
    Delete "$WINDIR\cygpng12.dll"
    Delete "$WINDIR\smpeg.dll"

    Delete "$SMPROGRAMS\BlingBling\Uninstall BlingBling libraries.lnk"
    RMDir "$SMPROGRAMS\BlingBling"

    Delete "${BLINGDIR}\uninstall-libs.exe"
    RMDir "${BLINGDIR}"
SectionEnd
