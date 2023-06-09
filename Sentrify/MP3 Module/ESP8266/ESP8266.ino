/*
  Example of using the WTV020SD16P chip on an ESP8266.

  Connect the WTV020SD16P to your ESP8266 and define the pins below.

  created 3 Nov 2019
  by Thomas A. Hirsch

  original version created by ELECTRONOOBS, 14 Oct 2016.

  This example code is in the public domain.
*/
#include <Wtv020sd16p.h>


// Constants to define the used pins.
#define resetPin 4 // The pin number of the reset pin.
#define clockPin 5 // The pin number of the clock pin.
#define dataPin 6 // The pin number of the data pin.
#define busyPin 7 // The pin number of the busy pin.

// Instance of WTV020SD16P
Wtv020sd16p wtv020sd16p(resetPin, clockPin, dataPin, busyPin);

void setup() {
  wtv020sd16p.reset();
}

void loop() {
  //wtv020sd16p.asyncPlayVoice(0);

  ////////////////////////////////////////////
  //This are all the functions for the WTV020 module
  //Use yours and delete the others
  //Each function is explained
  ///////////////////////////////////////////
  //Plays synchronously an audio file. Busy pin is used for this method.
  wtv020sd16p.playVoice(0);
  //Plays asynchronously an audio file.
  /*wtv020sd16p.asyncPlayVoice(1);
  //Plays audio file number 1 during 5 seconds.
  delay(5000);
  //Pauses audio file number 1 during 5 seconds.  
  wtv020sd16p.pauseVoice();
  delay(5000);
  //Resumes audio file number 1 during 5 seconds.
  wtv020sd16p.pauseVoice();
  delay(5000);  
  //Stops current audio file playing.
  wtv020sd16p.stopVoice();
  //Plays synchronously an audio file. Busy pin is used for this method.  
  wtv020sd16p.asyncPlayVoice(2);
  delay(2000);   
  //Mutes audio file number 2 during 2 seconds.
  wtv020sd16p.mute();
  delay(2000);
  //Unmutes audio file number 2 during 2 seconds.
  wtv020sd16p.unmute();
  delay(2000);    
  //Stops current audio file playing.
  wtv020sd16p.stopVoice();*/
}
