//---------------------------------------------------------------------------

#ifndef FormWS2812OneForAllH
#define FormWS2812OneForAllH
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

class TfrmWS2812OneForAll : public FormWS2812
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnWrite;
	TCheckBox *chbApplyImmediately;
	TCheckBox *chbWritePeriodically;
	TTimer *tmrWrite;
	TLabel *lblRed;
	TLabel *lblGreen;
	TLabel *lblBlue;
	TTrackBar *trbarRed;
	TTrackBar *trbarGreen;
	TTrackBar *trbarBlue;
	TLabel *lblDescription;
	void __fastcall btnWriteClick(TObject *Sender);
	void __fastcall tmrWriteTimer(TObject *Sender);
	void __fastcall trbarChange(TObject *Sender);
private:	// User declarations
	void Write(void);
public:		// User declarations
	__fastcall TfrmWS2812OneForAll(TComponent* Owner);
	void SetLedCount(unsigned int ledCount);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWS2812OneForAll *frmWS2812OneForAll;
//---------------------------------------------------------------------------
#endif
