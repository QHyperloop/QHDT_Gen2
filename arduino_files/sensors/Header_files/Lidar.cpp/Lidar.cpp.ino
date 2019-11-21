#include "Arduino.h"
#include "Lidar.h"

class Lidar{
  public:
    Lidar::Lidar(int trigger, int monit0r, int laserR, int laserL, unsigned long pulse){
    lidar_trigger = trigger;
    lidar_monitor = monit0r;
    laser_left = laserR;
    laser_right = laserL;
    pulseWidth = pulse;
      
    }

    Lidar::initLidar(){
        pinMode(lidar_trigger, OUTPUT); // Set pin 2 as trigger pin
        digitalWrite(lidar_trigger, LOW); // Set trigger LOW for continuous read

        pinMode(lidar_monitor, INPUT); // Set pin 3 as monitor pin

        pinMode(laser_left, INPUT);
        pinMode(laser_right, INPUT);
    }

    Lidar::runLidar(){
        // LIDAR
        Serial.print("\tIMU");
  
        pulseWidth = pulseIn(lidar_monitor, HIGH); // Count how long the pulse is high in microseconds

         // If we get a reading that isn't zero, let's print it
         if (pulseWidth != 0)
         {
            pulseWidth = pulseWidth / 10; // 10usec = 1 cm of distance
            //Serial.print("\tlidar: ");
            //Serial.print(pulseWidth); // Print the distance
         }
  
         digitalRead(laser_left);
         digitalRead(laser_right);
        /*
          Serial.print("\tLaser left: ");
          Serial.print(digitalRead(laser_left));
          Serial.print("\tLaser right: ");
          Serial.print(digitalRead(laser_right));
       */

        Serial.print("\tlidar/laser");
    }
};
