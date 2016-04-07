/*
Using interrupts in SAMD21

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

uint8_t int0 = 9;
uint8_t int1 = 10;
uint8_t int2 = 11;

uint8_t led = 13;
volatile int state = LOW;
volatile uint32_t i =0;

void setup() {
  serial.begin(115200);
  while (!serial);//Open serial Monitor to start the program
  pinMode(led, OUTPUT);
  pinMode(int0, INPUT_PULLUP);
  pinMode(int1, INPUT_PULLUP);
  pinMode(int2, INPUT_PULLUP);
  attachInterrupt(int0, blink, CHANGE);
  attachInterrupt(int1, blonk, CHANGE);
  attachInterrupt(int2, count, CHANGE);
  delay(2000);
  serial.println("Starting");
}

void loop() {
  digitalWrite(led, state);
}//end loop

/*
 * All hardware interrupts in samd21 have the same priority, 
 * because of that, if one interrupt function is running all 
 * new interrupts need to wait until the first ends its execution, 
 * like a "queue"-> FIFO
*/

void blink() {
  serial.println("Blink");
  state = !state;
}//end blink

void blonk() {//blink 2.0  XD
  serial.println("Blonk");
  state = !state;
}//end blonk

void count() {
  for(i = 0; i < 148039; i++){
    serial.println(i);
  }//end for
}//end count
