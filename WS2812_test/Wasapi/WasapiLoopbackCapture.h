/** \file
	\brief WASAPI capture: loopback on a render/output device (default), or
	direct capture from a capture/input device (microphone)

	Runs on its own worker thread - all COM calls happen there, never on the
	VCL main thread. Levels are handed off via a lock-free SPSC Fifo
	(common/fifo.h): the worker pushes, PopLevel() (UI thread) pops.
*/
//---------------------------------------------------------------------------
#ifndef WasapiLoopbackCaptureH
#define WasapiLoopbackCaptureH
//---------------------------------------------------------------------------

#include "WasapiMinimal.h"
#include "common/fifo.h"
#include "common/Mutex.h"
#include <string>

struct AudioLevel
{
	float left;		///< RMS level of one captured packet, roughly 0..1
	float right;	///< same as left when the source is mono
};

class WasapiLoopbackCapture
{
public:
	WasapiLoopbackCapture(void);
	~WasapiLoopbackCapture(void);

	/** \brief Start capture
		\param deviceId IMMDevice id (WasapiDeviceInfo::id) to capture from,
			or NULL/empty for the current default device (of whichever kind
			`isInput` selects). Falls back to the default device if this id
			can't be resolved.
		\param isInput false (default) = loopback on a render/output device;
			true = direct capture from a capture/input device (microphone)
		\return true if the worker thread started
	*/
	bool Start(const char *deviceId = NULL, bool isInput = false);
	/** \brief Stop capturing and join the worker thread */
	void Stop(void);
	bool IsRunning(void) const {
		return running;
	}

	/** \brief Pop the oldest pending level
		\return false if none pending
	*/
	bool PopLevel(AudioLevel &level);

	enum { SAMPLE_HISTORY = 4096 };	///< mono samples kept for GetLatestSamples()

	/** \brief Copy the most recent mono (L+R averaged) samples, oldest first
		\param count max samples to copy, up to SAMPLE_HISTORY
		\param sampleRate receives the capture sample rate (0 if not started yet)
		\return number of samples copied; less than count until enough audio was captured
	*/
	unsigned int GetLatestSamples(float *out, unsigned int count, unsigned int &sampleRate);

private:
	static unsigned __stdcall ThreadProc(void *param);
	void Run(void);
	void AppendSamples(const BYTE *data, UINT32 frames, const WAVEFORMATEX *format, bool silent);

	Mutex mutex;
	HANDLE thread;
	HANDLE stopEvent;
	volatile bool running;
	std::string deviceId;
	bool isInput;
	Fifo<AudioLevel, 64> fifo;
	float sampleHistory[SAMPLE_HISTORY];	///< ring buffer, guarded by mutex
	unsigned int sampleWritePos;
	unsigned int sampleCount;
	unsigned int sampleRate;

	WasapiLoopbackCapture(const WasapiLoopbackCapture&);
	WasapiLoopbackCapture& operator=(const WasapiLoopbackCapture&);
};

#endif
