#include "Ultrasonic.h"
#include "TFT_eSPI.h"

//Class Initializers
TFT_eSPI tft;
Ultrasonic ultrasonic(0);

void setup()
{
 //displayDistance =============================================================================================================================
 Serial.begin(9600);
 //=============================================================================================================================================
}
void loop()
{
rangeFinder();
}

//Functions

//Ultrasonic Range Finder Sensor
void rangeFinder(){
 long RangeInCentimeters;
 //Measues distance in CM with short delay to ensure fast and accurate measurements
 RangeInCentimeters = ultrasonic.MeasureInCentimeters();
 Serial.print(RangeInCentimeters);//0~400cm
 Serial.println(" cm");
 //Short delay to ensure quick refreshes of data measurements
 delay(100);
}