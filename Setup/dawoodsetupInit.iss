; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "DaWood Series Sync"
#define MyAppVersion "v2.0.2b"
#define MyAppPublisher "����� ��� �� ���"
#define MyAppURL "https://www.vnims.ru/"
#define MyAppExeName "DaWood_Sync.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F4E03F1A-FA86-4BEE-86AA-EE4C1348ECC2}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\Setup
LicenseFile=C:\Users\den2l\Downloads\License.txt
OutputBaseFilename=DaWoodSync_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\DaWoodSync\res\VnimsDataLogger2.ico

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\libGLESv2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5SerialPort.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\build-DaWoodSync-Desktop_Qt_5_15_1_MinGW_32_bit-Release\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\Documents\DavydovDenis\SourceCode\Projects\DaWoodSync\Driver\*"; DestDir: {app}\driver;  Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
Filename: "{app}\driver\stlink_winusb_install.bat"

;[UninstallDelete]
;Type: filesandordirs; Name: "{usertemplates} & {commontemplates}\DaWoodSeriesSync"