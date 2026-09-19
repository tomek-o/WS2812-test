//---------------------------------------------------------------------------

#ifndef FormWS2812AudioVisualisationH
#define FormWS2812AudioVisualisationH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "FormWS2812.h"
#include "WS2812.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <vector>
#include <string>
#include "Wasapi\WasapiLoopbackCapture.h"
#include "Wasapi\WasapiDevices.h"

class TfrmWS2812AudioVisualisation : public FormWS2812
{
__published:	// IDE-managed Components
	TButton *btnStart;
	TButton *btnStop;
	TTimer *tmrWrite;
	TLabel *lblGain;
	TTrackBar *trbarGain;
	TLabel *lblGainValue;
	TComboBox *cbAudioDevice;
	TButton *btnRefreshDevices;
	TComboBox *cbSource;
	TLabel *lblDevice;
	TImage *image;
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrWriteTimer(TObject *Sender);
	void __fastcall trbarGainChange(TObject *Sender);
	void __fastcall cbAudioDeviceChange(TObject *Sender);
	void __fastcall btnRefreshDevicesClick(TObject *Sender);
	void __fastcall cbSourceChange(TObject *Sender);
private:	// User declarations
	WasapiLoopbackCapture capture;
	float smoothedLeft, smoothedRight;
	float peakLeft, peakRight;
	int peakHoldLeft, peakHoldRight;
	std::vector<Ws2812Color> previewColors;
	std::vector<std::string> deviceIds;	///< parallel to cbAudioDevice->Items; [0] is "" (default)
	void Write(void);
	void UpdatePeak(float level, float &peak, int &holdCounter);
	void RenderVuBar(unsigned int offset, unsigned int count, float level, float peak);
	void UpdateGainLabel(void);
	void RefreshDeviceList(void);
public:		// User declarations
	__fastcall TfrmWS2812AudioVisualisation(TComponent* Owner);
	void SetLedCount(unsigned int ledCount);
	/** \brief Start WASAPI loopback capture; used by btnStartClick and auto-start */
	void StartCapture(void);
	/** \brief Re-sync on-screen controls with appSettings */
	void UpdateUi(void);
	void PaintOnImage(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWS2812AudioVisualisation *frmWS2812AudioVisualisation;
//---------------------------------------------------------------------------
#endif
