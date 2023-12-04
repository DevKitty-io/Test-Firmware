#define up_btn 9 // left button
#define dn_btn 18 // right button
#define lt_btn 11 // left button
#define rt_btn 7 // right button

#include "SH1106Wire.h"
#include <Adafruit_NeoPixel.h>
#include "DevKitty-Logo.h"
#include "QR-Code.h"
#include "graphics.h"

SH1106Wire display(0x3C, 33, 35);
Adafruit_NeoPixel strip(1, 12, NEO_RGB + NEO_KHZ800);
bool cat = false;

void setup(){
    Serial.begin(115200);
    init(); screenTester();
}

void loop() {
    rainbow(10);
    if (millis() > 3000 && !cat) {
          display.clear();
          display.drawXbm(0,0,128,64,DevKitty_Logo_bits);
          display.display();
          cat = true;
    }
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}

void init() {
    display.init();
    display.flipScreenVertically();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(DejaVu_Sans_Mono_10);
    display.clear();

    strip.begin();
    strip.show();
    strip.setBrightness(50);
}

/******* Tester Functions *******/

void screenTester() {
    display.drawXbm(0,0,128,64,DevKitty_Logo_bits);
    display.display();

    delay(1000);
    display.clear();
    display.drawXbm(0,0,128,64,QR_Code_bits);
    display.drawString(0,10,"Update your");
    display.drawString(0,20,"firmware at");
    display.drawString(0,30,"devkitty.io");

    display.drawLine(0,50,65,50);
    display.fillTriangle(65,50,62,53,62,47);

    display.display();
}