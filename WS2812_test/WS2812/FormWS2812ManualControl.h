//---------------------------------------------------------------------------

#ifndef FormWS2812ManualControlH
#define FormWS2812ManualControlH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "FormWS2812.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>

class TfrmWS2812ManualControl : public FormWS2812
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnWrite;
	TStringGrid *grid;
	TCheckBox *chbManualControlApplyImmediately;
	TCheckBox *chbWritePeriodically;
	TTimer *tmrWrite;
	TButton *btnClearAll;
	TButton *btnClearLed;
	void __fastcall gridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall gridDblClick(TObject *Sender);
	void __fastcall gridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall gridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall gridKeyPress(TObject *Sender, char &Key);
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall chbManualControlApplyImmediatelyClick(TObject *Sender);
	void __fastcall chbManualControlApplyImmediatelyKeyPress(TObject *Sender,
          char &Key);
	void __fastcall tmrWriteTimer(TObject *Sender);
	void __fastcall btnClearAllClick(TObject *Sender);
	void __fastcall btnClearLedClick(TObject *Sender);
private:	// User declarations
	void EndEdit(bool apply);
	void Write(void);
public:		// User declarations
	__fastcall TfrmWS2812ManualControl(TComponent* Owner);
	void SetLedCount(unsigned int ledCount);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWS2812ManualControl *frmWS2812ManualControl;
//---------------------------------------------------------------------------
#endif
