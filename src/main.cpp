#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <base64.h>
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
// sd_mosi 13
// sd_clk 14
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




  const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
  const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
  char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
  char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response




  //temp testing
  const char* album_id = "29dl63pyzkkAPZSY8vSqlD";
  char album_url[500] = "https://i.scdn.co/image/ab67616d00001e02ff9ca10b55ce82ae553c8228";

  //post for authorization
  const char* client_id = "8ec3fea2b42f42338ed8f3ca17dc7800";
  const char* redirect_uri = "http://127.0.0.1:3000";
  int httpCode = 0;
  JsonDocument doc;
  DeserializationError error;

  // http.begin("https://accounts.spotify.com/authorize?client_id=8ec3fea2b42f42338ed8f3ca17dc7800&response_type=code&redirect_uri=http://127.0.0.1:3000");
  
  // Serial.println("created request for authorization");
  // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); 
  // httpCode = http.GET();

  // if (httpCode != HTTP_CODE_OK && httpCode != 303){
  //   Serial.printf("Error with authorization: %d",httpCode);
  //   return;
  // }
  // // Serial.println(http.getString());
  // Serial.println(http.header("Location"));

  // doc.clear();
  // error = deserializeJson(doc, http.getString());
  
  // http.end();
  
  // if (error) {
  //     Serial.printf("Parse failed: %s", error.c_str());
  //     return;
  // }
  
  // const char* code = "AQBzzrV6GAsgHbnLvNbaaeGA7Fzstl4lOc95ffh6tPhUidlpZQvcAeF9AMHbFPBNDCmfyPzBBoGWZZo4DGBQ3m2HA2pS1EzhH9-xNWbTeDxLjdPKC3D5J_vpmr0x6XT4wmW7CDZVIZicyY5V-FRyDkKCb-lqMAnsmQ";
  // Serial.println("woo, code gotten!");
  // Serial.println(CODE);


  //post for token

  //https://accounts.spotify.com/authorize?response_type=code&client_id=8ec3fea2b42f42338ed8f3ca17dc7800&scope=user-read-currently-playing%20user-read-playback-state&redirect_uri=http://127.0.0.1:3000

  // const char* client_id = "8ec3fea2b42f42338ed8f3ca17dc7800";
  // const char* client_secret = "724c465c61d445908eb66810abe242c5";

  char access_token[500] = "Bearer BQA24EZxHN-_DvPtkl0f7T2st3zrS8ADqLH9HhYk7DPBxQtIJtQoqluMbcBHujMVQ0fbFQg3MjiWc02H--Ov9lRjIG_OHPe-ls6OcBjTSqDNIM8rpkENAUUsgKrwuFUQCCfOmfTQcYolUyvgHg3dyQXnbTmpte7NXJe6TRWLVX6hm2NGLG_A0GUp7di4kC8MdVWM1JeqoRjsZFFQ6W6jKvX9PnQulKft1jumILvT3DOcioFALvbEEOrdNq8";
  char refresh_token[500] = "AQAWDl0LqmghRViGap6P-atB0ZCg6FEYy5iax8FZIR4H3lQUgo2V2G4mb0hOy8exVLycvTQnjbVdJsdvj0vWBYdkU_DH8ECj7DmZahtgDIUFqAU-Q4s6xQn7D4jfXfycgag";
  
  bool getAccessToken = false;
  bool getRefreshToken = false;

  if(getAccessToken){

    const char* authorization = "Basic OGVjM2ZlYTJiNDJmNDIzMzhlZDhmM2NhMTdkYzc4MDA6NzI0YzQ2NWM2MWQ0NDU5MDhlYjY2ODEwYWJlMjQyYzU=";
    
    Serial.println(ESP.getFreeHeap());
    
    
    if(getRefreshToken){
      // given CODE (from browser), retrieves refresh token that should be copied into tokens.h
      http.begin("https://accounts.spotify.com/api/token");
      http.addHeader("Authorization", authorization);
      http.addHeader("content-type", "application/x-www-form-urlencoded");

      snprintf(request_buffer, IN_BUFFER_SIZE, "grant_type=authorization_code&code=%s&redirect_uri=http://127.0.0.1:3000", CODE);

      httpCode = http.POST(request_buffer);

      doc.clear();
      error = deserializeJson(doc, http.getString());
      http.end();
      if (error) {
          Serial.printf("Parse failed: %s", error.c_str());
          return;
      }
      if (httpCode != HTTP_CODE_OK){
        Serial.printf("Error fetching refresh code: %d",httpCode);
        Serial.printf(doc["error"]["message"]);
        return;
      }
      strncpy(refresh_token, doc["refresh_token"], 500);
      Serial.println("woo, success!");
      Serial.println("Refresh Token: ");
      Serial.println(refresh_token);
    }
    // access token retreival from refresh; to be done every hour
    else {
      http.begin("https://accounts.spotify.com/api/token");
      http.addHeader("content-type", "application/x-www-form-urlencoded");
      snprintf(request_buffer, IN_BUFFER_SIZE, "grant_type=refresh_token&refresh_token=%s", refresh_token);
      httpCode = http.POST(request_buffer);
      
      doc.clear();
      error = deserializeJson(doc, http.getString());
      http.end();
      if (error) {
          Serial.printf("Parse failed: %s", error.c_str());
          return;
      }
      if (httpCode != HTTP_CODE_OK){
        Serial.printf("Error fetching access code: %d",httpCode);
        Serial.printf(doc["error"]["message"]);
        return;
      }
    }
    const char* temp = doc["access_token"];
    
    snprintf(access_token, 500, "Bearer %s", temp);
  }
  Serial.println("woo, got access_token!");
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

  //begin SD
  if (!SD.begin(SD_CS, spi)){
    Serial.println("sd.begin() failed :(");
    return;
  }
  println("sd began!");

  http.begin(album_url);
  httpCode = http.GET();
  println("http gotten");
  if (httpCode == HTTP_CODE_OK){
    char filename[500];
    snprintf(filename, 500, "/%s.jpeg", album_id);
    File file = SD.open(filename, FILE_WRITE);
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
