#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
// #include <user_setup.h> //user for var defs

#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15  // Chip select control pin
#define TFT_DC   4  // Data Command control pin
#define TFT_RST  27  // Reset pin (could connect to RST pin)

#define SD_CS 5
#define SD_MISO 12
// #define SD_CONFIG SdioConfig(TFT_SCLK, TFT_MOSI, SD_MISO)

// SD sd;
// File file;

SPIClass spi = SPIClass(HSPI);

HTTPClient http;

Adafruit_ST7735 tft = Adafruit_ST7735(&spi, TFT_CS, TFT_DC, TFT_RST);

const int GETTING_PERIOD = 5000; //5 secs
uint32_t last_time = 0; //used for timing

//run off hotspot during checkpoints
const char* ssid = "Peacock";
const char* password = "anandpreet";

void print(const char* message){
  Serial.print(message);
  tft.print(message);
}

void println(const char* message){
  Serial.println(message);
  tft.println(message);
}

template<typename... Args>
void printf(Args... args){
  Serial.printf(args...);
  tft.printf(args...);
}


void setup() {
  Serial.begin(115200); //begin serial
  delay(1000);

  SPI.begin(TFT_SCLK, SD_MISO, TFT_MOSI, -1);

  tft.initR(INITR_BLACKTAB);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);

  ////////////////////// WIFI BEGIN ///////////////////////////
  //add timeout/restart
  // WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  println("\nconnecting");

  while (WiFi.status() != WL_CONNECTED){
      print(".");
      delay(100);
  }
  println("\nwoo, connected!");
  printf("to: %s\n", ssid);
  printf("local IP address: %s\n", WiFi.localIP().toString());

  // tft.fillScreen(ST7735_BLACK);

  ////////////////////// WIFI END //////////////////////////////

  //temp testing
  const char* album_id = "0zkgIyTdpvOpV5z4oK7c2j";
  const char* album_url = "https://i.scdn.co/image/ab67616d00001e02ff9ca10b55ce82ae553c8228";

  // const char* severname = "https://sync.api.cloudconvert.com/v2/jobs";

  // const char* token = "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJhdWQiOiIxIiwianRpIjoiNTc2MTJhM2EwMDE5OGQ5ZjY2NGI4YjhkM2FmZTNhYzFlZjJjMWY1ODU5YjNhMjkxZjUyMzkzMjk5ZWE2NDZjYmQzYWVkMjc2OTM1N2U0YjYiLCJpYXQiOjE3NzYwMzY2MzcuMzc4NzE2LCJuYmYiOjE3NzYwMzY2MzcuMzc4NzE3LCJleHAiOjQ5MzE3MTAyMzcuMzczNDg4LCJzdWIiOiI3NTA5MzgzNCIsInNjb3BlcyI6WyJ0YXNrLnJlYWQiLCJ0YXNrLndyaXRlIiwicHJlc2V0LnJlYWQiXX0.VxLtg6wZu3-e7d9WOxXr3TcJ_NswvkXF_R1lzkpX4ILvE59oiewu3rzXZMo9bZ3SgxvMN9qxi5t79hUHPkOSLaQ25Z18r1jORzGHlRs__EfHnnN8Km9HoGpKj8JjUEiyB71CuRl2NdIyU_SbZObn4tJTr1nuvhWsGdiEkfx6VFR5KigTEGMqptfqReSzabo8GiS3UxMFoS9pohS7iUuFYTlTmEopitSgIdTakn4lKyliR2ZbEYHwG8MNXaNiDze8sBTTvcw_SnS21Ne-BOYXXSfbZmF9NIkNLMfBILsQ2B77LUPnyO4ESseqKerO5Ghw4lcFLCE4ARBjtBtXvLGNMvRvr-7E3g64rsA7SRGEPpW5BOUvvKAgjH0K4i4dESURjxyzRcpCal9fwd9iEnXQ7y1XCPPQI1olL-9aGZy7I0HIVxos-mcaAh0ENQQH-5Ln4mz2DemMX4iUCq6TXdClIg1uk33EeDGqg8XvoOt1r2Ki4SmDIBOKQLmibkNs5aj7Uc6kpoW7Za2BT5jxnfTs4sjD4irjILGCk32K0kRj2qYmOpYijiLbO26FHZKE1yi0i7pOJJ03zBwWAGNXN6DQFgG24DcuLMZkjwQGQ-DDKsdN270j_zNHidDx7bTN-ISVL9MIN19XBiS55fH55p6jrOUNsNPCuop6Ji2_S0_Hn0w";
  
  
  if (!SD.begin(SD_CS, spi)){
    Serial.println("sd.begin() failed :(");
    return;
  }

  println("sd began");
  
  http.begin(album_url);
  // http.addHeader("Authorization", token);
  int httpCode = http.GET();

  println("http gotten");

  if (httpCode == HTTP_CODE_OK){
    File file = SD.open("/test.jpeg", FILE_WRITE);
    http.writeToStream(&file);
    file.close();
  }
  http.end();

  print("done!");


}

/*-----------------------------------
  
*/
void loop() {
  if ((millis() - last_time) > GETTING_PERIOD) { 
    // Serial.printf("\nhiya %d", last_time);
    // tft.fillScreen(ST77XX_BLACK);
    // tft.setCursor(0, 0);
    // tft.setTextColor(ST77XX_WHITE);
    // tft.print("text");
    // tft.fillScreen(TFT_BLACK);
    // tft.setCursor(0, 0, 1);
    // tft.printf("\nhello %d", last_time);

    last_time = millis();
  }
}
