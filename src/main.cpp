#include <credentials.h>
#include <EspMQTTClient.h>
#include <MqttKalmanPublish.h>
#include <NeoPixelBus.h>

const bool MQTT_RETAINED = true;

EspMQTTClient mqttClient(
	WIFI_SSID,
	WIFI_PASSWORD,
	MQTT_SERVER,
	MQTT_USERNAME,
	MQTT_PASSWORD,
	CLIENT_NAME,
	1883
);

#define BASE_TOPIC CLIENT_NAME "/"
#define BASE_TOPIC_SET BASE_TOPIC "set/"
#define BASE_TOPIC_STATUS BASE_TOPIC "status/"

const int PIN_ON = 5; // D1

MQTTKalmanPublish mkRssi(mqttClient, BASE_TOPIC_STATUS "rssi", MQTT_RETAINED, 12 * 5 /* every 5 min */, 10);

// Percentage from 0.0f to 1.0f
float bri = 0.05;
// Amount from 0 to 4
uint8_t candles = 0;
bool on = true;

// #include "vertical.h"
#include "square.h"

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIN_ON, OUTPUT);
	Serial.begin(115200);
	strip.Begin();

	mqttClient.enableDebuggingMessages();
	mqttClient.enableHTTPWebUpdater();
	mqttClient.enableOTA();
	mqttClient.enableLastWillMessage(BASE_TOPIC "connected", "0", MQTT_RETAINED);
}

void onConnectionEstablished()
{
	mqttClient.subscribe(BASE_TOPIC_SET "bri", [](const String &payload) {
		auto value = strtof(payload.c_str(), 0) / 100.0f;
		if (!isfinite(value)) return;
		bri = max(2 / 255.0f, min(1.0f, value));
		mqttClient.publish(BASE_TOPIC_STATUS "bri", String(bri * 100), MQTT_RETAINED);
	});

	mqttClient.subscribe(BASE_TOPIC_SET "on", [](const String &payload) {
		on = payload == "1" || payload == "true";
		mqttClient.publish(BASE_TOPIC_STATUS "on", payload, MQTT_RETAINED);
	});

	mqttClient.subscribe(BASE_TOPIC_SET "candles", [](const String &payload) {
		auto parsed = strtol(payload.c_str(), 0, 10);
		candles = max(0l, min(4l, parsed));
		mqttClient.publish(BASE_TOPIC_STATUS "candles", String(candles), MQTT_RETAINED);
	});

	mqttClient.publish(BASE_TOPIC "git-remote", GIT_REMOTE, MQTT_RETAINED);
	mqttClient.publish(BASE_TOPIC "git-version", GIT_VERSION, MQTT_RETAINED);
	mqttClient.publish(BASE_TOPIC "connected", "2", MQTT_RETAINED);
}

void loop()
{
	mqttClient.loop();
	digitalWrite(LED_BUILTIN, mqttClient.isConnected() ? HIGH : LOW);
	digitalWrite(PIN_ON, on ? HIGH : LOW);

	auto now = millis();

	static unsigned long nextMeasure = 0;
	if (mqttClient.isWifiConnected() && now >= nextMeasure)
	{
		nextMeasure = now + 5000;
		auto rssi = WiFi.RSSI();
		auto avgRssi = mkRssi.addMeasurement(rssi);
		Serial.printf("RSSI          in dBm: %8d    Average: %10.2f\n", rssi, avgRssi);
	}

	drawLoop();

	delay(50);
}
