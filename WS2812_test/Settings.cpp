//---------------------------------------------------------------------------


#pragma hdrstop

#include "Settings.h"
#include <algorithm>
#include <fstream>
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

Settings appSettings;

inline void strncpyz(char* dst, const char* src, int dstsize) {
	strncpy(dst, src, dstsize);
	dst[dstsize-1] = '\0';
}

int Settings::Read(AnsiString asFileName)
{
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;

	try
	{
		std::ifstream ifs(asFileName.c_str());
		std::string strConfig((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		bool parsingSuccessful = reader.parse( strConfig, root );
		if ( !parsingSuccessful )
		{
			return 2;
		}
	}
	catch(...)
	{
		return 1;
	}

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
	}

	{
		const Json::Value &jv = root["logging"];
		jv.getBool("logToFile", logging.logToFile);
		jv.getBool("flush", logging.flush);
		jv.getIntInRange("maxFileSize", logging.maxFileSize, Logging::MIN_MAX_FILE_SIZE, Logging::MAX_MAX_FILE_SIZE);
		jv.getUInt("maxUiLogLines", logging.maxUiLogLines);
	}

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

	return 0;
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
	}

	{
		Json::Value &jv = root["logging"];
		jv["logToFile"] = logging.logToFile;
		jv["flush"] = logging.flush;
		jv["maxFileSize"] = logging.maxFileSize;
		jv["maxUiLogLines"] = logging.maxUiLogLines;
	}

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

	std::string outputConfig = writer.write( root );

	try
	{
		std::ofstream ofs(asFileName.c_str());
		ofs << outputConfig;
		ofs.close();
	}
	catch(...)
	{
    	return 1;
	}

	return 0;
}


