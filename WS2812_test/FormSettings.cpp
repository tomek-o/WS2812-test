//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "ComPort.h"
#include "Log.h"
#include "FrameLogConf.h"
#include "WS2812\FrameAudioVisualisationConf.h"
#include "common\Autostart.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	this->appSettings = NULL;
	pages->ActivePage = tsGeneral;

	fraLogConf = new TfraLogConf(tsLogging, tmpSettings.logging);
	fraLogConf->Parent = tsLogging;
	fraLogConf->Visible = true;

	fraAudioVisualisationConf = new TfraAudioVisualisationConf(tsAudioVisualisation, tmpSettings.audioVisualisation);
	fraAudioVisualisationConf->Parent = tsAudioVisualisation;
	fraAudioVisualisationConf->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.alwaysOnTop;
	chbStartMinimizedToTray->Checked = tmpSettings.frmMain.startMinimizedToTray;
	chbAutostart->Checked = tmpSettings.frmMain.autostart;
	fraLogConf->Load();
	fraAudioVisualisationConf->Load();

	{
		// registration can go stale or be changed externally - offer to fix it up
		AnsiString autostartName, autostartCommand;
		GetAutostartIdentity(autostartName, autostartCommand);
		bool actuallyEnabled = Autostart::IsEnabled(autostartName, autostartCommand);
		if (actuallyEnabled != tmpSettings.frmMain.autostart)
		{
			AnsiString msg;
			msg.sprintf(
				"Actual autostart registration (%s) does not match the current setting (%s).\n\n"
				"Update the registration now to match the setting?",
				actuallyEnabled ? "enabled" : "disabled",
				tmpSettings.frmMain.autostart ? "enabled" : "disabled");
			if (MessageBox(this->Handle, msg.c_str(), this->Caption.c_str(), MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				if (tmpSettings.frmMain.autostart)
					Autostart::Enable(autostartName, autostartCommand);
				else
					Autostart::Disable(autostartName);
			}
		}
	}

	{
		// serial ports list
		std::vector<struct ComPort::S_COM_PORT> ports;
		if(ComPort::EnumeratePortsAlt()) {
			LOG("Error enumerating ports");
		}
		ports = ComPort::GetPorts();
		cbSerialPorts->Items->Clear();
		for(unsigned int i=0; i<ports.size(); i++) {
			cbSerialPorts->Items->Add(ports[i].name.c_str());
		}
		if(ports.size()) {
			cbSerialPorts->ItemIndex = 0;
		} else {
			cbSerialPorts->ItemIndex = -1;
		}
	}

	cbSerialPorts->Text = tmpSettings.serialPort.name;
	//edSerialPortName->Text = tmpSettings.serialPort.name;
	edSerialPortBaudrate->Text = tmpSettings.serialPort.baud;
	chbSerialPortOpenAtStartup->Checked = tmpSettings.serialPort.openAtStartup;
	chbSerialPortAutoReinit->Checked = tmpSettings.serialPort.autoReinit;

	cbLedCount->Text = tmpSettings.ws2812.ledCount;
	chbTurnOffOnClose->Checked = tmpSettings.ws2812.turnOffOnClose;
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	fraLogConf->Apply();
	fraAudioVisualisationConf->Apply();

	tmpSettings.frmMain.startMinimizedToTray = chbStartMinimizedToTray->Checked;

	tmpSettings.frmMain.autostart = chbAutostart->Checked;
	{
		AnsiString autostartName, autostartCommand;
		GetAutostartIdentity(autostartName, autostartCommand);
		if (tmpSettings.frmMain.autostart)
			Autostart::Enable(autostartName, autostartCommand);
		else
			Autostart::Disable(autostartName);
	}

	tmpSettings.serialPort.name = cbSerialPorts->Text;
	tmpSettings.serialPort.baud = StrToIntDef(edSerialPortBaudrate->Text, tmpSettings.serialPort.baud);
	tmpSettings.serialPort.openAtStartup = chbSerialPortOpenAtStartup->Checked;
	tmpSettings.serialPort.autoReinit = chbSerialPortAutoReinit->Checked;

	int ledCount = StrToIntDef(cbLedCount->Text, -1);
	if (ledCount >= 1 && ledCount <= Settings::WS2812::MAX_LED_COUNT)
	{
		tmpSettings.ws2812.ledCount = ledCount;
	}
	tmpSettings.ws2812.manualControl.resize(tmpSettings.ws2812.ledCount);
	tmpSettings.ws2812.turnOffOnClose = chbTurnOffOnClose->Checked;

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.alwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void TfrmSettings::GetAutostartIdentity(AnsiString &name, AnsiString &command)
{
	name = ExtractFileName(ChangeFileExt(Application->ExeName, ""));
	command = "\"" + Application->ExeName + "\"";
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::FormKeyPress(TObject *Sender, char &Key)
{
	if (Key == VK_ESCAPE)
	{
		Close();
	}
}
//---------------------------------------------------------------------------

