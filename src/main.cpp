/* Nota realizar un proceso de calibración al iniciar el programa, para esto
   se restará la diferencia entre el cuaternio actual y el cuaternio vertical
   y se sumará esta diferencia a cada cuaternio futuro. */

#include <Arduino.h>
#include <Wire.h>
#include <ESP32Servo.h>

#include <SPI.h> 
//#include <Quaternion&VectorAlgebra.h> 
#include "RF24.h"
#include <MPU_personalized_functions.h>




RF24 myRadio (14, 12);

Servo blMotor ;  //Motor brusless
Servo servo1 ; 
Servo servo2 ; 
Servo servo3 ; 

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 10;

Quaternion desiredQuat ; 
Quaternion currentQuat ; 

Matrix<4, 1> currentQuatArr ; 

void send(void){
  bool sended = false; 
  
  currentQuat = arr2Struc(getQuatArr(mpu6050)) ; 

  myRadio.stopListening(); 
  sended = myRadio.write(&currentQuat, sizeof(currentQuat));
  myRadio.startListening(); 

  if(sended){
    printQuat(currentQuat); 
  }
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

  blMotor.attach(2) ; //Motor brusless
  servo1.attach(25) ; 
  servo2.attach(26) ;
  servo3.attach(27) ; 
  blMotor.write(40) ; // Mantiene al motor apagado
  servo1.write(90); 
  servo2.write(90); 
  servo3.write(90); 

  Serial.begin(115200);

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
  myRadio.powerUp() ; 
  myRadio.startListening();

  delay(500) ; 
  Wire.begin(); // Iniciar I2C
  
  //inicializeMPU9250(mpu) ; // Inicializar el procesador de movimiento*/
  inicializeMPU6050(mpu6050) ; 

  prevMillis = millis();
  send(); 
}

void loop() {

  currentMillis = millis();

  if ((currentMillis - prevMillis) > 5 && (currentMillis - prevMillis < 6)) {
    mpu6050.update();
  } 

  if (currentMillis - prevMillis >= txIntervalMillis) {
    prevMillis = millis();
    send();
  }

  bool recieved = getData();

  showData(recieved) ; 


}