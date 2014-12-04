/* AACgyroclick.cpp : 2014.11.28 
 
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
#include <Arduino.h>
#include <helper_3dmath.h>
#include "AACsensors.h"
#include "AACswitch.h"
#include "AACgyroclick.h"

GyroClick::GyroClick() : buzzer( 4, 40, 0) {
   previousGrabTime = 0;
   //low timestep for rapid processing - smaller more sensitive
   timeStep = 10; // 10 works; must be at least 7
   refractory = 500;
   newPeriod = 0;
   cumS = 0;
   ignoreTime = 0;
   resetPeriod = 80; // 40 - 400 works - longer more sensitiv
   Avalue = 0, Xvalue = 0, Yvalue =0;
}

void GyroClick::setSensitivity( int s ) {
   resetPeriod = 40 + s;
}

void GyroClick::loop( Quaternion q ) {

  buzzer.loop();

   if( (millis() - previousGrabTime) >= timeStep)  {
      previousGrabTime = millis();
      Avalue = euclid( q, oldq );
      Xvalue += Avalue;

      if( (previousGrabTime - ignoreTime) > resetPeriod ) {
         Xvalue = 0;
         ignoreTime = previousGrabTime;
      }
      if( (millis() - newPeriod) > refractory ) {
         Yvalue = Xvalue > 0.030 ? 1 : 0;  /* 0.015 - tested value higher less sensitive*/
         if( Yvalue == 1 ) {
            buzzer.On(50);
            Keyboard.press(KEY_UP_ARROW); 
            Keyboard.press(KEY_DOWN_ARROW); 
            Keyboard.releaseAll();
            //            Serial.print( newPeriod );
            //            Serial.println ( " +" );
            Xvalue = 0;
            ignoreTime = previousGrabTime;
            newPeriod = ignoreTime;
         } // else Serial.print (".");

      }

      oldq = q;
   }
}

float GyroClick::euclid( Quaternion o, Quaternion n ) {

   float sumsqs = 0;
   sumsqs += (o.w - n.w)*(o.w - n.w);
   sumsqs += (o.x - n.x)*(o.x - n.x);
   sumsqs += (o.y - n.y)*(o.y - n.y);
   sumsqs += (o.z - n.z)*(o.z - n.z);
   return sqrt( sumsqs);
}
