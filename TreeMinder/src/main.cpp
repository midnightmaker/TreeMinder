#include <Arduino.h>

int ADCPin34 = 34;
int LiquidSensorEnablePin32 = 32;
float ADCMax = 2250;
float ADCMin = 1840;

int brightness = 235;

float ADCScale = 100.0f/(ADCMax-ADCMin);

int starLeds[] = {12,14,27,26,25,13};
uint8_t ledPattern[] = {0,0,0,0,0,0};
bool blinkEmergency = false;
int EmergencyLed = 13;


static void Rotate(int rotations);

void setup(){
  // configure LED PWM functionalitites
  Serial.begin(115200);

  // attach the channel to the GPIO to be controlled
  for( int i=0; i<6; i++)
  {
    ledcSetup(i, 5000, 8);
    ledcAttachPin(starLeds[i], i);
  }
  pinMode (LiquidSensorEnablePin32, OUTPUT); 
  digitalWrite(LiquidSensorEnablePin32, HIGH);
  Serial.println("Started...");
}

/*void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode (13, OUTPUT);
  pinMode (12, OUTPUT);
  pinMode (14, OUTPUT);
  pinMode (27, OUTPUT);
  pinMode (26, OUTPUT);
  pinMode (25, OUTPUT);
  pinMode (2, OUTPUT);
  pinMode (LiquidSensorEnablePin32, OUTPUT); 
  digitalWrite(LiquidSensorEnablePin32, HIGH);
  Serial.println("Started...");
  Rotate(5);

}
*/


const uint32_t blinkInterval = 500;

int idx = 0;
int waterLevel = 0;

void Rotate(int rotations)
{
  for( int i=0; i<rotations; i++)
  {
    for( int j=0; j<5; j++)
    {
      //digitalWrite( starLeds[j], LOW);
      ledcWrite(j, brightness);
      delay(50);
    //  digitalWrite( starLeds[j], HIGH);
      ledcWrite(j, 255);
      delay(50);
    }
    for( int j=4; j>-1; j--)
    {
     // digitalWrite( starLeds[j], LOW);
      ledcWrite(j, brightness);
      delay(50);
     // digitalWrite( starLeds[j], HIGH);
      ledcWrite(j, 255);
      delay(50);
    }
  }
}

float averageLevel = -1.0;

void loop() {
  
  waterLevel = analogRead(ADCPin34);
  float scaledValue = ADCScale * (waterLevel - ADCMin);
  if( scaledValue < 0.0 )
    scaledValue = 0.0;
  if( scaledValue > 100.0 )
    scaledValue = 100.0; 

  if( averageLevel < 0.0 )  
    averageLevel = scaledValue;
  else
  {
    averageLevel = averageLevel - (averageLevel - scaledValue)*0.2;
  }
  

  Serial.print(waterLevel);
  Serial.print("   %=");
  Serial.print(scaledValue);
  Serial.print("    Avg = ");
  Serial.println(averageLevel);


  // Default all off
  ledPattern[0] = HIGH;
  ledPattern[1] = HIGH;
  ledPattern[2] = HIGH;
  ledPattern[3] = HIGH;
  ledPattern[4] = HIGH;

  if( averageLevel < 5.0 )
  {
    if( ledPattern[5] == HIGH )
      ledPattern[5] = LOW;
      else
      ledPattern[5] = HIGH;
  }
  else
  {
    ledPattern[5] = LOW;
  }

  if( averageLevel > 10.0)
  {
    ledPattern[0] = LOW;
  }

  if( averageLevel > 20.0) 
  {
    ledPattern[0] = LOW;
    ledPattern[1] = LOW;
  }
  if( averageLevel > 40.0 )
  {
    ledPattern[0] = LOW;
    ledPattern[1] = LOW;
    ledPattern[2] = LOW;
  }
  if( averageLevel > 60.0 )
  {
    ledPattern[0] = LOW;
    ledPattern[1] = LOW;
    ledPattern[2] = LOW;
    ledPattern[3] = LOW;
  }
  if( averageLevel > 80.0 )
  {
    ledPattern[0] = LOW;
    ledPattern[1] = LOW;
    ledPattern[2] = LOW;
    ledPattern[3] = LOW;
    ledPattern[4] = LOW;
  }

  for( int j=0; j<6; j++)
  {
    if( ledPattern[j] == LOW )
    {
      ledcWrite(j, brightness);
    }
    else
    {
      ledcWrite(j, 255);
    }

    /*
    digitalWrite( starLeds[j], ledPattern[j]);
    Serial.print("Led = " );
    Serial.print(starLeds[j]);
    Serial.print("   Value =");
    Serial.println(ledPattern[j]);
    */
  }
 
  if( idx++ % 120 == 0 )
    Rotate(1);

  delay(500);
}