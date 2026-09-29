//---------------------------------------------------------------------------
#ifndef FrameSpectrumAnalyzerConfH
#define FrameSpectrumAnalyzerConfH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

struct SpectrumAnalyzerConf;

class TfraSpectrumAnalyzerConf : public TFrame
{
__published:	// IDE-managed Components
	TCheckBox *chbAutoStart;
	TCheckBox *chbPeakDetect;
	TCheckBox *chbAlternateDirection;
	TLabel *lblLedsPerBand;
	TEdit *edLedsPerBand;
	TLabel *lblFreqRange;
	TEdit *edMinFreq;
	TLabel *lblFreqDash;
	TEdit *edMaxFreq;
	TLabel *lblFreqUnit;
	TLabel *lblGain;
	TTrackBar *trbarGain;
	TLabel *lblGainValue;
	TLabel *lblMaxBrightness;
	TTrackBar *trbarMaxBrightness;
	TLabel *lblMaxBrightnessValue;
	TLabel *lblUpdateInterval;
	TLabel *lblUpdateIntervalValue;
	TTrackBar *trbarUpdateInterval;
	TLabel *lblMeterFloor;
	TLabel *lblMeterFloorValue;
	TTrackBar *trbarMeterFloor;
	TLabel *lblSmoothing;
	TLabel *lblSmoothingValue;
	TTrackBar *trbarSmoothing;
	void __fastcall trbarGainChange(TObject *Sender);
	void __fastcall trbarMaxBrightnessChange(TObject *Sender);
	void __fastcall trbarUpdateIntervalChange(TObject *Sender);
	void __fastcall trbarMeterFloorChange(TObject *Sender);
	void __fastcall trbarSmoothingChange(TObject *Sender);
private:	// User declarations
	SpectrumAnalyzerConf &conf;
	void UpdateGainLabel(void);
	void UpdateMaxBrightnessLabel(void);
	void UpdateIntervalLabel(void);
	void UpdateMeterFloorLabel(void);
	void UpdateSmoothingLabel(void);
public:		// User declarations
	__fastcall TfraSpectrumAnalyzerConf(TComponent* Owner, SpectrumAnalyzerConf &conf);
	void Load(void);
	void Apply(void);
};

#endif
