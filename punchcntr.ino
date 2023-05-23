/**************************************************************************
Punchcntr
Counts pulses coming from digital input and calculates sliding window of
one second and saves that in to array. The data are plotted as trend on
display SSD1306 plus intermediate values.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <assert.h>

#include "buffers.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define VIBR_ADDRESS D5 // Pin for the vibration sensor
#define VIBR_ADDRESS_ANALOG D0 // Pin for the vibration sensor
#define YELLOW_Y_MAX 16 // Number of rows for the yellow part of display
#define BLUE_Y_MIN 17 // Starting row for the blue part of display

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int maxRadius = 20;
int radius;
int radiusStep = 3;

void setup() {

  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);  // no effect
  display.println("Starting!");
  display.display();
  delay(500);
  pinMode(VIBR_ADDRESS_ANALOG, INPUT);

  pinMode(VIBR_ADDRESS, INPUT); /* Initialize Button Pin as Output */

}

CircBuffer<SCREEN_WIDTH, int> avgs;
DataBuffer hits;
int avgMax = 0;

void loop() {
  
  // Get data from sensor
  int sensorValue = digitalRead(VIBR_ADDRESS) ^ 1;
  int analog = analogRead(VIBR_ADDRESS_ANALOG);
  unsigned long time = micros();
  if (sensorValue) {
    hits.add(time);
  }

  // Count number of hits
  avgs.add(hits.n_hits_sec_back(time));

  // Draw yellow graph
  display.clearDisplay();
  for (int i = 0; i < SCREEN_WIDTH; ++i) {
    int iAvg = avgs.getNFwd(i);
    if (iAvg) {
      display.drawFastVLine(SCREEN_WIDTH - i - 1, (YELLOW_Y_MAX - iAvg * 2), iAvg * 2, 1);
    }
  }
  display.drawRect(0, BLUE_Y_MIN, SCREEN_WIDTH, SCREEN_HEIGHT-BLUE_Y_MIN, 1);

  // draw Circle
  if (sensorValue) {
    radius = maxRadius;
  } else {
    radius -= radiusStep;
  }
  display.fillCircle(20, BLUE_Y_MIN + (SCREEN_HEIGHT - BLUE_Y_MIN) / 2, radius, 1);

  // draw text
  display.setCursor(64, BLUE_Y_MIN + (SCREEN_HEIGHT - BLUE_Y_MIN) / 2);
  display.printf("%d %d", avgs.getCurr(), analog);

  // finish
  display.display();
  delay(50);
}
