#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>


class IMU{

  public:
     IMU(int pin);
     int reset_IMU;
     void imuReset();
     void initIMU();
     void runIMU();

  private:
    Adafruit_BNO055 bno1 = Adafruit_BNO055(55, 0x28);
    Adafruit_BNO055 bno2 = Adafruit_BNO055(55, 0x29);
    
    
  
};
