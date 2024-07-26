#include "SH1106Wire.h"
#include <Adafruit_NeoPixel.h>
#include "assets/DevKitty-Logo.h"
#include "assets/QR-Code.h"
#include "assets/graphics.h"

// Change for Target Device
#define CUTIECAT

#if defined(NUGGET)
    #define up_btn 9
    #define dn_btn 18
    #define lt_btn 11 
    #define rt_btn 7 
    #define NEOPIXEL_PIN 12
    #define SDA 33
    #define SCL 35

#elif defined(CUTIECAT)
    #define up_btn D0 
    #define dn_btn D1
    #define lt_btn D2 
    #define rt_btn D3 
    #define NEOPIXEL_PIN TX
#endif

#define DEBOUNCE_DELAY 50 

SH1106Wire display(0x3C, SDA, SCL);
Adafruit_NeoPixel strip(1, NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800);

bool neopixelOn = true; 
bool btnStatesPrev[4] = {HIGH, HIGH, HIGH, HIGH};
unsigned long lastTime[4] = {0, 0, 0, 0}; 

void getButtons(void *pvParameters);
void startPxl(void *pvParameters);

void setup() {
    Serial.begin(115200);
    init(); screenTester();

    pinMode(up_btn, INPUT_PULLUP);
    pinMode(dn_btn, INPUT_PULLUP);
    pinMode(lt_btn, INPUT_PULLUP);
    pinMode(rt_btn, INPUT_PULLUP);

    // Create tasks
    xTaskCreatePinnedToCore(getButtons,"Button Task",2048, NULL,1,NULL,1);
    xTaskCreatePinnedToCore(startPxl,"Animation Task",2048, NULL,1, NULL,0);
}

void loop() {
    vTaskDelay(portMAX_DELAY); 
}

void getButtons(void *pvParameters) {
    while (true) {
        unsigned long currTime = millis();
        bool btnStates[4] = {digitalRead(up_btn), digitalRead(dn_btn), digitalRead(lt_btn), digitalRead(rt_btn)};
        for (int i=0; i<4; i++) {
            if (btnStates[i] != btnStatesPrev[i]) {
                if ((currTime - lastTime[i]) > DEBOUNCE_DELAY) {
                    if (btnStates[i] == LOW) {
                        neopixelOn = !neopixelOn;
                        if (neopixelOn) { strip.begin(); strip.setBrightness(50); strip.show(); } 
                        else { strip.clear(); strip.show(); }
                    }
                    lastTime[i] = currTime;
                }
            }
            btnStatesPrev[i] = btnStates[i];
        }
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void startPxl(void *pvParameters) {
    while (true) {
        if (neopixelOn) {rainbow(10);}
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void rainbow(int wait) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
        if (!neopixelOn) return;

        for (int i= 0; i< strip.numPixels(); i++) { 
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