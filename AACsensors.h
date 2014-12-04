/* AACsensors.h
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

#ifndef __AACsensors_h__
#define __AACsensors_h__

#include <Arduino.h>

class Sensor
{
  public:
    Sensor(int, int, int);  // args: in_pin; scale; debug
    int Read();
    int RawValue();
    int HeldPeriod();

  private:
    int _sensorPin;     // analog input for Flex or Force
    int _scale;
    int sensorValue; 
    int sensorLevel;   // last level
    int sensorLasted;  // stable reading for how many iterations
    unsigned long _lastDetected;
    int _refractory;

    int _debug;
 };
 
class DigitalSensor
{
  public:
    DigitalSensor(int,int,int,int); // debug
    int Read(void);
    void setNormalHigh( int );
    void setPin ( int );
  private:
    unsigned long _lastDetected;
    int _sensorPin;
    int _refractory;
    int _normalhigh;
    int _debug;
};

// recalibrates when held at the same level for 5 cycles
class AnalogSensor
{
  public:
    AnalogSensor(int, int, int, int);
    void setNormalHigh( int );
    int Read();
    int Level(int); // number of levels differentiated: 2 or more
    void Reset();
    void Dump();
    void setPin ( int );
  private:
    int _normalhigh;
    int _Level3(), _Level4(); // old code for 3 or 4 level differentiation
    int minima, maxima;
    int sensorValue;
    int levelValue;
    int held, heldLevel;
    long lastheld;  // new style - recalibrate if held > 7 secs
    int _sensorPin;
    unsigned long _lastDetected;
    int _refractory;
    int _atRest;
    int _debug;
};

#endif
