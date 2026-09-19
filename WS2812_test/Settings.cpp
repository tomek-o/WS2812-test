//---------------------------------------------------------------------------


#pragma hdrstop

#include "Settings.h"
#include "common/SettingsUtils.h"
#include <algorithm>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

inline void strncpyz(char* dst, const char* src, int dstsize) {
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

void Settings::UpdateFromJsonValue(const Json::Value &root)
{
	{
		int maxX = GetSystemMetrics(SM_CXSCREEN);
		/** \todo Ugly fixed taskbar margin */
		int maxY = GetSystemMetrics(SM_CYSCREEN) - 32;

		const Json::Value &jv = root["frmMain"];
		jv.getIntInRange("width", frmMain.width, 250, maxX + 20);
		jv.getIntInRange("height", frmMain.height, 200, maxY + 20);
	#if 0
		jv.getIntInRange("positionX", frmMain.posX, 0, maxX - 100);
		jv.getIntInRange("positionY", frmMain.posY, 0, maxY - 100);
	#else
		jv.getInt("positionX", frmMain.posX);
		jv.getInt("positionY", frmMain.posY);
	#endif
		jv.getBool("maximized", frmMain.windowMaximized);
		jv.get("alwaysOnTop", frmMain.alwaysOnTop);
		jv.getInt("activeTabIndex", frmMain.activeTabIndex);
		jv.getBool("startMinimizedToTray", frmMain.startMinimizedToTray);
		jv.getBool("autostart", frmMain.autostart);
	}

	logging.fromJson(root["logging"]);

	{
		const Json::Value &jv = root["serialPort"];
		jv.getAString("name", serialPort.name);
		jv.getInt("baud", serialPort.baud);
		jv.getAString("parity", serialPort.parity);
		jv.getInt("data_bit", serialPort.data_bit);
		jv.getInt("stop_bit", serialPort.stop_bit);
		jv.getBool("openAtStartup", serialPort.openAtStartup);
		jv.getBool("autoReinit", serialPort.autoReinit);
	}

	{
		const Json::Value &jv = root["ws2812"];
		jv.getUInt("ledCount", ws2812.ledCount);
		if (ws2812.ledCount > WS2812::MAX_LED_COUNT)
			ws2812.ledCount = 8;
		{
			const Json::Value &jmc = jv["manualControl"];
			if (jmc.type() == Json::arrayValue)
			{
				ws2812.manualControl.resize(jmc.size());
				for (unsigned int i=0; i<jmc.size(); i++)
				{
					struct Ws2812Color& color = ws2812.manualControl[i];
					const Json::Value &jColor = jmc[i];
					color.r = static_cast<uint8_t>(jColor["r"].asUInt());
					color.g = static_cast<uint8_t>(jColor["g"].asUInt());
					color.b = static_cast<uint8_t>(jColor["b"].asUInt());
				}
			}
			ws2812.manualControl.resize(ws2812.ledCount);
		}
		jv.getBool("manualControlApplyImmediately", ws2812.manualControlApplyImmediately);
	}

	audioVisualisation.fromJson(root["audioVisualisation"]);
}

enum SettingsUtils::ReadStatus Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.

	enum SettingsUtils::ReadStatus status = SettingsUtils::ReadFileOrBackup(asFileName, root);
	if (status == SettingsUtils::READ_OK || status == SettingsUtils::READ_RECOVERED_FROM_BACKUP)
		UpdateFromJsonValue(root);
	return status;
}

int Settings::Write(AnsiString asFileName)
{
	Json::Value root;
	Json::StyledWriter writer;

	{
		Json::Value &jv = root["frmMain"];
		jv["width"] = frmMain.width;
		jv["height"] = frmMain.height;
		jv["positionX"] = frmMain.posX;
		jv["positionY"] = frmMain.posY;
		jv["maximized"] = frmMain.windowMaximized;
		jv["alwaysOnTop"] = frmMain.alwaysOnTop;
		jv["activeTabIndex"] = frmMain.activeTabIndex;
		jv["startMinimizedToTray"] = frmMain.startMinimizedToTray;
		jv["autostart"] = frmMain.autostart;
	}

	logging.toJson(root["logging"]);

	{
		Json::Value &jv = root["serialPort"];
		jv["name"] = serialPort.name;
		jv["baud"] = serialPort.baud;
		jv["parity"] = serialPort.parity;
		jv["data_bit"] = serialPort.data_bit;
		jv["stop_bit"] = serialPort.stop_bit;
		jv["openAtStartup"] = serialPort.openAtStartup;
		jv["autoReinit"] = serialPort.autoReinit;
	}

	{
		Json::Value &jv = root["ws2812"];
		jv["ledCount"] = ws2812.ledCount;
		{
			Json::Value &jmc = jv["manualControl"];
			jmc = Json::Value(Json::arrayValue);
			for (unsigned int i=0; i<ws2812.manualControl.size(); i++)
			{
				const struct Ws2812Color& color = ws2812.manualControl[i];
				Json::Value jColor = Json::Value(Json::objectValue);
				jColor["r"] = color.r;
				jColor["g"] = color.g;
				jColor["b"] = color.b;
				jmc.append(jColor);
			}
		}
		jv["manualControlApplyImmediately"] = ws2812.manualControlApplyImmediately;
	}

	audioVisualisation.toJson(root["audioVisualisation"]);

	std::string outputConfig = writer.write( root );
	return SettingsUtils::AtomicUpdateWithBackup(asFileName, outputConfig);
}


