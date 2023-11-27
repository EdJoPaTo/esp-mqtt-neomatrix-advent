#pragma once

const uint16_t TOTAL_WIDTH = 16;
const uint16_t TOTAL_HEIGHT = 16;

const uint16_t TOTAL_PIXELS = TOTAL_WIDTH * TOTAL_HEIGHT;

// Efficient connection via DMA on pin RDX0 GPIO3 RX
// See <https://github.com/Makuna/NeoPixelBus/wiki/FAQ-%231>
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(TOTAL_PIXELS);

// bitbanging (Fallback)
// const int PIN_MATRIX = 13; // D7
// NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> strip(TOTAL_PIXELS, PIN_MATRIX);

NeoTopology<ColumnMajorAlternating180Layout> topo(TOTAL_WIDTH, TOTAL_HEIGHT);

#include "matrix_helper.h"

void drawWreath()
{
	auto green = HsbColor(120 / 360.0f, 1.0f, 0.6f * bri * on);

	drawHorizontalLine(10, 6, 10, green);
	drawHorizontalLine(11, 3, 13, green);

	for (int y = 12; y <= 13; y++)
	{
		drawHorizontalLine(y, 1, 5, green);
		drawHorizontalLine(y, 11, 15, green);
	}

	drawHorizontalLine(14, 1, 15, green);
	drawHorizontalLine(15, 3, 13, green);
}

void drawCandle(int16_t x, int16_t y, bool lit)
{
	auto red = HsbColor(0 / 360.0f, 1.0f, bri * on);

	for (int i = 0; i < 6; i++)
	{
		drawHsbPixel(x, y + i, red);
		drawHsbPixel(x + 1, y + i, red);
		drawHsbPixel(x + 2, y + i, red);
	}

	if (lit)
	{
		auto flame = HsbColor(40 / 360.0f, 1.0f, bri * on);
		auto type = (millis() + x) % 4;

		for (unsigned long i = 0; i <= type; i++)
		{
			drawHsbPixel(x + 1, y - 1 - i, flame);
		}

		if (type == 1)
		{
			drawHsbPixel(x, y - 2, flame);
		}
		else if (type == 2)
		{
			drawHsbPixel(x + 2, y - 2, flame);
		}
	}

	delay(1);
}

void drawLoop()
{
	strip.ClearTo(0);

	drawWreath();

	drawCandle(1, 7, candles >= 1); // left
	drawCandle(9, 5, candles >= 2); // third
	drawCandle(13, 7, candles >= 3); // right
	drawCandle(5, 9, candles >= 4); // second

	strip.Show();
}
