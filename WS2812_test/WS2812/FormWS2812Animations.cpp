//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormWS2812Animations.h"
#include "TabManager.h"
#include "Settings.h"
#include "BtnController.h"
#include "AppStatus.h"
#include "ComPort.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWS2812Animations *frmWS2812Animations;


namespace
{

void StepMovingPixel(std::vector<Ws2812Color> &colors, unsigned int &interval)
{
	interval = 500;

	static unsigned int pos = 0;
	static char lastColor = 'b';

	unsigned int ledCount = colors.size();
	if (ledCount == 0)
		return;
	if (pos >= ledCount)
		pos = 0;

	if (pos == 0)
	{
		if (lastColor == 'b')
			lastColor = 'r';
		else if (lastColor == 'r')
			lastColor = 'g';
		else
			lastColor = 'b';
	}

	for (unsigned int i=0; i<ledCount; i++)
	{
		Ws2812Color &color = colors[i];
		color.r = 0;
		color.g = 0;
		color.b = 0;
	}

	Ws2812Color &pixel = colors[pos];
	if (lastColor == 'b')
		pixel.b = 255;
	else if (lastColor == 'r')
		pixel.r = 255;
	else
		pixel.g = 255;

	pos++;
}

void StepAllRandom(std::vector<Ws2812Color> &colors, unsigned int &interval)
{
	interval = 250;
	enum { MAX = 64 };
	for (unsigned int i=0; i<colors.size(); i++)
	{
		Ws2812Color &color = colors[i];
		color.r = static_cast<uint8_t>(rand() % MAX);
		color.g = static_cast<uint8_t>(rand() % MAX);
		color.b = static_cast<uint8_t>(rand() % MAX);
	}
}

void StepAllRandomHalfOff(std::vector<Ws2812Color> &colors, unsigned int &interval)
{
	interval = 250;
	enum { MAX = 64 };
	for (unsigned int i=0; i<colors.size(); i++)
	{
		Ws2812Color &color = colors[i];
		if (rand() % 2)
		{
			color.r = 0;
			color.g = 0;
			color.b = 0;
		}
		else
		{
			color.r = static_cast<uint8_t>(rand() % MAX);
			color.g = static_cast<uint8_t>(rand() % MAX);
			color.b = static_cast<uint8_t>(rand() % MAX);
		}
	}
}

void Step(int type, std::vector<Ws2812Color> &colors, unsigned int &interval)
{
	switch (type)
	{
	case 0:
		StepMovingPixel(colors, interval);
		break;
	case 1:
		StepAllRandom(colors, interval);
		break;
	case 2:
		StepAllRandomHalfOff(colors, interval);
		break;
	default:
		SetAppStatus("Unhandled animation type!");
		break;
	}
}

}	// namespace

//---------------------------------------------------------------------------
__fastcall TfrmWS2812Animations::TfrmWS2812Animations(TComponent* Owner)
	: FormWS2812(Owner)
{
	TabManager::Instance().Register(this);
}
//---------------------------------------------------------------------------

void TfrmWS2812Animations::SetLedCount(unsigned int ledCount)
{
	if (this->ledCount == ledCount)
		return;
	this->ledCount = ledCount;
}

void TfrmWS2812Animations::Write(void)
{
	if (!comPort.isOpened())
	{
		SetAppStatus("Cannot write: serial port is not opened");
		return;
	}

	std::vector<Ws2812Color> colors;
	colors.resize(ledCount);

	unsigned int interval = tmrWrite->Interval;
	Step(cbType->ItemIndex, colors, interval);
	if (tmrWrite->Interval != interval)
	{
		tmrWrite->Enabled = false;
		tmrWrite->Interval = interval;
		tmrWrite->Enabled = true;
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



void __fastcall TfrmWS2812Animations::btnStartClick(TObject *Sender)
{
	tmrWrite->Enabled = true;
	btnStart->Enabled = false;
	btnStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812Animations::btnStopClick(TObject *Sender)
{
	tmrWrite->Enabled = false;
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812Animations::tmrWriteTimer(TObject *Sender)
{
	Write();	
}
//---------------------------------------------------------------------------

