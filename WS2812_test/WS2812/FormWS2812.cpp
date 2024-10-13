//---------------------------------------------------------------------------


#pragma hdrstop

#include "FormWS2812.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

__fastcall FormWS2812::FormWS2812(TComponent* Owner)
	: TForm(Owner),
	ledCount(0)
{
	this->BorderStyle = bsNone;
	this->Align = alClient;
}
