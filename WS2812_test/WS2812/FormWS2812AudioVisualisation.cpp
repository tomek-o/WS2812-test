//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormWS2812AudioVisualisation.h"
#include "TabManager.h"
#include "Settings.h"
#include "AppStatus.h"
#include "ComPort.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWS2812AudioVisualisation *frmWS2812AudioVisualisation;

namespace
{

/** \brief Map linear amplitude (0..1) to a dB-normalized bar position (0..1)
	\param floorDb level mapped to 0; 0 dB (full scale) maps to 1
*/
float LinearToDbNormalized(float level, float floorDb)
{
	if (level <= 0.0f)
		return 0.0f;

	float db = static_cast<float>(20.0 * log10(level));
	if (db > 0.0f)
		db = 0.0f;
	if (db < floorDb)
		return 0.0f;

	return (db - floorDb) / -floorDb;
}

}	// namespace

//---------------------------------------------------------------------------
__fastcall TfrmWS2812AudioVisualisation::TfrmWS2812AudioVisualisation(TComponent* Owner)
	: FormWS2812(Owner),
	smoothedLeft(0.0f),
	smoothedRight(0.0f),
	peakLeft(0.0f),
	peakRight(0.0f),
	peakHoldLeft(0),
	peakHoldRight(0)
{
	TabManager::Instance().Register(this);
	trbarGain->Min = AudioVisualisationConf::MIN_GAIN_X10;
	trbarGain->Max = AudioVisualisationConf::MAX_GAIN_X10;
	cbSource->ItemIndex = appSettings.audioVisualisation.audioInput ? 1 : 0;
	UpdateUi();
	RefreshDeviceList();
}
//---------------------------------------------------------------------------

void TfrmWS2812AudioVisualisation::RefreshDeviceList(void)
{
	std::vector<WasapiDeviceInfo> found;
	if (appSettings.audioVisualisation.audioInput)
		EnumerateCaptureDevices(found);
	else
		EnumerateRenderDevices(found);

	std::string currentId = appSettings.audioVisualisation.audioDeviceId.c_str();
	int selectIndex = 0;

	cbAudioDevice->Items->BeginUpdate();
	cbAudioDevice->Items->Clear();
	deviceIds.clear();

	cbAudioDevice->Items->Add("Default");
	deviceIds.push_back("");

	for (unsigned int i = 0; i < found.size(); i++)
	{
		cbAudioDevice->Items->Add(found[i].name.c_str());
		deviceIds.push_back(found[i].id);
		if (found[i].id == currentId)
			selectIndex = static_cast<int>(deviceIds.size()) - 1;
	}

	cbAudioDevice->Items->EndUpdate();
	cbAudioDevice->ItemIndex = selectIndex;	// does not fire OnChange
}
//---------------------------------------------------------------------------

void TfrmWS2812AudioVisualisation::UpdateUi(void)
{
	trbarGain->Position = static_cast<int>(appSettings.audioVisualisation.gain * 10.0f + 0.5f);
	UpdateGainLabel();
	tmrWrite->Interval = appSettings.audioVisualisation.updateIntervalMs;
}

void TfrmWS2812AudioVisualisation::UpdateGainLabel(void)
{
	AnsiString text;
	text.sprintf("%.1fx", trbarGain->Position / 10.0f);
	lblGainValue->Caption = text;
}
//---------------------------------------------------------------------------

void TfrmWS2812AudioVisualisation::SetLedCount(unsigned int ledCount)
{
	if (this->ledCount == ledCount)
		return;
	this->ledCount = ledCount;
}

void TfrmWS2812AudioVisualisation::UpdatePeak(float level, float &peak, int &holdCounter)
{
	enum { HOLD_FRAMES = 20 };	// ~hold time before decaying, in frames
	const float DECAY_PER_FRAME = 0.03f;

	if (level >= peak)
	{
		peak = level;
		holdCounter = HOLD_FRAMES;
	}
	else if (holdCounter > 0)
	{
		holdCounter--;
	}
	else
	{
		peak -= DECAY_PER_FRAME;
		if (peak < 0.0f)
			peak = 0.0f;
	}
}

void TfrmWS2812AudioVisualisation::RenderVuBar(unsigned int offset, unsigned int count, float level, float peak)
{
	if (count == 0)
		return;

	bool peakDetect = appSettings.audioVisualisation.peakDetect;

	// with peak-detect on, the top LED is reserved for the marker so it
	// never just overwrites the bar's own leading LED at full level
	unsigned int barCount = (peakDetect && count > 1) ? (count - 1) : count;

	unsigned int lit = static_cast<unsigned int>(level * barCount + 0.5f);
	if (lit > barCount)
		lit = barCount;

	// full brightness here; maxBrightness is applied later, only for the real strip
	for (unsigned int i = 0; i < count; i++)
	{
		Ws2812Color &color = previewColors[offset + i];
		if (i < lit)
		{
			float pos = (count > 1) ? (static_cast<float>(i) / (count - 1)) : 0.0f;
			color.r = static_cast<uint8_t>(pos * 255.0f);
			color.g = static_cast<uint8_t>((1.0f - pos) * 255.0f);
			color.b = 0;
		}
		else
		{
			color.r = 0;
			color.g = 0;
			color.b = 0;
		}
	}

	if (!peakDetect)
		return;

	// drawn last so it stays visible above the bar (barCount-normalized,
	// same scale as the bar, so it lands on the reserved top LED at full level)
	unsigned int peakIndex = static_cast<unsigned int>(peak * barCount + 0.5f);
	if (peakIndex >= count)
		peakIndex = count - 1;
	Ws2812Color &peakColor = previewColors[offset + peakIndex];
	peakColor.r = 0;
	peakColor.g = 0;
	peakColor.b = 255;
}

void TfrmWS2812AudioVisualisation::Write(void)
{
	AudioLevel level;
	bool gotLevel = false;
	while (capture.PopLevel(level))
		gotLevel = true;
	if (!gotLevel)
	{
		level.left = smoothedLeft;
		level.right = smoothedRight;
	}

	unsigned int smoothingPct = appSettings.audioVisualisation.smoothingPct;
	smoothedLeft = (smoothedLeft * smoothingPct + level.left * (100 - smoothingPct)) / 100.0f;
	smoothedRight = (smoothedRight * smoothingPct + level.right * (100 - smoothingPct)) / 100.0f;

	float gain = appSettings.audioVisualisation.gain;
	float scaledLeft = smoothedLeft * gain;
	float scaledRight = smoothedRight * gain;
	if (scaledLeft > 1.0f)
		scaledLeft = 1.0f;
	else if (scaledLeft < 0.0f)
		scaledLeft = 0.0f;
	if (scaledRight > 1.0f)
		scaledRight = 1.0f;
	else if (scaledRight < 0.0f)
		scaledRight = 0.0f;

	float floorDb = static_cast<float>(appSettings.audioVisualisation.meterFloorDb);
	float barLeft = LinearToDbNormalized(scaledLeft, floorDb);
	float barRight = LinearToDbNormalized(scaledRight, floorDb);

	UpdatePeak(barLeft, peakLeft, peakHoldLeft);
	UpdatePeak(barRight, peakRight, peakHoldRight);

	previewColors.resize(ledCount);

	if (appSettings.audioVisualisation.stereoSeparate)
	{
		unsigned int half = ledCount / 2;
		RenderVuBar(0, half, barLeft, peakLeft);
		RenderVuBar(half, ledCount - half, barRight, peakRight);
	}
	else
	{
		RenderVuBar(0, ledCount, (barLeft + barRight) * 0.5f, (peakLeft + peakRight) * 0.5f);
	}

	// update the on-screen preview regardless of hardware being attached
	//pbxPreview->Invalidate();
	PaintOnImage();

	if (!comPort.isOpened())
	{
		SetAppStatus("Cannot write: serial port is not opened");
		return;
	}

	// per-channel cap for the real strip only, never applied to previewColors
	float maxBrightness = static_cast<float>(appSettings.audioVisualisation.maxBrightness);
	std::vector<Ws2812Color> ledColors(previewColors.size());
	for (unsigned int i = 0; i < previewColors.size(); i++)
	{
		const Ws2812Color &src = previewColors[i];
		Ws2812Color &dst = ledColors[i];
		dst.r = static_cast<uint8_t>(src.r * maxBrightness / 255.0f);
		dst.g = static_cast<uint8_t>(src.g * maxBrightness / 255.0f);
		dst.b = static_cast<uint8_t>(src.b * maxBrightness / 255.0f);
	}

	int status = Ws2812Write(ledColors);
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

void TfrmWS2812AudioVisualisation::PaintOnImage(void)
{
	Graphics::TBitmap *bmp = image->Picture->Bitmap;

	bmp->PixelFormat = pf24bit;
	bmp->Width = image->ClientWidth;
	bmp->Height = image->ClientHeight;

	TCanvas *canvas = bmp->Canvas;

	int width = bmp->Width;
	int height = bmp->Height;

	canvas->Brush->Style = bsSolid;
	canvas->Brush->Color = clBlack;
	canvas->FillRect(Rect(0, 0, width, height));

	if (ledCount == 0 || previewColors.size() != ledCount)
	{
		image->Invalidate();
		return;
	}

	int compositeHeight = height / 2;
	int channelHeight = (height - compositeHeight) / 3;

	double cellWidth = static_cast<double>(width) / ledCount;

	for (unsigned int i = 0; i < ledCount; ++i)
	{
		int x0 = static_cast<int>(i * cellWidth);
		int x1 = static_cast<int>((i + 1) * cellWidth);

		if (x1 <= x0)
			x1 = x0 + 1;

		const Ws2812Color &color = previewColors[i];

		TRect cell;

		// Composite
		cell.Left = x0;
		cell.Right = x1;
		cell.Top = 0;
		cell.Bottom = compositeHeight;
		canvas->Brush->Color = static_cast<TColor>(RGB(color.r, color.g, color.b));
		canvas->FillRect(cell);

		// R
		cell.Top = compositeHeight;
		cell.Bottom = compositeHeight + channelHeight;
		canvas->Brush->Color = static_cast<TColor>(RGB(color.r, 0, 0));
		canvas->FillRect(cell);

		// G
		cell.Top = cell.Bottom;
		cell.Bottom = cell.Top + channelHeight;
		canvas->Brush->Color = static_cast<TColor>(RGB(0, color.g, 0));
		canvas->FillRect(cell);

		// B
		cell.Top = cell.Bottom;
		cell.Bottom = height;
		canvas->Brush->Color = static_cast<TColor>(RGB(0, 0, color.b));
		canvas->FillRect(cell);
	}

	// Text
	canvas->Brush->Style = bsClear;
	canvas->Font->Color = clWhite;

	canvas->TextOut(2, 1, "preview");
	canvas->TextOut(2, compositeHeight + 1, "R");
	canvas->TextOut(2, compositeHeight + channelHeight + 1, "G");
	canvas->TextOut(2, compositeHeight + channelHeight * 2 + 1, "B");

	canvas->Brush->Style = bsSolid;

	// Tell TImage to repaint.
	image->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::btnStartClick(TObject *Sender)
{
	StartCapture();
}
//---------------------------------------------------------------------------

void TfrmWS2812AudioVisualisation::StartCapture(void)
{
	AnsiString deviceId = appSettings.audioVisualisation.audioDeviceId;
	if (!capture.Start(deviceId.IsEmpty() ? NULL : deviceId.c_str(), appSettings.audioVisualisation.audioInput))
	{
		SetAppStatus("Failed to start audio capture");
		return;
	}
	smoothedLeft = 0.0f;
	smoothedRight = 0.0f;
	peakLeft = 0.0f;
	peakRight = 0.0f;
	peakHoldLeft = 0;
	peakHoldRight = 0;
	tmrWrite->Enabled = true;
	btnStart->Enabled = false;
	btnStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::btnStopClick(TObject *Sender)
{
	tmrWrite->Enabled = false;
	capture.Stop();
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::tmrWriteTimer(TObject *Sender)
{
	Write();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::trbarGainChange(TObject *Sender)
{
	appSettings.audioVisualisation.gain = trbarGain->Position / 10.0f;
	UpdateGainLabel();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::cbAudioDeviceChange(TObject *Sender)
{
	int index = cbAudioDevice->ItemIndex;
	if (index >= 0 && index < static_cast<int>(deviceIds.size()))
		appSettings.audioVisualisation.audioDeviceId = deviceIds[index].c_str();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::btnRefreshDevicesClick(TObject *Sender)
{
	RefreshDeviceList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812AudioVisualisation::cbSourceChange(TObject *Sender)
{
	appSettings.audioVisualisation.audioInput = (cbSource->ItemIndex == 1);
	// switching source invalidates any previously picked device id from the
	// other list; RefreshDeviceList() re-selects "Default" since it won't match
	appSettings.audioVisualisation.audioDeviceId = "";
	RefreshDeviceList();
}
//---------------------------------------------------------------------------
