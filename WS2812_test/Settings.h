/** \file
*/
//---------------------------------------------------------------------------

#ifndef SettingsH
#define SettingsH
//---------------------------------------------------------------------------
#include <System.hpp>

#include "WS2812.h"
#include <vector>

class Settings
{
public:
	int Read(AnsiString asFileName);
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
		FrmMain(void):
			width(600), height(400),
			posX(30), posY(30),
			windowMaximized(false),
			alwaysOnTop(false)
		{}
	} frmMain;
	struct Logging
	{
		bool logToFile;
		bool flush;
		enum {
			MIN_MAX_FILE_SIZE = 0,
			MAX_MAX_FILE_SIZE = 1000*1024*1024
		};
		enum {
			DEF_MAX_FILE_SIZE = 10*1024*1024
		};
		int maxFileSize;
		unsigned int maxUiLogLines;
		Logging(void):
			logToFile(false),
			flush(false),
			maxFileSize(DEF_MAX_FILE_SIZE),
			maxUiLogLines(5000)
		{}
	} logging;
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
};

extern Settings appSettings;

#endif
