//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormWS2812OneForAll.h"
#include "TabManager.h"
#include "Settings.h"
#include "BtnController.h"
#include "AppStatus.h"
#include "ComPort.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWS2812OneForAll *frmWS2812OneForAll;

//---------------------------------------------------------------------------
__fastcall TfrmWS2812OneForAll::TfrmWS2812OneForAll(TComponent* Owner)
	: FormWS2812(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void TfrmWS2812OneForAll::SetLedCount(unsigned int ledCount)
{
	if (this->ledCount == ledCount)
		return;
	this->ledCount = ledCount;
}


void __fastcall TfrmWS2812OneForAll::btnWriteClick(TObject *Sender)
{
	Write();
}

void TfrmWS2812OneForAll::Write(void)
{
	BtnController btnCtrl(btnWrite);

	if (!comPort.isOpened())
	{
		SetAppStatus("Cannot write: serial port is not opened");
		return;
	}

	Ws2812Color color;
	color.r = static_cast<uint8_t>(trbarRed->Position);
	color.g = static_cast<uint8_t>(trbarGreen->Position);
	color.b = static_cast<uint8_t>(trbarBlue->Position);

	std::vector<Ws2812Color> colors;
	colors.resize(ledCount);
	for (unsigned int i=0; i<ledCount; i++)
	{
		colors[i] = color;
	}

	int status = Ws2812Write(colors);
	if (status != 0)
	{
		SetAppStatus("WS2812 write failed");
	}
	else
	{
    	SetAppStatus("");
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812OneForAll::tmrWriteTimer(TObject *Sender)
{
	if (chbWritePeriodically->Checked)
		Write();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812OneForAll::trbarChange(TObject *Sender)
{
	if (chbApplyImmediately->Checked)
		Write();	
}
//---------------------------------------------------------------------------

