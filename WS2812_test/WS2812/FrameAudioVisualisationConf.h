//---------------------------------------------------------------------------
#ifndef FrameAudioVisualisationConfH
#define FrameAudioVisualisationConfH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

struct AudioVisualisationConf;

class TfraAudioVisualisationConf : public TFrame
{
__published:	// IDE-managed Components
	TLabel *lblGain;
	TTrackBar *trbarGain;
	TLabel *lblGainValue;
	TCheckBox *chbStereoSeparate;
	TCheckBox *chbAutoStart;
	TCheckBox *chbPeakDetect;
	TLabel *lblMaxBrightness;
	TTrackBar *trbarMaxBrightness;
	TLabel *lblUpdateInterval;
	TLabel *lblUpdateIntervalValue;
	TTrackBar *trbarUpdateInterval;
	TLabel *lblMeterFloor;
	TLabel *lblMeterFloorValue;
	TTrackBar *trbarMeterFloor;
	TLabel *lblSmoothing;
	TLabel *lblSmoothingValue;
	TTrackBar *trbarSmoothing;
	TLabel *lblMaxBrightnessValue;
	void __fastcall trbarGainChange(TObject *Sender);
	void __fastcall trbarMaxBrightnessChange(TObject *Sender);
	void __fastcall trbarUpdateIntervalChange(TObject *Sender);
	void __fastcall trbarMeterFloorChange(TObject *Sender);
	void __fastcall trbarSmoothingChange(TObject *Sender);
private:	// User declarations
	AudioVisualisationConf &conf;
	void UpdateGainLabel(void);
	void UpdateMaxBrightnessLabel(void);
	void UpdateIntervalLabel(void);
	void UpdateMeterFloorLabel(void);
	void UpdateSmoothingLabel(void);
public:		// User declarations
	__fastcall TfraAudioVisualisationConf(TComponent* Owner, AudioVisualisationConf &conf);
	void Load(void);
	void Apply(void);
};

#endif
