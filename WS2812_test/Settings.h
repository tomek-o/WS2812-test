/** \file
*/
//---------------------------------------------------------------------------

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include <System.hpp>

#include "WS2812.h"
#include "logging/LogConf.h"
#include "WS2812/AudioVisualisationConf.h"
#include "common/SettingsUtils.h"
#include <vector>

namespace Json
{
	struct Value;
}

class Settings
{
public:
	enum SettingsUtils::ReadStatus Read(AnsiString asFileName);
	int Write(AnsiString asFileName);
	struct Gui
	{
		enum { SCALING_MIN = 50 };
		enum { SCALING_MAX = 200 };
		int scalingPct;					///< scaling (percentage)
		Gui(void):
			scalingPct(100)
		{}
	} gui;
	struct FrmMain
	{
		int posX, posY;				///< main window coordinates
		int height, width;			///< main window size
		bool windowMaximized;			///< is main window maximized?
		bool alwaysOnTop;
		int activeTabIndex;			///< last active tab in the main window's page control
		bool startMinimizedToTray;
		bool autostart;
		FrmMain(void):
			width(600), height(400),
			posX(30), posY(30),
			windowMaximized(false),
			alwaysOnTop(false),
			activeTabIndex(0),
			startMinimizedToTray(false),
			autostart(false)
		{}
	} frmMain;
	struct LogConf logging;
	struct SerialPort
	{
		AnsiString name;
		int baud;
		AnsiString parity;
		int data_bit;
		int stop_bit;
		bool openAtStartup;
		bool autoReinit;
		static const AnsiString namePrefix;
		SerialPort(void):
			name("COM10"),
			baud(2000000),
			parity("N"),
			data_bit(7),
			stop_bit(1),
			openAtStartup(true),
			autoReinit(true)
		{}
	} serialPort;
	struct WS2812
	{
		unsigned int ledCount;
		enum { MAX_LED_COUNT = 1024 };
		std::vector<Ws2812Color> manualControl;
		bool manualControlApplyImmediately;
		WS2812(void):
			ledCount(8),
			manualControlApplyImmediately(true)
		{}
	} ws2812;
	struct AudioVisualisationConf audioVisualisation;
private:
	void UpdateFromJsonValue(const Json::Value &root);
};

extern Settings appSettings;

#endif
