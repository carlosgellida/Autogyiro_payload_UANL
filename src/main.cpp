#include <Arduino.h>  
#include <ESP32Servo.h>
#include <SPI.h> 
#include "RF24.h"
#include <MPU_personalized_functions.h>

float time_passed ; 

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

Quaternion2 desiredQuat ; 
Quaternion2 currentQuat ; 

Matrix<4, 1> currentQuatArr ; 
Matrix<3, 1> desMoment; // Desired moment of force

void send(void){
  bool sended = false; 
  
  mpu.dmpGetQuaternion(&q, fifoBuffer);

  //currentQuatArr = quatProduct(giro90Y(), arrMod(quat2arr(q)) ) ; // Fix the quaternion

  currentQuatArr = arrMod(quat2arr(q)); 
  currentQuat = arr2Struc(currentQuatArr); //became it an structure

  myRadio.stopListening(); 
  sended = myRadio.write(&currentQuat, sizeof(currentQuat));
  myRadio.startListening(); 

  /*if(sended){
    printQuat(currentQuat); 
  }*/
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

void controlAlg(void){
    //This function apply a PD control to generates the desired 
    // moment of force 
    Matrix<4, 1> AxisAng = quat2AxisAng(quat2arr2(desiredQuat)); 
    Serial << "AxisAng: " << AxisAng << "\n"; 
    Matrix<3, 1> Axis = AxisAng.Submatrix(Slice<1, 4>(), Slice<0, 1>()) ; 
    float Kp = 1 ; // Proportionallity matrix 
    desMoment = escProd(Kp * AxisAng(0, 0), Axis); 
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
  if(radioWorks){
    Serial.println("Radio is working"); 
  }else{
    Serial.println("Radio doesn't Work");
    while(true){
      //keep device in a loop to avoid problems
    }
  } 
  myRadio.setPALevel(RF24_PA_MAX); 
  myRadio.setDataRate(RF24_2MBPS); 
  myRadio.openWritingPipe(slaveAddress);
  myRadio.openReadingPipe(1, masterAddress);
  //myRadio.setChannel(115); 	
  myRadio.setRetries(1,3);
  myRadio.powerUp() ; 
  myRadio.startListening();

  delay(500) ; 

  initializeMPU6050(mpu) ; 
    
  time_passed = float(micros()); 
}



void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    // read a packet from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { 
        //This function will be available each 10ms
        micros() ; 
        send(); 
        printQuat(desiredQuat) ; 
        controlAlg();
        Serial << "Desired moment: (x, y, z) " << desMoment << "\n" ; 
    } 


    getData();
}
