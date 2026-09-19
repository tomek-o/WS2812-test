#pragma hdrstop

#include "LogConf.h"
#include <json/json.h>

#pragma package(smart_init)

LogConf::LogConf(void):
	logToFile(false),
	flush(false),
	maxFileSize(DEF_MAX_FILE_SIZE),
	timestamps(true),
	logRotate(DEF_LOGROTATE),
	maxUiLogLines(5000),
	windowWidth(630), windowHeight(400),
	showWindowAtStartup(false)
{
	consoleFont.name = "Courier New";
}

void LogConf::fromJson(const Json::Value &jv)
{
	if (jv.type() != Json::objectValue)
		return;
	jv.getBool("logToFile", logToFile);
	jv.getBool("flush", flush);
	jv.getIntInRange("maxFileSize", maxFileSize, MIN_MAX_FILE_SIZE, MAX_MAX_FILE_SIZE);
	jv.getUIntInRange("maxUiLogLines", maxUiLogLines, MIN_MAX_UI_LOG_LINES, MAX_MAX_UI_LOG_LINES);
	jv.getBool("timestamps", timestamps);
	jv.getUIntInRange("logRotate", logRotate, 0, MAX_LOGROTATE);
	jv.getUInt("windowWidth", windowWidth);
	jv.getUInt("windowHeight", windowHeight);
	jv.getBool("showWindowAtStartup", showWindowAtStartup);
	consoleFont.fromJson(jv["consoleFont"]);
}

void LogConf::toJson(Json::Value &jv) const
{
	jv = Json::Value(Json::objectValue);
	jv["logToFile"] = logToFile;
	jv["flush"] = flush;
	jv["maxFileSize"] = maxFileSize;
	jv["maxUiLogLines"] = maxUiLogLines;
	jv["timestamps"] = timestamps;
	jv["logRotate"] = logRotate;
	jv["windowWidth"] = windowWidth;
	jv["windowHeight"] = windowHeight;
	jv["showWindowAtStartup"] = showWindowAtStartup;
	consoleFont.toJson(jv["consoleFont"]);
}

bool LogConf::operator==(const LogConf &right) const
{
	return (
		logToFile == right.logToFile &&
		flush == right.flush &&
		maxFileSize == right.maxFileSize &&
		maxUiLogLines == right.maxUiLogLines &&
		timestamps == right.timestamps &&
		logRotate == right.logRotate &&
		windowWidth == right.windowWidth &&
		windowHeight == right.windowHeight &&
		showWindowAtStartup == right.showWindowAtStartup &&
		consoleFont == right.consoleFont
	);
}
