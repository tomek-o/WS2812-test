//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormSettings.h"
#include "ComPort.h"
#include "Log.h"
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
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::FormShow(TObject *Sender)
{
    assert(appSettings);
	tmpSettings = *appSettings;
	chbAlwaysOnTop->Checked = tmpSettings.frmMain.alwaysOnTop;

	chbLogToFile->Checked = tmpSettings.logging.logToFile;
	cmbMaxUiLogLines->ItemIndex = -1;
	for (int i=0; i<cmbMaxUiLogLines->Items->Count; i++)
	{
		if ((unsigned int)StrToInt(cmbMaxUiLogLines->Items->Strings[i]) >= tmpSettings.logging.maxUiLogLines)
		{
			cmbMaxUiLogLines->ItemIndex = i;
			break;
		}
	}
	if (cmbMaxUiLogLines->ItemIndex == -1)
	{
		cmbMaxUiLogLines->ItemHeight = cmbMaxUiLogLines->Items->Count - 1;
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
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	this->Close();	
}
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::btnApplyClick(TObject *Sender)
{
	tmpSettings.logging.logToFile = chbLogToFile->Checked;

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

	*appSettings = tmpSettings;
	this->Close();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::chbAlwaysOnTopClick(TObject *Sender)
{
	tmpSettings.frmMain.alwaysOnTop = chbAlwaysOnTop->Checked;	
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::cmbMaxUiLogLinesChange(TObject *Sender)
{
	tmpSettings.logging.maxUiLogLines = StrToInt(cmbMaxUiLogLines->Text);	
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

