#ifndef AudioVisualisationConfH
#define AudioVisualisationConfH

#include <System.hpp>

namespace Json
{
	class Value;
}

struct AudioVisualisationConf
{
	enum { MIN_MAX_BRIGHTNESS = 2, DEFAULT_MAX_BRIGHTNESS = 20, MAX_MAX_BRIGHTNESS = 255 };
	/** \note floor set by Win32 timer resolution (~15 ms); WM_TIMER can lag further */
	enum { MIN_UPDATE_INTERVAL_MS = 15, MAX_UPDATE_INTERVAL_MS = 100 };
	enum { MIN_METER_FLOOR_DB = -80, MAX_METER_FLOOR_DB = -10 };
	enum { MIN_GAIN_X10 = 1, MAX_GAIN_X10 = 50 };	///< gain slider range, gain*10
	enum { MIN_SMOOTHING_PCT = 0, MAX_SMOOTHING_PCT = 95 };

	/** \brief How the two bar halves grow, when stereoSeparate is on
		DEFAULT: left grows from the outer edge inward, right grows from the
			center outward (existing behavior, kept as a selectable option)
		OUTWARD: both channels anchored at the center, growing outward
			towards their edge as level rises
		INWARD: both channels anchored at their outer edge, growing inward
			towards the center as level rises
	*/
	enum { STEREO_DIRECTION_DEFAULT = 0, STEREO_DIRECTION_OUTWARD = 1, STEREO_DIRECTION_INWARD = 2 };

	float gain;					///< multiplier applied to the captured audio level
	bool stereoSeparate;		///< false: mix L+R into one bar; true: L drives one half, R the other
	unsigned int stereoDirection;	///< one of STEREO_DIRECTION_*, only relevant when stereoSeparate is on
	bool autoStart;				///< start WASAPI loopback capture at application startup
	unsigned int maxBrightness;	///< cap (0-255) on any single R/G/B channel, to limit current draw
	unsigned int updateIntervalMs;	///< effect refresh interval; lower = faster/smoother
	int meterFloorDb;				///< dB level mapped to bar position 0 (0 dB = full scale = top)
	bool peakDetect;				///< show a peak-hold marker; reserves the top LED for it
	unsigned int smoothingPct;		///< 0 = instant; higher = smoother but slower to rise/fall
	AnsiString audioDeviceId;		///< IMMDevice id to capture from; empty = system default
	bool audioInput;				///< false (default) = output/loopback; true = microphone/input

	AudioVisualisationConf(void);

	void fromJson(const Json::Value &jv);
	void toJson(Json::Value &jv) const;
};

#endif
