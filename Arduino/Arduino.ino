/*
  lfuse: 0x62
  hfuse: 0x5f
  efuse: 0xff
 
  ATtiny lib https://github.com/SpenceKonde/ATTinyCore
  Pin 1 = PB5 = Reset
  Pin 2 = PB3 = A0
  Pin 3 = PB4 = A2
  Pin 4 = GND
  Pin 5 = PB0 = pwm0
  Pin 6 = PB1 = pwm1
  Pon 7 = PB2 = A1
  Pin 8 = VCC
 */

#include <EEPROM.h>
int pwm = 0;
int pwmMax = 159;                 // set PWM frequency to 100kHz
int cMax = 180;                   // set maximum current in mA/10
int cMin = 60;                    // set minimum current in mA/10
int cStep = 20;

int buttonSelect = 4;             // Pin 3
int buttonUp = 2;                 // Pin 7
int buttonDown = 0;               // Pin 5
int debounceTime = 100;           

boolean selectActive = false;
boolean setupActive = false;

boolean debounceUp = false;
boolean debounceDown = false;
unsigned long buttonTime = 0;

int cTarget = EEPROM.read(0);                 // reading saved Current from EEPROM EEPROM.read(0)
int hysteresis = 7;                           // add a hysteresis

void setup() {   
  analogReference(INTERNAL1V1);   // set analog reference to 1.1 Volts
  
  // initialize the digital pin as an output.  
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(1, OUTPUT);
   
  PLLCSR = 6;
  TCCR1 = 0xe3;                   // 11100011 PCK/4
  GTCCR = 0;
  
  OCR1C = pwmMax;  
}

void loop() {

  if (digitalRead(buttonSelect) == LOW) {
    if (selectActive == false) {
      selectActive = true;
      buttonTime = millis();
    }
    if ((millis() - buttonTime > 5000) && (setupActive == false)) {
      setupActive = true;
      animate();
      buttonTime = millis();
    }
  } else {
    if (selectActive == true) {
      selectActive = false;
    }
  }

  if (setupActive == true) {
    if (millis() - buttonTime > 5000) {
      setupActive = false;
      animate();
      EEPROM.update(0, cTarget);
    }

    if (digitalRead(buttonUp) == LOW) {
      buttonTime = millis();
      debounceUp = true;
    }
    if (digitalRead(buttonDown) == LOW) {
      buttonTime = millis();
      debounceDown = true;
    }

    if (cTarget < cMax - cStep && (millis() - buttonTime > debounceTime) && debounceUp == true) {
      cTarget += cStep;
      debounceUp = false;
    } else if (cTarget > cMin + cStep && (millis() - buttonTime > debounceTime) && debounceDown == true) {
      cTarget -= cStep;
      debounceDown = false;
    }
  }
  test(cTarget);
}

void test(int target) {
  int average = 0;

  for (int x = 0; x < 10; x = x + 1) average += analogRead(A0);
  average = map(average/10, 0, 1023, 0, 1100);

  if(average < cMax && average < target + hysteresis) pwm++;
  else if (average > target + hysteresis) pwm--;

  OCR1A = pwm;
}

void animate() {
  OCR1A = 0;
  delay(100);
  OCR1A = pwm;
  delay(100);
  OCR1A = 0;
  delay(100);
  OCR1A = pwm;
}

