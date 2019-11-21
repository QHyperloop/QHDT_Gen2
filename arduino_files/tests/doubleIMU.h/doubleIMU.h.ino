#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


void setupIMU(void)
{
  // Double BNO testing
  int reset_IMU = 2;
  
  Adafruit_BNO055 bno1 = Adafruit_BNO055(55, 0x28);
  //Adafruit_BNO055 bno2 = Adafruit_BNO055(55, 0x29);

  float offset,velocity = 0, distance = 0;
  long last, current;
  
  // reset the IMUs to the proper addresses to ensure proper connection
  pinMode(reset_IMU, OUTPUT);
  digitalWrite(reset_IMU, HIGH);
  delay(500);
  digitalWrite(reset_IMU, LOW);
  delay(1000);
  digitalWrite(reset_IMU, HIGH);

  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if (!bno1.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO1 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  
  bno1.setExtCrystalUse(true);
  delay(1000);
  imu::Vector<3> ACCEL1 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  
  offset = ACCEL1.x();
  last = millis();
}

void doubleIMU(void)
{
  /* Get a new sensor event */

  sensors_event_t event1;
  bno1.getEvent(&event1);


  imu::Vector<3> ACCEL1 = bno1.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  Serial.print("A: ");
  Serial.print(ACCEL1.x()-offset, 4);
  Serial.print("\tV: ");
  Serial.print(velocity, 4);
  Serial.print("\tX: ");
  Serial.print(distance, 4);
  current = millis();
  Serial.print('\t');
  Serial.println(current - last);
  
  velocity += 0.001 * (current - last) * (ACCEL1.x()-offset);
  distance += 0.001 * (current - last) * velocity;
  last = current;

}
