#ifndef SpectrumAnalyzerConfH
#define SpectrumAnalyzerConfH

#include <System.hpp>

namespace Json
{
	class Value;
}

struct SpectrumAnalyzerConf
{
	enum { MIN_MAX_BRIGHTNESS = 2, DEFAULT_MAX_BRIGHTNESS = 20, MAX_MAX_BRIGHTNESS = 255 };
	/** \note floor set by Win32 timer resolution (~15 ms); WM_TIMER can lag further */
	enum { MIN_UPDATE_INTERVAL_MS = 15, MAX_UPDATE_INTERVAL_MS = 100 };
	enum { MIN_METER_FLOOR_DB = -80, MAX_METER_FLOOR_DB = -10 };
	enum { MIN_GAIN_X10 = 1, MAX_GAIN_X10 = 50 };	///< gain slider range, gain*10
	enum { MIN_SMOOTHING_PCT = 0, MAX_SMOOTHING_PCT = 95 };
	enum { MIN_LEDS_PER_BAND = 1, DEFAULT_LEDS_PER_BAND = 8, MAX_LEDS_PER_BAND = 128 };
	enum { MIN_FREQ_HZ = 20, MAX_FREQ_HZ = 20000 };

	float gain;					///< multiplier applied to band amplitudes
	bool autoStart;				///< start audio capture at application startup
	unsigned int maxBrightness;	///< cap (0-255) on any single R/G/B channel, to limit current draw
	unsigned int updateIntervalMs;	///< effect refresh interval; lower = faster/smoother
	int meterFloorDb;				///< dB level mapped to bar position 0 (0 dB = full scale = top)
	bool peakDetect;				///< show a peak-hold marker; reserves the top LED of each band
	unsigned int smoothingPct;		///< 0 = instant; higher = smoother but slower to rise/fall
	unsigned int ledsPerBand;		///< LEDs per spectrum band; band count = ledCount / ledsPerBand
	unsigned int minFreqHz;			///< lower edge of the first (lowest) band
	unsigned int maxFreqHz;			///< upper edge of the last (highest) band
	bool alternateDirection;		///< every other band grows the opposite way (serpentine-wired matrix)
	AnsiString audioDeviceId;		///< IMMDevice id to capture from; empty = system default
	bool audioInput;				///< false (default) = output/loopback; true = microphone/input

	SpectrumAnalyzerConf(void);

	void fromJson(const Json::Value &jv);
	void toJson(Json::Value &jv) const;
};

#endif
