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

/*
 * This example has special considerations
 * 1. min and max need to be undefined
 * 2. The default compiler meabe does not work
 * you need to change arm-none-eabi-gcc 
 * for arm-none-eabi-g++
 * The compiler spec. are in platform.txt 
 * Note = if you use Feather M0 you need to search in 
 * .arduino15/packages/adafruit/hardware/samd/1.0.8/
 * if you use Sparkfun SAMD21 you need to search in
 * .arduino15/packages/SparkFun/hardware/samd/1.1.0/ 
 * lines 35, 37 and 39 for both cases
*/

#undef min
#undef max

#include <queue>
#include <vector>
#include <cstdlib>

using namespace std;

//If you use Sparkfun samd21 change Serial for SerialUSB
#define serial Serial

void queue_f();//Do not forget this prototipe function

void setup() {
  serial.begin(115200);
  // put your setup code here, to run once:
  
}

void loop() {
  serial.println("QUEUE");
  delay(2000);
  queue_f();
}

void queue_f(){
  queue<int> queue_v;

  // Insert Elements
  queue_v.push(2);
  queue_v.push(302);
  queue_v.push(54);
  queue_v.push(30);

  // Show Elements
  while (!queue_v.empty()){
    serial.println(queue_v.front());
    queue_v.pop();
  }//end while
  serial.println();
}//end queue_f

