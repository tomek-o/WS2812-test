//---------------------------------------------------------------------------


#pragma hdrstop

#include "AppStatus.h"
#include "FormMain.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

void SetAppStatus(AnsiString text)
{
	frmMain->StatusBar->SimpleText = text;
}

bool IsTabVisible(TForm *tabForm)
{
	// each tab's content form is parented into its own TTabSheet
	// (TabManager::Register), so compare against the sheet, not the form
	return frmMain->Visible &&
		frmMain->WindowState != wsMinimized &&
		tabForm->Parent == frmMain->pages->ActivePage;
}
