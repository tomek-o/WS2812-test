//---------------------------------------------------------------------------

#ifdef __BORLANDC__
#	pragma hdrstop
#endif

#include "ComPort.h"
#include "Log.h"
#include <stdlib.h>
#include "msports.h"

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#	pragma package(smart_init)
#endif

namespace {

const char* THIS_FILE = "ComPort";

std::vector<struct ComPort::S_COM_PORT> ComPort::ports;

std::string GetErrorStr(DWORD error)
{
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr+bufLen);
      
      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}

VOID CALLBACK DoneCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	LOG("DoneCallback, dwErrorCode = %u, numberOfBytesTransferred = %u", dwErrorCode, dwNumberOfBytesTransfered);
	// process data
	//SetEvent(lpOverlapped->hEvent);
//	rxBufferFilled = dwNumberOfBytesTransfered;
}

}

ComPort comPort;

int ComPort::EnumeratePorts(void)
{
    OSVERSIONINFO osvinfo;
    memset(&osvinfo, 0, sizeof(osvinfo));
    osvinfo.dwOSVersionInfoSize=sizeof(osvinfo);

    GetVersionEx(&osvinfo);

    switch (osvinfo.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_WINDOWS:
		return W9xEnumeratePorts();
	case VER_PLATFORM_WIN32_NT:
		if (osvinfo.dwMajorVersion > 4)
            return W2kEnumeratePorts();
        else
			return ERR_UNSUPPORTED_OS;
    default:
        return ERR_UNSUPPORTED_OS;
    }
}

enum ComPort::E_ERR ComPort::W9xEnumeratePorts(void)
{
	return ScanEnumBranch("ENUM");
}

enum ComPort::E_ERR ComPort::W2kEnumeratePorts(void)
{
    return ScanEnumBranch("SYSTEM\\CURRENTCONTROLSET\\ENUM");
}

std::string ComPort::GetRegistryString(const HKEY hKey, const std::string& name)
{
    DWORD type;
    DWORD size;
	if (RegQueryValueEx(hKey, name.c_str(), NULL, &type, NULL, &size) != ERROR_SUCCESS || type != REG_SZ && type != REG_EXPAND_SZ)
		return "";	// not found
	if (size > 0)
	{
		char* buffer = new char[size + 1];
		RegQueryValueEx(hKey, name.c_str(), NULL, NULL, (BYTE*) buffer, &size);
		buffer[size] = 0;
		std::string result(buffer);
		delete [] buffer;
		return result;
	}
	return "";		// empty
}

std::string ComPort::OpenRegistrySubkey(const HKEY hKey,
	DWORD index, REGSAM regsam,PHKEY phkResult)
{
	FILETIME filetime;
	DWORD size;
	DWORD MaxSubKeyLen = 0;
	DWORD SubKeys = 0;
	DWORD status = RegQueryInfoKey(hKey, NULL, NULL, NULL,
		&SubKeys,&MaxSubKeyLen,NULL,NULL,
		NULL,NULL,NULL,NULL);
	//if((status == ERROR_SUCCESS) && SubKeys)
	if (status != ERROR_SUCCESS)
	{
		/** \todo RegQueryInfoKey returns 5 not allowing to get maximum subkey size in ENUM branch */
		MaxSubKeyLen = 512;
	}

	size = MaxSubKeyLen;
	char* buffer = new char[size + 1];
	RegEnumKeyEx(hKey, index, buffer, &size, 0, NULL, NULL, &filetime);
	buffer[size] = 0;
	std::string result(buffer);
	delete [] buffer;
	if (RegOpenKeyEx(hKey, result.c_str(), 0, regsam, phkResult) == ERROR_SUCCESS)
		return result;
	return "";		// empty
}

enum ComPort::E_ERR ComPort::ScanEnumBranch(std::string path)
{
	static const char lpstrPortsClass[] = "PORTS";
	static const char lpstrPortsClassGUID[] = "{4D36E978-E325-11CE-BFC1-08002BE10318}";
	char lpClass[sizeof(lpstrPortsClass)];
	DWORD  cbClass;
	char lpClassGUID[sizeof(lpstrPortsClassGUID)];
	DWORD  cbClassGUID;

	ports.clear();
	struct S_COM_PORT port;

	HKEY hkEnum;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_ENUMERATE_SUB_KEYS, &hkEnum))
		return ERR_REGISTRY_READ;

	for (DWORD dwIndex1=0; ; ++dwIndex1)
	{
		HKEY hkLevel1;
		port.technology = OpenRegistrySubkey(hkEnum, dwIndex1, KEY_ENUMERATE_SUB_KEYS, &hkLevel1);
		if (port.technology == "")
		{
			RegCloseKey(hkEnum);
			return ERR_NONE;
		}

		for (DWORD dwIndex2=0; ; ++dwIndex2)
		{
			HKEY hkLevel2;
			std::string tmp = OpenRegistrySubkey(hkLevel1,
				dwIndex2, KEY_ENUMERATE_SUB_KEYS, &hkLevel2);
			if (tmp == "")
			{
				RegCloseKey(hkLevel2);
				break;
			}

			for (DWORD dwIndex3=0; ; ++dwIndex3)
            {
				HKEY hkLevel3;
				tmp = OpenRegistrySubkey(hkLevel2, dwIndex3, KEY_READ, &hkLevel3);
				if (tmp == "")
				{
					RegCloseKey(hkLevel3);
					break;
				}
				// search for "CLASS" or "CLASSGUID"
				cbClass=sizeof(lpClass);
				if (RegQueryValueEx(hkLevel3,TEXT("CLASS"),NULL,NULL,
									(LPBYTE)lpClass,&cbClass) != ERROR_SUCCESS ||
					strcmp(lpClass,lpstrPortsClass))
				{
					cbClassGUID=sizeof(lpClassGUID);
					if (RegQueryValueEx(hkLevel3,TEXT("CLASSGUID"),NULL,NULL,
										(LPBYTE)lpClassGUID,&cbClassGUID) != ERROR_SUCCESS ||
						strcmp(lpClassGUID,lpstrPortsClassGUID))
					{
						RegCloseKey(hkLevel3);
						continue;
					}
				}

				port.name = GetRegistryString(hkLevel3, "PORTNAME");
				if (port.name == "")
				{
					HKEY hkDevParameters;
					if (RegOpenKeyEx(hkLevel3, "DEVICE PARAMETERS", 0, KEY_READ,
						&hkDevParameters) == ERROR_SUCCESS)
					{
						port.name = GetRegistryString(hkDevParameters, "PORTNAME");
						RegCloseKey(hkDevParameters);
					}
				}
                if (port.name.find("COM", 0) != std::string::npos)
                {
					port.description = GetRegistryString(hkLevel3, "FRIENDLYNAME");
					ports.push_back(port);
                }
				RegCloseKey(hkLevel3);
			}
			RegCloseKey(hkLevel2);
		}
        RegCloseKey(hkLevel1);
    }
}

int ComPort::EnumeratePortsAlt(void)
{
	ports.clear();

	HKEY hkCommMap;
	if (ERROR_SUCCESS != RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
		0,
		KEY_QUERY_VALUE,
		&hkCommMap))
	{
        return ERR_REGISTRY_READ;
	}

	void* pValNameBuff = 0;
	void* pValueBuff = 0;
	__try
	{
		DWORD dwValCount, dwMaxCharValNameLen, dwMaxWideValueSize;
		if (ERROR_SUCCESS != RegQueryInfoKey(
			hkCommMap,
			NULL, NULL, NULL, NULL, NULL, NULL,
			&dwValCount,
			&dwMaxCharValNameLen,
			&dwMaxWideValueSize,
			NULL, NULL))
		{
	    	RegCloseKey(hkCommMap);
    		return ERR_REGISTRY_READ;
		}

		// The max value name size is returned in TCHARs not including
		// the terminating null character.
		const dwMaxCharValNameSize = dwMaxCharValNameLen + 1;
		pValNameBuff = malloc(dwMaxCharValNameSize*(sizeof (TCHAR)));
		if (!pValNameBuff)
		{
            return -1;
		}

		// The max value size is returned in bytes needed to hold the UNICODE
		// strings including terminating null character regardless of the
		// function type (ANSI or UNICODE).
		const dwMaxByteValueSize = (dwMaxWideValueSize/2)*(sizeof (TCHAR));
		pValueBuff = malloc(dwMaxByteValueSize);
		if (!pValueBuff)
		{
            return -1;
		}

		for (DWORD dwIndex = 0; dwIndex < dwValCount; ++dwIndex)
		{
			DWORD dwCharValNameSize = dwMaxCharValNameSize;
			DWORD dwByteValueSize = dwMaxByteValueSize;
			DWORD dwType;
			LONG nRes = RegEnumValue(
				hkCommMap,
				dwIndex,
				(LPTSTR)pValNameBuff,
				&dwCharValNameSize,
				NULL,
				&dwType,
				(LPBYTE)pValueBuff,
				&dwByteValueSize);
			if (nRes != ERROR_SUCCESS)
			{
				break;
			}
			if (dwType != REG_SZ)
			{
				continue;
			}

    		struct S_COM_PORT port;
	    	port.name = (char*)pValueBuff;
    		ports.push_back(port);
		}
	}
	__finally
	{
		free(pValueBuff);
		free(pValNameBuff);
		RegCloseKey(hkCommMap);
	}
    return 0;
}

int ComPort::ReleasePortNumbers(std::vector<int> &ports)
{
	HANDLE hInstance = LoadLibrary("msports.dll");
	if (!hInstance)
		return -1;
	typedef LONG WINAPI (*pComDBOpen) (PHCOMDB PHComDB);
	typedef LONG WINAPI (*pComDBClose) (HCOMDB PHComDB);
	typedef LONG WINAPI (*pComDBReleasePort) (HCOMDB   HComDB, DWORD ComNumber);

	pComDBOpen pComDBOpenFn = (pComDBOpen)		GetProcAddress(hInstance, "ComDBOpen");
	pComDBClose pComDBCloseFn = (pComDBClose)		GetProcAddress(hInstance, "ComDBClose");
	pComDBReleasePort pComDBReleasePortFn = (pComDBReleasePort)   GetProcAddress(hInstance, "ComDBReleasePort");
	if (pComDBOpenFn == NULL ||
		pComDBCloseFn == NULL ||
		pComDBReleasePortFn == NULL)
	{
		FreeLibrary(hInstance);
		return -2;
	}

	//update COM port database
	HCOMDB comDB;
	LONG status;
	if ((status = pComDBOpenFn(&comDB)) != ERROR_SUCCESS)
	{
		FreeLibrary(hInstance);
		return ERR_COMDB;
	}
	for (unsigned int i=0; i<ports.size(); i++)
	{
		if ((status = pComDBReleasePortFn(comDB, ports[i])) != ERROR_SUCCESS)
		{
			pComDBCloseFn(comDB);
			FreeLibrary(hInstance);
			return ERR_COMDBRELEASE;
		}
	}
	pComDBCloseFn(comDB);
	(void)status;
	FreeLibrary(hInstance);
	return ERR_NONE;
}

ComPort::ComPort(void):
	handle(INVALID_HANDLE_VALUE),
	rxBufferFilled(0)
{
}

int ComPort::Open(std::string name, unsigned int baudrate, bool overlapped)
{
	Close();

	DCB dcb;

	handle = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, overlapped?FILE_FLAG_OVERLAPPED:0, NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		LOG("COM port open failed\n");
		return -2;
	}

	enum { OUT_QUEUE_SIZE =	8192 };
	enum { IN_QUEUE_SIZE = 8192 };

	SetupComm(handle, IN_QUEUE_SIZE, OUT_QUEUE_SIZE);

	dcb.DCBlength = sizeof(dcb);

	GetCommState(handle, &dcb);

	dcb.BaudRate = baudrate;
	dcb.fParity = FALSE; //TRUE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 7;

	int hw_ctrl = 0;

	if (hw_ctrl == 0)
	{
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		//dcb.fDtrControl = DTR_CONTROL_ENABLE;
		//dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDsrSensitivity = FALSE;
		dcb.fAbortOnError = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.fErrorChar = FALSE;
		dcb.fNull = FALSE;
	}
	else
	{
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
//		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fRtsControl = RTS_CONTROL_TOGGLE;

		dcb.fOutxCtsFlow = TRUE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDsrSensitivity = FALSE;
		dcb.fAbortOnError = FALSE;
		dcb.fOutX = FALSE;
		dcb.fInX = FALSE;
		dcb.fErrorChar = FALSE;
		dcb.fNull = FALSE;
	}
	

	SetCommState(handle, &dcb);

	COMMTIMEOUTS ComTimeouts;
	GetCommTimeouts(handle, &ComTimeouts);

	ComTimeouts.ReadIntervalTimeout = 1;
	ComTimeouts.ReadTotalTimeoutMultiplier = 1;
	ComTimeouts.ReadTotalTimeoutConstant = 2;
	ComTimeouts.WriteTotalTimeoutMultiplier = 500;
	ComTimeouts.WriteTotalTimeoutConstant = 1000;

	SetCommTimeouts(handle, &ComTimeouts);

	PurgeComm(handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	return ERR_NONE;
}

int ComPort::Close(void)
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
	return ERR_NONE;
}

int ComPort::ReadOverlapped(unsigned int count)
{
	if (count > sizeof(rxBuffer))
	{
		LOG("ReadOverlapped: internal buffer too small!\n");
		return -1;
	}

	if (handle == INVALID_HANDLE_VALUE)
		return -2;

	memset(&ov, 0, sizeof(ov));
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = 0;//INVALID_HANDLE_VALUE; //eventHandle; // ignored completely by ReadFileEx?

	memset(rxBuffer, 0, sizeof(rxBuffer));

	if (!ReadFileEx(handle, rxBuffer, count, &ov, DoneCallback))
	{
		DWORD error = GetLastError();
		if (error != ERROR_IO_PENDING)
		{
			return -1;
		}
	}
	else
	{
		DWORD error = GetLastError();
		LOG("ReadFileEx: success, GetLastError = %d (%s)", error, GetErrorStr(error).c_str());
	}
	return 0;
}

BOOL ReadComm(HANDLE hCommDev, LPVOID lpBuffer, LPDWORD lpNumberOfBytesRead, DWORD BufSize)
{
	*lpNumberOfBytesRead = 0;
	return ReadFile(hCommDev, lpBuffer, BufSize, lpNumberOfBytesRead, NULL);
}

BOOL WriteComm(HANDLE hCommDev, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	DWORD nNumberOfBytesWritten;
	BOOL writeResult = WriteFile(hCommDev, lpBuffer, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL);
	if (nNumberOfBytesWritten != nNumberOfBytesToWrite)
	{
		return FALSE;
	}
	return writeResult;
}

