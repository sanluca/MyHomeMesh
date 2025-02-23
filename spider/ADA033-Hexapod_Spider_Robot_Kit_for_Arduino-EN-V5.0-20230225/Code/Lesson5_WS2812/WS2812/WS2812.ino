/*************************************
File name:WS2812.ino
Description:Let WS2812 light up in different colors.
Website:www.addept.com
E-mail:support@addept.com
Author:felix
Date:2020/05/22 
*************************************/
#include <Adafruit_NeoPixel.h>    
#define led_numbers  6 //WS2812 number  of LED
#define PIN  A1  //WS2812 PIN                                        
Adafruit_NeoPixel strip = Adafruit_NeoPixel(led_numbers, PIN, NEO_GRB + NEO_KHZ800);//NEO_KHZ400+NEO_RGB
void setup() {
  //Initialize WS2812
  strip.begin();
  //Set the WS2812 brightness
  strip.setBrightness(50);
}
void loop() {
  //Get the color value at random
  int R=random(0,255);
  int G=random(0,255);
  int B=random(0,255);
  for(int i=0;i<led_numbers;i++){
    //i:Which one lamp ,strip.Color(R,G,B):R、G、Bcolor value 
    strip.setPixelColor(i,strip.Color(R,G,B));
    //Light WS2812
    strip.show();
    delay(50);
  }
}
