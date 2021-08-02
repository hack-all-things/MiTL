#include <EEPROM.h>
const int led = LED_BUILTIN;
unsigned int bytesMem = 4096;
bool doneOnce = false;

void setup() 
{
  pinMode(led, OUTPUT);
  Serial.begin(38400);
}

void loop() {
  if(!doneOnce) {
    // clear all EEPROM with null
    for(unsigned int i=0 ; i<bytesMem ; i++) {
      EEPROM.write(i,0); // write null
    }
  
    // init first 4 bytes with "MiTL"
    EEPROM.write(0,'M');
    EEPROM.write(1,'i');
    EEPROM.write(2,'T');
    EEPROM.write(3,'L');
  
    Serial.println("done!");
    flashLED();
    doneOnce = true;  
  }
}

void flashLED()
{
  digitalWrite(led, HIGH);
  delay(100);
  
  digitalWrite(led, LOW);
  delay(100);
   
  digitalWrite(led, HIGH);
  delay(100);
  
  digitalWrite(led, LOW);
  delay(100);  
  
  digitalWrite(led, HIGH);
  delay(100);  
   
  digitalWrite(led, LOW);  
}
