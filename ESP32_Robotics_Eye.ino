#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
int ir = 34;

unsigned long previousMillis = 0;
const long interval1 = 400;
const long interval2 = 180;
const long interval1_center = 650;
const long interval2_center = 400;

unsigned long lastMotionMillis = 0;
const unsigned long sadDelay = 600000;
const unsigned long boredDelay = 100000;

enum EyeState { EYE_OPEN, EYE_CLOSE, EYE_IDLE, EYE_BORED, EYE_SAD };
void eye_up();
void eye_open(bool centered = false);
void eye_close(bool centered = false);
void eye_sad();
void eye_bored();
EyeState eyeState = EYE_IDLE;

int eyeOffset = 0;
int eyeDirection = 1;
const int maxOffset = 15;
unsigned long movementMillis = 0;
const unsigned long movementInterval = 325;

bool isWaitingAtCenter = false;
unsigned long centerWaitMillis = 0;
const unsigned long centerPauseDuration = 8000;

unsigned long boredMillis = 0;

void setup() {
  pinMode(ir, INPUT);
  u8g2.begin();
  lastMotionMillis = millis();
}

void loop() {
  int Read = digitalRead(ir);
  unsigned long currentMillis = millis();

  if (!isWaitingAtCenter) {
    if (currentMillis - movementMillis >= movementInterval) {
      movementMillis = currentMillis;
      eyeOffset += eyeDirection;

      if (eyeOffset >= maxOffset || eyeOffset <= -maxOffset) {
        eyeDirection *= -1;
      }

      if (eyeOffset == 0) {
        isWaitingAtCenter = true;
        centerWaitMillis = currentMillis;
      }

      if (eyeState == EYE_OPEN) eye_open();
      else if (eyeState == EYE_CLOSE) eye_close();
      else if (eyeState == EYE_SAD) eye_sad();
      else if (eyeState == EYE_BORED) eye_bored();
    }
  } else {
    if (currentMillis - centerWaitMillis >= centerPauseDuration) {
      isWaitingAtCenter = false;
    } else {
      if (eyeState == EYE_OPEN) eye_open(true);
      else if (eyeState == EYE_CLOSE) eye_close(true);
      else if (eyeState == EYE_SAD) eye_sad();
    }
  }

  if (Read == 0) {
    lastMotionMillis = currentMillis;
    eye_up();
    eyeState = EYE_IDLE;
    previousMillis = currentMillis;
    return;
  }

  if (currentMillis - lastMotionMillis >= sadDelay) {
    if (eyeState == EYE_IDLE || eyeState == EYE_CLOSE || eyeState == EYE_OPEN) {
      eyeState = EYE_BORED;
      boredMillis = currentMillis;
    }
  }

  switch (eyeState) {
    case EYE_IDLE:
      eyeState = EYE_CLOSE;
      previousMillis = currentMillis;
      break;

    case EYE_CLOSE:
      if (currentMillis - previousMillis >= (isWaitingAtCenter ? interval1_center : interval1)) {
        eyeState = EYE_OPEN;
        previousMillis = currentMillis;
      }
      break;

    case EYE_OPEN:
      if (currentMillis - previousMillis >= (isWaitingAtCenter ? interval2_center : interval2)) {
        eyeState = EYE_CLOSE;
        previousMillis = currentMillis;
      }
      break;

    case EYE_BORED:
      if (currentMillis - boredMillis >= boredDelay) {
        eyeState = EYE_SAD;
      }
      break;

    case EYE_SAD:
      break;
  }
}

// Le funzioni eye_open, eye_close, eye_up, eye_sad, eye_bored rimangono invariate


void eye_open(bool centered) {
  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);
  int eyeWidth = 30;
  int eyeHeight = 30;
  int leftX = 22 + eyeOffset;
  int rightX = 66 + eyeOffset;
  int pupilShift = eyeDirection > 0 ? 6 : -6;

  if (centered) {
    u8g2.drawBox(22, 17, eyeWidth, eyeHeight);
    u8g2.drawBox(66, 17, eyeWidth, eyeHeight);
    u8g2.drawLine(22, 10, 52, 10);
    u8g2.drawLine(66, 10, 96, 10);
    u8g2.drawDisc(22 + 15, 32, 4);
    u8g2.drawDisc(66 + 15, 32, 4);
  } else {
    int shrinkAmount = 4;
    bool goingRight = eyeDirection > 0;
    if (goingRight) {
      u8g2.drawBox(leftX, 17 + shrinkAmount, eyeWidth, eyeHeight - shrinkAmount);
      u8g2.drawBox(rightX, 17, eyeWidth, eyeHeight);
      u8g2.drawLine(leftX, 10, leftX + 16, 7);
      u8g2.drawLine(rightX, 10, rightX + eyeWidth, 10);
    } else {
      u8g2.drawBox(leftX, 17, eyeWidth, eyeHeight);
      u8g2.drawBox(rightX, 17 + shrinkAmount, eyeWidth, eyeHeight - shrinkAmount);
      u8g2.drawLine(leftX, 10, leftX + eyeWidth, 10);
      u8g2.drawLine(rightX + eyeWidth, 10, rightX + 16, 7);
    }

    u8g2.drawDisc(leftX + 15 + pupilShift, 32, 4);
    u8g2.drawDisc(rightX + 15 + pupilShift, 32, 4);
  }

  u8g2.sendBuffer();
}

void eye_close(bool centered) {
  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);
  int eyeWidth = 30;
  int leftX = 22 + eyeOffset;
  int rightX = 66 + eyeOffset;

  if (centered) {
    u8g2.drawBox(22, 32, eyeWidth, 5);
    u8g2.drawBox(66, 32, eyeWidth, 5);
    u8g2.drawLine(22, 10, 52, 10);
    u8g2.drawLine(66, 10, 96, 10);
  } else {
    int shrinkAmount = 3;
    bool goingRight = eyeDirection > 0;

    if (goingRight) {
      u8g2.drawBox(leftX, 32 + shrinkAmount, eyeWidth, 4);
      u8g2.drawBox(rightX, 32, eyeWidth, 5);
      u8g2.drawLine(leftX, 10, leftX + 16, 7);
      u8g2.drawLine(rightX, 10, rightX + eyeWidth, 10);
    } else {
      u8g2.drawBox(leftX, 32, eyeWidth, 5);
      u8g2.drawBox(rightX, 32 + shrinkAmount, eyeWidth, 4);
      u8g2.drawLine(leftX, 10, leftX + eyeWidth, 10);
      u8g2.drawLine(rightX + eyeWidth, 10, rightX + 16, 7);
    }
  }

  u8g2.sendBuffer();
}

void eye_up() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawBox(66, 20, 30, 20);
  u8g2.drawBox(22, 20, 30, 20);
  u8g2.drawFilledEllipse(81, 21, 14, 9);
  u8g2.drawFilledEllipse(37, 21, 14, 9);
  u8g2.drawFilledEllipse(37, 10, 9, 3);
  u8g2.drawFilledEllipse(81, 10, 9, 3);
  u8g2.sendBuffer();
}

void eye_sad() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawBox(66 + eyeOffset, 35, 30, 18);
  u8g2.drawBox(22 + eyeOffset, 35, 30, 18);
  u8g2.drawFilledEllipse(81 + eyeOffset, 47, 14, 9);
  u8g2.drawFilledEllipse(37 + eyeOffset, 47, 14, 9);
  u8g2.drawLine(22 + eyeOffset, 30, 36 + eyeOffset, 25);
  u8g2.drawLine(96 + eyeOffset, 30, 82 + eyeOffset, 25);
  u8g2.sendBuffer();
}

void eye_bored() {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawBox(66 + eyeOffset, 35, 30, 15);
  u8g2.drawBox(22 + eyeOffset, 35, 30, 15);
  u8g2.drawFilledEllipse(81 + eyeOffset, 45, 12, 8);
  u8g2.drawFilledEllipse(37 + eyeOffset, 45, 12, 8);
  u8g2.sendBuffer();
}
