//---------------------------------------------------------------------------

#ifndef WS2812H
#define WS2812H
//---------------------------------------------------------------------------

#include <vector>
#include <stdint.h>

struct Ws2812Color
{
	uint8_t r, g, b;
	Ws2812Color(void):
		r(0), g(0), b(0)
	{}
};

int Ws2812Write(const std::vector<Ws2812Color> &colors);

#endif
