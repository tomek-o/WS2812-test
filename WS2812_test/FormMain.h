//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ActnList.hpp>
#include <ImgList.hpp>
#include <ComCtrls.hpp>
#include <StdActns.hpp>
#include <ExtCtrls.hpp>

//#define ACCEPT_WM_DROPFILES

//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu;
	TMenuItem *miFile;
	TMenuItem *miSettings;
	TMenuItem *miHelp;
	TMenuItem *miAbout;
	TMenuItem *miCommonSettings;
	TActionList *ActionList;
	TImageList *imglistActions;
	TAction *actShowAbout;
	TAction *actShowSettings;
	TStatusBar *StatusBar;
	TMenuItem *miView;
	TMenuItem *miViewLog;
	TAction *actShowLog;
	TFileExit *FileExit;
	TMenuItem *Exit1;
	TPanel *pnlTop;
	TPageControl *pages;
	TButton *btnSerialOpen;
	TLabel *lblSerialPortState;
	TTimer *tmrSerialReinit;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall actShowAboutExecute(TObject *Sender);
	void __fastcall actShowSettingsExecute(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall actShowLogExecute(TObject *Sender);
	void __fastcall tmrSerialReinitTimer(TObject *Sender);
	void __fastcall btnSerialOpenClick(TObject *Sender);
private:	// User declarations
#ifdef ACCEPT_WM_DROPFILES
	void __fastcall WMDropFiles(TWMDropFiles &message);
#endif
	void __fastcall OnRestore(TObject *Sender);
	bool serialOpenCalled;	
	int SerialOpen(void);
	void UpdateLedCount(void);	
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
	BEGIN_MESSAGE_MAP
#ifdef ACCEPT_WM_DROPFILES
		MESSAGE_HANDLER(WM_DROPFILES, TWMDropFiles, WMDropFiles)
#endif
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
