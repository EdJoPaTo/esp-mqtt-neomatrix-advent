#include <Adafruit_NeoMatrix.h>
#include <credentials.h>
#include <EspMQTTClient.h>

#define CLIENT_NAME "espMatrixAdvent"
const bool MQTT_RETAINED = true;

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_SERVER, // MQTT Broker server ip
  CLIENT_NAME, // Client name that uniquely identify your device
  1883 // The MQTT port, default to 1883. this line can be omitted
);

#define BASIC_TOPIC CLIENT_NAME "/"
#define BASIC_TOPIC_SET BASIC_TOPIC "set/"
#define BASIC_TOPIC_STATUS BASIC_TOPIC "status/"

const int PIN_MATRIX = 13; // D7
const int PIN_ON = 5; // D1

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


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 32, PIN_MATRIX,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint8_t BRIGHTNESS_OFFSET = 7;

uint8_t bri = 20;
uint8_t candles = 0;
boolean on = true;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(PIN_ON, OUTPUT);
  Serial.begin(115200);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.fillScreen(0);

  matrix.setCursor(0, 0);
  matrix.show();

  client.enableDebuggingMessages();
  client.enableHTTPWebUpdater();
  client.enableLastWillMessage(BASIC_TOPIC "connected", "0", MQTT_RETAINED);
}

void onConnectionEstablished() {
  client.subscribe(BASIC_TOPIC_SET "bri", [](const String & payload) {
    int newBri = strtol(payload.c_str(), 0, 10);
    if (bri != newBri) {
      bri = max(0, min(255 - BRIGHTNESS_OFFSET, newBri));
      client.publish(BASIC_TOPIC_STATUS "bri", String(bri), MQTT_RETAINED);
    }
  });

  client.subscribe(BASIC_TOPIC_SET "on", [](const String & payload) {
    boolean newOn = payload != "0";
    if (on != newOn) {
      on = newOn;
      client.publish(BASIC_TOPIC_STATUS "on", payload, MQTT_RETAINED);
    }
  });

  client.subscribe(BASIC_TOPIC_SET "candles", [](const String & payload) {
    int parsed = strtol(payload.c_str(), 0, 10);
    int newCandles = max(0, min(4, parsed));
    if (candles != newCandles) {
      candles = max(0, min(4, newCandles));
      client.publish(BASIC_TOPIC_STATUS "candles", String(candles), MQTT_RETAINED);
    }
  });

  client.publish(BASIC_TOPIC "connected", "2", MQTT_RETAINED);
  client.publish(BASIC_TOPIC_STATUS "candles", String(candles), MQTT_RETAINED);
  client.publish(BASIC_TOPIC_STATUS "bri", String(bri), MQTT_RETAINED);
  client.publish(BASIC_TOPIC_STATUS "on", on ? "1" : "0", MQTT_RETAINED);
}

void drawHorizontalLine(int16_t y, int16_t x_start, int16_t x_end, uint16_t color) {
  int16_t x_min = 0;
  int16_t x_max = matrix.width() - 1;
  for (auto x = max(x_min, x_start); x <= min(x_end, x_max); x++) {
    matrix.drawPixel(x, y, color);
  }
}

void drawWreath(uint8_t brightness) {
  auto green = ColorHSV(120 * 182, 255, brightness);

  drawHorizontalLine(10, 6, 10, green);
  drawHorizontalLine(11, 3, 13, green);

  for (int y = 12; y <= 13; y++) {
    drawHorizontalLine(y, 1, 5, green);
    drawHorizontalLine(y, 11, 15, green);
  }

  drawHorizontalLine(14, 1, 15, green);
  drawHorizontalLine(15, 3, 13, green);
}

void drawCandle(int16_t x, int16_t y, int16_t height, uint8_t brightness, bool lit) {
  auto red = ColorHSV(0 * 182, 255, brightness);

  for (int i = 0; i < height; i++) {
    drawHorizontalLine(y - i, x, x + 2, red);
  }

  auto glow = 0;
  drawHorizontalLine(y + 1, x - 1, x + 3, glow);
  drawHorizontalLine(y + 2, x - 2, x + 4, glow);
  drawHorizontalLine(y + 3, x - 3, x + 5, glow);
  drawHorizontalLine(y + 4, x - 1, x + 3, glow);
  drawHorizontalLine(y + 5, x + 0, x + 2, glow);
  drawHorizontalLine(y + 6, x + 1, x + 1, glow);

  if (lit) {
    auto flame = ColorHSV(40 * 182, 255, brightness);
    int height = (millis() + x) % 5;

    for (int i = 0; i <= height; i++) {
      matrix.drawPixel(x + 1, y + 1 + i, flame);
    }

    int width = (millis() + x) % 2;
    int direction = (millis() + x) % 3;

    if (direction == 0 && height > 1) {
      drawHorizontalLine(y + 2, x + 1, x + 1 + width, flame);
    } else if (direction == 1 && height > 2) {
      drawHorizontalLine(y + 3, x + 1 - width, x + 1, flame);
    }
  }

  delay(1);
}

void loop() {
  client.loop();
  digitalWrite(LED_BUILTIN, client.isConnected() ? HIGH : LOW);
  digitalWrite(PIN_ON, on ? HIGH : LOW);

  auto brightness = (bri + BRIGHTNESS_OFFSET) * on;

  matrix.fillScreen(ColorHSV(120 * 182, 255, brightness / 4));

  drawCandle(1, 3, 4, brightness, candles >= 1);
  drawCandle(5, 11, 5, brightness, candles >= 4);
  drawCandle(0, 19, 5, brightness, candles >= 3);
  drawCandle(4, 26, 4, brightness, candles >= 2);

  auto glow_color = 0;
  matrix.drawPixel(0, 3, glow_color);
  matrix.drawPixel(7, 26, glow_color);

  matrix.show();

  delay(50);
}

uint16_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
    uint8_t r, g, b, r2, g2, b2;

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
    if(hue < 510) {         // Red to Green-1
      b = 0;
      if(hue < 255) {       //   Red to Yellow-1
        r = 255;
        g = hue;            //     g = 0 to 254
      } else {              //   Yellow to Green-1
        r = 510 - hue;      //     r = 255 to 1
        g = 255;
      }
    } else if(hue < 1020) { // Green to Blue-1
      r = 0;
      if(hue <  765) {      //   Green to Cyan-1
        g = 255;
        b = hue - 510;      //     b = 0 to 254
      } else {              //   Cyan to Blue-1
        g = 1020 - hue;     //     g = 255 to 1
        b = 255;
      }
    } else if(hue < 1530) { // Blue to Red-1
      g = 0;
      if(hue < 1275) {      //   Blue to Magenta-1
        r = hue - 1020;     //     r = 0 to 254
        b = 255;
      } else {              //   Magenta to Red-1
        r = 255;
        b = 1530 - hue;     //     b = 255 to 1
      }
    } else {                // Last 0.5 Red (quicker than % operator)
      r = 255;
      g = b = 0;
    }

    // Apply saturation and value to R,G,B
    uint32_t v1 =   1 + val; // 1 to 256; allows >>8 instead of /255
    uint16_t s1 =   1 + sat; // 1 to 256; same reason
    uint8_t  s2 = 255 - sat; // 255 to 0

    r2 = ((((r * s1) >> 8) + s2) * v1) >> 8;
    g2 = ((((g * s1) >> 8) + s2) * v1) >> 8;
    b2 = ((((b * s1) >> 8) + s2) * v1) >> 8;
    return matrix.Color(r2, g2, b2);
}
