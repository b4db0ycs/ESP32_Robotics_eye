#include <U8g2lib.h>
#include <Wire.h>  

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
int ir = 34;

unsigned long previousMillis = 0;
const long interval1 = 1000;
const long interval2 = 300;

unsigned long lastMotionMillis = 0;
const unsigned long sadDelay = 600000;

enum EyeState { EYE_OPEN, EYE_CLOSE, EYE_IDLE, EYE_SAD };
EyeState eyeState = EYE_IDLE;

void setup() {
  pinMode(ir, INPUT);
  u8g2.begin();  
  lastMotionMillis = millis();
}

void loop() {
  int Read = digitalRead(ir);
  unsigned long currentMillis = millis();

  if (Read == 0) {
    lastMotionMillis = currentMillis;
    if (eyeState == EYE_SAD) {
      eye_up();
      eyeState = EYE_IDLE;
    } else {
      eye_up();
      eyeState = EYE_IDLE;
    }
    previousMillis = currentMillis; 
  } else {
    if (currentMillis - lastMotionMillis >= sadDelay) {
      if (eyeState != EYE_SAD) {
        eye_sad();
        eyeState = EYE_SAD;
      }
      return;
    }

    switch (eyeState) {
      case EYE_IDLE:
        eye_open();
        previousMillis = currentMillis;
        eyeState = EYE_CLOSE;
        break;
        
      case EYE_CLOSE:
        if (currentMillis - previousMillis >= interval1) {
          eye_close();
          previousMillis = currentMillis;
          eyeState = EYE_OPEN;
        }
        break;
        
      case EYE_OPEN:
        if (currentMillis - previousMillis >= interval2) {
          eye_open();
          previousMillis = currentMillis;
          eyeState = EYE_IDLE;
        }
        break;

      case EYE_SAD:
        // già gestito sopra
        break;
    }
  }
}

void eye_up() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawBox(80, 20, 40, 20);
  u8g2.drawBox(10, 20, 40, 20);
  u8g2.drawFilledEllipse(99.5, 21, 19.5, 11);
  u8g2.drawFilledEllipse(29.5, 21, 19.5, 11);
  u8g2.drawFilledEllipse(30, 10, 12, 4);
  u8g2.drawFilledEllipse(100, 10, 12, 4);
  u8g2.sendBuffer();
}

void eye_open() {
  u8g2.clearBuffer(); 
  u8g2.setBitmapMode(1);
  u8g2.drawBox(14, 13, 36, 36);
  u8g2.drawBox(78, 13, 36, 36);
  u8g2.drawLine(14, 8, 50, 8);
  u8g2.drawLine(78, 8, 114, 8);  
  u8g2.sendBuffer();
}

void eye_close() {
  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);
  u8g2.drawBox(14, 27, 36, 7);
  u8g2.drawBox(78, 27, 36, 7);
  u8g2.drawLine(14, 8, 50, 8);
  u8g2.drawLine(78, 8, 114, 8);  
  u8g2.sendBuffer();
}

void eye_sad() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawBox(80, 35, 40, 20);
  u8g2.drawBox(10, 35, 40, 20);
  u8g2.drawFilledEllipse(99.5, 48, 19.5, 11);
  u8g2.drawFilledEllipse(29.5, 48, 19.5, 11);
  u8g2.drawLine(10, 30, 30, 25);
  u8g2.drawLine(120, 30, 100, 25);
  u8g2.sendBuffer();
}
