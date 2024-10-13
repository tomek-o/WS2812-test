//---------------------------------------------------------------------------


#pragma hdrstop

#include "WS2812.h"
#include "ComPort.h"
#include "Log.h"
#include <System.hpp>

//---------------------------------------------------------------------------

#pragma package(smart_init)

#define PROMPT "WS2812: "

namespace
{

const uint8_t ws_code[] = {
		  ~0x24, ~0x64, ~0x2c, ~0x6c,
		  ~0x25, ~0x65, ~0x2d, ~0x6d
};

void ws_encode(uint8_t r, uint8_t g, uint8_t b, uint8_t *buf)
{
	uint32_t value = (g << 16) | (r << 8) | b;

	for (int i = 0; i < 8; i++) {
		uint32_t tmp = (value >> 21) & 0x7;
		*buf++ = ws_code[tmp];
		value <<= 3;
	}
}

}

int Ws2812Write(const std::vector<Ws2812Color> &colors)
{
	if (!comPort.isOpened())
	{
		LOG(PROMPT"Cannot write: serial port is not opened\n");
		return -1;
	}
	if (colors.size() > 5000)
	{
		LOG(PROMPT"Cannot write: number of LEDs exceeds limit\n");
		return -1;
	}
	if (colors.empty())
	{
		LOG(PROMPT"Nothing to write, number of LEDs = 0\n");
		return 0;
	}

	std::vector<uint8_t> output;
	output.resize(colors.size() * 8);
	uint8_t *ptr = &output[0];

	for (unsigned int i=0; i<colors.size(); i++)
	{
		const Ws2812Color &color = colors[i];
		// 24 bit source -> 72 bit output
		// 72 bit output at 9 bits/byte -> 8 bytes
		ws_encode(color.r, color.g, color.b, ptr);
		ptr += 8;
	}

#if 1
	AnsiString bytesText = "Bytes TX: ";
	for (unsigned int i=0; i<output.size(); i++)
	{
    	bytesText.cat_printf("%02X ", output[i]);
	}
	LOG(PROMPT"%s\n", bytesText.c_str());
#endif

	BOOL res = WriteComm(comPort.GetHandle(), &output[0], output.size());
	if (!res)
	{
		LOG(PROMPT"UART write failed, closing port\n");
		comPort.Close();
		return -1;
	}

	return 0;
}

