; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "AlarmBox"
#define MyAppVersion "1.7"
#define MyAppPublisher "SSemenkin"
#define MyAppURL "https://t.me/ssemenkin"
#define MyAppExeName "AlarmBox.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{5A3D03A2-01B8-491C-9133-F98E2EEECC6B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=C:\Users\User\Desktop\AlarmBoxSetup
OutputBaseFilename=AlarmBox_1.7_Setup
SetupIconFile=C:\Users\User\Documents\AlarmBox\src\icons\app.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\User\Desktop\AlarmBox\AlarmBox.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libGLESv2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\MapGraphics.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libssl-1_1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\libcrypto-1_1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\User\Desktop\AlarmBox\bearer\*"; DestDir: "{app}\bearer}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\User\Desktop\AlarmBox\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

