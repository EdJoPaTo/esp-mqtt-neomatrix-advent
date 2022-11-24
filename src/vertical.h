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
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 32, PIN_MATRIX,
	NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
	NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
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

void drawCandle(int16_t x, int16_t y, int16_t height, bool lit)
{
	auto red = ColorHSV(0, 255, 255);

	for (int i = 0; i < height; i++)
	{
		drawHorizontalLine(y - i, x, x + 2, red);
	}

	auto glow = 0;
	drawHorizontalLine(y + 1, x - 1, x + 3, glow);
	drawHorizontalLine(y + 2, x - 2, x + 4, glow);
	drawHorizontalLine(y + 3, x - 3, x + 5, glow);
	drawHorizontalLine(y + 4, x - 1, x + 3, glow);
	drawHorizontalLine(y + 5, x + 0, x + 2, glow);
	drawHorizontalLine(y + 6, x + 1, x + 1, glow);

	if (lit)
	{
		auto flame = ColorHSV(40, 255, 255);
		int height = (millis() + x) % 5;

		for (int i = 0; i <= height; i++)
		{
			matrix.drawPixel(x + 1, y + 1 + i, flame);
		}

		int width = (millis() + x) % 2;
		int direction = (millis() + x) % 3;

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
	matrix.fillScreen(ColorHSV(120, 255, 100));

	drawCandle(1, 3, 4, candles >= 1);
	drawCandle(5, 11, 5, candles >= 4);
	drawCandle(0, 19, 5, candles >= 3);
	drawCandle(4, 26, 4, candles >= 2);

	auto glow_color = 0;
	matrix.drawPixel(0, 3, glow_color);
	matrix.drawPixel(7, 26, glow_color);
}
