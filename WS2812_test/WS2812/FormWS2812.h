//---------------------------------------------------------------------------

#ifndef FormWS2812H
#define FormWS2812H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

class FormWS2812 : public TForm
{
protected:
	unsigned int ledCount;
public:
	__fastcall FormWS2812(TComponent* Owner);
	virtual void SetLedCount(unsigned int ledCount) = 0;
};

#endif
