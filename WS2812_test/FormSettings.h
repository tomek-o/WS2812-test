//---------------------------------------------------------------------------

#ifndef FormSettingsH
#define FormSettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "Settings.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class TfraLogConf;
class TfraAudioVisualisationConf;

class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlBottom;
	TButton *btnCancel;
	TButton *btnApply;
	TPageControl *pages;
	TTabSheet *tsGeneral;
	TTabSheet *tsLogging;
	TTabSheet *tsAudioVisualisation;
	TCheckBox *chbAlwaysOnTop;
	TCheckBox *chbStartMinimizedToTray;
	TCheckBox *chbAutostart;
	TTabSheet *tsSerialPort;
	TCheckBox *chbSerialPortOpenAtStartup;
	TCheckBox *chbSerialPortAutoReinit;
	TLabel *lblSerialPortName;
	TLabel *lblSerialPortBaudrate;
	TComboBox *cbSerialPorts;
	TEdit *edSerialPortBaudrate;
	TLabel *lblLedCount;
	TComboBox *cbLedCount;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
	void __fastcall btnApplyClick(TObject *Sender);
	void __fastcall chbAlwaysOnTopClick(TObject *Sender);
	void __fastcall FormKeyPress(TObject *Sender, char &Key);
private:	// User declarations
	void GetAutostartIdentity(AnsiString &name, AnsiString &command);
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	Settings *appSettings;
	Settings tmpSettings;
	TfraLogConf *fraLogConf;
	TfraAudioVisualisationConf *fraAudioVisualisationConf;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
