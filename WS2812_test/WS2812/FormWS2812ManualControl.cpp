//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormWS2812ManualControl.h"
#include "TabManager.h"
#include "Settings.h"
#include "BtnController.h"
#include "AppStatus.h"
#include "ComPort.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmWS2812ManualControl *frmWS2812ManualControl;

static uint8_t& ColumnColor(struct Ws2812Color &color, int column)
{
	switch (column)
	{
	case 1:
		return color.r;
	case 2:
		return color.g;
	default:
		return color.b;
	}
}

//---------------------------------------------------------------------------
__fastcall TfrmWS2812ManualControl::TfrmWS2812ManualControl(TComponent* Owner)
	: FormWS2812(Owner)
{
	TabManager::Instance().Register(this);
	grid->Cells[0][0] = "LED id";
	grid->Cells[1][0] = "Red";
	grid->Cells[2][0] = "Green";
	grid->Cells[3][0] = "Blue";
	grid->Cells[4][0] = "Result color sample";
	grid->ColWidths[0] = 40;
	grid->ColWidths[4] = 120;
}
//---------------------------------------------------------------------------

void TfrmWS2812ManualControl::SetLedCount(unsigned int ledCount)
{
	if (this->ledCount == ledCount)
		return;
	this->ledCount = ledCount;
	if (appSettings.ws2812.manualControl.size() != ledCount)
	{
		appSettings.ws2812.manualControl.resize(ledCount);
	}

	grid->RowCount = ledCount + 1;
	for (unsigned int i=0; i<ledCount; i++)
	{
		grid->Cells[0][i+1] = i;
		const struct Ws2812Color &color = appSettings.ws2812.manualControl[i];
		grid->Cells[1][i+1] = color.r;
		grid->Cells[2][i+1] = color.g;
		grid->Cells[3][i+1] = color.b;
	}
}

void __fastcall TfrmWS2812ManualControl::gridDrawCell(TObject *Sender, int ACol,
      int ARow, TRect &Rect, TGridDrawState State)
{
	if (ARow < 1)
		return;
	if (ACol == 4)
	{
		const struct Ws2812Color &wcolor = appSettings.ws2812.manualControl[ARow-1];
		TColor color = static_cast<TColor>(wcolor.r + (static_cast<unsigned int>(wcolor.g) << 8) + (static_cast<unsigned int>(wcolor.b) << 16));
		grid->Canvas->Brush->Color = color;
		grid->Canvas->FillRect(Rect);
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmWS2812ManualControl::gridDblClick(TObject *Sender)
{
	grid->Options << goEditing;	
}
//---------------------------------------------------------------------------
void __fastcall TfrmWS2812ManualControl::gridSelectCell(TObject *Sender,
      int ACol, int ARow, bool &CanSelect)
{
	if (grid->EditorMode == true && grid->Options.Contains(goEditing))
	{
		EndEdit(true);
	}
}
//---------------------------------------------------------------------------

void TfrmWS2812ManualControl::EndEdit(bool apply)
{
	struct Ws2812Color &wcolor = appSettings.ws2812.manualControl[grid->Row - 1];
	uint8_t &color = ColumnColor(wcolor, grid->Col);
	AnsiString text = grid->Cells[grid->Col][grid->Row];
	int newColor = StrToIntDef(text, -1);
	if (apply && newColor >= 0 && newColor <= 255)
	{
		color = static_cast<uint8_t>(newColor);
		grid->Repaint();
		if (appSettings.ws2812.manualControlApplyImmediately)
		{
        	Write();
		}
	}
	grid->Cells[grid->Col][grid->Row] = color;
	grid->Options >> goEditing;
	grid->EditorMode = false;
}

void __fastcall TfrmWS2812ManualControl::gridKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if (Key == VK_ESCAPE)
	{
		EndEdit(false);
	}
	else if (
		((Key == VK_DOWN || Key == VK_NEXT /* page down */) && grid->Row != grid->RowCount - 1) ||
		((Key == VK_UP || Key == VK_PRIOR /* page up */) && grid->Row > 1)
		)
	{
		if (grid->EditorMode == true && grid->Options.Contains(goEditing))
		{
			EndEdit(true);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmWS2812ManualControl::gridKeyPress(TObject *Sender,
      char &Key)
{
	if (Key == VK_RETURN)
	{
		if (grid->EditorMode == false)
		{
			grid->Options << goEditing;
		}
		else
		{
			EndEdit(true);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmWS2812ManualControl::btnWriteClick(TObject *Sender)
{
	Write();
}

void TfrmWS2812ManualControl::Write(void)
{
	BtnController btnCtrl(btnWrite);

	if (!comPort.isOpened())
	{
		SetAppStatus("Cannot write: serial port is not opened");
		return;
	}

	int status = Ws2812Write(appSettings.ws2812.manualControl);
	if (status != 0)
	{
		SetAppStatus("WS2812 write failed");
	}
	else
	{
    	SetAppStatus("");
	}
}
//---------------------------------------------------------------------------
void __fastcall TfrmWS2812ManualControl::FormCreate(TObject *Sender)
{
	chbManualControlApplyImmediately->Checked = appSettings.ws2812.manualControlApplyImmediately;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812ManualControl::chbManualControlApplyImmediatelyClick(
      TObject *Sender)
{
	appSettings.ws2812.manualControlApplyImmediately = chbManualControlApplyImmediately->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812ManualControl::chbManualControlApplyImmediatelyKeyPress(
      TObject *Sender, char &Key)
{
	appSettings.ws2812.manualControlApplyImmediately = chbManualControlApplyImmediately->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812ManualControl::tmrWriteTimer(TObject *Sender)
{
	if (chbWritePeriodically->Checked)
		Write();	
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812ManualControl::btnClearAllClick(TObject *Sender)
{
	Ws2812Color color;
	for (unsigned int i=0; i<appSettings.ws2812.manualControl.size(); i++)
	{
		appSettings.ws2812.manualControl[i] = color;
		grid->Cells[1][i+1] = color.r;
		grid->Cells[2][i+1] = color.g;
		grid->Cells[3][i+1] = color.b;
	}
	EndEdit(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmWS2812ManualControl::btnClearLedClick(TObject *Sender)
{
	struct Ws2812Color &wcolor = appSettings.ws2812.manualControl[grid->Row - 1];
	wcolor.r = 0;
	wcolor.g = 0;
	wcolor.b = 0;
	grid->Cells[1][grid->Row] = wcolor.r;
	grid->Cells[2][grid->Row] = wcolor.g;
	grid->Cells[3][grid->Row] = wcolor.b;
	EndEdit(true);
}
//---------------------------------------------------------------------------

