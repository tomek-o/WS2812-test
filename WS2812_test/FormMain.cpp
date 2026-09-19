//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMain.h"
#include "FormAbout.h"
#include "FormSettings.h"
#include "Settings.h"
#include "FormLog.h"
#include "Log.h"
#include "ComPort.h"
#include "TabManager.h"
#include "FormWS2812.h"
#include "WS2812\FormWS2812AudioVisualisation.h"
#include "common\TrayIcon.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner),
	serialOpenCalled(false)
{
#ifdef ACCEPT_WM_DROPFILES
	// inform OS that we accepting dropping files
	DragAcceptFiles(Handle, True);
#endif

	trIcon = new TrayIcon(this);
	trIcon->OnLeftBtnDown = OnTrayIconLeftBtnDown;
	trIcon->SetPopupMenu(popupTray);
	trIcon->SetIcon(Application->Icon);
	trIcon->SetHint(Application->Title);
	trIcon->ShowInTray(true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
	appSettings.Read(asConfigFile);
	if (this->BorderStyle != bsSingle)
	{
		this->Width = appSettings.frmMain.width;
		this->Height = appSettings.frmMain.height;
	}
	this->Top = appSettings.frmMain.posY;
	this->Left = appSettings.frmMain.posX;
	if (appSettings.frmMain.alwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	if (appSettings.frmMain.windowMaximized)
		this->WindowState = wsMaximized;
	UpdateLogConfig();

	{
		AnsiString tmp;
		tmp.sprintf("%s (re)open", appSettings.serialPort.name.c_str());
		btnSerialOpen->Caption = tmp;
	}		
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	AnsiString asConfigFile = ChangeFileExt( Application->ExeName, ".json" );
	appSettings.frmMain.windowMaximized = (this->WindowState == wsMaximized);
	if (!appSettings.frmMain.windowMaximized)
	{
		// these values are meaningless is wnd is maximized
		appSettings.frmMain.width = this->Width;
		appSettings.frmMain.height = this->Height;
		appSettings.frmMain.posY = this->Top;
		appSettings.frmMain.posX = this->Left;
	}

	appSettings.logging.windowWidth = frmLog->Width;
	appSettings.logging.windowHeight = frmLog->Height;

	appSettings.frmMain.activeTabIndex = pages->ActivePageIndex;

	appSettings.Write(asConfigFile);

	comPort.Close();

	CanClose = true;	
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::actShowAboutExecute(TObject *Sender)
{
	frmAbout->ShowModal();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowSettingsExecute(TObject *Sender)
{
	Settings prevSettings = appSettings;
	frmSettings->appSettings = &appSettings;
	frmSettings->ShowModal();
	if (appSettings.frmMain.alwaysOnTop)
		this->FormStyle = fsStayOnTop;
	else
		this->FormStyle = fsNormal;
	UpdateLogConfig();
	frmLog->SetLogLinesLimit(appSettings.logging.maxUiLogLines);
	if (frmLog->Visible)
		frmLog->UpdateUi();
	frmWS2812AudioVisualisation->UpdateUi();

	{
		AnsiString tmp;
		tmp.sprintf("%s (re)open", appSettings.serialPort.name.c_str());
		btnSerialOpen->Caption = tmp;
	}

	if (serialOpenCalled)
	{
        // apply new serial port settings immediately
		if (prevSettings.serialPort.name != appSettings.serialPort.name ||
			prevSettings.serialPort.baud != appSettings.serialPort.baud)
		{
			SerialOpen();
		}
	}

	UpdateLedCount();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormShow(TObject *Sender)
{
	// re-validate position in case monitors changed while minimized to tray
	OnRestore(NULL);
}
//---------------------------------------------------------------------------

void TfrmMain::UpdateLedCount(void)
{
	std::vector<TForm*>& forms = TabManager::Instance().GetForms();
	for (unsigned int i=0; i<forms.size(); i++)
	{
		FormWS2812 *frm = dynamic_cast<FormWS2812*>(forms[i]);
		if (frm)
		{
			frm->SetLedCount(appSettings.ws2812.ledCount);
		}
	}
}

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	CLog::Instance()->Destroy();
}
//---------------------------------------------------------------------------

void TfrmMain::UpdateLogConfig(void)
{
	CLog *log = CLog::Instance();
	if (appSettings.logging.logToFile)
		log->SetFile(ChangeFileExt(Application->ExeName, ".log").c_str());
	else
		log->SetFile("");
	log->SetFlush(appSettings.logging.flush);
	log->SetMaxFileSize(appSettings.logging.maxFileSize);
	log->SetTimestamps(appSettings.logging.timestamps);
	log->SetLogRotateCnt(appSettings.logging.logRotate);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;

	// runs regardless of window visibility, so "start minimized" still works
	frmLog->SetLogLinesLimit(appSettings.logging.maxUiLogLines);
	CLog::Instance()->callbackLog = frmLog->OnLog;

	if (appSettings.frmMain.activeTabIndex >= 0 &&
		appSettings.frmMain.activeTabIndex < pages->PageCount)
	{
		pages->ActivePageIndex = appSettings.frmMain.activeTabIndex;
	}

	if (appSettings.serialPort.openAtStartup)
	{
		SerialOpen();
	}

	if (appSettings.audioVisualisation.autoStart)
	{
		frmWS2812AudioVisualisation->StartCapture();
	}

	UpdateLedCount();

	if (appSettings.logging.showWindowAtStartup)
		frmLog->Show();

	LOG("Application started\n");
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::actShowLogExecute(TObject *Sender)
{
	if (!frmLog->Visible)
		frmLog->Show();
	frmLog->BringToFront();
}
//---------------------------------------------------------------------------

void TfrmMain::ToggleVisibility(void)
{
	Visible = !Visible;
	if (Visible)
	{
		Application->Restore();
		ShowWindow(Application->Handle, SW_SHOW);	// show taskbar button
		SetActiveWindow(Handle);
		SetForegroundWindow(Handle);
		SetWindowPos(Handle, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		OnRestore(NULL);
	}
	else
	{
		ShowWindow(Application->Handle, SW_HIDE);	// hide taskbar button
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::OnTrayIconLeftBtnDown(TObject *Sender)
{
	ToggleVisibility();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miMinimizeTrayClick(TObject *Sender)
{
	Visible = false;
	ShowWindow(Application->Handle, SW_HIDE);	// hide taskbar button
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miTrayShowClick(TObject *Sender)
{
	if (!Visible)
		ToggleVisibility();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::miTrayExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------

#ifdef ACCEPT_WM_DROPFILES
// fires an event when a file, or files are dropped onto the application.
void __fastcall TfrmMain::WMDropFiles(TWMDropFiles &message)
{
    AnsiString FileName;
    FileName.SetLength(MAX_PATH);

	int Count = DragQueryFile((HDROP)message.Drop, 0xFFFFFFFF, NULL, MAX_PATH);

	// ignore all files but first one
	if (Count > 1)
		Count = 1;

	// index through the files and query the OS for each file name...
	for (int index = 0; index < Count; ++index)
	{
		// the following code gets the FileName of the dropped file. it
		// looks cryptic but that's only because it is. Hey, Why do you think
		// Delphi and C++Builder are so popular anyway? Look up DragQueryFile
		// the Win32.hlp Windows API help file.
		FileName.SetLength(DragQueryFile((HDROP)message.Drop, index,FileName.c_str(), MAX_PATH));
//		appSettings.Editor.asDefaultDir = ExtractFileDir(FileName);
		AppOpenFile(FileName);
	}

	// tell the OS that you're finished...
	DragFinish((HDROP) message.Drop);
}
#endif

void __fastcall TfrmMain::OnRestore(TObject *Sender)
{
	bool monitorFound = false;
	for (int i=0; i<Screen->MonitorCount; i++)
	{
		TMonitor *monitor = Screen->Monitors[i];
		enum { MARGIN = 30 };
		if (
			(Left + Width + MARGIN >= monitor->Left) &&
			(Left - MARGIN <= monitor->Left + monitor->Width) &&
			(Top + 5 >= monitor->Top) &&
			(Top - MARGIN <= monitor->Top + monitor->Height)
			)
		{
			monitorFound = true;
			break;
		}
	}
	if (!monitorFound)
	{
		if (Screen->MonitorCount > 0)
		{
			LOG("Moving main window to first monitor\n");
			TMonitor *monitor = Screen->Monitors[0];
			Left = monitor->Left + 50;
			Top = monitor->Top + 50;
			if (monitor)
			{
				LOG("Moving main window to first monitor\n");
				Left = monitor->Left + 50;
				Top = monitor->Top + 50;
			}
		}
	}
}

void __fastcall TfrmMain::tmrSerialReinitTimer(TObject *Sender)
{
	if (!serialOpenCalled)
		return;
	if (appSettings.serialPort.autoReinit)
	{
		if (!comPort.isOpened())
			SerialOpen();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnSerialOpenClick(TObject *Sender)
{
	SerialOpen();
}
//---------------------------------------------------------------------------

int TfrmMain::SerialOpen(void)
{
	serialOpenCalled = true;

	const AnsiString namePrefix = "\\\\.\\";
	AnsiString name = appSettings.serialPort.name;
	if (name.Pos(namePrefix) != 1)
	{
		name = namePrefix + name;
	}

	int status = comPort.Open(name.c_str(), appSettings.serialPort.baud, false);
	AnsiString text;
	if (status == 0)
	{
		text.sprintf("%s opened, %u bps", appSettings.serialPort.name.c_str(), appSettings.serialPort.baud);
		lblSerialPortState->Font->Color = clWindowText;
	}
	else
	{
		text.sprintf("Failed to open %s", appSettings.serialPort.name.c_str());
		lblSerialPortState->Font->Color = clRed;
	}
	lblSerialPortState->Caption = text;
	return status;
}
