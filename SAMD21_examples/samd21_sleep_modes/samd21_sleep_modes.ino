/*
This sketch just show you how to enter in some sleep mode
to reduce the power consumption in SAMD21. I use wake up interrupts(WFI)
but you can change this with wake up event (WFE)

The diference betwen them can see here 
http://stackoverflow.com/questions/27188807/arm-sleep-mode-entry-and-exit-differences-wfe-wfi

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

void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(13, LOW);
}//end setup

void loop(){
  //This is the way I found to reduce power consumtion M0
  //If you need DAC or ADC just reenable them
  DAC_disable();
  ADC_disable();  
  
  /*
   * Be carefull uncommenting this line!!!It reduce de power consumtion
   *Systick handle millis(), micros(), delay() and other timing functions
    *This like disable Systick, if you don't need clocks or timing
    *do not touch it
  */
  
  //SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

  /*
   * For more information about sleep mode see
   * page 123 : 15.6.2.8 Sleep Mode Controller 
   * in Atmel | SMART SAM D21 [DATASHEET]
  */
  //Use one of this functions, not both  
  //normal_sleep_mode(0);//Set your sleep mode
  deep_sleep_WFI();//Lowest Consumption posible
 
}

void DAC_disable(void){
  DAC->CTRLA.bit.ENABLE = 0x00;//Disable digital to analog converter
  while (DAC->STATUS.bit.SYNCBUSY == 1);//Wait for sync
}//end DAC_disable

void ADC_disable(void){
  ADC->CTRLA.bit.ENABLE = 0x00;//Disable analog to digital converter
  while (ADC->STATUS.bit.SYNCBUSY == 1);  
}//end ADC_disable

void DAC_enable(void){
  DAC->CTRLA.bit.ENABLE = 0x01;//Disable digital to analog converter
  while (DAC->STATUS.bit.SYNCBUSY == 1);//Wait for sync
}//end DAC_disable

void ADC_enable(void){
  ADC->CTRLA.bit.ENABLE = 0x01;//Disable analog to digital converter
  while (ADC->STATUS.bit.SYNCBUSY == 1);  
}//end ADC_disable

void sleep_modes(uint8_t mode){
/*
 * This function have 3 diferent modes
 * 0, 1 and 2. Greater the number, 
 * the lowest consumption
*/
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  PM->SLEEP.reg = mode;
  __DSB();//Data Sincronitation Barrier
  __WFI();
}//end sleep_modes

void deep_sleep_WFI(void){
  SCB->SCR |=  SCB_SCR_SLEEPDEEP_Msk;
  __DSB();//Data Sincronitation Barrier
  __WFI();
}//end deep_sleep_WFI

