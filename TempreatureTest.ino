// Temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>
 
// Data wire is plugged into pin 7 on the Arduino
#define ONE_WIRE_BUS 7
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// led screen
#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2);
const int switchPin = 6;
int switchState = 0;
int prevSwitchState = 0;
int reply;
float currentTemperature = 0.0;

// Led Strip control
#define MIN_TEMP 20
#define MAX_TEMP 50
// don't futz with these, illicit sums later
#define RED       9// pin for red LED
#define GREEN    8 // pin for green - never explicitly referenced
#define BLUE     10 // pin for blue - never explicitly referenced
#define SIZE    255
#define DELAY    0
#define HUE_MAX  6.0
#define HUE_DELTA 0.01

//long deltas[3] = { 5, 6, 7 };
long rgb[3];
long rgbval;
// for reasons unknown, if value !=0, the LED doesn't light. Hmm ...
// and saturation seems to be inverted
float hue=0.0, saturation=1, value=1;

long bright[3] = { 107, 67, 256};
//long bright[3] = { 256, 256, 256};

long k, temp_value;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  
  // lcd
  lcd.begin(16,2);
  pinMode(switchPin,INPUT);
  lcd.print(currentTemperature);

    // Start up the library
  sensors.begin();

 //Led
   randomSeed(analogRead(4));
  for (k=0; k<3; k++) {
    pinMode(RED + k, OUTPUT);
    rgb[k]=0;
    analogWrite(RED + k, rgb[k] * bright[k]/256);
  }

}

void loop() {
  
 sensors.requestTemperatures(); // Send the command to get temperatures
 float temperature = sensors.getTempCByIndex(0);
  Serial.print("Temperature for Device 1 is: ");
  Serial.print(temperature);
  Serial.println();
 
  if(currentTemperature!=temperature){
    // teprature has changed - need to update
    currentTemperature = temperature;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("The temp:");
    lcd.print(currentTemperature);
    lcd.print("c");
    lcd.setCursor(0,1);
    if(currentTemperature < 20.0){
      lcd.print("Ice Cold.Go Home!");
    }
     else if(currentTemperature < 32.0){
      lcd.print("Still not hot");
    }
    else if(currentTemperature < 35.0){
      lcd.print("Getting closer..");
    }
    else if(currentTemperature < 40.0){
      lcd.print("Warm!.Dip in!");
    }
    else if(currentTemperature < 45.0){
      lcd.print("HotTubTimeMachine!");
    }
    else if(currentTemperature > 45.0){
      lcd.print("Too Hot!!!!");
    }

    // Dealing with Led Strip
      float temperatureForLed = constrain(currentTemperature, MIN_TEMP, MAX_TEMP);
  
  float deltaTemp = (MAX_TEMP - MIN_TEMP);
  float deltaHue = 4 - 0;
  hue = map((temperatureForLed - MIN_TEMP) * 100, 0, deltaTemp * 100, deltaHue * 100, 0) / 100.0;
  
  //Serial << "Temperature: " << temperature << endl;
  //Serial << "HUE: " << hue << endl;
  
  rgbval=HSV_to_RGB(hue, saturation, value);
  rgb[0] = (rgbval & 0x00FF0000) >> 16; // there must be better ways
  rgb[1] = (rgbval & 0x0000FF00) >> 8;
  rgb[2] = rgbval & 0x000000FF;
  for (k=0; k<3; k++) { // for all three colours
    analogWrite(RED + k, rgb[k] * bright[k]/256);
  }
  }


  delay(400);

}

long HSV_to_RGB( float h, float s, float v ) {
  /* modified from Alvy Ray Smith's site: http://www.alvyray.com/Papers/hsv2rgb.htm */
  // H is given on [0, 6]. S and V are given on [0, 1].
  // RGB is returned as a 24-bit long #rrggbb
  int i;
  float m, n, f;

  // not very elegant way of dealing with out of range: return black
  if ((s<0.0) || (s>1.0) || (v<1.0) || (v>1.0)) {
    return 0L;
  }

  if ((h < 0.0) || (h > 6.0)) {
    return long( v * 255 ) + long( v * 255 ) * 256 + long( v * 255 ) * 65536;
  }
  i = floor(h);
  f = h - i;
  if ( !(i&1) ) {
    f = 1 - f; // if i is even
  }
  m = v * (1 - s);
  n = v * (1 - s * f);
  switch (i) {
  case 6:
  case 0: 
    return long(v * 255 ) * 65536 + long( n * 255 ) * 256 + long( m * 255);
  case 1: 
    return long(n * 255 ) * 65536 + long( v * 255 ) * 256 + long( m * 255);
  case 2: 
    return long(m * 255 ) * 65536 + long( v * 255 ) * 256 + long( n * 255);
  case 3: 
    return long(m * 255 ) * 65536 + long( n * 255 ) * 256 + long( v * 255);
  case 4: 
    return long(n * 255 ) * 65536 + long( m * 255 ) * 256 + long( v * 255);
  case 5: 
    return long(v * 255 ) * 65536 + long( m * 255 ) * 256 + long( n * 255);
  }
}
