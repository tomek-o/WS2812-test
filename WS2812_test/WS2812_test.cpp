//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "FormMain.h"
#include "Settings.h"
//---------------------------------------------------------------------------
USEFORM("logging\FrameLogConf.cpp", fraLogConf); /* TFrame: File Type */
USEFORM("logging\FormLog.cpp", frmLog);
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("WS2812\FormWS2812Animations.cpp", frmWS2812Animations);
USEFORM("WS2812\FormWS2812ManualControl.cpp", frmWS2812ManualControl);
USEFORM("WS2812\FormWS2812OneForAll.cpp", frmWS2812OneForAll);
USEFORM("WS2812\FormWS2812AudioVisualisation.cpp", frmWS2812AudioVisualisation);
USEFORM("WS2812\FrameAudioVisualisationConf.cpp", fraAudioVisualisationConf); /* TFrame: File Type */
//---------------------------------------------------------------------------
#pragma link "common.lib"
#pragma link "jsoncpp.lib"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "WS2812 test";
		Application->ShowMainForm = false;	// decided below, once settings are loaded
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		Application->CreateForm(__classid(TfrmWS2812ManualControl), &frmWS2812ManualControl);
		Application->CreateForm(__classid(TfrmWS2812Animations), &frmWS2812Animations);
		Application->CreateForm(__classid(TfrmWS2812OneForAll), &frmWS2812OneForAll);
		Application->CreateForm(__classid(TfrmWS2812AudioVisualisation), &frmWS2812AudioVisualisation);
		if (appSettings.frmMain.startMinimizedToTray)
			ShowWindow(Application->Handle, SW_HIDE);	// hide taskbar button; tray icon still shows
		else
			frmMain->Show();

		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
