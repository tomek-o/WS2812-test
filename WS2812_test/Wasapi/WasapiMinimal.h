/** \file
	\brief Minimal WASAPI declarations

	BDS2006's SDK predates Vista, so mmdeviceapi.h/audioclient.h don't exist
	here. Interfaces are hand-declared in their real vtable order - these are
	calls into the OS's mmdevapi.dll, not something implemented locally.
*/
//---------------------------------------------------------------------------
#ifndef WasapiMinimalH
#define WasapiMinimalH
//---------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <mmsystem.h>	// WAVEFORMATEX

//---------------------------------------------------------------------------
// mmdeviceapi.h subset
//---------------------------------------------------------------------------

/** \note BDS2006 defaults to 1-byte size to small enums which breaks COM ABI.
	On the other hand swithing to int-sized enum in settings broken more things.
*/
typedef int EDataFlow;

#define eRender   ((EDataFlow)0)
#define eCapture  ((EDataFlow)1)
#define eAll      ((EDataFlow)2)

const char* GetEDataFlowName(EDataFlow flow);

/** \note BDS2006 defaults to 1-byte size to small enums which breaks COM ABI.
	On the other hand swithing to int-sized enum in settings broken more things.
*/
typedef int ERole;

#define eConsole       ((ERole)0)
#define eMultimedia    ((ERole)1)
#define eCommunications ((ERole)2)

#define DEVICE_STATE_ACTIVE 0x1

struct IMMDevice;
struct IMMDeviceCollection;

DECLARE_INTERFACE_(IMMDeviceEnumerator, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    STDMETHOD(EnumAudioEndpoints)(
        THIS_
        EDataFlow dataFlow,
        DWORD dwStateMask,
        IMMDeviceCollection **ppDevices) PURE;

    STDMETHOD(GetDefaultAudioEndpoint)(
        THIS_
        EDataFlow dataFlow,
        ERole role,
        IMMDevice **ppEndpoint) PURE;

    STDMETHOD(GetDevice)(
        THIS_
        LPCWSTR pwstrId,
        IMMDevice **ppDevice) PURE;

    STDMETHOD(RegisterEndpointNotificationCallback)(
        THIS_
        void *pClient) PURE;

    STDMETHOD(UnregisterEndpointNotificationCallback)(
        THIS_
        void *pClient) PURE;
};

DECLARE_INTERFACE_(IMMDevice, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	/** pActivationParams is really a PROPVARIANT*; void* is fine since we
		always pass NULL and only pointer size matters for the ABI. */
	STDMETHOD(Activate)(THIS_ REFIID iid, DWORD dwClsCtx, void *pActivationParams, void **ppInterface) PURE;
	STDMETHOD(OpenPropertyStore)(THIS_ DWORD stgmAccess, void **ppProperties) PURE;
	STDMETHOD(GetId)(THIS_ LPWSTR *ppstrId) PURE;
	STDMETHOD(GetState)(THIS_ DWORD *pdwState) PURE;
};

DECLARE_INTERFACE_(IMMDeviceCollection, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD(GetCount)(THIS_ UINT *pcDevices) PURE;
	STDMETHOD(Item)(THIS_ UINT nDevice, IMMDevice **ppDevice) PURE;
};

/** \brief Stand-in for the Vista Property System's PROPERTYKEY (fmtid+pid);
	same 20-byte layout, different name only to avoid clashing with any
	system declaration that may or may not already exist in this old SDK.
*/
struct WasapiPropertyKey
{
	GUID fmtid;
	unsigned long pid;
};

/** \note real PROPVARIANT (from wtypes.h) is used as-is: it's an ancient
	OLE Automation/structured-storage type, already available here.
*/
DECLARE_INTERFACE_(IPropertyStore, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD(GetCount)(THIS_ DWORD *cProps) PURE;
	STDMETHOD(GetAt)(THIS_ DWORD iProp, WasapiPropertyKey *pkey) PURE;
	STDMETHOD(GetValue)(THIS_ const WasapiPropertyKey &key, PROPVARIANT *pv) PURE;
	STDMETHOD(SetValue)(THIS_ const WasapiPropertyKey &key, const void *propvar) PURE;
	STDMETHOD(Commit)(THIS) PURE;
};

//---------------------------------------------------------------------------
// audioclient.h subset
//---------------------------------------------------------------------------

typedef __int64 REFERENCE_TIME;

typedef enum _AUDCLNT_SHAREMODE
{
	AUDCLNT_SHAREMODE_SHARED = 0,
	AUDCLNT_SHAREMODE_EXCLUSIVE = 1
} AUDCLNT_SHAREMODE;

#define AUDCLNT_STREAMFLAGS_EVENTCALLBACK   0x00040000
#define AUDCLNT_STREAMFLAGS_LOOPBACK        0x00020000
#define AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY 0x1
#define AUDCLNT_BUFFERFLAGS_SILENT              0x2

struct IAudioCaptureClient;

DECLARE_INTERFACE_(IAudioClient, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD(Initialize)(THIS_ AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags,
		REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity,
		const WAVEFORMATEX *pFormat, LPCGUID AudioSessionGuid) PURE;
	STDMETHOD(GetBufferSize)(THIS_ UINT32 *pNumBufferFrames) PURE;
	STDMETHOD(GetStreamLatency)(THIS_ REFERENCE_TIME *phnsLatency) PURE;
	STDMETHOD(GetCurrentPadding)(THIS_ UINT32 *pNumPaddingFrames) PURE;
	STDMETHOD(IsFormatSupported)(THIS_ AUDCLNT_SHAREMODE ShareMode,
		const WAVEFORMATEX *pFormat, WAVEFORMATEX **ppClosestMatch) PURE;
	STDMETHOD(GetMixFormat)(THIS_ WAVEFORMATEX **ppDeviceFormat) PURE;
	STDMETHOD(GetDevicePeriod)(THIS_ REFERENCE_TIME *phnsDefaultDevicePeriod,
		REFERENCE_TIME *phnsMinimumDevicePeriod) PURE;
	STDMETHOD(Start)(THIS) PURE;
	STDMETHOD(Stop)(THIS) PURE;
	STDMETHOD(Reset)(THIS) PURE;
	STDMETHOD(SetEventHandle)(THIS_ HANDLE eventHandle) PURE;
	STDMETHOD(GetService)(THIS_ REFIID riid, void **ppv) PURE;
};

DECLARE_INTERFACE_(IAudioCaptureClient, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD(GetBuffer)(THIS_ BYTE **ppData, UINT32 *pNumFramesToRead,
		DWORD *pdwFlags, UINT64 *pu64Position, UINT64 *pu64QPCPosition) PURE;
	STDMETHOD(ReleaseBuffer)(THIS_ UINT32 NumFramesRead) PURE;
	STDMETHOD(GetNextPacketSize)(THIS_ UINT32 *pNumFramesInNextPacket) PURE;
};

//---------------------------------------------------------------------------
// public GUID values, defined in WasapiMinimal.cpp
//---------------------------------------------------------------------------

extern const GUID CLSID_MMDeviceEnumerator;
extern const GUID IID_IMMDeviceEnumerator;
extern const GUID IID_IAudioClient;
extern const GUID IID_IAudioCaptureClient;
extern const WasapiPropertyKey PKEY_Device_FriendlyName;


#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
DEFINE_GUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,
	0x00000003, 0x0000, 0x0010,
	0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);
DEFINE_GUID(KSDATAFORMAT_SUBTYPE_PCM,
	0x00000001, 0x0000, 0x0010,
	0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

typedef struct
{
	WAVEFORMATEX Format;
	WORD wValidBitsPerSample;
	DWORD dwChannelMask;
	GUID SubFormat;
} WAVEFORMATEXTENSIBLE;		

#endif
