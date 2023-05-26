#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "DueCANLayer.h"
#include "Adafruit_FRAM_I2C.h"
#include "max6675.h"
#include "constants.h"
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>


enum PodState {
  INIT,
  FAULT,
  SAFE_TO_APPROACH,
  READY_TO_LAUNCH,
  LAUNCH,
  COAST,
  BRAKE,
  CRAWL,
  GROUNDWARNING,
  STDBY
};

PodState the_STATE = FAULT;

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

// CAN VARIABLES and CONSTANTS
byte fullstop[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte crawl[] = { 0x00, 0x00, 0x09, 0xc4, 0x00, 0x00, 0x00, 0x00 };
byte mediumSpeed[] = { 0x00, 0x00, 0x13, 0x88, 0x00, 0x00, 0x00, 0x00 };
byte highSpeed[] = { 0x00, 0x00, 0x48, 0x5c, 0x00, 0x00, 0x00, 0x00 };
byte tempdata[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte current[] = { 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00 };
byte canData[60];

struct TelemetryPacket {
  uint16_t minCellVoltage;
  uint16_t maxCellVoltage;
  uint16_t lowestCellTemp;
  uint16_t highestCellTemp;
};

TelemetryPacket packet;

int speedOfMotor;
int isoState;
int isoState2 = 1;

//LED Pins
int REDLED = 41;
int YELLOWLED = 40;
int GREENLED = 39;

//General Purpose Stuff
int buttonPin = 37;

//Braking pins
int brakeSolenoidPin = 38;
int tankPressurePin = A7;
int Caliper1PressurePin = A6;
int Caliper2PressurePin = A1;
int tankPressure = 0;
int Caliper1Pressure = 0;
int Caliper2Pressure = 0;

//Timers
int ledState = LOW;
unsigned long previousMillis = 0;
unsigned long previousMillis1 = 0;
unsigned long previousMillisRED = 0;
unsigned long previousMillisYellow = 0;
unsigned long previousMilli = 0;
unsigned long previousMillisGreen = 0;


//Buffers and IDs for sending and recieving stuff
char tempbuff[10] = "0";
byte dataList[60] = {};
int BMSBuffID = 5;
int ESCBuffID = 10;
int thermocoupleBuffID = 15;
int IMUBuffID = 20;

//For SD card logging
File dataFile;

//GPS
TinyGPSPlus gps;
float speedKph = 0;
double START_LAT = 0.0;
double START_LON = 0.0;
double lat = 0.0;
double lon = 0.0;
double distance = 0;
bool firstTime = true;

//Thermocouple Pins
//Shared SPI
int thermoDO = 74;
int thermoCLK = 72;
//Individual CS pins
int thermoCS1 = 5;
int thermoCS2 = 6;
int thermoCS3 = 7;

//Thermocouple declarations
MAX6675 thermocouple1(thermoCLK, thermoCS1, thermoDO);
MAX6675 thermocouple2(thermoCLK, thermoCS2, thermoDO);
MAX6675 thermocouple3(thermoCLK, thermoCS3, thermoDO);

/**************************************************************************/
//CAN BMS Structs
struct data356 {
  int16_t voltage;
  int16_t current;
  int16_t temp;
};

struct data356 ID356(CAN_FRAME frame) {
  //V, A, temp
  data356 data;
  data.voltage = frame.data.s0;
  data.current = frame.data.s1;
  data.temp = frame.data.s2;
  return data;
}

struct data373 {
  uint16_t minCellVoltage;
  uint16_t maxCellVoltage;
  uint16_t lowestCellTemp;
  uint16_t highestCellTemp;
};

struct data373 ID373(CAN_FRAME frame) {
  //  min cell V, max cell V, lowest cell temp, high cell temp
  data373 data;
  data.minCellVoltage = frame.data.s0;
  data.maxCellVoltage = frame.data.s1;
  data.lowestCellTemp = frame.data.s2;
  data.highestCellTemp = frame.data.s3;
  return data;
}


// void speedSelect(int speed) {
//   switch (speed) {
//     case 0:
//       Serial.println("off");
//       digitalWrite(stopLED, HIGH);
//       digitalWrite(crawlLED, LOW);
//       digitalWrite(speedLED, LOW);
//       memcpy(tempdata, fullstop, 8);
//       break;
//     case 1:
//       Serial.println("slow");
//       digitalWrite(stopLED, LOW);
//       digitalWrite(crawlLED, HIGH);
//       digitalWrite(speedLED, LOW);
//       memcpy(tempdata, crawl, 8);
//       break;
//     case 2:
//       Serial.println("medium");
//       memcpy(tempdata, mediumSpeed, 8);
//       digitalWrite(stopLED, LOW);
//       digitalWrite(crawlLED, LOW);
//       digitalWrite(speedLED, HIGH);
//       break;
//     case 3:
//       Serial.println("high");
//       memcpy(tempdata, highSpeed, 8);
//       digitalWrite(stopLED, LOW);
//       digitalWrite(crawlLED, LOW);
//       digitalWrite(speedLED, HIGH);
//       break;
//   }
// }

byte*
createDataBuff(int buffID, byte* buffer) {
  int arrayLength = sizeof(buffer) / sizeof(buffer[0]);
  byte myArray[arrayLength];
  for (int i = 0; i < arrayLength; i++) {
    myArray[i + 1] = buffer[i];
  }
  myArray[0] = buffID;
  return myArray;
}


int readIsolationState(CAN_FRAME frameToSend) {
  Can0.sendFrame(frameToSend);
  CAN_FRAME response;
  if (Can0.read(response)) {
    if (response.id == 0xA100100) {
      byte statusBits = response.data.bytes[1];
      byte isolationState = statusBits & 0x03;
      if (isolationState == 0b10) {
        //Re-call the function and keep checking the fault, if it gets worse, then do the error stuff
        return 7;  //Special value that will set yellow led to be solid, possible warning
      } else if (isolationState == 0b11) {
        //Set the state LED to RED, there is a fault, turn the motors off right away and apply brakes
        Serial.println("not fine");
        return 0;  //FAULT STATE, stay away from pod, ground fault detected
      } else if (isolationState == 0) {
        return 8;  //Nothing out of the ordinary, proceed as normal
      } else {
        return -1;
      }
    }
  }
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
  double earthRadius = 6371000;  // in meters

  double dLat = (lat2 - lat1) * M_PI / 180.0;
  double dLon = (lon2 - lon1) * M_PI / 180.0;

  double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) * sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double distance = earthRadius * c;

  return distance;
}


bool checkSerial() {}


/**************************************************************************/
void setup(void) {
  Serial.begin(115200);
  Serial2.begin(9600);  // GPS Serial Port
  Serial3.begin(9600);

  while (!Serial) delay(10);  // wait for serial port to open!

  //Init SD card with CS pin at 4
  if (!SD.begin(31)) {
    Serial.println("initialization failed!");
  }
  dataFile = SD.open("test.txt", FILE_WRITE | O_RDWR | O_TRUNC);
  if (!dataFile) {
    Serial.println("error opening test.txt");
  } else {
    dataFile.println("test");
  }

  pinMode(GREENLED, OUTPUT);
  pinMode(YELLOWLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  pinMode(tankPressurePin, INPUT);
  pinMode(Caliper1PressurePin, INPUT);
  pinMode(Caliper2PressurePin, INPUT);

  pinMode(brakeSolenoidPin, OUTPUT);

  digitalWrite(REDLED, HIGH);
  digitalWrite(YELLOWLED, HIGH);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(brakeSolenoidPin, HIGH);

  if (canInit(0, CAN_BPS_500K) == CAN_OK)
    Serial.print("CAN0 Initialized Successfully.\n\r");
  else
    Serial.print("CAN0 Initialization Failed.\n\r");

  delay(1000);


  CAN_FRAME IMD_Frame;
  IMD_Frame.id = 0xA100101;
  IMD_Frame.data.bytes[0] = 0xE0;  // Multiplexor for isolation state request
  IMD_Frame.length = 1;
  IMD_Frame.extended = 1;

  //First check if there are any ground faults before proceeding with anything. Done on powerup.
  PodState currentState = STDBY;
  for (int i = 0; i <= 2; i++) {
    isoState = readIsolationState(IMD_Frame);  //Checks for ground faults
  }
  if (isoState == 8) {
    the_STATE = STDBY;
    Serial.println("Isolation State: Good");
    if (dataFile) {
      dataFile.print("System Time: ");
      dataFile.print(millis());
      dataFile.println(" Isolation State: Good");
    }
  } else if (isoState == 7) {
    the_STATE = GROUNDWARNING;
    Serial.println("Isolation State: Warning");
    if (dataFile) {
      dataFile.print("System Time: ");
      dataFile.print(millis());
      dataFile.println(" Isolation State: Warning");
    }
  } else {
    the_STATE = FAULT;
    Serial.println("Isolation State: DANGER");
    if (dataFile) {
      dataFile.print("System Time: ");
      dataFile.print(millis());
      dataFile.println(" Isolation State: DANGER");
    }
  }
  delay(500);


  bool SerialReady = false;
  //This is where we initialize, wait until the pod is mounted and everthing is ready
  while (!SerialReady) {
    if (Serial3.available()) {
      int dataIn = Serial3.read() - '0';  // Subtract the ASCII value of '0' to get the actual value
      Serial.print("Received state: ");
      Serial.println(dataIn);
      //If data is recieved from network arduino indicating initialization state, continue with operation of pod
      if (dataIn == INIT) {
        SerialReady = true;
        Serial.println("POD Initialized");
        if (dataFile) {
          dataFile.print("System Time: ");
          dataFile.print(millis());
          dataFile.println(" POD Initialized");
        }
      }
    }
  }

  //Add a button somewhere that triggers the safe to approach state, once the pod is mounted
  bool PodMounted = false;
  bool firstBrakes = true;
  while (!PodMounted) {
    if (firstBrakes) {
      digitalWrite(brakeSolenoidPin, LOW);
      firstBrakes = false;
    }
    int buttonValue = digitalRead(buttonPin);
    unsigned long currentMillisRED = millis();
    if (currentMillisRED - previousMillisRED >= 750) {
      previousMillisRED = currentMillisRED;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(REDLED, ledState);
    }
    if (buttonValue == LOW) {
      // If button pushed, pod is safe to approach
      the_STATE = SAFE_TO_APPROACH;
      Serial.println("POD Safe To Approach");
      if (dataFile) {
        dataFile.print("System Time: ");
        dataFile.print(millis());
        dataFile.println(" POD Safe To Approach");
      }
      //Delay just in case;
      delay(5000);
      PodMounted = true;
    }
  }

  //Wait until we recieve input from remote GUI to indicate pod is ready to launch
  bool RTL = false;
  firstBrakes = true;
  digitalWrite(REDLED, HIGH);

  while (!RTL) {
    if (firstBrakes) {
      digitalWrite(brakeSolenoidPin, HIGH);  // dump pressure
      delay(5000);                           // wait for pressure to empty, brakes are now on
      //next, pressurize the tank, and wait for the next state to release the brakes, close the solenoid and wait
      digitalWrite(brakeSolenoidPin, LOW);
      firstBrakes = false;
      //Whenever the valves are opened, brakes will be off
    }
    String message = "";
    message = "0." + "0.";
    unsigned long currentMillisYellow = millis();
    if (currentMillisYellow - previousMillisYellow >= 750) {
      previousMillisYellow = currentMillisYellow;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(YELLOWLED, ledState);
    }
    Can0.watchFor();
    CAN_FRAME myFrame;
    if (Can0.rx_avail()) {
      //Serial.println(Can0.available())
      Can0.read(myFrame);
      if (myFrame.id == 0x356) {
        data356 data = ID356(myFrame);
        message = message + "." + String((data.voltage) / 100.0) + ".";
        // Serial3.print(voltage);

      } else if (myFrame.id == 0x373) {
        data373 data = ID373(myFrame);

        packet.minCellVoltage = data.minCellVoltage;
        packet.maxCellVoltage = data.maxCellVoltage;
        packet.lowestCellTemp = data.lowestCellTemp;
        packet.highestCellTemp = data.highestCellTemp;

        //message = message + String(data.minCellVoltage) + "." + String(data.maxCellVoltage) + "." + String(data.lowestCellTemp) + "." + String(data.highestCellTemp);

        // Serial3.print(voltTemps);
      }
    }
    //Solenoid is normally open, brakes are normally closed
    //So brakes will be on withott pressure and will retract once they are given pressure
    //To relase the brakes, close the solenoid (give it power)
    //To apply the brakes, take power away from the solenoid

    float averageSystemTemp = (thermocouple1.readCelsius() + thermocouple2.readCelsius() + thermocouple3.readCelsius()) / 3.0;
    message = "." + message + "." + String(averageSystemTemp) + ".";
    //Read pressures from each thing

    tankPressure = analogRead(tankPressurePin);
    tankPressure = map(tankPressure, 181, 296, 0, 80);

    Caliper1Pressure = analogRead(Caliper1PressurePin);
    Caliper1Pressure = map(Caliper1Pressure, 181, 296, 0, 80);

    Caliper2Pressure = analogRead(Caliper2PressurePin);
    Caliper2Pressure = map(Caliper2Pressure, 181, 296, 0, 80);

    message = message + String(tankPressure) + "." + String(Caliper1Pressure) + "." + String(Caliper2Pressure) + ".0";

    Serial3.print(message);

    if (Serial3.available()) {
      // Read the incoming data and convert it to an integer
      int theState = Serial3.read() - '0';  // Subtract the ASCII value of '0' to get the actual value
      Serial.print("Received state: ");
      Serial.println(theState);
      if (theState = READY_TO_LAUNCH) {
        RTL = true;
        Serial.println("POD Ready To Launch");
        if (dataFile) {
          dataFile.print("System Time: ");
          dataFile.print(millis());
          dataFile.println(" POD  Ready To Launch");
        }
      }
    }
  }


  bool groundFault = false;
  bool lossOfSerial = false;
  //Do not proceed with procedures unless we are sure there is no ground faults and that we have serial connnection
  //while (!groundFault && !lossOfSerial) {
  bool launch = false;
  digitalWrite(YELLOWLED, HIGH);
  while (!launch) {
    String message = "";
    //Perform the ready to lauch procedures (brakes still on, blinking yellow lights, apply current brake)
    //Always keep checking for ground faults and for loss of comms on radios
    unsigned long currentMillis1 = millis();
    if (currentMillis1 - previousMillis1 >= 500) {
      previousMillis1 = currentMillis1;
      isoState2 = readIsolationState(IMD_Frame);  //Checks for ground faults
      if (isoState2 == 0) {
        Serial.println("fault state");
        if (dataFile) {
          dataFile.print("System Time: ");
          dataFile.print(millis());
          dataFile.println(" Ground Warning!");
        }
      }
    }
    if (Serial3.available()) {
      int theState = Serial3.read() - '0';
      if (theState <= -1) {
        Serial.println("Issue with Serial");
        digitalWrite(brakeSolenoidPin, HIGH);  //dump pressure and apply the brakes
        if (dataFile) {
          dataFile.print("System Time: ");
          dataFile.print(millis());
          dataFile.println(" Issues With Serial");
        }
      }
    }

    while (Serial2.available() > 0) {
      gps.encode(Serial2.read());
    }
    if (gps.location.isValid() && gps.speed.isValid()) {
      speedKph = gps.speed.kmph();
      dataFile.print("Speed: ");
      dataFile.println(speedKph);

      message = String(speedKph) + ".";

      // Serial3.print(speed);
    }
    //Watch for distance and time since launch, and watch for speed
    //If speed is too high, the coast
    //Coast until reached max distance or  time since launch experied

    unsigned long currentMillisGreen = millis();
    if (currentMillisGreen - previousMillisGreen >= 750) {
      previousMillisGreen = currentMillisGreen;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(GREENLED, ledState);
    }
    Can0.watchFor();
    CAN_FRAME myFrame;
    if (Can0.rx_avail()) {
      //Serial.println(Can0.available())
      Can0.read(myFrame);
      if (myFrame.id == 0x356) {
        data356 data = ID356(myFrame);
        message = message + "." + String((data.voltage) / 100.0) + ".";
        // Serial3.print(voltage);

      } else if (myFrame.id == 0x373) {
        data373 data = ID373(myFrame);

        packet.minCellVoltage = data.minCellVoltage;
        packet.maxCellVoltage = data.maxCellVoltage;
        packet.lowestCellTemp = data.lowestCellTemp;
        packet.highestCellTemp = data.highestCellTemp;

        String voltTemps = " Min/Max Voltages and Temps: " + String(data.minCellVoltage) + "." + String(data.maxCellVoltage) + "." + String(data.lowestCellTemp) + "." + String(data.highestCellTemp);

        //Serial3.print(voltTemps);
        //Serial.println(voltTemps);
      }
    }

    float averageSystemTemp = (thermocouple1.readCelsius() + thermocouple2.readCelsius() + thermocouple3.readCelsius()) / 3.0;
    message = "." + message + "." + String(averageSystemTemp) + ".";
    //Read pressures from each thing

    tankPressure = analogRead(tankPressurePin);
    tankPressure = map(tankPressure, 181, 296, 0, 80);

    Caliper1Pressure = analogRead(Caliper1PressurePin);
    Caliper1Pressure = map(Caliper1Pressure, 181, 296, 0, 80);

    Caliper2Pressure = analogRead(Caliper2PressurePin);
    Caliper2Pressure = map(Caliper2Pressure, 181, 296, 0, 80);

    message = message + String(tankPressure) + "." + String(Caliper1Pressure) + "." + String(Caliper2Pressure);

    if (gps.location.isValid()) {
      if (firstTime) {
        START_LAT = gps.location.lat();
        START_LON = gps.location.lng();
        firstTime = false;
      }
      distance = calculateDistance(
        START_LAT, START_LON,
        gps.location.lat(), gps.location.lng());
      Serial.println(distance);
      message = "." + String(distance);
      dataFile.print("Distance: ");
      dataFile.println(distance);
    }

    if (dataFile) {
      dataFile.print("System Time: ");
      dataFile.print(millis());
      dataFile.println(message);
    }

    Serial3.print(message);

    if (Serial3.available()) {
      // Read the incoming data and convert it to an integer
      int theState = Serial3.read() - '0';  // Subtract the ASCII value of '0' to get the actual value
      Serial.print("Received state: ");
      Serial.println(theState);
      if (theState = LAUNCH) {
        //Recieved state is launch, delay 10 seconds, allowing everyone to clear the pod again
        Serial.println("Launch State");
        if (dataFile) {
          dataFile.print("System Time: ");
          dataFile.print(millis());
          dataFile.println(" POD Launching");
        }
        delay(10000);
        launch = true;
      }
    }
  }

  //We are now in the launch phase
  //Brakes off, motors on full beans
  unsigned long timeSinceLaunch = millis();  //if this exceeds certain time, apply brakes
  bool inLaunch = false;
  bool coast = false;
  bool brake = false;
  Serial.println("ABOUT TO LAUNCH ");
  while (!inLaunch) {
    String message = "";
    isoState = readIsolationState(IMD_Frame);  //Checks for ground faults
    if (isoState == 0)
      //faultSTATE();
      dataFile.println("Isolation State: DANGER");
    digitalWrite(brakeSolenoidPin, HIGH);  //dump pressure and apply the brakes
    if (Serial3.available()) {
      int theState = Serial3.read() - '0';
      if (theState = -1) {
        //faultSTATE();
        dataFile.println("No Connection: FAULT");
        digitalWrite(brakeSolenoidPin, HIGH);  //dump pressure and apply the brakes
      }
    }
    digitalWrite(YELLOWLED, LOW);
    digitalWrite(GREENLED, HIGH);

    while (Serial2.available() > 0) {
      gps.encode(Serial2.read());
    }
    if (gps.location.isValid() && gps.speed.isValid()) {
      speedKph = gps.speed.kmph();
      dataFile.print("Speed: ");
      dataFile.println(speedKph);

      message = String(speedKph) + ".";
      // Serial3.print(speed);
    }

    Can0.watchFor();
    CAN_FRAME myFrame;
    if (Can0.rx_avail()) {
      //Serial.println(Can0.available())
      Can0.read(myFrame);
      if (myFrame.id == 0x356) {
        data356 data = ID356(myFrame);
        message = message + "." + String((data.voltage) / 100.0) + ".";
        // Serial3.print(voltage);

      } else if (myFrame.id == 0x373) {
        data373 data = ID373(myFrame);

        packet.minCellVoltage = data.minCellVoltage;
        packet.maxCellVoltage = data.maxCellVoltage;
        packet.lowestCellTemp = data.lowestCellTemp;
        packet.highestCellTemp = data.highestCellTemp;

        String voltTemps = " Min/Max Voltages and Temps: " + String(data.minCellVoltage) + "." + String(data.maxCellVoltage) + "." + String(data.lowestCellTemp) + "." + String(data.highestCellTemp);

        //Serial3.print(voltTemps);
        //Serial.println(voltTemps);
      }
    }

    float averageSystemTemp = (thermocouple1.readCelsius() + thermocouple2.readCelsius() + thermocouple3.readCelsius()) / 3.0;
    message = "." + message + "." + String(averageSystemTemp) + ".";
    //Read pressures from each thing

    tankPressure = analogRead(tankPressurePin);
    tankPressure = map(tankPressure, 181, 296, 0, 80);

    Caliper1Pressure = analogRead(Caliper1PressurePin);
    Caliper1Pressure = map(Caliper1Pressure, 181, 296, 0, 80);

    Caliper2Pressure = analogRead(Caliper2PressurePin);
    Caliper2Pressure = map(Caliper2Pressure, 181, 296, 0, 80);

    message = message + String(tankPressure) + "." + String(Caliper1Pressure) + "." + String(Caliper2Pressure);


    //Just one motor for now to test on motor
    canTx(0, 0x00000349, true, highSpeed, 8);

    //Watch for distance and time since launch, and watch for speed
    //If speed is too high, the coast
    //Coast until reached max distance or  time since launch experied
    if (gps.location.isValid()) {
      if (firstTime) {
        START_LAT = gps.location.lat();
        START_LON = gps.location.lng();
        firstTime = false;
      }
      distance = calculateDistance(
        START_LAT, START_LON,
        gps.location.lat(), gps.location.lng());
      Serial.println(distance);
      message = "." + String(distance);
      dataFile.print("Distance: ");
      dataFile.println(distance);
    }

    if (dataFile) {
      dataFile.print("System Time: ");
      dataFile.print(millis());
      dataFile.println(message);
    }

    Serial3.print(message);

    if (speedKph >= LAUNCH_SPEED_THRESH) {
      inLaunch = true;
      the_STATE = COAST;
      canTx(0, 0x00000349, true, crawl, 8);
    } else if (distance >= BRAKE_DIST_THRESH || timeSinceLaunch - millis() >= LAUNCH_TIME_THRESH) {
      inLaunch = true;
      the_STATE = BRAKE;
      digitalWrite(brakeSolenoidPin, HIGH);  //dump pressure and apply the brakes
      canTx(0, 0x00000349, true, fullstop, 8);
    }
  }

  dataFile.close();
}
void loop() {
}


//Brakes are normally closed, so they will be applied if there is no pressure to the system
//Once pressure is applied, the brakes will be deactivted
//So to start the pod, once the pod is on the track, keep the solenoid off, or open, to keep the pressure on the brakes,
//once the pod is in the launch state, we need to close the solenoid (turn it on) to give pressure to the brakes, which would release them
//Once the pod is in braking mode, open the solenoid (turn it off), which would apply the brakes

//   bool nextState = false;
//   //Now in the coasting state, only can go into the brake state once the distance exceeded or the time since launch exceeds the coast thresh
//   while (!nextState) {
//     isoState = readIsolationState(IMD_Frame);  //Checks for ground faults
//     if (isoState == 0)
//       faultSTATE();
//     if (Serial3.available()) {
//       int theState = Serial3.read() - '0';
//       if (theState = -1) {
//         faultSTATE();
//       }
//     }
//     while (Serial2.available() > 0)
//       gps.encode(Serial3.read());

//     if (gps.location.isValid() && gps.speed.isValid()) {
//       speedKph = gps.speed.kmph();
//       dataFile.print("Speed: ");
//       dataFile.println(speedKph);
//     }

//     if (gps.location.isValid())
//     {
//         distance = calculateDistance(
//         START_LAT, START_LON,
//         gps.location.lat(), gps.location.lng()
//       );
//       Serial.println(distance);
//       dataFile.print("Distance: ");
//       dataFile.println(distance);
//     }
//     if (the_STATE = COAST) {
//       canTx(0, 0x00000349, true, crawl, 8);
//       if (distance >= BRAKE_DIST_THRESH || timeSinceLaunch - millis() >= LAUNCH_TIME_THRESH) {
//         the_STATE = BRAKE;
//       }
//     } else if (the_STATE = BRAKE) {
//       //Set current brake on motors to however many amps(figure out)
//       canTx(0, 0x00000349, true, current, 8);
//       applyBrakes();
//     }
//   }
// }
// }

