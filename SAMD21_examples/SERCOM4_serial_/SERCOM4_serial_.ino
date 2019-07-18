#include <Arduino.h>   // required before wiring_private.h
#include "wiring_private.h" // pinPeripheral() function
 
Uart Serial2 (&sercom4, A1, A2, SERCOM_RX_PAD_0, UART_TX_PAD_0);
void SERCOM4_Handler()
{
  Serial2.IrqHandler();
}
 
void setup() {
  Serial.begin(115200);
 
  Serial2.begin(115200);
  
  // Assign pins A1(TX) & A2(RX) SERCOM functionality
  pinPeripheral(A1, PIO_SERCOM_ALT);
  pinPeripheral(A2, PIO_SERCOM_ALT);
}
 
uint8_t i=0;
void loop() {
  Serial.print(i);
  Serial2.write(i++);
  if (Serial2.available()) {
    Serial.print(" -> 0x"); Serial.print(Serial2.read(), HEX);
  }
  Serial.println();
  
  delay(250);
 
}
