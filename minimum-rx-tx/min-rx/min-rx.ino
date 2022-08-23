#include <FastLED.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

// pins
const byte DATA_PIN = 6; // neo-pixel data pin

// LED array
const byte NUM_LEDS = 12;
CRGB leds[NUM_LEDS];

typedef struct neopixel_data
{
  bool display = true;
  int hue;
  int saturation;
  int value;
} neopixel_data;

// Where incoming data is stored
neopixel_data data;

// Init ESP Now with fallback
void InitESPNow()
{
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
  }
  else
  {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP()
{
  const char *SSID = "RX_1";
  bool result = WiFi.softAP(SSID, "RX_1_Password", CHANNEL, 0);
  if (!result)
  {
    Serial.println("AP Config failed.");
  }
  else
  {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  WiFi.mode(WIFI_AP);
  configDeviceAP();

  // This is the mac address of the Receiver in AP Mode
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *dataIn, int data_len)
{

  memcpy(&data, dataIn, sizeof(data));
  Serial.println("Hue");
  Serial.println(data.hue);
  Serial.println("Saturation");
  Serial.println(data.saturation);
  Serial.println("Value");
  Serial.println(data.value);

  // Display Change on NeoPixels
  if (data.display)
  {
    fill_solid(leds, NUM_LEDS, CHSV(data.hue, data.saturation, data.value));
    FastLED.show();
  }
}

void loop()
{
}