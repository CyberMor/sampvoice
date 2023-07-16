#define MyAppName "SampVoice"
#define MyAppVersion "4.0"
#define MyAppPublisher "Daniel Mor"
#define MyAppURL "https://github.com/CyberMor/sampvoice"

[Setup]
AppId={{01EA93A6-5255-44FE-ADB9-FDC3EFE89675}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
UsePreviousAppDir=no

DisableFinishedPage=no
DisableWelcomePage=yes
DisableReadyPage=yes
DirExistsWarning=no
AppendDefaultDirName=no

UninstallDisplayName={#MyAppName} {#MyAppVersion}
UninstallDisplayIcon=icon.ico

DefaultDirName={code:GetPathGTA}
DefaultGroupName={#MyAppName}
LanguageDetectionMethod=none
AllowNoIcons=yes
OutputDir=..\binaries
OutputBaseFilename=installer
SetupIconFile=icon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "..\binaries\sampvoice.asi"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\client\files\binaries\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs uninsneveruninstall
Source: "..\client\files\languages\*"; DestDir: "{app}\sampvoice\languages"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\client\files\resources\*"; DestDir: "{app}\sampvoice\resources"; Flags: ignoreversion recursesubdirs createallsubdirs

[Code]

function GetPathGTA(Param:String) : string;
var
	path:string;
	p:Integer;
begin
	if RegQueryStringValue(HKCU, 'Software\SAMP', 'gta_sa_exe', path)
	then
		begin
			p := Pos('gta_sa.exe', path)
			result := Copy(path, 1, p - 1)
		end
	else result := '';
end;

function NextCheck(Sender: TWizardPage): Boolean;
begin
	if not FileExists((WizardDirValue() + '\gta_sa.exe'))
	then
		begin
			MsgBox('Select a directory with the GTA San Andreas', mbInformation, MB_OK)
			result := False
		end
	else
		result := True;
end;

procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
begin
	Confirm := False;
end;

procedure InitializeWizard();
var
	Page: TWizardPage;
begin
	Page := PageFromID(wpSelectDir);
	
	Page.Caption := 'Location GTA San Andreas'
  Page.Description := 'To continue the installation, select the folder with the game.'
	
	Page.OnNextButtonClick := @NextCheck;
end;
