//---------------------------------------------------------------------------

#ifndef FormWS2812AnimationsH
#define FormWS2812AnimationsH
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

class TfrmWS2812Animations : public FormWS2812
{
__published:	// IDE-managed Components
	TTimer *tmrWrite;
	TComboBox *cbType;
	TButton *btnStart;
	TButton *btnStop;
	TLabel *lblType;
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrWriteTimer(TObject *Sender);
private:	// User declarations
	void Write(void);
public:		// User declarations
	__fastcall TfrmWS2812Animations(TComponent* Owner);
	void SetLedCount(unsigned int ledCount);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWS2812Animations *frmWS2812Animations;
//---------------------------------------------------------------------------
#endif
