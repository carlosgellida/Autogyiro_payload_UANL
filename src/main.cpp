#include <Arduino.h>
#include <Wire.h>

#include <SPI.h>  
#include "RF24.h"
#include <MPU_personalized_functions.h>

RF24 myRadio (14, 12);

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 10;

Quaternion desiredQuat ; 
Quaternion currentQuat ; 

void send(void){
  bool sended = false; 
  float time = float(micros());
  float Quaternion[4] ;

  if (mpu.update()){
    currentQuat = get_Quaternion() ; 
    /*Quaternion[0] = mpu.getQuaternionW() ; 
    Quaternion[1] = mpu.getQuaternionX() ; 
    Quaternion[2] = mpu.getQuaternionY() ; 
    Quaternion[3] = mpu.getQuaternionZ() ; */
    /*Serial.print("currentQuat: "); 
    Serial.print(" "); 
    printQuat(currentQuat); */
  }

  myRadio.stopListening(); 
  sended = myRadio.write(&currentQuat, sizeof(currentQuat));

  /*if(sended){
    Serial.print("time for transmision: "); 
    Serial.println(float(micros()) - time); 
    //Serial.print("currentQuat: "); 
    //printQuat(currentQuat);
  }*/

  myRadio.startListening(); 
}

bool getData(void){
  bool recieved = false; 
  if ( myRadio.available()) {
    //Serial.println("Mesaje available!!!"); 
    myRadio.read( &desiredQuat, sizeof(desiredQuat) );
    recieved = true; 
  } 
  return recieved; 
}

void showData(bool recieved){
  // If the info from joystick was recieved this 
  // functions shows the desired quaternion
  if(recieved){
    Serial.print("Desired quaternion (qw qx qy qz): "); 
    printQuat(desiredQuat); 
  }
}

void setup() {
  //Serial.begin(115200);

  delay(500);

  bool radioWorks = myRadio.begin(); 
  /*if(radioWorks){
    Serial.println("Radio is working"); 
  }else{
    Serial.println("Radio doesn't Work");
    while(true){
      //keep device in a loop to avoid problems
    }
  } */
  myRadio.setPALevel(RF24_PA_MAX); 
  myRadio.setDataRate(RF24_2MBPS); 
  myRadio.openWritingPipe(slaveAddress);
  myRadio.openReadingPipe(1, masterAddress);
  //myRadio.setChannel(115); 	
  myRadio.setRetries(3,5);
  myRadio.startListening();

  delay(500) ; 
  Wire.begin(); // Iniciar I2C
  
  inicializeMPU9250(mpu) ; // Inicializar el procesador de movimiento
  
  prevMillis = millis();
  send(); 
}

void loop() {

  currentMillis = millis();

  if ((currentMillis - prevMillis > 5) && (currentMillis - prevMillis < 6)) {
    prevMillis = millis();
    mpu.update();
  }

  if (currentMillis - prevMillis >= txIntervalMillis) {
    prevMillis = millis();
    send();
  }

  bool recieved = getData();

  showData(recieved) ;

}