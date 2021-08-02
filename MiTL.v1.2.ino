#include <EEPROM.h>

unsigned int bytesMem = 4096;
unsigned int curEEPROMbyte = 0;
const int  sampleSize = 7;
const int led = LED_BUILTIN;
const int target = 10;
bool voltsWasLessThanFive = false;
unsigned int iVolts = 0;
unsigned int iSample = 0;
unsigned int iAvg = 0;
unsigned int iPrevAvg = 0;
int numSameAvgs = 0;
unsigned int maxVolts = 250;
bool waitForFiveVolts = false;
bool debug = false;

 void WriteToEEPROM(char value) {
   if(curEEPROMbyte < bytesMem) {
     Serial.println("Writing to EEPROM:" + value);
     EEPROM.write(curEEPROMbyte, value);
     curEEPROMbyte ++;
   }
 }

void Save(int val){
  if(val <= 5)                      {WriteToEEPROM('0'); Serial.println("PIN pad keypress detected: 0");}
  else if(val > 5 && val <= 12)     {WriteToEEPROM('1'); Serial.println("PIN pad keypress detected: 1");}
  else if(val > 12 && val <= 20)    {WriteToEEPROM('2'); Serial.println("PIN pad keypress detected: 2");}
  else if(val > 20 && val <= 30)    {WriteToEEPROM('3'); Serial.println("PIN pad keypress detected: 3");}
  else if(val > 30 && val <= 40)    {WriteToEEPROM('4'); Serial.println("PIN pad keypress detected: 4");}
  else if(val > 40 && val <= 55)    {WriteToEEPROM('5'); Serial.println("PIN pad keypress detected: 5");}
  else if(val > 55 && val <= 75)    {WriteToEEPROM('6'); Serial.println("PIN pad keypress detected: 6");}
  else if(val > 75 && val <= 90)    {WriteToEEPROM('7'); Serial.println("PIN pad keypress detected: 7");}
  else if(val > 90 && val <= 120)   {WriteToEEPROM('8'); Serial.println("PIN pad keypress detected: 8");}
  else if(val > 120 && val <= 145)  {WriteToEEPROM('9'); Serial.println("PIN pad keypress detected: 9");}
  else                              {WriteToEEPROM('U'); Serial.println("Unknown PIN pad keypress detected: " + val);}
}

void SetupEEPROM() {
   int M = EEPROM.read(0);
   char cM = M;
   int i = EEPROM.read(1);
   char ci = i;
   int T = EEPROM.read(2);
   char cT  = T;
   int L = EEPROM.read(3);
   char cL = L;
   
   if((cM=='M') && (ci=='i') && (cT=='T') && (cL=='L')) {
     // EEPROM has been initialized. Where is the end of data?
     for(unsigned int i=4 ; i<bytesMem ; i++){
       if(EEPROM.read(i) == 0){
         curEEPROMbyte = i;
         Serial.print("EEPROM data start: ");       
         Serial.println(curEEPROMbyte);       
         break; // the end
       }
     }
   }
   else {
     // EEPROM has NOT been initialized
     Serial.println("Initializing EEPROM...");
     
     EEPROM.write(0,'M');
     EEPROM.write(1,'i');
     EEPROM.write(2,'T');
     EEPROM.write(3,'L');

     for(unsigned int i=4 ; i<bytesMem ; i++) {
       EEPROM.write(i,0); // write nulls to the end
     }

     curEEPROMbyte = 4;
   }
 }

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(A0,INPUT);
  analogReference(INTERNAL);                
  Serial.begin(38400);
  Serial.println("Man in The Lock");
  Serial.println("");
  SetupEEPROM();
}

void loop()
{
  iVolts = analogRead(A0);
  if(debug)Serial.println("iVolts = " + String(iVolts));
  
  if(iVolts==1023 && voltsWasLessThanFive)
  {
    if(debug)Serial.println("Back to 5v");
    waitForFiveVolts = false;
    voltsWasLessThanFive = false;
  }
    
  iVolts = analogRead(A0);

  if(iVolts == 0 && !waitForFiveVolts)
  {
    if(debug)Serial.println("Voltage dropped to zero");
    iSample = 0;
    
    // take a sample of voltages
    for(int i=0 ; i < sampleSize ; i++)
    {
      iSample += analogRead(A0);
    }
    
    iAvg = iSample/sampleSize;
    voltsWasLessThanFive = true;
    
    // if its within 1, treat it as a match
    if((iPrevAvg == iAvg-1) || (iPrevAvg == iAvg+1))
    {
      numSameAvgs ++;
      if(debug)Serial.println("(1) numSameAvgs: " + (String)numSameAvgs);
      
      if(numSameAvgs == target)
      {
        iPrevAvg = 0;
        numSameAvgs = 0;
        Save(iAvg);
        waitForFiveVolts = true;
        delay(250);
      }
    }
    else if(iPrevAvg != iAvg)
    {
      numSameAvgs = 1;    
      if(debug) Serial.println("(3) Voltage " + (String)iAvg + " NOT same as prev: " + (String)iPrevAvg);
      iPrevAvg = iAvg;
    }
    else
    {
      numSameAvgs ++;
      if(debug) Serial.println("(1) numSameAvgs: " + (String)numSameAvgs);
      
      if(numSameAvgs == target)
      {
        iPrevAvg = 0;
        numSameAvgs = 0;
        Save(iAvg);
        waitForFiveVolts = true;
        delay(250);
      }
    }
  }

  iVolts = analogRead(A0);
  if(iVolts <= maxVolts && !waitForFiveVolts)
  {
    iSample = 0;
    
    // take a sample of voltages
    for(int i=0 ; i < sampleSize ; i++)
    {
      iSample += analogRead(A0);
    }
    
    iAvg = iSample/sampleSize;
    
    if(iAvg < maxVolts)
    {
      voltsWasLessThanFive = true;

      // if its within 1, treat it as a match
      if((iPrevAvg == iAvg-1) || (iPrevAvg == iAvg+1))
      {
        numSameAvgs ++;
         if(debug) Serial.println("(1) numSameAvgs: " + (String)numSameAvgs);
      
        if(numSameAvgs == target)
        {
          iPrevAvg = 0;
          numSameAvgs = 0;
          if(debug) Serial.println("(7) met target: " + (String)iAvg);
          Save(iAvg);
          waitForFiveVolts = true;
          delay(250);
        }
      }
      else if(iPrevAvg != iAvg)
      {
        numSameAvgs = 1;   
        if(debug) Serial.println("(8) Voltage " + (String)iAvg + " NOT same as prev: " + (String)iPrevAvg);
        iPrevAvg = iAvg; 
      }
      else
      {
        numSameAvgs ++;
        if(debug) Serial.println("(1) numSameAvgs: " + (String)numSameAvgs);
  
        if(numSameAvgs == target)
        {
          iPrevAvg = 0;
          numSameAvgs = 0;
          if(debug) Serial.println("(10) met target: " + (String)iAvg);
          Save(iAvg);
          waitForFiveVolts = true;
          delay(250);
        }
      }
    }
  }     
}
