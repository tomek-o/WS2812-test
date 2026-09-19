//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "WasapiDevices.h"
#include "WasapiMinimal.h"
#include "Log.h"
#include "common/StaticCheck.h"
#include <string.h>

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#   pragma package(smart_init)
#endif

#define PROMPT "WasapiDevices: "

namespace
{

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
	if (FAILED(device->OpenPropertyStore(STGM_READ, (void**)&store)) ||
        store == NULL)
    {
        return name;
    }

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

bool EnumerateDevices(
    EDataFlow flow,
    std::vector<WasapiDeviceInfo> &devices)
{
    devices.clear();

    // Initialize COM on the calling thread.
    //
    // If COM is already initialized with a different apartment model,
    // CoInitializeEx returns RPC_E_CHANGED_MODE. In that case COM is
    // already available to this thread, so continue without taking
    // ownership of its COM initialization.

    HRESULT comHr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    if (FAILED(comHr) && comHr != RPC_E_CHANGED_MODE)
    {
        LOG(PROMPT"CoInitializeEx failed: 0x%08X\n", comHr);
        return false;
    }

    bool success = false;

    IMMDeviceEnumerator *enumerator = NULL;
    IMMDeviceCollection *collection = NULL;

    do
    {
		HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator);
        if (FAILED(hr))
        {
            LOG(PROMPT"CoCreateInstance failed: 0x%08X\n", hr);
            break;
        }

		LOG(PROMPT"Enumerating [%s] devices\n", GetEDataFlowName(flow));

		hr = enumerator->EnumAudioEndpoints((DWORD)flow, DEVICE_STATE_ACTIVE, &collection);
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

        LOG(PROMPT"Devices count = %u\n", count);

        for (UINT i = 0; i < count; ++i)
        {
            IMMDevice *device = NULL;

			hr = collection->Item(i, &device);
            if (FAILED(hr) || device == NULL)
                continue;

            WasapiDeviceInfo info;

            LPWSTR idW = NULL;

            hr = device->GetId(&idW);
            if (SUCCEEDED(hr) && idW != NULL)
            {
                info.id = WideToAnsi(idW);
                CoTaskMemFree(idW);
            }

            info.name = GetDeviceFriendlyName(device);

            if (info.name.empty())
                info.name = info.id;

            if (!info.id.empty())
                devices.push_back(info);

            device->Release();
        }

        success = true;

    } while (false);

    LOG(PROMPT"Enumeration finished\n");

    if (collection != NULL)
        collection->Release();

    if (enumerator != NULL)
        enumerator->Release();

    // Only uninitialize COM if this function initialized it.
    if (SUCCEEDED(comHr))
        CoUninitialize();

    return success;
}

} // namespace

bool EnumerateRenderDevices(std::vector<WasapiDeviceInfo> &devices)
{
    return EnumerateDevices(eRender, devices);
}

bool EnumerateCaptureDevices(std::vector<WasapiDeviceInfo> &devices)
{
    return EnumerateDevices(eCapture, devices);
}
