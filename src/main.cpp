/* Nota realizar un proceso de calibración al iniciar el programa, para esto
   se restará la diferencia entre el cuaternio actual y el cuaternio vertical
   y se sumará esta diferencia a cada cuaternio futuro. */

#include <Arduino.h>
#include <Wire.h>
#include <ESP32Servo.h>

#include <SPI.h>  
#include "RF24.h"
#include <MPU_personalized_functions.h>
#include <Quaternion&VectorAlgebra.h>



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
    //Serial.print("Desired quaternion (qw qx qy qz): "); 
    //printQuat(desiredQuat); 
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
  myRadio.startListening();

  delay(500) ; 
  Wire.begin(); // Iniciar I2C
  
  inicializeMPU9250(mpu) ; // Inicializar el procesador de movimiento*/
  
  prevMillis = millis();
  send(); 
}

void loop() {

  /* currentMillis = millis();

  if ((currentMillis - prevMillis) > 5 && (currentMillis - prevMillis < 6)) {
    mpu.update();
  } 

  if (currentMillis - prevMillis >= txIntervalMillis) {
    prevMillis = millis();
    send();
  }

  bool recieved = getData();

  showData(recieved) ; */

  Matrix<3, 3> a1 = {1, 1, 1, 2, 4, -1, 1, -1, 2}; 

  int res; 
  Matrix<3, 3> a1_inv = a1.Inverse(&res) ; 
  Serial << "res: " << res << "\n"; 

  Serial << "a1: " << a1 << "\n" ;
  Serial << "a1_inv: " << a1_inv << "\n";

  Serial << "a1 * a1_inv: " << a1 * a1_inv << "\n"; 

  Serial << "a upright corner: " << a1.Submatrix(Slice<0, 2>(), Slice<1, 3>()) << "\n" ;

  Matrix<3, 2> vecI = {1, 1, 1, 2, 2, 2}; 
  /*Matrix<3, 1> vecII = {2, 3, 4}; 
  Matrix<1, 3, Array<1, 3, float>>  vecIII = {3, 3, 3} ; 

  Serial << "crossProduct: " << crossProduct(vecI, vecII) << "\n"; 

  Serial << "skewSim: " << skewSim33(vecII) << "\n"; */

  Matrix<2,2> A;
  A.Fill(1);

  Matrix<2,1> B;
  B.Fill(2);

  Matrix<2,1> C = A * B;

  Serial << "Matrix times vector" << C << "\n" ;

  while(true){
    //keep doing nothing
  } 

}