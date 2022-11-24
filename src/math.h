#pragma once

#include <ESPxRGB.h>

uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
	uint8_t r;
	uint8_t g;
	uint8_t b;

	xhsv2rgb8(hue * (HSV_HUE_MAX / 360.0f), sat, val, &r, &g, &b);

	// 888 to 565
	return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}
