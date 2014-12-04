/* AACsensors.cpp
 * 
 * Copyright (c) 2014 Ability Spectrum.  For more information, see
 * 
 * http://abilityspectrum.com/
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "AACsensors.h"

Sensor::Sensor( int in_pin, int scale, int debug) {
  _debug = debug;
  _sensorPin = in_pin;
  pinMode(_sensorPin,INPUT);
  _scale = scale;

}

int Sensor::Read() {

  /* Sensor BEGIN */
  sensorValue = analogRead(_sensorPin); 
  int newlevel = 0;
  int x=0;
  while( sensorValue>x ) {
    x=x+_scale;
    newlevel = newlevel + 1;
  }
  if(sensorLevel == newlevel) {
    sensorLasted +=1;
  } else {
    if( _debug ) {  // level changed, print how long previous level lasted & the new level
      int x=0;
      Serial.print(" "); Serial.println(sensorLasted);
      Serial.print(newlevel);
      while (newlevel>x) {
        Serial.print("*");
        x=x+1;
      }
     } 
    sensorLasted = 0;
    sensorLevel = newlevel;
  }
  return sensorLevel;
}

int Sensor::RawValue() { return sensorValue; }
int Sensor::HeldPeriod() { return sensorLasted; }

/////////////

DigitalSensor::DigitalSensor(int pin, int ref=500, int normalhigh=0, int debug=0) {
  _sensorPin=pin;
  _refractory = ref;
  _normalhigh = normalhigh;
  _debug=debug;

  _lastDetected= millis();
  pinMode(pin,INPUT);
}

void DigitalSensor::setNormalHigh ( int normalhigh = 0 ) {
  _normalhigh = normalhigh;
  /* ylh
  if (_normalhigh == 0 ) {
    pinMode(_sensorPin, INPUT);
  } else {
    pinMode(_sensorPin, INPUT_PULLUP);
  }
  digitalWrite( _sensorPin, _normalhigh );
  */
}

void DigitalSensor::setPin( int p ) {
  _sensorPin = p;
    pinMode(_sensorPin, INPUT);
  // setNormalHigh( _normalhigh );
}

int DigitalSensor::Read() {
  int x;
  x=digitalRead(_sensorPin);
  if(_debug) {Serial.print("Pin "); Serial.print(_sensorPin); Serial.print(" = "); Serial.println(x); }

  if( (millis() - _lastDetected) < _refractory ) {
    return 0;  // force read of 0 if within refractroy period
  }
  if( x==_normalhigh ) {
    return 0;
  } else {
    _lastDetected = millis();
    return 1;
  }
}


// Analog sensor: differentiates ternary and quarternary levels
// recalibrates when held at the same level for n=6 cycles (reads) && 8 seconds
// Args: input pin,  refractory period,  atRest, debug
// Forced return of last value if read within refractory period 
// atRest is the rest level that is common. No recalibration at this level
AnalogSensor::AnalogSensor(int pin=A4, int ref=0, int atRest=0, int debug=0) {
  if( debug ) Serial.begin(9600);
  _sensorPin=pin;
  _refractory = ref;
  _atRest= atRest;
  _debug=debug;
  pinMode(pin,INPUT);

  Reset();
}

void AnalogSensor::Reset() {
  minima=10000;
  maxima=-10000;
  held=0; heldLevel=_atRest;
  lastheld=millis();
  if(_debug>2 ) Serial.println("                          * Reset");

  Level(2);
}

void AnalogSensor::setPin ( int p ) {
  _sensorPin = p;
  /* ylh
  if (_atRest == 0 ) {
    pinMode(_sensorPin, INPUT);
  } else {
    pinMode(_sensorPin, INPUT_PULLUP);
  }
  */
}
void AnalogSensor::setNormalHigh ( int normalhigh = 0 ) {
  _atRest= normalhigh;
  heldLevel = _atRest;
  /*
  if (_atRest == 0 ) {
    pinMode(_sensorPin, INPUT);
  } else {
    pinMode(_sensorPin, INPUT_PULLUP);
  }
  //  ???? digitalWrite( _sensorPin, _atRest );
  */
}

int AnalogSensor::Read() {

   sensorValue = analogRead(_sensorPin);
   if( sensorValue < minima) minima = sensorValue;
   if( sensorValue > maxima) maxima = sensorValue;
   if( abs(maxima-minima) <100) {
     // usually following recalibration; need to push min and max apart
     // recalibrate only when NOT at the _atRest state so adjust either min or max
     if(_atRest==0) {  
       minima=minima-50;     
     } else {
       maxima=maxima+50;
     }
   }
   return sensorValue;
}

// takes reading, how many levels?  3 means 0,1,2 as possible return values
int AnalogSensor::Level(int level) {
  int x;

  Read();
  if( (millis() - _lastDetected) < _refractory ) {
   //Serial.print((millis()-_lastDetected));
   //Serial.println("     ----- in refractory ----" );
   return heldLevel;   // return last value if within refractroy period
  }

   if( level<0 || level >4 ) level = 4; // not practical beyond 3 levels
   x=map( sensorValue, minima, maxima, 0, level);
   x=constrain( x, 0, level-1);  // throw away the high anchor

  // go into calibration mode
  if( x!=_atRest ) {
    if( x==heldLevel ) {
      held++; 
      if( held>6 && (millis() - lastheld) >10000 ) {
        Reset();
        lastheld = millis();
        _lastDetected = millis();
      }
    } else {
       heldLevel=x;
       held=0;
       lastheld = millis();
       _lastDetected = millis();
    }
  }

  levelValue = x; 
  return levelValue; // also a class variable
}

void AnalogSensor::Dump() {
  if(_debug) {
    Serial.print("Sensor=");
    Serial.print(sensorValue);
    Serial.print("   "); Serial.print(minima);
    Serial.print("-"); Serial.print(maxima);  
    Serial.println();

    if( _debug>1 ) {
      Serial.print(" now=");
      Serial.print(millis());
      Serial.print(" last=");
      Serial.print(_lastDetected);
      Serial.print(" cnt=");
      Serial.print(held);
      Serial.print(" reported=");
      Serial.print(heldLevel);
      Serial.print(" actual=");
      Serial.println(levelValue);
    }
  }
}

// old code
int AnalogSensor::_Level3() {
  if( abs(sensorValue - minima) < (maxima-minima)/3 ) return 0;
  if( abs(maxima - sensorValue) < (maxima-minima)/3 ) return 2;
  return 1;
}

int AnalogSensor::_Level4() {
  if( abs(sensorValue - minima) < (maxima-minima)/4 ) return 0;
  if( abs(maxima - sensorValue) < (maxima-minima)/4 ) return 3;
  if( abs(sensorValue - minima) < (maxima-minima)/2 ) return 1;
  return 2;
}


