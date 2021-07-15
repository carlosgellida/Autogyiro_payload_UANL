#include <Arduino.h>
#include <SPI.h>  
#include "RF24.h"

RF24 myRadio (14, 12);
byte addresses[][6] = {"0"};
//uint8_t addresses[][6] = {"1Node", "2Node"};

struct package {
  int id = 1;
  float temperature = 18.3;
  char  text[300] = "Text to be transmit";
  int recibido = 0; 
};

float time0, time1, time2, time3; 

float quaternion[5] = {1, 2, 3, 6, 10}; 
float quaternion2[5] = {1, 1.0, 2.0, 3.0, 6.1}; 

typedef struct package Package;
Package dataRecieve;
Package dataTransmit;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  myRadio.begin();  
  //myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  //myRadio.setDataRate( RF24_250KBPS );
  myRadio.setDataRate(RF24_1MBPS); 
  //myRadio.setAutoAck(true) ; 
  
}

void loop() {
  time0 = float(millis());  

  bool sended = false;   

  // Sólo cuando se inicia el radio antes de cambiar a modo escritura de datos
  // el programa funciona bien, aunque aún se desconoce porqué ocurre esto, sin
  // embargo esto demora 11ms, lo que obliga a la comunicación a realizarse de una 
  // manera más lenta. Esto es un problema sin resolver. 
  myRadio.begin(); 

  myRadio.stopListening();
  myRadio.openWritingPipe(addresses[0]);

  sended = myRadio.writeFast(&quaternion2, sizeof(quaternion2));
    
  if(sended){   
    Serial.println("Transmit: ");
  }
  
  delay(1) ;

  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
  bool recieved = false; 
  time1 = float(millis()); 
  quaternion[0] = 0; 
  
  while(!recieved){
  if ( myRadio.available()) {
    while (myRadio.available()){
      myRadio.read( &quaternion, sizeof(quaternion) );
      if(quaternion[0] == 1){
        break; 
      }
    }
    Serial.println("Recieve: ");
    recieved = true; 
   }else{
     time2 = float(millis()); 
     if(time2 - time1 > 30){
       Serial.println("--------- Data Lost -------");
       break; 
     }
   }

  }

  /*while(recieved == false){
    if ( myRadio.available()) {
      myRadio.read( &quaternion, sizeof(quaternion) );
      Serial.println("Recieve: ");
      recieved = true; 
    } 
  }*/
  myRadio.closeReadingPipe(1);
  delay(1);

  time3 = float(millis());
  while(time3 - time0 < 30){
    //Espera a que la el tiempo transcurrido desde el inicio del programa 
    //sea mayor que 10ms
    time3 = float(millis());
  } 
  Serial.println(time3); 
}