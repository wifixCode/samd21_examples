/*
Using sysTicHook interrupts in SAMD21

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

volatile uint32_t tick = 0;
uint8_t led = 13;
volatile int state = LOW;

/*
 * In this example I use systickHook, instead of SysTick_Handler
 * because arduino library limitations
*/
extern "C" {
    // sysTicks will be triggered once per 1 ms
    int sysTickHook(void) {
        tick++;
        if(tick >= 1000){//Excecute every 1s
          tick = 0;//Restart count
          blink();
        }//end if
        return 0;// continue with the original handler 
        // NOTE = return 1; // disable original handler 
    }//end sysTickHook
}//end extern C

void setup() {
  SysTick_Config(SystemCoreClock / 1000);//Configure systick
  serial.begin(115200);
  while (!serial);//Wait for serial Monitor(no needed)
  pinMode(led, OUTPUT);
}//end setup

void loop() {
  // put your main code here, to run repeatedly:  
}//end loop

void blink() {
  serial.println("Blink");
  state = !state;
  digitalWrite(led, state);
}//end blink


