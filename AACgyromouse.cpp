/* AACgyromouse.cpp : 2014.11.28 
 
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
#include "AACswitch.h"
#include "AACsensors.h"
#include "AACgyromouse.h"


   // A0==18 on Pro Micro: once stopped, mouse inactive for 300ms
   // A1==19 on Pro Micro: 50ms pause once clicked
GyroMouse::GyroMouse() : An1( A1, 50, 1, 0), An0( A0, 300, 1, 0),
   buzzer ( 4, 40, 1 ) // BUZZER on pin 4
{
   mouseactivated=0;
   _sens = 13;
}

void GyroMouse::setSensitivity( int s ) {
   _sens  =constrain(  map( abs(s), 0, 100, 0,31), 1, 30);  
}
  

void GyroMouse::mouseReset() {
   _inverted1 = 0; // 0 when using light sensor; 1 when using cap touch
   _inverted0 = 1; // 0 when using light sensor; 1 when using cap touch
   //range = 16;
   //threshold = range/5;
   //center = range/2;
   minima[0]=-20; 
   minima[1]=-20;
   maxima[0]=20;    
   maxima[1]=20;

   minima[0]=1000; 
   minima[1]=1000;
   maxima[0]=-1000;    
   maxima[1]=-1000;
   msize[0]=msize[1]=1;

}

int GyroMouse::readAxis(int axisNumber, VectorInt16 gyro) {
   int distance = 0;    // distance from center of the output range
   int reading, current;

   if( axisNumber==0 )
      reading = -gyro.x; //VARIABLES L3G20 -gyro.g.x;
   else
      reading = gyro.z*7/5; // VARIABLES L3G20 -gyro.g.z;

   if( reading < minima[axisNumber] ) minima[axisNumber]=reading;
   if( reading > maxima[axisNumber] ) maxima[axisNumber]=reading;


   if( abs(reading)<20) { // ignore L3G4 readings below 2000
      current = 0;  
   }
   else {
      if( reading >0 ) {
         current= 1;
      } 
      else {
         current= -1;
      } 
   } 
   // following numbers work for L3G2
   // msize[axisNumber] =constrain(  map( abs(reading), 0, 6000, 0,6), 1, 5);  
   // L3G2 msize[axisNumber] =constrain(  map( abs(readi   // L 6000, 0,7), 1, 6);
   //
   // for MPU6050
   // good: msize[axisNumber] =constrain(  map( abs(reading), 0, 80, 0,11), 1, 10);   
   msize[axisNumber] =constrain(  map( abs(reading), 0, 80, 0,_sens + 3 ), 1, _sens + 2);   
   return current;
}
   
void GyroMouse::DoMouseWork(VectorInt16 gyro) {
   int mouseX,mouseY;
   mouseReading[0] = readAxis(0, gyro); 
   mouseReading[1] = readAxis(1, gyro);

   if( !mouseactivated ) return;

   mouseX=0;
   mouseY=0;
   for( int i=0; i<2; i++) {
      if( mouseReading[i]==0 ) { // current = 0
         if( prev[i]!=0) {  // move as previous
            if(i==0) mouseX=prev[i]*msize[i];
            else mouseY=prev[i]*msize[i];
            mouseReading[i] = prev[i];
         } 
         else {  // previous not moved as well: do nothing

         }
      }
      else {  // current = moved
         if(mouseReading[i]==prev[i]) {  // same direction
            if(i==0) mouseX=mouseReading[i]*msize[i];
            else mouseY=mouseReading[i]*msize[i];
         } 
         else { 
            if( prev[i]==0 ){   // prev not moved
               if(i==0) mouseX=mouseReading[i]*msize[i];
               else mouseY=mouseReading[i]*msize[i];
               prev[i]=mouseReading[i];
            } 
            else {  // reverse direction

               buzzer.Times(1);

               unsigned long d=millis();
               while( millis() - d < 300) { // VARIABLES *********** 300 for Nexus 7
                 delay(30);
                 buzzer.loop();
               }

               prev[0]=prev[1]=0;
               msize[0]=msize[1]=3;   //                msize[0]=msize[1]=3; = 2 
               mouseX=0;
               mouseY=0;                     
            }    
         }
      }
   }

   Mouse.move(mouseX,mouseY,0);
}

void GyroMouse::normalHigh0( int in ) {
  if( in > 1 ) in=1; //either 0 or 1
  _inverted0 = in;
}
void GyroMouse::normalHigh1( int in ) {
  if( in > 1 ) in=1; //either 0 or 1
  _inverted1 = in;
}
void GyroMouse::loop(VectorInt16 gyro) {
   int x,y,z;

   buzzer.loop();

   if( An1.Level(2) == _inverted1  && Mouse.isPressed()) {
      Mouse.release();
      //Serial.println("Released.");
      buzzer.On(30);
      //serialBuzzer.Go(1,1);
   }
   if( An1.Level(2) != _inverted1  && !Mouse.isPressed()) {
      Mouse.press();
      //Serial.print("Pressed. ");
      buzzer.On(20);
      //serialBuzzer.Go(1,2);
   }

   // ylh
   if ( An0.Level(2) == _inverted0 ) { // 0 == for cap touch; An0.Level(2) == 1 if using light sensor
      mouseactivated = 1;
      DoMouseWork( gyro );
      delayMicroseconds(5);   // 4000 for Nexus 7
   } 
   else {
      mouseactivated = 0;
      prev[0]=prev[1]=0;
   }

}
