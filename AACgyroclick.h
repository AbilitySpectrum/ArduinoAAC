/* AACgyroclick.h 2014.11.28 : works with drawArduinoDatav2
 
 (c) Ability Spectrum, 2014.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */
#ifndef __AACgyroclick_H__
#define __AACgyroclick_H__

#include <Arduino.h>
#include <helper_3dmath.h>
#include "AACsensors.h"
#include "AACswitch.h"

class GyroClick {
public:
   GyroClick();
   //void reset();
   void setSensitivity( int );
   void loop( Quaternion );

private:
   float euclid( Quaternion, Quaternion);

   Quaternion oldq;
   long previousGrabTime;
   //low timestep for rapid processing - smaller more sensitive
   long timeStep; // 10 works; must be at least 7
   long refractory;
   long newPeriod;
   float cumS;
   long ignoreTime;

   long resetPeriod; // 40 - 400 works - longer more sensitiv
   float Avalue, Xvalue, Yvalue;
   Switch buzzer;
};

#endif
