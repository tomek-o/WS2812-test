#pragma hdrstop

#include "AudioVisualisationConf.h"
#include <json/json.h>

#pragma package(smart_init)

AudioVisualisationConf::AudioVisualisationConf(void):
	gain(1.5f),
	stereoSeparate(false),
	stereoDirection(STEREO_DIRECTION_DEFAULT),
	autoStart(false),
	maxBrightness(DEFAULT_MAX_BRIGHTNESS),
	updateIntervalMs(25),
	meterFloorDb(-40),
	peakDetect(true),
	smoothingPct(0),
	audioInput(false)
{
}

void AudioVisualisationConf::fromJson(const Json::Value &jv)
{
	if (jv.type() != Json::objectValue)
		return;
	jv.getFloat("gain", gain);
	if (gain < MIN_GAIN_X10 / 10.0f)
		gain = MIN_GAIN_X10 / 10.0f;
	else if (gain > MAX_GAIN_X10 / 10.0f)
		gain = MAX_GAIN_X10 / 10.0f;
	jv.getBool("stereoSeparate", stereoSeparate);
	jv.getUIntInRange("stereoDirection", stereoDirection, STEREO_DIRECTION_DEFAULT, STEREO_DIRECTION_INWARD);
	jv.getBool("autoStart", autoStart);
	jv.getUIntInRange("maxBrightness", maxBrightness, MIN_MAX_BRIGHTNESS, MAX_MAX_BRIGHTNESS);
	jv.getUIntInRange("updateIntervalMs", updateIntervalMs, MIN_UPDATE_INTERVAL_MS, MAX_UPDATE_INTERVAL_MS);
	jv.getIntInRange("meterFloorDb", meterFloorDb, MIN_METER_FLOOR_DB, MAX_METER_FLOOR_DB);
	jv.getBool("peakDetect", peakDetect);
	jv.getUIntInRange("smoothingPct", smoothingPct, MIN_SMOOTHING_PCT, MAX_SMOOTHING_PCT);
	jv.getAString("audioDeviceId", audioDeviceId);
	jv.getBool("audioInput", audioInput);
}

void AudioVisualisationConf::toJson(Json::Value &jv) const
{
	jv = Json::Value(Json::objectValue);
	jv["gain"] = gain;
	jv["stereoSeparate"] = stereoSeparate;
	jv["stereoDirection"] = stereoDirection;
	jv["autoStart"] = autoStart;
	jv["maxBrightness"] = maxBrightness;
	jv["updateIntervalMs"] = updateIntervalMs;
	jv["meterFloorDb"] = meterFloorDb;
	jv["peakDetect"] = peakDetect;
	jv["smoothingPct"] = smoothingPct;
	jv["audioDeviceId"] = audioDeviceId;
	jv["audioInput"] = audioInput;
}
