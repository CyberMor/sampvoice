#define MyAppName "SAMPVOICE"
#define MyAppVersion "3.0"
#define MyAppPublisher "Daniel Mor"
#define MyAppURL "https://vk.com/sampvoicecom"

[Setup]
AppId={{01EA93A6-5255-44FE-ADB9-FDC3EFE89675}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
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
LanguageDetectionMethod=uilanguage
AllowNoIcons=yes
OutputDir=..\bin
OutputBaseFilename=sv_setup
SetupIconFile=icon.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Files]
Source: "files\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs uninsneveruninstall
Source: "..\bin\sampvoice.asi"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

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
			result := Copy(path, 1, p-1)
		end
	else result := '';
end;

function NextCheck(Sender: TWizardPage): Boolean;
begin
	if not FileExists((WizardDirValue() + '\gta_sa.exe'))
	then
		begin
			if ActiveLanguage() = 'russian'
			then MsgBox('Выберите директорию с GTA!', mbInformation, MB_OK)
			else MsgBox('Select a directory with the GTA!', mbInformation, MB_OK);
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
	
	if ActiveLanguage() = 'russian'
	then
		begin
			Page.Caption := 'Местоположение GTA:SA'
			Page.Description := 'Для продолжения установки выберите папку с игрой.'
		end
	else
		begin
			Page.Caption := 'Location GTA:SA'
			Page.Description := 'To continue the installation, select the folder with the game.'
		end;
	
	Page.OnNextButtonClick := @NextCheck;
end;
