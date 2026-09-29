//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormWS2812SpectrumAnalyzer.h"
#include "TabManager.h"
#include "Settings.h"
#include "AppStatus.h"
#include "ComPort.h"
#include <math.h>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWS2812SpectrumAnalyzer *frmWS2812SpectrumAnalyzer;

namespace
{

/** \brief FFT length; 2048 @ 48 kHz = ~23 Hz bin width, ~43 ms window */
enum { FFT_SIZE = 2048 };
const double PI = 3.14159265358979323846;

/** \brief In-place iterative radix-2 complex FFT; size must be a power of 2 */
void Fft(std::vector<float> &re, std::vector<float> &im)
{
	const unsigned int n = re.size();

	// bit-reversal permutation
	for (unsigned int i = 1, j = 0; i < n; i++)
	{
		unsigned int bit = n >> 1;
		for (; j & bit; bit >>= 1)
			j ^= bit;
		j ^= bit;
		if (i < j)
		{
			float tmp = re[i]; re[i] = re[j]; re[j] = tmp;
			tmp = im[i]; im[i] = im[j]; im[j] = tmp;
		}
	}

	for (unsigned int len = 2; len <= n; len <<= 1)
	{
		const double angle = -2.0 * PI / len;
		const double wStepRe = cos(angle);
		const double wStepIm = sin(angle);
		const unsigned int half = len / 2;
		for (unsigned int i = 0; i < n; i += len)
		{
			double wRe = 1.0, wIm = 0.0;
			for (unsigned int k = 0; k < half; k++)
			{
				const unsigned int a = i + k;
				const unsigned int b = a + half;
				const double vRe = re[b] * wRe - im[b] * wIm;
				const double vIm = re[b] * wIm + im[b] * wRe;
				re[b] = static_cast<float>(re[a] - vRe);
				im[b] = static_cast<float>(im[a] - vIm);
				re[a] = static_cast<float>(re[a] + vRe);
				im[a] = static_cast<float>(im[a] + vIm);
				const double nextRe = wRe * wStepRe - wIm * wStepIm;
				wIm = wRe * wStepIm + wIm * wStepRe;
				wRe = nextRe;
			}
		}
	}
}

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
__fastcall TfrmWS2812SpectrumAnalyzer::TfrmWS2812SpectrumAnalyzer(TComponent* Owner)
	: FormWS2812(Owner),
	samples(FFT_SIZE),
	imag(FFT_SIZE),
	window(FFT_SIZE)
{
	TabManager::Instance().Register(this);
	for (unsigned int i = 0; i < FFT_SIZE; i++)
		window[i] = static_cast<float>(0.5 - 0.5 * cos(2.0 * PI * i / (FFT_SIZE - 1)));
	trbarGain->Min = SpectrumAnalyzerConf::MIN_GAIN_X10;
	trbarGain->Max = SpectrumAnalyzerConf::MAX_GAIN_X10;
	cbSource->ItemIndex = appSettings.spectrumAnalyzer.audioInput ? 1 : 0;
	UpdateUi();
}
//---------------------------------------------------------------------------

void TfrmWS2812SpectrumAnalyzer::RefreshDeviceList(void)
{
	std::vector<WasapiDeviceInfo> found;
	if (appSettings.spectrumAnalyzer.audioInput)
		EnumerateCaptureDevices(found);
	else
		EnumerateRenderDevices(found);

	std::string currentId = appSettings.spectrumAnalyzer.audioDeviceId.c_str();
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

void TfrmWS2812SpectrumAnalyzer::UpdateUi(void)
{
	trbarGain->Position = static_cast<int>(appSettings.spectrumAnalyzer.gain * 10.0f + 0.5f);
	UpdateGainLabel();
	tmrWrite->Interval = appSettings.spectrumAnalyzer.updateIntervalMs;
}

void TfrmWS2812SpectrumAnalyzer::UpdateGainLabel(void)
{
	AnsiString text;
	text.sprintf("%.1fx", trbarGain->Position / 10.0f);
	lblGainValue->Caption = text;
}
//---------------------------------------------------------------------------

void TfrmWS2812SpectrumAnalyzer::SetLedCount(unsigned int ledCount)
{
	if (this->ledCount == ledCount)
		return;
	this->ledCount = ledCount;
}

void TfrmWS2812SpectrumAnalyzer::ResetBands(void)
{
	smoothed.clear();
	peaks.clear();
	peakHolds.clear();
}

void TfrmWS2812SpectrumAnalyzer::UpdatePeak(float level, float &peak, int &holdCounter)
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

void TfrmWS2812SpectrumAnalyzer::ComputeBands(std::vector<float> &bandLevels)
{
	const unsigned int bandCount = bandLevels.size();
	for (unsigned int b = 0; b < bandCount; b++)
		bandLevels[b] = 0.0f;

	unsigned int sampleRate = 0;
	unsigned int got = capture.GetLatestSamples(&samples[0], FFT_SIZE, sampleRate);
	if (got == 0 || sampleRate == 0)
		return;

	// not enough history yet (just started): right-align and zero-pad the start
	if (got < FFT_SIZE)
	{
		unsigned int pad = FFT_SIZE - got;
		for (int i = got - 1; i >= 0; i--)
			samples[i + pad] = samples[i];
		for (unsigned int i = 0; i < pad; i++)
			samples[i] = 0.0f;
	}

	for (unsigned int i = 0; i < FFT_SIZE; i++)
	{
		samples[i] *= window[i];
		imag[i] = 0.0f;
	}

	Fft(samples, imag);

	// Hann window coherent gain is 0.5, so a full-scale sine peaks at
	// |X| = N/4; scale so that maps to amplitude 1.0
	const float scale = 4.0f / FFT_SIZE;
	const double binHz = static_cast<double>(sampleRate) / FFT_SIZE;
	const unsigned int maxBin = FFT_SIZE / 2 - 1;

	double minHz = appSettings.spectrumAnalyzer.minFreqHz;
	double maxHz = appSettings.spectrumAnalyzer.maxFreqHz;
	if (maxHz > sampleRate / 2.0)
		maxHz = sampleRate / 2.0;
	if (minHz >= maxHz)
		minHz = maxHz / 2.0;
	const double ratio = maxHz / minHz;

	for (unsigned int b = 0; b < bandCount; b++)
	{
		// log-spaced band edges
		double loHz = minHz * pow(ratio, static_cast<double>(b) / bandCount);
		double hiHz = minHz * pow(ratio, static_cast<double>(b + 1) / bandCount);
		unsigned int loBin = static_cast<unsigned int>(loHz / binHz + 0.5);
		unsigned int hiBin = static_cast<unsigned int>(hiHz / binHz + 0.5);
		if (loBin < 1)
			loBin = 1;	// skip DC
		if (hiBin <= loBin)
			hiBin = loBin + 1;	// narrow low bands still get one bin
		if (hiBin > maxBin + 1)
			hiBin = maxBin + 1;
		if (loBin > maxBin)
			loBin = maxBin;

		float level = 0.0f;
		for (unsigned int k = loBin; k < hiBin; k++)
		{
			float magnitude = static_cast<float>(sqrt(samples[k] * samples[k] + imag[k] * imag[k])) * scale;
			if (magnitude > level)
				level = magnitude;
		}
		bandLevels[b] = level;
	}
}

void TfrmWS2812SpectrumAnalyzer::RenderBar(unsigned int offset, unsigned int count, float level, float peak, bool reversed)
{
	if (count == 0)
		return;

	bool peakDetect = appSettings.spectrumAnalyzer.peakDetect;

	// with peak-detect on, the LED furthest from the anchor is reserved for the marker
	unsigned int barCount = (peakDetect && count > 1) ? (count - 1) : count;

	unsigned int lit = static_cast<unsigned int>(level * barCount + 0.5f);
	if (lit > barCount)
		lit = barCount;

	// i is distance from the anchor (0 = lowest level); reversed puts the
	// anchor at the far end of the range instead of at "offset"
	for (unsigned int i = 0; i < count; i++)
	{
		unsigned int index = offset + (reversed ? (count - 1 - i) : i);
		Ws2812Color &color = previewColors[index];
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

	unsigned int peakI = static_cast<unsigned int>(peak * barCount + 0.5f);
	if (peakI >= count)
		peakI = count - 1;
	unsigned int peakIndex = offset + (reversed ? (count - 1 - peakI) : peakI);
	Ws2812Color &peakColor = previewColors[peakIndex];
	peakColor.r = 0;
	peakColor.g = 0;
	peakColor.b = 255;
}

void TfrmWS2812SpectrumAnalyzer::Write(void)
{
	// levels are not used here, but the FIFO must not overflow
	AudioLevel level;
	while (capture.PopLevel(level))
		;

	const SpectrumAnalyzerConf &conf = appSettings.spectrumAnalyzer;

	unsigned int bandLeds = conf.ledsPerBand;
	if (bandLeds > ledCount)
		bandLeds = ledCount;
	unsigned int bandCount = (bandLeds > 0) ? (ledCount / bandLeds) : 0;

	if (smoothed.size() != bandCount)
	{
		smoothed.assign(bandCount, 0.0f);
		peaks.assign(bandCount, 0.0f);
		peakHolds.assign(bandCount, 0);
	}

	std::vector<float> bandLevels(bandCount);
	ComputeBands(bandLevels);

	previewColors.assign(ledCount, Ws2812Color());	// leftover LEDs past the last band stay black

	const unsigned int smoothingPct = conf.smoothingPct;
	const float floorDb = static_cast<float>(conf.meterFloorDb);
	for (unsigned int b = 0; b < bandCount; b++)
	{
		smoothed[b] = (smoothed[b] * smoothingPct + bandLevels[b] * (100 - smoothingPct)) / 100.0f;
		float scaled = smoothed[b] * conf.gain;
		if (scaled > 1.0f)
			scaled = 1.0f;
		float bar = LinearToDbNormalized(scaled, floorDb);
		UpdatePeak(bar, peaks[b], peakHolds[b]);
		bool reversed = conf.alternateDirection && (b % 2 == 1);
		RenderBar(b * bandLeds, bandLeds, bar, peaks[b], reversed);
	}

	// skip the preview redraw when nobody can see it
	if (IsTabVisible(this))
		PaintOnImage();

	if (!comPort.isOpened())
	{
		SetAppStatus("Cannot write: serial port is not opened");
		return;
	}

	// per-channel cap for the real strip only, never applied to previewColors
	float maxBrightness = static_cast<float>(conf.maxBrightness);
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
		SetAppStatus("WS2812 write failed");
	else
		SetAppStatus("");
}
//---------------------------------------------------------------------------

void TfrmWS2812SpectrumAnalyzer::PaintOnImage(void)
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

	unsigned int bandLeds = appSettings.spectrumAnalyzer.ledsPerBand;
	if (bandLeds > ledCount)
		bandLeds = ledCount;
	unsigned int bandCount = (bandLeds > 0) ? (ledCount / bandLeds) : 0;

	if (bandCount == 0 || previewColors.size() != ledCount)
	{
		image->Invalidate();
		return;
	}

	// one vertical column per band, lowest level at the bottom; bands with
	// alternateDirection reversed on the strip are flipped back here so
	// every column grows upwards
	const double colWidth = static_cast<double>(width) / bandCount;
	const double rowHeight = static_cast<double>(height) / bandLeds;
	const int gap = (colWidth >= 4 && rowHeight >= 4) ? 1 : 0;

	for (unsigned int b = 0; b < bandCount; b++)
	{
		bool reversed = appSettings.spectrumAnalyzer.alternateDirection && (b % 2 == 1);
		int x0 = static_cast<int>(b * colWidth);
		int x1 = static_cast<int>((b + 1) * colWidth) - gap;
		if (x1 <= x0)
			x1 = x0 + 1;

		for (unsigned int i = 0; i < bandLeds; i++)
		{
			unsigned int index = b * bandLeds + (reversed ? (bandLeds - 1 - i) : i);
			const Ws2812Color &color = previewColors[index];

			int y1 = height - static_cast<int>(i * rowHeight);
			int y0 = height - static_cast<int>((i + 1) * rowHeight) + gap;
			if (y1 <= y0)
				y1 = y0 + 1;

			// unlit LEDs drawn dark gray so the grid stays visible
			if (color.r == 0 && color.g == 0 && color.b == 0)
				canvas->Brush->Color = static_cast<TColor>(RGB(24, 24, 24));
			else
				canvas->Brush->Color = static_cast<TColor>(RGB(color.r, color.g, color.b));
			canvas->FillRect(Rect(x0, y0, x1, y1));
		}
	}

	image->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::btnStartClick(TObject *Sender)
{
	StartCapture();
}
//---------------------------------------------------------------------------

void TfrmWS2812SpectrumAnalyzer::StartCapture(void)
{
	AnsiString deviceId = appSettings.spectrumAnalyzer.audioDeviceId;
	if (!capture.Start(deviceId.IsEmpty() ? NULL : deviceId.c_str(), appSettings.spectrumAnalyzer.audioInput))
	{
		SetAppStatus("Failed to start audio capture");
		return;
	}
	ResetBands();
	tmrWrite->Enabled = true;
	btnStart->Enabled = false;
	btnStop->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::btnStopClick(TObject *Sender)
{
	tmrWrite->Enabled = false;
	capture.Stop();
	btnStop->Enabled = false;
	btnStart->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::tmrWriteTimer(TObject *Sender)
{
	Write();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::trbarGainChange(TObject *Sender)
{
	appSettings.spectrumAnalyzer.gain = trbarGain->Position / 10.0f;
	UpdateGainLabel();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::cbAudioDeviceChange(TObject *Sender)
{
	int index = cbAudioDevice->ItemIndex;
	if (index >= 0 && index < static_cast<int>(deviceIds.size()))
		appSettings.spectrumAnalyzer.audioDeviceId = deviceIds[index].c_str();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::btnRefreshDevicesClick(TObject *Sender)
{
	RefreshDeviceList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::cbSourceChange(TObject *Sender)
{
	appSettings.spectrumAnalyzer.audioInput = (cbSource->ItemIndex == 1);
	// previously picked device id belongs to the other list
	appSettings.spectrumAnalyzer.audioDeviceId = "";
	RefreshDeviceList();
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812SpectrumAnalyzer::tmrStartupTimer(TObject *Sender)
{
	tmrStartup->Enabled = false;
	RefreshDeviceList();
}
//---------------------------------------------------------------------------
