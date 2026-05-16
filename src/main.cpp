#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SD.h>
#include <string.h>
#include <ArduinoJson.h>
#include <TJpg_Decoder.h>
#include <tokens.h> //user for var defs

#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

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

//render things

// end bitmap things


bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  // tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}



const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response


char album_url[500];
char filename[500];

int httpCode = 0;
JsonDocument doc;
DeserializationError error;

char access_token[500];




void setup() {
  Serial.begin(115200); //begin serial
  delay(1000);

  SPI.begin(TFT_SCLK, SD_MISO, TFT_MOSI, -1);

  tft.initR(INITR_BLACKTAB);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(true);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 0);


  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(8);
  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

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

  tft.fillScreen(ST7735_BLACK);

  ////////////////////// WIFI END //////////////////////////////
  ////////////////////// HELPER FUNCTIONS //////////////////////

  

  ////////////////////// HELPER FUNCTIONS END//////////////////////
  
  delay(100);
  if (!SD.begin(SD_CS, spi)){
    Serial.println("sd.begin() failed :(");
    return;
  }
  Serial.println("sd began");



  

  // add a loop to keep trying until valid access_token?
  bool getAccessToken = true;
  if(getAccessToken){
      // access token retreival from refresh; to be done every hour
      Serial.println(ESP.getFreeHeap());
      
      http.begin("https://accounts.spotify.com/api/token");
      http.addHeader("Authorization", authorization);
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
        // Serial.printf(doc["error"]["message"]);
        return;
      }
      const char* temp = doc["access_token"];
      snprintf(access_token, 500, "Bearer %s", temp);
    }

  //play album 




}

/*-----------------------------------
  
*/
void loop() {
  if ((millis() - last_time) > GETTING_PERIOD) { 
    // tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);


    /////////// GET PLAYBACK ////////////

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
    Serial.println("about to get album id/url");

    snprintf(filename, 500, "/%s.jpeg", doc["item"]["album"]["id"].as<const char*>());
    Serial.printf("filename: %s\n", filename);

    if(!SD.exists(filename)){
      Serial.println("getting album url");
      JsonArray images = doc["item"]["album"]["images"];
      bool found = false;
      for(int i = 0; i < images.size(); i++){
        if(images[i]["height"].as<int>() > 600){
          strcpy(album_url, images[i]["url"].as<const char*>());
          found = true;
          TJpgDec.setJpgScale(8);
          break;
        }
      }
      if(!found){
        TJpgDec.setJpgScale(1);
        strcpy(album_url, doc["item"]["album"]["images"][0]["url"].as<const char*>());
      }
      Serial.printf("woo, got album url! %s\n", album_url);

      // save image to SD card
      http.begin(album_url);
      // http.addHeader("Authorization", token);
      httpCode = http.GET();

      Serial.println("http gotten");

      if (httpCode == HTTP_CODE_OK){
        File file = SD.open(filename, FILE_WRITE);
        http.writeToStream(&file);
        file.close();
      }
      http.end();
      Serial.println("saved jpeg to sd");
    }

    Serial.println("in to display!");

    uint16_t w = 0, h = 0;
    TJpgDec.getSdJpgSize(&w, &h, filename);
    Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

    // Draw the image, top left at 0,0
    TJpgDec.drawSdJpg(0, 0, filename);
    Serial.println("done!");


    last_time = millis();
  }
}