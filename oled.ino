// https://github.com/ThingPulse/esp8266-oled-ssd1306/tree/master/examples/SSD1306UiDemo
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// Include the UI lib
#include "OLEDDisplayUi.h"

// Include custom images

#include "images.h"
#include <WiFi.h>
  
const char* ssid = "";
const char* password =  "";
  
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("Connected to AP!");
 		Serial.println(WiFi.localIP());
    Serial.print("SSID Length: ");
    Serial.println(info.connected.ssid_len);
 
    Serial.print("SSID: ");
    for(int i=0; i<info.connected.ssid_len; i++){
      Serial.print((char) info.connected.ssid[i]);
    }
 
    Serial.print("\nBSSID: ");
    for(int i=0; i<6; i++){
      Serial.printf("%02X", info.connected.bssid[i]);
 
      if(i<5){
        Serial.print(":");
      }
    }
     
    Serial.print("\nChannel: ");
    Serial.println(info.connected.channel);
 
    Serial.print("Auth mode: ");
    Serial.println(info.connected.authmode);  
} 

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 4, 15);
// SH1106Wire display(0x3c, D3, D5);

OLEDDisplayUi ui     ( &display );

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // draw an xbm image.
  // Please note that everything that should be transitioned
  // needs to be drawn relative to x and y

  display->drawXbm(x + 34, y + 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "Gateway: " + WiFi.gatewayIP().toString());

  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 20 + y, "Localip: 192.168.*.**"); // WiFi.localIP().toString()
  
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 30 + y, "Subnet: " + WiFi.subnetMask().toString());
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 10 + y, String(ESP.getSdkVersion()).c_str());

  // The coordinates define the center of the text
  //display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(0 + x, 20 + y, String(ESP.getCpuFreqMHz()).c_str()); 

  // The coordinates define the right end of the text
  //display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(0 + x, 30 + y, String(ESP.getFreeSketchSpace()).c_str());
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demo for drawStringMaxWidth:
  // with the third parameter you can define the width after which words will be wrapped.
  // Currently only spaces and "-" are allowed for wrapping
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128, "By @atmon3r");
}

void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4 };

// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
   //Initialiser l'afficheur oled (sinon ne fonctionne pas)
    pinMode(16,OUTPUT);
    digitalWrite(16, LOW); // set  GPIO16 low to reset OLED
    delay(50); 
    digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 to high

	// The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(60);

	// Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();
  
 		// Wifi part
    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);      
    WiFi.begin(ssid, password);
}


void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    delay(remainingTimeBudget);
  }
}
