/*
 * ASshort_longDigital v5b.1
 
 Pro Micro for V using DIGITAL sensor
 
 v3 uses buzzer requiring modulation
 
 short click ON DOWN sends iPad Select
    v5 closes pinControl port as well
 long click ON UP sends relay signal
 
 Added timer so that beyond longclickTime then
 sends relay signal
 
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

#include <AACsensors.h>
#include <AACswitch.h>

const int testing = 0; // set this to 0 for final version
const int CapTouch = 0; // set this to 1 if for capacitive touch sensor

//digital param

const int pinIn=A0;  // digital sensor 7 or A0 ++++
const int pinControl = A3;   // select = 5 or A3
Switch control( pinControl, 100, 1 );

Switch buzzer( 6, 40, 1 ); // USED buzzer = 4 or A2 +++++
Switch relay( A4, 200, 0);  // relay = 2 or A4
SerialBuzzer serialBuzzer( A2, testing ); // pin 3, debug  // NOT USED

/*
const int pinIn=7;  // digital sensor 7
const int pinControl = 5;   // select = 5
Switch control( pinControl, 100, 1 );

Switch buzzer( 4, 40, 1 ); // USED buzzer = 4
Switch relay( 2, 200, 0);  // relay = 2
SerialBuzzer serialBuzzer( 3, testing ); // pin, debug  // NOT USED
*/

//analog params
const int nLevels = 3; // number of levels
const int triggerA = 0; // reading at this value or less will trigger; must be less than nLevels
const long ignoreTime = 400;  //400
const long longclickTime = 1200;  //1200
const long discardTime = 2500;  //2500

long lastTriggeredA = 0;
int lastReadA=1;
long now;
int count=0;

//DigitalSensor sD(pinIn, 30, 1, 0);  // Pin, Refractory period, Normal High, Debug
//AnalogSensor sA(A0, 200, nLevels-1, 0);

const int ylh[14] = { 
   3,1,3,3,0,1,3,1,1,0,1,1,1,1 }; //-.-- .-.. ....
const int morseTime=50;

void Signature() {
   //if(testing) return;

   for( int j=0; j<15; j++ ) {
      switch(ylh[j]) {
      case 0:
         delay(morseTime*3);
         break;
      default:
         for (int i=0; i<morseTime*ylh[j]; i++ ) {
            serialBuzzer.Go(1,1);
         }
         delay(morseTime);
         break;
      }
   }
}

void tryTriggerLongA(int byWhat) {
   now = millis();
   if( ( now > (lastTriggeredA + longclickTime) ) &&
      ( now < (lastTriggeredA + discardTime) ) )
   {
      if( testing == 0 )
      {
         relay.On(500);
         buzzer.On(500);
         serialBuzzer.Go (1,byWhat);
      } 
      else {
         Serial.println(count);
         if( byWhat==1 ) {
            Serial.print("\n*A--------A*\n");  // Triggered
         } 
         else {
            Serial.print("\n*A       A*\n");  // Triggered
         }
         if( testing == 1) {  //non silent testing
            relay.On(300);
            buzzer.On(300);
         }
      }
   }
}

void setup() {    
   pinMode(pinIn, INPUT);

   buzzer.On(200);
   serialBuzzer.Go(3,1);
   //Signature();
}

void loop() {
   int xA;

   delay(10);
   relay.Check();
   buzzer.Check();
   control.Check();

   count++;
   xA = digitalRead(pinIn);
   if( CapTouch ) { xA = 1- digitalRead(pinIn); } // invert for capacitive touch sensor
  

   //xA=sD.Read();  // digital read using library routine 
   //************** DISABLE ANALOG  xA=sA.Level(nLevels);  //analog read

   if( testing>0 && xA!=lastReadA) {
      //Serial.print(count);
      Serial.print("a=");
      Serial.print(xA);
      Serial.print(" ");
      Serial.flush();
   }


   if( xA!=lastReadA ) {
      // state changed
      if( xA==triggerA ) {  // DOWN TICK
         now = millis();  
         if( now < (lastTriggeredA + ignoreTime) ) {  //ignore
            return;
         }
         else  {   // trigger short
            if( testing == 0 ) {
               /* iPad click */
               Keyboard.press(KEY_UP_ARROW); 
               Keyboard.press(KEY_DOWN_ARROW); 
               Keyboard.releaseAll();
               
               control.On(100);
               
               /* app click 
               Keyboard.press('1');
               Keyboard.releaseAll();
               */
               
               buzzer.On(10);
            }  
            else {
               Serial.print(count);
               Serial.print(" *A*\n");  // Triggered
               relay.On(50);
               buzzer.On(10);
            }       

            serialBuzzer.Go(1,1);
            lastTriggeredA = millis();
         }
      }
      else { // UP TICK
         tryTriggerLongA(1);
      }
   } 
   else {     //xA==last level
      if( xA==triggerA ) {
         tryTriggerLongA(2);
      }
   }
   lastReadA = xA; 
}
















