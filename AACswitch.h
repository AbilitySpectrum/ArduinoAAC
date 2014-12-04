/* AACswitch.h
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

#ifndef __AACswitch_h__
#define __AACswitch_h__

#include <Arduino.h>

class Switch
{
  public:
    Switch(int, int, int);  // args: in_pin; scale; debug

    void Check();
    void loop();
    void On( int );
    void Times( int );
    void setPin( int );

  private:
    int _switchPin;    
    int _period;
    int _switched;
    long _lastSwitched;
    int _debug;
 };

class SerialBuzzer
{
  public:
    SerialBuzzer(int, int);  // args: in_pin; debug (if >1 then silent mode)

    void Go( int, int ); // n times modulated, "wavelength" ms

  private:
    int _buzzPin;    
    int _debug;
 };
 
#endif
