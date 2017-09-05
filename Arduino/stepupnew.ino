/*
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
//#include <SoftwareSerial.h>
//#define RX    99                  // no Pin
//#define TX    3                   //  Pin 3

#include <EEPROM.h>
int buttonSelect = 4;             // Pin 3
int buttonUp = 2;                 // Pin 7
int buttonDown = 0;               // Pin 5

int pwmMax = 159;                 // 100kHz

int debounceTime = 100;
int debounceUp = 0;
int debounceDown = 0;
unsigned long buttonTime = 0;

int cTarget = EEPROM.read(1);                // set starting Current to 10.0 mA
int hysteresis = 5;               // plus a hysteresis of 0.3 mA

void setup() {   
//  Serial.begin(9600);
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
  //OCR1A = 10;
}

void loop() {
  if(digitalRead(buttonUp) == LOW && digitalRead(buttonSelect) == LOW) {
    buttonTime = millis();
    debounceUp = 1;
  }
  if(digitalRead(buttonDown) == LOW && digitalRead(buttonSelect) == LOW) {
    buttonTime = millis();
    debounceDown = 1;
  }

  if(cTarget <= 190 && (millis() - buttonTime > debounceTime) && debounceUp == 1) {
    cTarget += 10;
    debounceUp = 0;
    EEPROM.update(1, cTarget);
  }
  if(cTarget >= 10 && (millis() - buttonTime > debounceTime) && debounceDown == 1) {
    cTarget -= 10;
    debounceDown = 0;
    EEPROM.update(1, cTarget);
  }

  test(cTarget);
}

void test(int target) {
  static int pwm = 0;
  int average = 0;

  for (int x = 0; x < 10; x = x + 1) average += analogRead(A0);
  average = map(average/10, 0, 1023, 0, 1100);

  if(average < target + hysteresis) pwm++;
  else if (average > target + hysteresis) pwm--;

  OCR1A = pwm;
}
