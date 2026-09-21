//---------------------------------------------------------------------------

#ifndef AppStatusH
#define AppStatusH
//---------------------------------------------------------------------------

#include <System.hpp>

namespace Forms
{
	class TForm;
}

void SetAppStatus(AnsiString text);

/** \brief Is the given tab currently visible on screen?
	(main window visible and not minimized, and this tab is the active page)
*/
bool IsTabVisible(Forms::TForm *tabForm);

#endif
