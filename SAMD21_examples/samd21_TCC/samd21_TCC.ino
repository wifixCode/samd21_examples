/*
Using Timer counters for control in SAMD21

Ricardo Mena C
ricardo@crcibernetica.com
http://crcibernetica.com
 License
 **********************************************************************************
 This program is free software; you can redistribute it 
 and/or modify it under the terms of the GNU General    
 Public License as published by the Free Software       
 Foundation; either version 3 of the License, or        
 (at your option) any later version.                    
                                                        
 This program is distributed in the hope that it will   
 be useful, but WITHOUT ANY WARRANTY; without even the  
 implied warranty of MERCHANTABILITY or FITNESS FOR A   
 PARTICULAR PURPOSE. See the GNU General Public        
 License for more details.                              
                                                        
 You should have received a copy of the GNU General    
 Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
                                                        
 Licence can be viewed at                               
 http://www.gnu.org/licenses/gpl-3.0.txt
 Please maintain this license information along with authorship
 and copyright notices in any redistribution of this code
 **********************************************************************************
*/

//If you use Sparkfun samd21 change Serial for SerialUSB
#define serial Serial

uint8_t led = 13;
volatile int state = LOW;
volatile uint32_t tick = 0x0;   // Counter for superticks (overflow interrupts)
volatile uint32_t tic = 0x0;   // Counter for superticks (overflow interrupts)

void setup() {
  serial.begin(115200);
  while (!Serial);//Wait for serial Monitor(no needed)
  pinMode(led, OUTPUT);
  configure_TCCn_clock();
}//end setup

void loop(){
  
}//end loop

void blink() {
  serial.println("Blink");
  state = !state;
  digitalWrite(led, state);
}//end blink

//=====================================================

void configure_TCCn_clock(){
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );         // Wait for synchronization
  Tcc* TC = (Tcc*) TCC0;                            // get timer struct
  TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;               // Disable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync
  TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV8;        // Set perscaler
  TC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;            // Set wave form configuration 
  while (TC->SYNCBUSY.bit.WAVE == 1);               // wait for sync
  TC->PER.reg = 0xFFFF;                             // Set counter Top using the PER register  
  while (TC->SYNCBUSY.bit.PER == 1);                // wait for sync
  TC->INTENSET.reg = 0;                             // disable all interrupts
  TC->INTENSET.bit.OVF = 1;                         // enable overfollow
  NVIC_EnableIRQ(TCC0_IRQn);                        // Enable InterruptVector
  TC->CTRLA.reg |= TCC_CTRLA_ENABLE ;               // Enable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync 
}//end configure_TCn_clock

void TCC0_Handler(){        // Interrupt on overflow
  Tcc* TC = (Tcc*) TCC0;    // get timer struct
  tick++;                   
  TC->INTFLAG.bit.OVF = 1;  // writing a one clears the flag ovf flag
  if(tic >= 91.6*10){       // //Reset so never gets above 10 s
    blink();
    tick = 0x0;
  }//end if
}//end TCC0_Handler
