//---------------------------------------------------------------------------

#ifndef FormWS2812SpectrumAnalyzerH
#define FormWS2812SpectrumAnalyzerH
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

/** \brief Audio spectrum analyzer: strip split into consecutive groups of
	SpectrumAnalyzerConf::ledsPerBand LEDs, each group a bar showing one
	log-spaced frequency band (lowest frequency first)
*/
class TfrmWS2812SpectrumAnalyzer : public FormWS2812
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
	TTimer *tmrStartup;
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall tmrWriteTimer(TObject *Sender);
	void __fastcall trbarGainChange(TObject *Sender);
	void __fastcall cbAudioDeviceChange(TObject *Sender);
	void __fastcall btnRefreshDevicesClick(TObject *Sender);
	void __fastcall cbSourceChange(TObject *Sender);
	void __fastcall tmrStartupTimer(TObject *Sender);
private:	// User declarations
	WasapiLoopbackCapture capture;
	std::vector<float> samples;		///< FFT input/real part, FFT_SIZE
	std::vector<float> imag;		///< FFT imaginary part, FFT_SIZE
	std::vector<float> window;		///< Hann window, FFT_SIZE
	std::vector<float> smoothed;	///< per band, linear amplitude
	std::vector<float> peaks;		///< per band, bar-normalized
	std::vector<int> peakHolds;		///< per band
	std::vector<Ws2812Color> previewColors;
	std::vector<std::string> deviceIds;	///< parallel to cbAudioDevice->Items; [0] is "" (default)
	void Write(void);
	void ComputeBands(std::vector<float> &bandLevels);
	void UpdatePeak(float level, float &peak, int &holdCounter);
	void RenderBar(unsigned int offset, unsigned int count, float level, float peak, bool reversed);
	void UpdateGainLabel(void);
	void RefreshDeviceList(void);
	void ResetBands(void);
public:		// User declarations
	__fastcall TfrmWS2812SpectrumAnalyzer(TComponent* Owner);
	void SetLedCount(unsigned int ledCount);
	/** \brief Start audio capture; used by btnStartClick and auto-start */
	void StartCapture(void);
	/** \brief Re-sync on-screen controls with appSettings */
	void UpdateUi(void);
	void PaintOnImage(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmWS2812SpectrumAnalyzer *frmWS2812SpectrumAnalyzer;
//---------------------------------------------------------------------------
#endif
