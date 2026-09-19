/** \file
	\brief Enumerate active audio render (output) or capture (input) devices,
	with friendly names
*/
//---------------------------------------------------------------------------
#ifndef WasapiDevicesH
#define WasapiDevicesH
//---------------------------------------------------------------------------

#include <string>
#include <vector>

struct WasapiDeviceInfo
{
	std::string id;		///< IMMDevice::GetId(), stable across reboots
	std::string name;	///< friendly name; falls back to id if unavailable
};

/** \brief Enumerate active render (output/playback) devices
	Runs its own short-lived worker thread internally (COM never touches
	the calling thread) and blocks until it's done.
	\return true on success
*/
bool EnumerateRenderDevices(std::vector<WasapiDeviceInfo> &devices);

/** \brief Enumerate active capture (input/microphone) devices
	Same behavior as EnumerateRenderDevices, just the other data flow.
	\return true on success
*/
bool EnumerateCaptureDevices(std::vector<WasapiDeviceInfo> &devices);

#endif
