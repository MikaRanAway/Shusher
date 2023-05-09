//--MQTT--------------------------------------------------------------
#include <rpcWiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
//for simplicity, the internet settings are hardcoded for now but will be a part of a header file later in the git ignore.
const char* ssid = "ISAACHP";
const char* password = "isaac123";
const char* server = "192.168.137.1";
WiFiClient wioClient;
PubSubClient client(wioClient);
//---------------------------------------------------------------------
#define baseThreshold 49
#define sampleWindow 10
#define peakToPeakAverages 10
#define DEBUG 
const int brightnesslevellights = 20;
int val;
int sample;
int loudness;
int loudnessMaxReachedCount;
int constexpr Thresholds[] = {baseThreshold,baseThreshold+3,baseThreshold+6,baseThreshold+9,baseThreshold+12,baseThreshold+15,baseThreshold+18,baseThreshold+20,baseThreshold+22,baseThreshold+26};



// RGB LED Stick-------------------------------------------------------
#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 10
#define DATA_PIN 0
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_RGB);
//----------------------------------------------------------------------


void setup() {
  // put your setup code here, to run once:
  //--MQTT------------
  WiFi.begin(ssid, password);
  client.setServer(server,1883);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
}
  //-------------------
  #ifdef DEBUG
 Serial.begin(115200);
 #endif
 ledStartupTest();
 pinMode(WIO_MIC, INPUT);
 
}

void loop() {
  // put your main code here, to run repeatedly:
LoudnessSensorLoudValue();
setLedStick();
// this is a known issue where it has to connect to even display data
if (!client.connected()) {
    reconnect();
  }

}
//Functions

//Loudness Sensor Thresholds
void LoudnessSensorLoudValue() {
  
    //this portion of code was taken from https://how2electronics.com/iot-decibelmeter-sound-sensor-esp8266/ it creates whats known as an 'envelope' to encompass the sound. This is necessary due to the way the analog value is recorded and how sound is a wave.
  
   float peakToPeak = 0;                                  // peak-to-peak level
 
   unsigned int signalMax = 0;                            //minimum value
   unsigned int signalMin = 1024;                         //maximum value because its 10 bit
 
                                                          // collect data for 50 mS
  unsigned long startMillis = 0;                   // Start of sample window
  for(unsigned int i = 0; i <peakToPeakAverages ; i++){
    startMillis = millis(); 
    while (millis() - startMillis < sampleWindow)
    {
        sample = analogRead(WIO_MIC);                    //get reading from microphone
        Serial.print("0, ");                             // debugging printing
        Serial.println(sample);
        delay(1);
        if (sample < 1024)                                  // toss out spurious readings
        {
          if (sample > signalMax)
          {
              signalMax = sample;                           // save just the max levels
          }
          else if (sample < signalMin)
          {
              signalMin = sample;                           // save just the min levels
          }
        }
    }
    peakToPeak += signalMax - signalMin;                    // max - min = peak-peak amplitude
   }
   peakToPeak /= peakToPeakAverages;
   val = map(peakToPeak,20,900,49.5,90);              // maps the value to a "decibel" (this value is not entirely accurate and is influenced by the microphone used and its relative sensitivity)
   //Serial.println(peakToPeak);
	/////

 //Serial.println(val);

// based on the decibel a loudness value is assigned

  if (val <= Thresholds[0]) {
  (loudness = 1);
  client.publish("shusher/loudness", "1");
  }
  else if (val > Thresholds[0] && val <=  Thresholds[1]) {
   (loudness = 2);
   client.publish("shusher/loudness", "2");
  }
   else if (val >  Thresholds[1] && val <=  Thresholds[2]) {
   (loudness = 3);
   client.publish("shusher/loudness", "3");
  }
   else if (val >  Thresholds[2] && val <=  Thresholds[3]) {
   (loudness = 4);
   client.publish("shusher/loudness", "4");
  }
   else if (val >  Thresholds[3] && val <=  Thresholds[4]) {
   (loudness = 5);
   client.publish("shusher/loudness", "5");
   }
 else if (val >  Thresholds[4] && val <=  Thresholds[5]) {
   (loudness = 6);
   client.publish("shusher/loudness", "6");
  }   
   else if (val >  Thresholds[5] && val <=  Thresholds[6]) {
   (loudness = 7);
   client.publish("shusher/loudness", "7");
  }
   else if (val >  Thresholds[6] && val <=  Thresholds[7]) {
   (loudness = 8);
   client.publish("shusher/loudness", "8");
  }
 else if (val >  Thresholds[7] && val <=  Thresholds[8]) {
   (loudness = 9);
   client.publish("shusher/loudness", "9");
  }  
   else if (val >  Thresholds[8] ) {

   (loudness = 10);
   client.publish("shusher/loudness", "10");

   loudness = 10;
   loudnessMaxReachedCount++; // adds one to a count of how many times the max threshold was reached
  
  }
 
  }
// RGB LED Stick Functions
void ledStartupTest(){    // Testing that all LEDs work(LightShow ;) )
  strip.begin();
  strip.setBrightness(brightnesslevellights);
  strip.clear(); 
  for(int i= 0; i<=NUM_LEDS;i++){
    delay(200);
    if(i<3){
    strip.setPixelColor(i, 255,0,0);       //setting the color of led number 1-3 to green
    
  }
    else if(i<7){
    strip.setPixelColor(i,255,255,0);    //setting the color of led number 4-7 yellow
  }

    else if(i<10) {
    strip.setPixelColor(i,0,255,0);       //Setting the color of LED number 8-9 red
  }
    else if(i== 10){                      //the whole Stick flashes red when the last LED is reached.
       for(int j = 0; j < 5; j++){
        for(int k = 0; k < NUM_LEDS; k++){
          strip.setPixelColor(k, 0, 255, 0);
        }
        strip.show();
        delay(200);
        strip.clear();
        strip.show();
        delay(200);
        }
    }
    delay(150);
    strip.show();
  }
  strip.clear();
  

}



void setLedStick(){                 //Activating the LEDs dependent on the loudness which is determined by the thresholds set at the top.
  
  if(loudness >= 1){
    strip.setPixelColor(0, 255,0,0);
  }
  if(loudness >= 2){
    strip.setPixelColor(1, 255,0,0);
  }
  if(loudness >= 3){
    strip.setPixelColor(2, 255,0,0);
  }
  if(loudness >= 4){
    strip.setPixelColor(3, 255,255,0);
  }
  if(loudness >= 5){
    strip.setPixelColor(4, 255,255,0);
  }
  if(loudness >= 6){
    strip.setPixelColor(5, 255,255,0);
  }
  if(loudness >= 7){
    strip.setPixelColor(6, 255,255,0);
  }
  if(loudness >= 8){
    strip.setPixelColor(7, 0,255,0);
  }
  if(loudness >= 9){
    strip.setPixelColor(8, 0,255,0);
  }
  
   if(loudness >= 10){                 //when the stick is at maximum(meaning led number 10) the whole stick flashes red
     for(int j = 0; j < 5; j++){
        for(int k = 0; k < NUM_LEDS; k++){
          strip.setPixelColor(k, 0, 255, 0);
        }
        strip.show();
        delay(200);
        strip.clear();
        strip.show();
        delay(200);
        }
    }
  
  strip.show();
  strip.clear();
  } 

  void reconnect() {                                                  // method is taken fron the MQTT workshop
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WioTerminal";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}




//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------





//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------