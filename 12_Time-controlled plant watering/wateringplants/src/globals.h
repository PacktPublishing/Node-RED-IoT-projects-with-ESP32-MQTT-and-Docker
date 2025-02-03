#include <Arduino.h>
#ifndef globals_h
#define globals_h

unsigned long previousMillis = millis();
const byte relaisPumpGluecksfeder = 20;

//HC-SR04
int trigger1=2;
int echo1=3;

long distance1=0;
//long distance2=0;

enum states
{
  GETDATA,
  PUMPING,
  TIMEFORSLEEP,
  WAIT
} state;

// alterantives
//  states state;
//  states state2;

#endif