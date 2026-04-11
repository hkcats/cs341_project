#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
// #include <user_setup.h> //user for var defs

#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15  // Chip select control pin
#define TFT_DC   12  // Data Command control pin
#define TFT_RST  27  // Reset pin (could connect to RST pin)

// TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

const int GETTING_PERIOD = 5000; //5000; //periodicity of getting a number fact.
uint32_t last_time = 0; //used for timing

void setup() {
  Serial.begin(115200); //begin serial

  // pinMode(TFT_RST, OUTPUT);
  // digitalWrite(TFT_RST, HIGH);
  // delay(10);
  // digitalWrite(TFT_RST, LOW);
  // delay(100);
  // digitalWrite(TFT_RST, HIGH);
  // delay(100);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);

  // tft.init();
  // tft.setRotation(2);
  // tft.setTextSize(1);
  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_GREEN, TFT_BLACK);

}


/*-----------------------------------
  
*/
void loop() {
  if ((millis() - last_time) > GETTING_PERIOD) { 
    Serial.printf("\nhiya %d", last_time);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextWrap(true);
    tft.print("text");
    // tft.fillScreen(TFT_BLACK);
    // tft.setCursor(0, 0, 1);
    // tft.printf("\nhello %d", last_time);

    last_time = millis();
  }
}
