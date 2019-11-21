#include "Arduino.h"
#include "IMU.h"

IMU::IMU(int pin){
  reset_IMU = pin;
}

void IMU::imuReset(){
  // reset the IMUs to the proper addresses to ensure proper connection
  pinMode(reset_IMU, OUTPUT);
  digitalWrite(reset_IMU, HIGH);
  delay(500);
  digitalWrite(reset_IMU, LOW);
  delay(1000);
  digitalWrite(reset_IMU, HIGH);
}

void IMU::initIMU(){
   Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if (!bno1.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO1 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  if (!bno2.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO2 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  bno1.setExtCrystalUse(true);
  bno2.setExtCrystalUse(true);
  delay(1000);
}

void IMU::runIMU(){
  // IMU

  sensors_event_t event1;
  bno1.getEvent(&event1);


  sensors_event_t event2;
  bno2.getEvent(&event2);

  /* Display the floating point data */
  /*
    Serial.print("oX: ");
    Serial.print(event1.orientation.x, 4);
    Serial.print("\toY: ");
    Serial.print(event1.orientation.y, 4);
    Serial.print("\toZ: ");
    Serial.print(event1.orientation.z, 4);
    Serial.print("\t\toX: ");
    Serial.print(event2.orientation.x, 4);
    Serial.print("\toY: ");
    Serial.print(event2.orientation.y, 4);
    Serial.print("\toZ: ");
    Serial.print(event2.orientation.z, 4);
    Serial.println("");
  */

  event1.orientation.x;
  event1.orientation.y;
  event1.orientation.z;

  event2.orientation.x;
  event2.orientation.y;
  event2.orientation.z;


  imu::Vector<3> ACCEL1 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  imu::Vector<3> ACCEL2 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  /*
    Serial.print("aX: ");
    Serial.print(ACCEL1.x(), 4);
    Serial.print("\taY: ");
    Serial.print(ACCEL1.y(), 4);
    Serial.print("\tZa: ");
    Serial.print(ACCEL1.z(), 4);
    Serial.print("\t\taX: ");
    Serial.print(ACCEL2.x(), 4);
    Serial.print("\taY: ");
    Serial.print(ACCEL2.y(), 4);
    Serial.print("\taZ: ");
    Serial.print(ACCEL2.z(), 4);
  */
  ACCEL1.x();
  ACCEL1.y();
  ACCEL1.z();
  ACCEL2.x();
  ACCEL2.y();
  ACCEL2.z();
}
