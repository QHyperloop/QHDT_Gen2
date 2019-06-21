#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <time.h>
#include <math.h>
#include <Wire.h>
#include "enums.c"
#include "consts.c"



// Initialize the states
struct StateDetails
{
  const bool motor;
  const bool brake;
  const bool led_indicator;
};

// represent how the arduino should allow motor, brake and led operation for all states (except fault)
StateDetails States[state::STATES] = {{true, false, true},{false, true, false},{false, true, true},{true, false, true},{true, false, true},{false, true, true},{true, false, true}};

// Network Data
char telemetry[PACKET_SIZE];

// pin mapping
int statePin = 3;
int motor = 9;
int brake = 10;
int network_led = 11;
int safe_led = 12;
int power_led = 13;
int RTL_switch = 22;
int RTL_switch_reset = 24;


int networkState;
bool initialBrake;

state currentState, prevState;
unsigned long TSI;


void establishInternal() {
  bool connection = false;
  while (!connection)
  {
    // If the chip receives a command
    if (Serial3.available() > 0)
    {
      char data = Serial3.read();
      if (data == 'A')
      {
        Serial3.write('B');
        connection = true;
      }
    }
    digitalWrite(network_led, HIGH);
    delay(100);
    digitalWrite(network_led, LOW);
    delay(900);
  }
  Serial.println("Internal connection made!");
}

void printState(state s)
{
  switch(s)
    {
      case(state::STA):
      Serial.print("STA");
      break; 
      case(state::RTL):
      Serial.print("RTL");
      break; 
      case(state::LAUNCH):
      Serial.print("Launch");
      break; 
      case(state::COAST):
      Serial.print("Coast");
      break; 
      case(state::BRAKE):
      Serial.print("Brake");
      break; 
      case(state::CRAWL):
      Serial.print("Crawl");
      break; 
      default:
      Serial.print("Fault");
      break; 
    }
}

void establishExternal() {
  bool connection = false;
  while (!connection)
  {
    // If the chip receives a command read data and check for predetermined character
    if (Serial3.available() > 0)
    {
      char data = Serial3.read();
      if (data == 'C')
      {
        connection = true;
      }
    }
    // Blink lights while waiting
    digitalWrite(network_led, HIGH);
    delay(100);
    digitalWrite(network_led, LOW);
    delay(100);
    digitalWrite(network_led, HIGH);
    delay(100);
    digitalWrite(network_led, LOW);
    delay(700);
  }
  Serial.println("External connection made!");
}

void setup(void)
{
  // set pin modes for indicators
  pinMode(power_led, OUTPUT);
  pinMode(network_led, OUTPUT);
  pinMode(safe_led, OUTPUT);
  pinMode(brake, OUTPUT);
  pinMode(motor, OUTPUT);

  pinMode(RTL_switch, INPUT);
  pinMode(RTL_switch_reset, OUTPUT);


  // initiate default state on start up
  networkState = state::STA;
  currentState = state::STA;
  prevState = state::STATES;

  // Indicate that the pod is on at all times with the power_led
  digitalWrite(power_led, HIGH);
  // Default to the network not connecting and indicate
  digitalWrite(network_led, LOW);
  // pod is set to not safe to approach, which ensures proper boot up into STA.
  digitalWrite(safe_led, LOW);

  // TODO reset fault switch

  delay(1000);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial3.begin(115200);  // start serial for output


  // boot up internal processes, including sensors and internal connections
  establishInternal();

  establishExternal();
  // Connections are set and the network is now properly working.
  
  digitalWrite(network_led, HIGH);
}






void loop(void)
{
  // CHECK PHASE
  
  // check network data
  //networkState = int(analogRead(statePin)/42);
  
  if (Serial3.available() > 0)
    {
      networkState = Serial3.read() - '0'; // convert the character to a integer
      Serial.print(networkState);
    }
  
  // TODO check sensor data
  // TODO calculations
  // ensure pod is within thresholds, otherwise cause a fault and report error

  // STATE PHASE
  if (networkState == state::FAULT) currentState = state::FAULT; // fault pod upon network request

  // record start time change
  if (currentState != prevState)
  {
    // make if statement one shot
    prevState = currentState;
    // reset time state initialized 
    TSI = millis();
    // set related outputs
    digitalWrite(brake, !States[currentState].brake);
    digitalWrite(motor, States[currentState].motor);
    digitalWrite(safe_led, States[currentState].led_indicator);

    // one shot per case
    switch(currentState)
    {
      case(state::STA):
      Serial.println("resetting switch");
      digitalWrite(RTL_switch_reset, HIGH);
      delay(100);
      digitalWrite(RTL_switch_reset, LOW);
      break; 
      case(state::RTL):
      initialBrake = true;
      break; 
      case(state::LAUNCH):
      break; 
      case(state::COAST):
      break; 
      case(state::BRAKE):
      break; 
      case(state::CRAWL):
      break; 
      default:
      break;
    }
    
    // Record time that state is entered (debugging only)
    printState(currentState);
    Serial.print(" entered at time ");
    Serial.println(TSI);
  }
  
  // check state (switch case)
  switch(currentState)
    {
      case(state::STA):
      {
        if (digitalRead(RTL_switch) && networkState == state::RTL)
        {
          currentState = state::RTL;
        }
        if (!digitalRead(RTL_switch))
        {
          // reset RTL switch while the switch is off to ensure the pod does not go immediately to RTL when turned on
          networkState = state::STA;
        }
        break; 
      }
      case(state::RTL):
      {
        if (networkState == state::LAUNCH)
        {
          currentState = state::LAUNCH;
        }
        break; 
      }
      case(state::LAUNCH):
      {
        // 10 second launch phase
        if (millis() - TSI < 9800) 
        {
          digitalWrite(safe_led, (millis()/500)%2); //Blink indicating launch is about to occur
        }
        else if (millis() - TSI < 10000)
        {
          digitalWrite(safe_led, HIGH); //set high for actual launch
        }
        // run motors
        else
        {
          ; // TODO launch the pod based on TSI and velocity function
        }
        if (networkState == state::COAST)currentState = state::COAST;
        if (networkState == state::BRAKE)currentState = state::BRAKE;
      }
      break; 
      case(state::COAST):
      if (millis() - TSI > 10000) // 10 second launch phase
      {
        currentState = state::BRAKE;
      }
      break; 
      case(state::BRAKE):
      if (!initialBrake)
      { 
        //TODO brake and stay halted
        currentState = state::STA;
      }
      if (millis() - TSI > 10000 && initialBrake) // 10 second launch phase
      {
        currentState = state::CRAWL;
        initialBrake = false;
      }
      break; 
      case(state::CRAWL):
      if (millis() - TSI > 10000) // 10 second launch phase
      {
        currentState = state::BRAKE;
      }
      break; 
      
      default:
      // fault occurs, use while loop to send data back
      while (true);
      break;
    }
    
  // NETWORK PHASE
  
  telemetry[0] = TEAM_ID;
  telemetry[1] = (byte)currentState;
  // todo add remaining data
  for (int i = 2; i < PACKET_SIZE; i = i + 4)
  {
    //dataToBEBytes(telemetry[i], data?);
    // temp set all data to 0
    for (int n = 0; n < 4; n++)
    {
      telemetry[i+n] = 0;
    }
  }

  // send packet to NA via serial comm
  for (int i = 0; i < PACKET_SIZE; i++)
  {
    Serial3.write((char)telemetry[i]);
  }
  Serial3.write((byte)0x4); // send end of transmission byte
  Serial3.flush();
}
