//---------------------------------------------------------------------------


#pragma hdrstop

#include "TabManager.h"
#include "FormMain.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

TabManager TabManager::instance;

TabManager::TabManager()
{

}


int TabManager::Register(TForm *frm)
{
	TTabSheet *pTabSheet = new TTabSheet(frmMain->pages);
	pTabSheet->PageControl = frmMain->pages;
	pTabSheet->Caption = frm->Caption;
	//pTabSheet->Align = alClient;

	frm->Parent = pTabSheet;
	frm->Visible = true;

    forms.push_back(frm);
	return 0;
}
