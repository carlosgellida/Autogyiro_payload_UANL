#include <Arduino.h>
#include "printf.h"
#include <MPU_personalized_functions.h>
#include <Radio_personalized_functions.h>

String Arr2Str(float Arr1[5]) {
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
} 

void loop() {

  
    // This device is a TX node

    unsigned long start_timer = micros();                    // start the timer
    payload_str = Arr2Str(payload) ; 
    bool report = radio.write(&payload, sizeof(payload));      // transmit & save the report
    unsigned long end_timer = micros();                      // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));          // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);                 // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(Arr2Str(payload));                                   
    } else {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }

    delay(1000);  // slow transmissions down by 1 second

} 