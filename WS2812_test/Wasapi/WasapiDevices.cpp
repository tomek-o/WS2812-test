//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#include "WasapiDevices.h"
#include "WasapiMinimal.h"
#include "Log.h"
#include <process.h>
#include <string.h>

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#	pragma package(smart_init)
#endif

#define PROMPT "WasapiDevices: "

namespace
{

struct EnumThreadParams
{
	std::vector<WasapiDeviceInfo> *devices;
	EDataFlow flow;
	bool success;
};

std::string WideToAnsi(const wchar_t *wide)
{
	if (wide == NULL)
		return "";
	int size = WideCharToMultiByte(CP_ACP, 0, wide, -1, NULL, 0, NULL, NULL);
	if (size <= 0)
		return "";
	std::vector<char> buf(size);
	WideCharToMultiByte(CP_ACP, 0, wide, -1, &buf[0], size, NULL, NULL);
	return std::string(&buf[0]);
}

std::string GetDeviceFriendlyName(IMMDevice *device)
{
	std::string name;

	IPropertyStore *store = NULL;
	if (FAILED(device->OpenPropertyStore(STGM_READ, (void**)&store)) || store == NULL)
		return name;

	PROPVARIANT variant;
	memset(&variant, 0, sizeof(variant));
	if (SUCCEEDED(store->GetValue(PKEY_Device_FriendlyName, &variant)))
	{
		if (variant.vt == VT_LPWSTR && variant.pwszVal != NULL)
		{
			name = WideToAnsi(variant.pwszVal);
			CoTaskMemFree(variant.pwszVal);
		}
	}

	store->Release();
	return name;
}

unsigned __stdcall EnumThreadProc(void *param)
{
	EnumThreadParams *params = static_cast<EnumThreadParams*>(param);
	params->success = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		LOG(PROMPT"CoInitializeEx failed: 0x%08X\n", hr);
		return 0;
	}

	IMMDeviceEnumerator *enumerator = NULL;
	IMMDeviceCollection *collection = NULL;

	do
	{
		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
			IID_IMMDeviceEnumerator, (void**)&enumerator);
		if (FAILED(hr))
		{
			LOG(PROMPT"CoCreateInstance failed: 0x%08X\n", hr);
			break;
		}

		hr = enumerator->EnumAudioEndpoints(params->flow, DEVICE_STATE_ACTIVE, (void**)&collection);
		if (FAILED(hr))
		{
			LOG(PROMPT"EnumAudioEndpoints failed: 0x%08X\n", hr);
			break;
		}

		UINT count = 0;
		hr = collection->GetCount(&count);
		if (FAILED(hr))
		{
			LOG(PROMPT"GetCount failed: 0x%08X\n", hr);
			break;
		}

		for (UINT i = 0; i < count; i++)
		{
			IMMDevice *device = NULL;
			if (FAILED(collection->Item(i, &device)) || device == NULL)
				continue;

			WasapiDeviceInfo info;
			LPWSTR idW = NULL;
			if (SUCCEEDED(device->GetId(&idW)) && idW != NULL)
			{
				info.id = WideToAnsi(idW);
				CoTaskMemFree(idW);
			}
			info.name = GetDeviceFriendlyName(device);
			if (info.name.empty())
				info.name = info.id;

			if (!info.id.empty())
				params->devices->push_back(info);

			device->Release();
		}

		params->success = true;
	}
	while (false);

	if (collection != NULL)
		collection->Release();
	if (enumerator != NULL)
		enumerator->Release();

	CoUninitialize();
	return 0;
}

bool EnumerateDevices(EDataFlow flow, std::vector<WasapiDeviceInfo> &devices)
{
	devices.clear();

	EnumThreadParams params;
	params.devices = &devices;
	params.flow = flow;
	params.success = false;

	unsigned int threadId = 0;
	HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, EnumThreadProc, &params, 0, &threadId);
	if (thread == NULL)
	{
		LOG(PROMPT"Failed to start enumeration thread\n");
		return false;
	}

	WaitForSingleObject(thread, 5000);
	CloseHandle(thread);

	return params.success;
}

}	// namespace

bool EnumerateRenderDevices(std::vector<WasapiDeviceInfo> &devices)
{
	return EnumerateDevices(eRender, devices);
}

bool EnumerateCaptureDevices(std::vector<WasapiDeviceInfo> &devices)
{
	return EnumerateDevices(eCapture, devices);
}
