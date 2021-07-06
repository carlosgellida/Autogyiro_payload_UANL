#include <Arduino.h>
#include "printf.h"
#include <MPU_personalized_functions.h>
#include <Radio_personalized_functions.h>

String Arr2Str(int Arr1[5]) {
  // This function transform an 5 elements array 
  // into a 5 terms string
  String Arr_transform ; 
  Arr_transform = String(Arr1[0]); 
  for(int i = 1; i < 5; i++){
    Arr_transform += " " ; //Delimitado por espacios
    Arr_transform += String(Arr1[i]) ; 
  }
  Arr_transform += " " ; // Add an aditional space at the end
  return Arr_transform; 
}


void setup() {

  Serial.begin(115200);
  /* while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }*/

  initializeRadio();

  payload[0] = 0; 
  payload[1] = 1; 
  payload[2] = 2; 
  payload[3] = 3; 
  payload[4] = 4; 
} 

void loop() {

      // This device is a TX node

    payload[0] = float(millis()); 
    bool report = radio.writeFast(&payload, sizeof(payload));      // transmit & save the report

    if (report) {
      Serial.println(F("Transmission successful! "));          // payload was delivered                                
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }

    /* //radio.stopListening();  // put radio in TX mode
    payload_str = Arr2Str(payload) ; 
    bool report = radio.write(&payload, sizeof(payload));      // transmit & save the report


    if (!report) {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    } */
    /*else {
      // Sí la información se envío, ponerse a escuchar
      Serial.println("It begin to listen!");
      radio.startListening();
      while(true){
      uint8_t pipe;
      if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
        uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
        radio.read(&payload, bytes);            // fetch payload from FIFO
        Serial.print(F("Received "));
        Serial.print(bytes);                    // print the size of the payload
        Serial.print(F(" bytes on pipe "));
        Serial.print(pipe);                     // print the pipe number
        Serial.print(F(": "));
        Serial.println(Arr2Str(payload));                // print the payload's value
        break; 
      }
      }
      radio.stopListening();  // put radio in TX mode
      }*/
    delay(10);  // slow transmissions down by 1 second

} 