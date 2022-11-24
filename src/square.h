#pragma once

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN_MATRIX,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
	NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
	NEO_GRB + NEO_KHZ800);

void drawHorizontalLine(int16_t y, int16_t x_start, int16_t x_end, uint16_t color)
{
	int16_t x_min = 0;
	int16_t x_max = matrix.width() - 1;
	for (auto x = max(x_min, x_start); x <= min(x_end, x_max); x++)
	{
		matrix.drawPixel(x, y, color);
	}
}

void drawWreath()
{
	auto green = ColorHSV(120, 255, 160);

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
	auto red = ColorHSV(0, 255, 255);

	for (int i = 0; i < 6; i++)
	{
		matrix.drawPixel(x, y + i, red);
		matrix.drawPixel(x + 1, y + i, red);
		matrix.drawPixel(x + 2, y + i, red);
	}

	if (lit)
	{
		auto flame = ColorHSV(40, 255, 255);
		int type = (millis() + x) % 4;

		for (int i = 0; i <= type; i++)
		{
			matrix.drawPixel(x + 1, y - 1 - i, flame);
		}

		if (type == 1)
		{
			matrix.drawPixel(x, y - 2, flame);
		}
		else if (type == 2)
		{
			matrix.drawPixel(x + 2, y - 2, flame);
		}
	}

	delay(1);
}

void drawLoop()
{
	matrix.fillScreen(0);

	drawWreath();

	drawCandle(1, 7, candles >= 1);	// left
	drawCandle(9, 5, candles >= 2);	// third
	drawCandle(13, 7, candles >= 3); // right
	drawCandle(5, 9, candles >= 4);	// second
}
