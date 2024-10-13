/** \file
	\brief COM port module (WinAPI)
	\date May 2009

	COM port module. Should work w2k, wXP
	and hopefully newer versions.
	Enumeration of currently present ports may not work with win98.
*/

//---------------------------------------------------------------------------
#ifndef ComPortH
#define ComPortH
//---------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>

class ComPort
{
private:
	enum E_ERR {
		ERR_NONE = 0,
		ERR_UNSUPPORTED_OS,
		ERR_REGISTRY_READ,
		ERR_NO_MEMORY,
		ERR_COMDB,
		ERR_COMDBRELEASE,
	};

	struct S_COM_PORT
	{
		std::string name;		///< "COMx"
		std::string description;///< "OTI USB to serial bridge (COMx)", available with EnumerateKnownPorts
		std::string technology;	///< "ACPI", "USB", available with EnumerateKnownPorts
	};
	static std::vector<struct S_COM_PORT> ports;
	static enum E_ERR W9xEnumeratePorts(void);
	static enum E_ERR W2kEnumeratePorts(void);
	static enum E_ERR ScanEnumBranch(std::string path);
	/** Read string from registry
		\return empty string on error (non distinguished from empty value)
	*/
	static std::string GetRegistryString(const HKEY hKey, const std::string& name);
	/** Open registry subkey at index and return it's name
		\return empty string on error
	*/
	static std::string OpenRegistrySubkey(const HKEY hKey,
		DWORD index, REGSAM regsam, ::PHKEY phkResult);

	HANDLE handle;
	OVERLAPPED ov;	
	unsigned char rxBuffer[8*1024];
	unsigned int rxBufferFilled;
public:
	/** \brief Enumerate available COM ports
		\return 0 on success
	*/
	static int EnumeratePorts(void);
	/** \brief Alternative way to enumerate currently available COM ports
		\return 0 on success
	*/
	static int EnumeratePortsAlt(void);
	static std::vector<struct S_COM_PORT>& GetPorts(void) {
    	return ports;
	}
	/** \brief Release COM port numbers
		\return 0 on success
	*/
	static int ReleasePortNumbers(std::vector<int> &ports);

	ComPort(void);
	int Open(std::string name, unsigned int baudrate, bool overlapped);
	int Close(void);
	HANDLE GetHandle(void) {
		return handle;
	}

	bool isOpened(void) const {
		return (handle != INVALID_HANDLE_VALUE);
	}

	int ReadOverlapped(unsigned int count);
};


BOOL ReadComm(HANDLE hCommDev, LPVOID lpBuffer, LPDWORD lpNumberOfBytesRead, DWORD BufSize);
BOOL WriteComm(HANDLE hCommDev, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);


extern ComPort comPort;


//---------------------------------------------------------------------------
#endif
