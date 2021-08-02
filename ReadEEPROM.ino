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
    for(unsigned int i=0 ; i<bytesMem ; i++) {
      int j = EEPROM.read(i);
      char c = j;
      
      if(c==0) {
        Serial.println("EEPROM end");
        break;
      }
      else {
        Serial.print("EEPROM read: ");
        Serial.println(c);
      }
      delay(50);
    }
    doneOnce = true;  
  }
}
