//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#include "WasapiLoopbackCapture.h"
#include "Log.h"
#include "common/ScopedLock.h"
#include <process.h>
#include <math.h>
#include <vector>

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#	pragma package(smart_init)
#endif

#define PROMPT "WasapiCapture: "

namespace
{

/** \brief Per-channel RMS of one interleaved packet; extra channels ignored */
void ComputeRms(const BYTE *data, UINT32 frames, const WAVEFORMATEX *format, float &left, float &right)
{
	left = right = 0.0f;

	if (data == NULL || frames == 0 || format == NULL || format->nChannels == 0)
	{
		return;
	}

	bool isFloat = (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT);

	if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
	{
		const WAVEFORMATEXTENSIBLE *ext = reinterpret_cast<const WAVEFORMATEXTENSIBLE *>(format);

		isFloat = (ext->SubFormat.Data1 == WAVE_FORMAT_IEEE_FLOAT);
	}

	const UINT32 channels = format->nChannels;
	const UINT32 rightChannel = (channels >= 2) ? 1 : 0;

	double sumSquaresL = 0.0;
	double sumSquaresR = 0.0;

	if (isFloat && format->wBitsPerSample == 32)
	{
		const float *samples = reinterpret_cast<const float *>(data);

		for (UINT32 i = 0; i < frames; ++i)
		{
			double l = samples[i * channels];
			double r = samples[i * channels + rightChannel];

			sumSquaresL += l * l;
			sumSquaresR += r * r;
		}
	}
	else if (!isFloat && format->wBitsPerSample == 16)
	{
		const short *samples = reinterpret_cast<const short *>(data);
		for (UINT32 i = 0; i < frames; ++i)
		{
			double l = static_cast<double>(samples[i * channels]) / 32768.0;
			double r = static_cast<double>(samples[i * channels + rightChannel]) / 32768.0;
			sumSquaresL += l * l;
			sumSquaresR += r * r;
		}
	}
	else
	{
		// Unsupported format.
		// Don't interpret arbitrary data as float/short.
		LOG(PROMPT"Unsupported RMS format: tag=0x%04X, bits=%u\n", format->wFormatTag, format->wBitsPerSample);
		return;
	}

	left = static_cast<float>(sqrt(sumSquaresL / static_cast<double>(frames)));
	right = static_cast<float>(sqrt(sumSquaresR / static_cast<double>(frames)));
}

std::vector<wchar_t> AnsiToWide(const std::string &ansi)
{
	int len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, NULL, 0);
	std::vector<wchar_t> wide((len > 0) ? len : 1);
	if (len > 0)
		MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, &wide[0], len);
	else
		wide[0] = L'\0';
	return wide;
}

}	// namespace

//---------------------------------------------------------------------------
WasapiLoopbackCapture::WasapiLoopbackCapture(void):
	thread(NULL), stopEvent(NULL), running(false), isInput(false)
{
}

WasapiLoopbackCapture::~WasapiLoopbackCapture(void)
{
	Stop();
}

bool WasapiLoopbackCapture::Start(const char *deviceId, bool isInput)
{
	if (running)
		return true;

	this->deviceId = (deviceId != NULL) ? deviceId : "";
	this->isInput = isInput;

	stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (stopEvent == NULL)
	{
		LOG(PROMPT"Failed to create stop event\n");
		return false;
	}

	running = true;
	unsigned int threadId = 0;
	thread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, &threadId);
	if (thread == NULL)
	{
		LOG(PROMPT"Failed to start capture thread\n");
		running = false;
		CloseHandle(stopEvent);
		stopEvent = NULL;
		return false;
	}

	return true;
}

void WasapiLoopbackCapture::Stop(void)
{
	if (thread == NULL)
		return;

	SetEvent(stopEvent);
	WaitForSingleObject(thread, 5000);
	CloseHandle(thread);
	thread = NULL;
	CloseHandle(stopEvent);
	stopEvent = NULL;
	running = false;
}

bool WasapiLoopbackCapture::PopLevel(AudioLevel &level)
{
    ScopedLock<Mutex> lock(mutex);
	AudioLevel *item = fifo.getReadable();
	if (item == NULL)
		return false;
	level = *item;
	fifo.pop();
	return true;
}

unsigned __stdcall WasapiLoopbackCapture::ThreadProc(void *param)
{
	WasapiLoopbackCapture *self = static_cast<WasapiLoopbackCapture*>(param);
	self->Run();
	return 0;
}

void WasapiLoopbackCapture::Run(void)
{
    LOG(PROMPT"Starting capture thread\n");

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		LOG(PROMPT"CoInitializeEx failed: 0x%08X\n", hr);
		running = false;
		return;
	}

	IMMDeviceEnumerator *enumerator = NULL;
	IMMDevice *device = NULL;
	IAudioClient *audioClient = NULL;
	IAudioCaptureClient *captureClient = NULL;
	WAVEFORMATEX *mixFormat = NULL;

	do
	{
		// Find device
		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator);

		if (FAILED(hr))
		{
			LOG(PROMPT"CoCreateInstance failed: 0x%08X\n", hr);
			break;
		}

		if (!deviceId.empty())
		{
			std::vector<wchar_t> wideId = AnsiToWide(deviceId);
			hr = enumerator->GetDevice(&wideId[0], &device);
			if (FAILED(hr))
			{
				LOG(PROMPT"GetDevice(%s) failed: 0x%08X, falling back to default\n", deviceId.c_str(), hr);
				device = NULL;
			}
		}

		if (device == NULL)
		{
			hr = enumerator->GetDefaultAudioEndpoint(isInput ? eCapture : eRender, eConsole, &device);
			if (FAILED(hr))
			{
				// no role-default assigned (seen on Windows 7, and even on
				// Windows 10 with some capture devices) even though a device
				// is present; fall back to the first active device instead
				// of giving up outright
				LOG(PROMPT"GetDefaultAudioEndpoint failed: 0x%08X, falling back to first active device\n", hr);

				IMMDeviceCollection *collection = NULL;
				HRESULT hrEnum = enumerator->EnumAudioEndpoints(isInput ? eCapture : eRender, DEVICE_STATE_ACTIVE, &collection);
				if (SUCCEEDED(hrEnum) && collection != NULL)
				{
					UINT count = 0;
					if (SUCCEEDED(collection->GetCount(&count)) && count > 0)
						collection->Item(0, &device);
					collection->Release();
				}

				if (device == NULL)
				{
					LOG(PROMPT"No active device found\n");
					break;
				}
			}
		}

		// Activate audio client
		hr = device->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audioClient);
		if (FAILED(hr))
		{
			LOG(PROMPT"Activate(IAudioClient) failed: 0x%08X\n", hr);
			break;
		}

		// Get device mix format
		hr = audioClient->GetMixFormat(&mixFormat);
		if (FAILED(hr))
		{
			LOG(PROMPT"GetMixFormat failed: 0x%08X\n", hr);
			break;
		}

		LOG(PROMPT"Format: %u Hz, %u channels, %u bits, tag=0x%04X\n",
			mixFormat->nSamplesPerSec, mixFormat->nChannels, mixFormat->wBitsPerSample, mixFormat->wFormatTag);

		// IMPORTANT:
		// Do NOT use AUDCLNT_STREAMFLAGS_EVENTCALLBACK here.
		// Windows 7 does not signal the event for event-driven
		// loopback capture. Timer/polling mode works on both Win7
		// and modern Windows.
		DWORD streamFlags = 0;

		if (!isInput)
			streamFlags |= AUDCLNT_STREAMFLAGS_LOOPBACK;

		hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, streamFlags, 0, 0, mixFormat, NULL);
		if (FAILED(hr))
		{
			LOG(PROMPT"Initialize failed: 0x%08X\n", hr);
			break;
		}

		// Get capture service
		hr = audioClient->GetService(IID_IAudioCaptureClient, (void**)&captureClient);
		if (FAILED(hr))
		{
			LOG(PROMPT"GetService(IAudioCaptureClient) failed: 0x%08X\n", hr);
			break;
		}

		// Start capture
		hr = audioClient->Start();
		if (FAILED(hr))
		{
			LOG(PROMPT"Start failed: 0x%08X\n", hr);
			break;
		}

		LOG(PROMPT"%s capture started, %u Hz, %u channels, %u bits\n",
			isInput ? "Input" : "Loopback",
			mixFormat->nSamplesPerSec, mixFormat->nChannels, mixFormat->wBitsPerSample);

		// Timer-driven capture.
		// 10 ms is more than fast enough for an RMS level meter and works on Windows 7 as well as Windows 10.
		bool stopRequested = false;

		while (!stopRequested)
		{
			DWORD waitResult = WaitForSingleObject(stopEvent, 10);

			if (waitResult == WAIT_OBJECT_0)
				break;

			if (waitResult == WAIT_FAILED)
			{
				LOG(PROMPT"WaitForSingleObject failed: %u\n", GetLastError());
				break;
			}

			// Drain all currently available packets.
			for (;;)
			{
				UINT32 packetLength = 0;

				hr = captureClient->GetNextPacketSize(&packetLength);
				if (FAILED(hr))
				{
					LOG(PROMPT"GetNextPacketSize failed: 0x%08X\n", hr);
					stopRequested = true;
					break;
				}

				if (packetLength == 0)
					break;

				BYTE *data = NULL;
				UINT32 framesAvailable = 0;
				DWORD flags = 0;

				hr = captureClient->GetBuffer(&data, &framesAvailable, &flags, NULL, NULL);
				if (FAILED(hr))
				{
					LOG(PROMPT"GetBuffer failed: 0x%08X\n", hr);
					stopRequested = true;
					break;
				}

				AudioLevel newSlot;

				if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
				{
					newSlot.left = 0.0f;
					newSlot.right = 0.0f;
				}
				else
				{
					ComputeRms(data, framesAvailable, mixFormat, newSlot.left, newSlot.right);
				}

				// Put level into FIFO.
				{
					ScopedLock<Mutex> lock(mutex);
					AudioLevel *slot = fifo.getWriteable();
					if (slot != NULL)
					{
						*slot = newSlot;
						fifo.push();
					}
					else
					{
						LOG(PROMPT"FIFO full, cannot write\n");
					}
				}

				hr = captureClient->ReleaseBuffer(framesAvailable);
				if (FAILED(hr))
				{
					LOG(PROMPT"ReleaseBuffer failed: 0x%08X\n", hr);
					stopRequested = true;
					break;
				}
			}
		}

		// Stop capture
		audioClient->Stop();

	}
	while (false);


	if (captureClient != NULL)
	{
		captureClient->Release();
		captureClient = NULL;
	}

	if (audioClient != NULL)
	{
		audioClient->Release();
		audioClient = NULL;
	}

	if (device != NULL)
	{
		device->Release();
		device = NULL;
	}

	if (enumerator != NULL)
	{
		enumerator->Release();
		enumerator = NULL;
	}

	if (mixFormat != NULL)
	{
		CoTaskMemFree(mixFormat);
		mixFormat = NULL;
	}

	CoUninitialize();

	running = false;

	LOG(PROMPT"Capture thread exiting\n");
}
