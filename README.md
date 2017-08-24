# RgbLed 
Arduino library to control rgb led (common cathode) as status indicator.

Eg.

````C++
#include <RgbLed.h>

RgbLed rgb_led(D8,D6,D7); //initialize with pins
unsigned long counter = 0;

void setup() {
  rgb_led.setColor(LED_COLOR_BLUE);
}

void loop() {
  //blocking functions like 'delay(1000);' will screw up library's internal loop.
  doSomethingNonBlocking();
  rgb_led.execute(); //need to run this to for internal loop
}

void doSomethingNonBlocking() {
  counter++;
  if (counter == 3000) {
    rgb_led.blinkColor(LED_COLOR_GREEN);
  } else if (counter == 6000) {
    rgb_led.blinkColor(LED_COLOR_RED);
  } else if (counter == 9000) {
    rgb_led.setColor(LED_COLOR_BLUE, true); //persist, will be blue again after finished blinking
  } else if (counter == 12000) {
    rgb_led.blinkColor(LED_COLOR_MAGENTA); 
  } else if (counter == 15000) {
    rgb_led.rotateColor(); //loop through all the colors
  }
  delay(1); //make it run slower
}
````
