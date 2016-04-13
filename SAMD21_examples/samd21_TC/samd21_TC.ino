/*
Using Timer counters in SAMD21

This sketch was based on this example:
https://forum.arduino.cc/index.php?topic=332275.msg2538251#msg2538251

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


/*
 * You can use TC3, TC5. I can't TC7(library limitarion for this TC)
 * Examples for configuration
 * configure_TCn_clock(TC3, TC3_IRQn, GCM_TCC2_TC3);
 * configure_TCn_clock(TC5, TC5_IRQn, GCM_TC4_TC5);
 * configure_TCn_clock(TC7, TC7_IRQn, GCM_TC6_TC7);
 * 
 * Don't forget change your real_time configuration, for example
 * double current_second = real_time(REG_TC3_COUNT16_COUNT);
 * 
*/

//If you use Sparkfun samd21 change Serial for SerialUSB
#define serial Serial

uint8_t led = 13;
volatile int state = LOW;
volatile uint32_t tick = 0x0 ;   // Counter for superticks (overflow interrupts)

void setup() {
  serial.begin(115200);
  pinMode(led, OUTPUT);
  configure_TCn_clock(TC5, TC5_IRQn, GCM_TC4_TC5);
}//end setup

void loop() {
  double current_second = real_time(REG_TC5_COUNT16_COUNT);
  if( int(current_second) >= 10){
    reset_start_TCn(TC5);            //Reset so never gets above 10s
    stop_TCn(TC5);
    blink();                         //Excecute this function
    start_TCn(TC5);
  }//end if
}//end loop

void blink() {
  serial.println("Blink");
  state = !state;
  digitalWrite(led, state);
}//end blink


//=====================================================

void TC3_Handler(){                  // Interrupt on overflow
  TcCount16* TC = (TcCount16*) TC3;  // get timer struct
  tick++;                            // Increment the supertick register
  TC->INTFLAG.bit.OVF = 1;           // writing a one clears the ovf flag
}//end TC3_Handler

void TC5_Handler(){                  // Interrupt on overflow
  TcCount16* TC = (TcCount16*) TC5;  // get timer struct
  tick++;                            // Increment the supertick register
  TC->INTFLAG.bit.OVF = 1;           // writing a one clears the ovf flag
}//end TC5_Handler

double real_time(uint16_t REG_TCn_COUNT16_COUNT){//Get Time in seconds
  double  realTime =  (tick * 1.2288E-2) + (REG_TCn_COUNT16_COUNT * 1.875E-7) ;
  return realTime;
}//end real_time

void configure_TCn_clock(Tc* TCn, IRQn_Type TCn_IRQn, uint16_t GCM_TCx_TCn){//  Setup the Generic clock register
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCx_TCn));
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
  TcCount16* TC = (TcCount16*) TCn;               // get timer struct
  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;              // Disable TC
  while (TC->STATUS.bit.SYNCBUSY == 1);           // wait for sync
  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;         // Set Timer counter Mode to 16 bits
  while (TC->STATUS.bit.SYNCBUSY == 1);           // wait for sync
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_NFRQ;         // Set TC as normal Normal Frq
  while (TC->STATUS.bit.SYNCBUSY == 1);           // wait for sync
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV8;       // Set perscaler
  while (TC->STATUS.bit.SYNCBUSY == 1);           // wait for sync
  TC->INTENSET.reg = 0;                           // disable all interrupts
  TC->INTENSET.bit.OVF = 1;                       // enable overfollow interrup
  NVIC_EnableIRQ(TCn_IRQn);                       // Enable InterruptVector
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;               // Enable TC
  while (TC->STATUS.bit.SYNCBUSY == 1);           // wait for sync
}//end configure_TCn_clock

void reset_start_TCn(Tc* TCn){
   TcCount16* TC = (TcCount16*) TCn; // get timer struct
   tick = 0x00;                      // SAMD21 superclicks
   TC->CTRLBSET.reg |= TC_CTRLBCLR_CMD_RETRIGGER;   // restart
}//end reset_start_TCn

void stop_TCn(Tc* TCn){
  TcCount16* TC = (TcCount16*) TCn; // get timer struct
  TC->CTRLBSET.reg |= TC_CTRLBSET_CMD_STOP;   // Stop counter
}//end stop_TCn

void start_TCn(Tc* TCn){
  TcCount16* TC = (TcCount16*) TCn; // get timer struct
  TC->CTRLBSET.reg |= TC_CTRLBSET_CMD_RETRIGGER;   //  Start
}//end start_TCn
