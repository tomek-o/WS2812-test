#pragma hdrstop

#include "SpectrumAnalyzerConf.h"
#include <json/json.h>

#pragma package(smart_init)

SpectrumAnalyzerConf::SpectrumAnalyzerConf(void):
	gain(2.0f),
	autoStart(false),
	maxBrightness(DEFAULT_MAX_BRIGHTNESS),
	updateIntervalMs(25),
	meterFloorDb(-50),
	peakDetect(true),
	smoothingPct(30),
	ledsPerBand(DEFAULT_LEDS_PER_BAND),
	minFreqHz(40),
	maxFreqHz(16000),
	alternateDirection(false),
	audioInput(false)
{
}

void SpectrumAnalyzerConf::fromJson(const Json::Value &jv)
{
	if (jv.type() != Json::objectValue)
		return;
	jv.getFloat("gain", gain);
	if (gain < MIN_GAIN_X10 / 10.0f)
		gain = MIN_GAIN_X10 / 10.0f;
	else if (gain > MAX_GAIN_X10 / 10.0f)
		gain = MAX_GAIN_X10 / 10.0f;
	jv.getBool("autoStart", autoStart);
	jv.getUIntInRange("maxBrightness", maxBrightness, MIN_MAX_BRIGHTNESS, MAX_MAX_BRIGHTNESS);
	jv.getUIntInRange("updateIntervalMs", updateIntervalMs, MIN_UPDATE_INTERVAL_MS, MAX_UPDATE_INTERVAL_MS);
	jv.getIntInRange("meterFloorDb", meterFloorDb, MIN_METER_FLOOR_DB, MAX_METER_FLOOR_DB);
	jv.getBool("peakDetect", peakDetect);
	jv.getUIntInRange("smoothingPct", smoothingPct, MIN_SMOOTHING_PCT, MAX_SMOOTHING_PCT);
	jv.getUIntInRange("ledsPerBand", ledsPerBand, MIN_LEDS_PER_BAND, MAX_LEDS_PER_BAND);
	jv.getUIntInRange("minFreqHz", minFreqHz, MIN_FREQ_HZ, MAX_FREQ_HZ);
	jv.getUIntInRange("maxFreqHz", maxFreqHz, MIN_FREQ_HZ, MAX_FREQ_HZ);
	if (minFreqHz >= maxFreqHz)
	{
		minFreqHz = 40;
		maxFreqHz = 16000;
	}
	jv.getBool("alternateDirection", alternateDirection);
	jv.getAString("audioDeviceId", audioDeviceId);
	jv.getBool("audioInput", audioInput);
}

void SpectrumAnalyzerConf::toJson(Json::Value &jv) const
{
	jv = Json::Value(Json::objectValue);
	jv["gain"] = gain;
	jv["autoStart"] = autoStart;
	jv["maxBrightness"] = maxBrightness;
	jv["updateIntervalMs"] = updateIntervalMs;
	jv["meterFloorDb"] = meterFloorDb;
	jv["peakDetect"] = peakDetect;
	jv["smoothingPct"] = smoothingPct;
	jv["ledsPerBand"] = ledsPerBand;
	jv["minFreqHz"] = minFreqHz;
	jv["maxFreqHz"] = maxFreqHz;
	jv["alternateDirection"] = alternateDirection;
	jv["audioDeviceId"] = audioDeviceId;
	jv["audioInput"] = audioInput;
}
