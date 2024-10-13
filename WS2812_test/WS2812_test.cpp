//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("FormMain.cpp", frmMain);
USEFORM("FormSettings.cpp", frmSettings);
USEFORM("FormAbout.cpp", frmAbout);
USEFORM("LogUnit.cpp", frmLog);
USEFORM("WS2812\FormWS2812Animations.cpp", frmWS2812Animations);
USEFORM("WS2812\FormWS2812ManualControl.cpp", frmWS2812ManualControl);
USEFORM("WS2812\FormWS2812OneForAll.cpp", frmWS2812OneForAll);
//---------------------------------------------------------------------------
#pragma link "common.lib"
#pragma link "jsoncpp.lib"

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "WS2812 test";
		Application->CreateForm(__classid(TfrmMain), &frmMain);
		Application->CreateForm(__classid(TfrmSettings), &frmSettings);
		Application->CreateForm(__classid(TfrmAbout), &frmAbout);
		Application->CreateForm(__classid(TfrmLog), &frmLog);
		Application->CreateForm(__classid(TfrmWS2812ManualControl), &frmWS2812ManualControl);
		Application->CreateForm(__classid(TfrmWS2812Animations), &frmWS2812Animations);
		Application->CreateForm(__classid(TfrmWS2812OneForAll), &frmWS2812OneForAll);
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
