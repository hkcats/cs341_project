#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <string.h>
#include <ArduinoJson.h>
#include <tokens.h> //user for var defs

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

  
  if (!SD.begin(SD_CS, spi)){
    Serial.println("sd.begin() failed :(");
    return;
  }
  println("sd began");



  const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
  const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
  char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
  char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response


  char album_url[500] = "https://i.scdn.co/image/ab67616d00001e02ff9ca10b55ce82ae553c8228";

  int httpCode = 0;
  JsonDocument doc;
  DeserializationError error;

  char access_token[500] = "Bearer BQB0XNxpXgBhuCugELdn8C8IsN0lGuA2NYvIChNLXzRzf2Zzv3uA7koAQb0HN3Qs0wY7g-X4IfJ_zjwGWVkW5lBf1A9WiTZuK-PCoqy_mV87UVwLloouQySKwF5u_WvjRcfcxxd1xijJnXbyASxGbGwbpr95eoBxOxMSW7X9kl6D9xZkvQpLezkcAQSxzxbhYv-MNeNWci754BMx5c7RjMLg_z3HLKwGrKYBvt-Z0KaOtur6gpxNO4G0R9A";
  // char refresh_token[500] = "AQAWDl0LqmghRViGap6P-atB0ZCg6FEYy5iax8FZIR4H3lQUgo2V2G4mb0hOy8exVLycvTQnjbVdJsdvj0vWBYdkU_DH8ECj7DmZahtgDIUFqAU-Q4s6xQn7D4jfXfycgag";
  
  Serial.println(access_token);


  //play album 

  //get playback/album info

  strncpy(request_buffer, "https://api.spotify.com/v1/me/player", IN_BUFFER_SIZE);
  
  Serial.println("request_buffer formatted");
  
  http.begin(request_buffer);
  http.addHeader("Authorization", access_token);
  httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK){
    Serial.printf("Error getting playback info: %d",httpCode);
  }

  doc.clear();
  error = deserializeJson(doc, http.getString());
  http.end();
  if (error) {
      Serial.printf("Parse failed: %s", error.c_str());
      return;
  }

  if (httpCode != HTTP_CODE_OK){
    Serial.printf("Error getting playback info: %d",httpCode);
    Serial.printf(doc["error"]["message"]);
    return;
  }
  Serial.println("about to get album url");

  JsonArray images = doc["item"]["album"]["images"];
  bool found = false;
  for(int i = 0; i < images.size(); i++){
    if(images[i]["height"].as<int>() < 100){
      strcpy(album_url, images[i]["url"]);
      found = true;
      break;
    }
  }
  if(!found){
    strcpy(album_url, doc["item"]["album"]["images"][0]["url"]);
  }

  Serial.printf("woo, got album url! %s\n", album_url);








  
  http.begin(album_url);
  // http.addHeader("Authorization", token);
  httpCode = http.GET();

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