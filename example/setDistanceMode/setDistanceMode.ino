/*!
   file getDistance.ino
   SEN0302 SEN0245 Distance Ranging Sensor
   The module is connected with Arduino Uno to read the distance
   This demo can set distance mode(short for 2m, long for 4m)
   This demo can set Threshold and get distance inside the threshold
   Copyright    [DFRobot](http://www.dfrobot.com), 2016
   Copyright    GNU Lesser General Public License
   version  V0.1
   date  2019-4-8
*/
#include "Arduino.h"
#include "Wire.h"
#include "DFRobot_VL53L1X.h"

volatile  int8_t GPIO1TRIG = 0;

DFRobot_VL53L1X sensor(&Wire);


void setup(void)
{
  Wire.begin();

  Serial.begin(9600);

  while (sensor.begin() != true){
    Serial.println("Sensor init failed!");
    delay(1000);
  }
  attachInterrupt(0, interrupt, RISING);
  
  /*ThreshLow(in mm) is the threshold under which the device raises an interrupt if window = 0,less than ThreshHigh*/
  /*ThreshHigh(in mm) is the threshold above which the device raises an interrupt if window = 1,less than measuring range */
  /*Window(0 = eBelow, 1 = eAbove, 2 = eOut and 3 = eIn)*/
  sensor.setDistanceThreshold(200, 300, eBelow);
  Serial.print("ThresholdWindow: ");
  Serial.println(sensor.getDistanceThresholdWindow());
  Serial.print("ThresholdLow:    ");
  Serial.println(sensor.getDistanceThresholdLow());
  Serial.print("ThresholdHigh:   ");
  Serial.println(sensor.getDistanceThresholdHigh());
  sensor.setDistanceModeShort();//Measuring range is about 2m
  //sensor.setDistanceModeLong();//Measuring range is about 4m
  sensor.startRanging();
  //sensor.checkForDataReady();//Return true for no interrupt,return false for interrupt
  pinMode(LED_BUILTIN, OUTPUT);
  delay(100);
}

void loop() 
{
  while(GPIO1TRIG == 0){}
  GPIO1TRIG = 0;
  int distance;
  distance = sensor.getDistance();
  Serial.print("Distance(mm): ");
  Serial.println(distance);
}

int ledF = 0;

void interrupt(){
  GPIO1TRIG = 1;
  if(ledF) {
    digitalWrite(LED_BUILTIN, LOW);  
    ledF = 0;
  } else {
    digitalWrite(LED_BUILTIN, HIGH);   
    ledF = 1;
  }
}


