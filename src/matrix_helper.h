#pragma once

uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
	uint8_t r, g, b, r2, g2, b2;

	// Remap 0-360 to 0-65535
	hue = hue * 182;

	// Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
	// 0 is not the start of pure red, but the midpoint...a few values above
	// zero and a few below 65536 all yield pure red (similarly, 32768 is the
	// midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
	// each for red, green, blue) really only allows for 1530 distinct hues
	// (not 1536, more on that below), but the full unsigned 16-bit type was
	// chosen for hue so that one's code can easily handle a contiguous color
	// wheel by allowing hue to roll over in either direction.
	hue = (hue * 1530L + 32768) / 65536;

	// Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
	if (hue < 510)
	{ // Red to Green-1
		b = 0;
		if (hue < 255)
		{ //   Red to Yellow-1
			r = 255;
			g = hue; //     g = 0 to 254
		}
		else
		{						//   Yellow to Green-1
			r = 510 - hue; //     r = 255 to 1
			g = 255;
		}
	}
	else if (hue < 1020)
	{ // Green to Blue-1
		r = 0;
		if (hue < 765)
		{ //   Green to Cyan-1
			g = 255;
			b = hue - 510; //     b = 0 to 254
		}
		else
		{						 //   Cyan to Blue-1
			g = 1020 - hue; //     g = 255 to 1
			b = 255;
		}
	}
	else if (hue < 1530)
	{ // Blue to Red-1
		g = 0;
		if (hue < 1275)
		{						 //   Blue to Magenta-1
			r = hue - 1020; //     r = 0 to 254
			b = 255;
		}
		else
		{ //   Magenta to Red-1
			r = 255;
			b = 1530 - hue; //     b = 255 to 1
		}
	}
	else
	{ // Last 0.5 Red (quicker than % operator)
		r = 255;
		g = b = 0;
	}

	// Apply saturation and value to R,G,B
	uint32_t v1 = 1 + val;	// 1 to 256; allows >>8 instead of /255
	uint16_t s1 = 1 + sat;	// 1 to 256; same reason
	uint8_t s2 = 255 - sat; // 255 to 0

	r2 = ((((r * s1) >> 8) + s2) * v1) >> 8;
	g2 = ((((g * s1) >> 8) + s2) * v1) >> 8;
	b2 = ((((b * s1) >> 8) + s2) * v1) >> 8;

	// 888 to 565
	return ((uint16_t)(r2 & 0xF8) << 8) | ((uint16_t)(g2 & 0xFC) << 3) | (b2 >> 3);
}

const int16_t X_MIN = 0;
const int16_t X_MAX = TOTAL_WIDTH - 1;
void drawHorizontalLine(int16_t y, int16_t x_start, int16_t x_end, uint16_t color)
{
	for (auto x = max(X_MIN, x_start); x <= min(x_end, X_MAX); x++)
	{
		matrix.drawPixel(x, y, color);
	}
}
