/* AACgyromouse.h : 2014.11.28 
 
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
#ifndef _AACgyromouse_h__
#define _AACgyromouse_h__
#include <Arduino.h>
#include "helper_3dmath.h"
#include "AACswitch.h"
#include "AACsensors.h"

class GyroMouse 
{
public:
   GyroMouse();
   void mouseReset();
   void loop(VectorInt16);
   void setSensitivity(int); // between 0 and 100
   void normalHigh0(int);  // mouse on off: 0 for light sensor; 1 for cap touch 
   void normalHigh1(int);  // mouse click: 0 for light sensor; 1 for cap touch

   void DoMouseWork(VectorInt16);
   int readAxis(int, VectorInt16);

   // AnalogSensor args: input, refractory, rest level, debug at 1 or 2
   // An1 for left hand (press,release), An0 for right hand (stop mouse)
   AnalogSensor An0; // (int, int, int, int ); // A0==18 on Pro Micro: once stopped, mouse inactive for 300ms
   AnalogSensor An1; // (int, int, int, int );  // A1==19 on Pro Micro: 50ms pause once clicked
   Switch buzzer; // BUZZER on pin 4

private:

   int _inverted0, _inverted1;
   int minima[2], maxima[2]; // ylh not used
   int mouseReading[2], prev[2], prev2[2];
   int msize[2];
   int mouseactivated;
   int _sens;
};
#endif
