//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameSpectrumAnalyzerConf.h"
#include "SpectrumAnalyzerConf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfraSpectrumAnalyzerConf::TfraSpectrumAnalyzerConf(TComponent* Owner, SpectrumAnalyzerConf &conf)
	: TFrame(Owner),
	conf(conf)
{
	AnsiString caption;
	caption.sprintf("LEDs per band (%d-%d)", SpectrumAnalyzerConf::MIN_LEDS_PER_BAND, SpectrumAnalyzerConf::MAX_LEDS_PER_BAND);
	edLedsPerBand->Hint = caption;
	caption.sprintf("Lowest band start / highest band end (%d-%d Hz)", SpectrumAnalyzerConf::MIN_FREQ_HZ, SpectrumAnalyzerConf::MAX_FREQ_HZ);
	edMinFreq->Hint = caption;
	edMaxFreq->Hint = caption;
	edLedsPerBand->ShowHint = true;
	edMinFreq->ShowHint = true;
	edMaxFreq->ShowHint = true;

	trbarGain->Min = SpectrumAnalyzerConf::MIN_GAIN_X10;
	trbarGain->Max = SpectrumAnalyzerConf::MAX_GAIN_X10;

	trbarMaxBrightness->Min = SpectrumAnalyzerConf::MIN_MAX_BRIGHTNESS;
	trbarMaxBrightness->Max = SpectrumAnalyzerConf::MAX_MAX_BRIGHTNESS;
	caption.sprintf("Max brightness (%d-%d, caps per-channel value to limit current draw)",
		SpectrumAnalyzerConf::MIN_MAX_BRIGHTNESS, SpectrumAnalyzerConf::MAX_MAX_BRIGHTNESS);
	lblMaxBrightness->Caption = caption;

	trbarUpdateInterval->Min = SpectrumAnalyzerConf::MIN_UPDATE_INTERVAL_MS;
	trbarUpdateInterval->Max = SpectrumAnalyzerConf::MAX_UPDATE_INTERVAL_MS;
	caption.sprintf(
		"Update interval (%d-%d ms; actual rate is limited by Windows' timer resolution, ~15 ms)",
		SpectrumAnalyzerConf::MIN_UPDATE_INTERVAL_MS, SpectrumAnalyzerConf::MAX_UPDATE_INTERVAL_MS);
	lblUpdateInterval->Caption = caption;

	trbarMeterFloor->Min = SpectrumAnalyzerConf::MIN_METER_FLOOR_DB;
	trbarMeterFloor->Max = SpectrumAnalyzerConf::MAX_METER_FLOOR_DB;
	caption.sprintf("Meter floor (%d..%d dB; band level mapped to the bottom of the bar)",
		SpectrumAnalyzerConf::MIN_METER_FLOOR_DB, SpectrumAnalyzerConf::MAX_METER_FLOOR_DB);
	lblMeterFloor->Caption = caption;

	trbarSmoothing->Min = SpectrumAnalyzerConf::MIN_SMOOTHING_PCT;
	trbarSmoothing->Max = SpectrumAnalyzerConf::MAX_SMOOTHING_PCT;
	caption.sprintf("Smoothing (%d-%d%%; higher = smoother but slower to rise/fall)",
		SpectrumAnalyzerConf::MIN_SMOOTHING_PCT, SpectrumAnalyzerConf::MAX_SMOOTHING_PCT);
	lblSmoothing->Caption = caption;
}
//---------------------------------------------------------------------------

void TfraSpectrumAnalyzerConf::Load(void)
{
	chbAutoStart->Checked = conf.autoStart;
	chbPeakDetect->Checked = conf.peakDetect;
	chbAlternateDirection->Checked = conf.alternateDirection;
	edLedsPerBand->Text = conf.ledsPerBand;
	edMinFreq->Text = conf.minFreqHz;
	edMaxFreq->Text = conf.maxFreqHz;
	trbarGain->Position = static_cast<int>(conf.gain * 10.0f + 0.5f);
	UpdateGainLabel();
	trbarMaxBrightness->Position = conf.maxBrightness;
	UpdateMaxBrightnessLabel();
	trbarUpdateInterval->Position = conf.updateIntervalMs;
	UpdateIntervalLabel();
	trbarMeterFloor->Position = conf.meterFloorDb;
	UpdateMeterFloorLabel();
	trbarSmoothing->Position = conf.smoothingPct;
	UpdateSmoothingLabel();
}

void __fastcall TfraSpectrumAnalyzerConf::trbarGainChange(TObject *Sender)
{
	UpdateGainLabel();
}

void TfraSpectrumAnalyzerConf::UpdateGainLabel(void)
{
	AnsiString text;
	text.sprintf("%.1fx", trbarGain->Position / 10.0f);
	lblGainValue->Caption = text;
}

void __fastcall TfraSpectrumAnalyzerConf::trbarMaxBrightnessChange(TObject *Sender)
{
	UpdateMaxBrightnessLabel();
}

void TfraSpectrumAnalyzerConf::UpdateMaxBrightnessLabel(void)
{
	AnsiString text;
	text.sprintf("%d", trbarMaxBrightness->Position);
	lblMaxBrightnessValue->Caption = text;
}

void __fastcall TfraSpectrumAnalyzerConf::trbarUpdateIntervalChange(TObject *Sender)
{
	UpdateIntervalLabel();
}

void TfraSpectrumAnalyzerConf::UpdateIntervalLabel(void)
{
	int ms = trbarUpdateInterval->Position;
	AnsiString text;
	text.sprintf("%d ms (~%d Hz)", ms, (ms > 0) ? (1000 / ms) : 0);
	lblUpdateIntervalValue->Caption = text;
}

void __fastcall TfraSpectrumAnalyzerConf::trbarMeterFloorChange(TObject *Sender)
{
	UpdateMeterFloorLabel();
}

void TfraSpectrumAnalyzerConf::UpdateMeterFloorLabel(void)
{
	AnsiString text;
	text.sprintf("%d dB", trbarMeterFloor->Position);
	lblMeterFloorValue->Caption = text;
}

void __fastcall TfraSpectrumAnalyzerConf::trbarSmoothingChange(TObject *Sender)
{
	UpdateSmoothingLabel();
}

void TfraSpectrumAnalyzerConf::UpdateSmoothingLabel(void)
{
	AnsiString text;
	text.sprintf("%d%%", trbarSmoothing->Position);
	lblSmoothingValue->Caption = text;
}

void TfraSpectrumAnalyzerConf::Apply(void)
{
	conf.autoStart = chbAutoStart->Checked;
	conf.peakDetect = chbPeakDetect->Checked;
	conf.alternateDirection = chbAlternateDirection->Checked;

	// invalid/out-of-range entries keep the previous value
	int ledsPerBand = StrToIntDef(edLedsPerBand->Text, -1);
	if (ledsPerBand >= SpectrumAnalyzerConf::MIN_LEDS_PER_BAND && ledsPerBand <= SpectrumAnalyzerConf::MAX_LEDS_PER_BAND)
		conf.ledsPerBand = ledsPerBand;
	int minFreq = StrToIntDef(edMinFreq->Text, -1);
	int maxFreq = StrToIntDef(edMaxFreq->Text, -1);
	if (minFreq >= SpectrumAnalyzerConf::MIN_FREQ_HZ && maxFreq <= SpectrumAnalyzerConf::MAX_FREQ_HZ && minFreq < maxFreq)
	{
		conf.minFreqHz = minFreq;
		conf.maxFreqHz = maxFreq;
	}

	conf.gain = trbarGain->Position / 10.0f;
	conf.maxBrightness = trbarMaxBrightness->Position;
	conf.updateIntervalMs = trbarUpdateInterval->Position;
	conf.meterFloorDb = trbarMeterFloor->Position;
	conf.smoothingPct = trbarSmoothing->Position;
}
//---------------------------------------------------------------------------
