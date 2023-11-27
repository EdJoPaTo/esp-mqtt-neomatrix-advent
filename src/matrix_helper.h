#pragma once

void drawHsbPixel(uint16_t x, uint16_t y, HsbColor hsb)
{
	auto i = topo.Map(x, y);
	strip.SetPixelColor(i, hsb);
}

const int16_t X_MIN = 0;
const int16_t X_MAX = TOTAL_WIDTH - 1;
void drawHorizontalLine(int16_t y, int16_t x_start, int16_t x_end, HsbColor hsb)
{
	for (auto x = max(X_MIN, x_start); x <= min(x_end, X_MAX); x++)
	{
		drawHsbPixel(x, y, hsb);
	}
}
