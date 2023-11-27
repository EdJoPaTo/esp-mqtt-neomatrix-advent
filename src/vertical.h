#pragma once

const uint16_t TOTAL_WIDTH = 8;
const uint16_t TOTAL_HEIGHT = 32;

const uint16_t TOTAL_PIXELS = TOTAL_WIDTH * TOTAL_HEIGHT;

// Efficient connection via DMA on pin RDX0 GPIO3 RX
// See <https://github.com/Makuna/NeoPixelBus/wiki/FAQ-%231>
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(TOTAL_PIXELS);

// bitbanging (Fallback)
// const int PIN_MATRIX = 13; // D7
// NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> strip(TOTAL_PIXELS, PIN_MATRIX);

NeoTopology<RowMajorAlternating180Layout> topo(TOTAL_WIDTH, TOTAL_HEIGHT);

#include "matrix_helper.h"

void drawCandle(int16_t x, int16_t y, int16_t height, bool lit)
{
	auto red = HsbColor(0 / 360.0f, 1.0f, bri * on);

	for (int i = 0; i < height; i++)
	{
		drawHorizontalLine(y - i, x, x + 2, red);
	}

	auto glow = HsbColor(0 / 360.f, 1.0f, 0.0f);
	drawHorizontalLine(y + 1, x - 1, x + 3, glow);
	drawHorizontalLine(y + 2, x - 2, x + 4, glow);
	drawHorizontalLine(y + 3, x - 3, x + 5, glow);
	drawHorizontalLine(y + 4, x - 1, x + 3, glow);
	drawHorizontalLine(y + 5, x + 0, x + 2, glow);
	drawHorizontalLine(y + 6, x + 1, x + 1, glow);

	if (lit)
	{
		auto flame = HsbColor(40 / 360.0f, 1.0f, bri * on);
		auto height = (millis() + x) % 5;

		for (unsigned long i = 0; i <= height; i++)
		{
			drawHsbPixel(x + 1, y + 1 + i, flame);
		}

		auto width = (millis() + x) % 2;
		auto direction = (millis() + x) % 3;

		if (direction == 0 && height > 1)
		{
			drawHorizontalLine(y + 2, x + 1, x + 1 + width, flame);
		}
		else if (direction == 1 && height > 2)
		{
			drawHorizontalLine(y + 3, x + 1 - width, x + 1, flame);
		}
	}

	delay(1);
}

void drawLoop()
{
	auto bg = HsbColor(120 / 360.0f, 1.0f, 0.4f * bri * on);
	strip.ClearTo(bg);

	drawCandle(1, 3, 4, candles >= 1);
	drawCandle(5, 11, 5, candles >= 4);
	drawCandle(0, 19, 5, candles >= 3);
	drawCandle(4, 26, 4, candles >= 2);

	auto glow_color = HsbColor(0 / 360.f, 1.0f, 0.0f);
	drawHsbPixel(0, 3, glow_color);
	drawHsbPixel(7, 26, glow_color);

	strip.Show();
}
