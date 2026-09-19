//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FrameAudioVisualisationConf.h"
#include "AudioVisualisationConf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfraAudioVisualisationConf::TfraAudioVisualisationConf(TComponent* Owner, AudioVisualisationConf &conf)
	: TFrame(Owner),
	conf(conf)
{
	trbarGain->Min = AudioVisualisationConf::MIN_GAIN_X10;
	trbarGain->Max = AudioVisualisationConf::MAX_GAIN_X10;

	trbarMaxBrightness->Min = AudioVisualisationConf::MIN_MAX_BRIGHTNESS;
	trbarMaxBrightness->Max = AudioVisualisationConf::MAX_MAX_BRIGHTNESS;

	AnsiString caption;
	caption.sprintf("Max brightness (%d-%d, caps per-channel value to limit current draw)",
		AudioVisualisationConf::MIN_MAX_BRIGHTNESS, AudioVisualisationConf::MAX_MAX_BRIGHTNESS);
	lblMaxBrightness->Caption = caption;

	trbarUpdateInterval->Min = AudioVisualisationConf::MIN_UPDATE_INTERVAL_MS;
	trbarUpdateInterval->Max = AudioVisualisationConf::MAX_UPDATE_INTERVAL_MS;

	AnsiString intervalCaption;
	intervalCaption.sprintf(
		"Update interval (%d-%d ms; actual rate is limited by Windows' timer resolution, ~15 ms)",
		AudioVisualisationConf::MIN_UPDATE_INTERVAL_MS, AudioVisualisationConf::MAX_UPDATE_INTERVAL_MS);
	lblUpdateInterval->Caption = intervalCaption;

	trbarMeterFloor->Min = AudioVisualisationConf::MIN_METER_FLOOR_DB;
	trbarMeterFloor->Max = AudioVisualisationConf::MAX_METER_FLOOR_DB;

	AnsiString floorCaption;
	floorCaption.sprintf("Meter floor (%d..%d dB; loudness mapped to the bottom of the bar)",
		AudioVisualisationConf::MIN_METER_FLOOR_DB, AudioVisualisationConf::MAX_METER_FLOOR_DB);
	lblMeterFloor->Caption = floorCaption;

	trbarSmoothing->Min = AudioVisualisationConf::MIN_SMOOTHING_PCT;
	trbarSmoothing->Max = AudioVisualisationConf::MAX_SMOOTHING_PCT;

	AnsiString smoothingCaption;
	smoothingCaption.sprintf("Smoothing (%d-%d%%; higher = smoother but slower to rise/fall)",
		AudioVisualisationConf::MIN_SMOOTHING_PCT, AudioVisualisationConf::MAX_SMOOTHING_PCT);
	lblSmoothing->Caption = smoothingCaption;
}
//---------------------------------------------------------------------------

void TfraAudioVisualisationConf::Load(void)
{
	trbarGain->Position = static_cast<int>(conf.gain * 10.0f + 0.5f);
	UpdateGainLabel();
	chbStereoSeparate->Checked = conf.stereoSeparate;
	chbAutoStart->Checked = conf.autoStart;
	chbPeakDetect->Checked = conf.peakDetect;
	trbarMaxBrightness->Position = conf.maxBrightness;
	trbarUpdateInterval->Position = conf.updateIntervalMs;
	UpdateIntervalLabel();
	trbarMeterFloor->Position = conf.meterFloorDb;
	UpdateMeterFloorLabel();
	trbarSmoothing->Position = conf.smoothingPct;
	UpdateSmoothingLabel();
}

void __fastcall TfraAudioVisualisationConf::trbarGainChange(TObject *Sender)
{
	UpdateGainLabel();
}

void TfraAudioVisualisationConf::UpdateGainLabel(void)
{
	AnsiString text;
	text.sprintf("%.1fx", trbarGain->Position / 10.0f);
	lblGainValue->Caption = text;
}

void __fastcall TfraAudioVisualisationConf::trbarUpdateIntervalChange(TObject *Sender)
{
	UpdateIntervalLabel();
}

void TfraAudioVisualisationConf::UpdateIntervalLabel(void)
{
	int ms = trbarUpdateInterval->Position;
	AnsiString text;
	text.sprintf("%d ms (~%d Hz)", ms, (ms > 0) ? (1000 / ms) : 0);
	lblUpdateIntervalValue->Caption = text;
}

void __fastcall TfraAudioVisualisationConf::trbarMeterFloorChange(TObject *Sender)
{
	UpdateMeterFloorLabel();
}

void TfraAudioVisualisationConf::UpdateMeterFloorLabel(void)
{
	AnsiString text;
	text.sprintf("%d dB", trbarMeterFloor->Position);
	lblMeterFloorValue->Caption = text;
}

void __fastcall TfraAudioVisualisationConf::trbarSmoothingChange(TObject *Sender)
{
	UpdateSmoothingLabel();
}

void TfraAudioVisualisationConf::UpdateSmoothingLabel(void)
{
	AnsiString text;
	text.sprintf("%d%%", trbarSmoothing->Position);
	lblSmoothingValue->Caption = text;
}

void TfraAudioVisualisationConf::Apply(void)
{
	conf.gain = trbarGain->Position / 10.0f;
	conf.stereoSeparate = chbStereoSeparate->Checked;
	conf.autoStart = chbAutoStart->Checked;
	conf.peakDetect = chbPeakDetect->Checked;
	conf.maxBrightness = trbarMaxBrightness->Position;
	conf.updateIntervalMs = trbarUpdateInterval->Position;
	conf.meterFloorDb = trbarMeterFloor->Position;
	conf.smoothingPct = trbarSmoothing->Position;
}
//---------------------------------------------------------------------------
