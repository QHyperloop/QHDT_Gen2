#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "states.h"

//return state enum given a Stream
//refSer is pass by reference but never manipulated
state getState(Stream &refSer)
{
  if (refSer.available()) { //check if there is a state
    char receivedChar = refSer.read(); //turn stream into char
    Serial.print(receivedChar); //print char that represents state
    if ( '0' <= receivedChar && receivedChar <= '0' + state::CRAWL) 
    {
      return (state)((int)receivedChar - (int)'0'); //return state given char
    }
  }
  return state::STATE_NONE; //if there is no state return STATE_NONE
}



//return state given recieved state, the current state, the motor pin, the break pin and tsi
state checkState(state receivedState, state currentState, unsigned long *TSI, int motor_pin, int brake_pin)
{
  //set current state to FAULT if received state is FAULT but not the current state
  if (receivedState == state::FAULT && currentState != state::FAULT) {
    currentState = state::FAULT;
    *TSI = millis();
    Serial.print("TSI: "); //print the tsi
    Serial.println(*TSI);
  }
  // check state (switch case)
  switch (currentState)
  {



    case (state::STA):
      {
        if (digitalRead(RTL_switch) && receivedState == state::RTL)
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("RTL?");


          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::RTL;
        }
        break;
      }

    case (state::RTL):
      {
        if (receivedState == state::STA) //if current state is RTL and received state is STA return STA
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("STA?");
          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::STA;
        }

        if (receivedState == state::LAUNCH) //if current state is RTL and received state is LAUNCH return LAUNCH
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, HIGH);
          Serial.print("LAUNCH?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::LAUNCH;
        }
        break;
      }
    case (state::LAUNCH):
      {

        /*  ADD CODE HERE
        1. Send a PWM signal to the motor controller
        2. Determine whether to send an off signal (<166) or on signal (166) based on a timer (avoid using delay)
          Try millis()!
        
        */
        
        if (receivedState == state::COAST) //if current state is launch and received state is coast, return coast
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, HIGH);
          Serial.print("Coast?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::COAST;
        }


        if (receivedState == state::BRAKE) //if current state is launch and received state is break, return break
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("Brake?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::BRAKE;
        }
        break;
      }
    case (state::COAST):
      /*  ADD CODE HERE
        1. Send a PWM signal to the motor controller
        2. Determine whether to send an off signal (<166) or on signal (166) based on a timer (avoid using delay)
          Try millis()!
        
        */
      
      {
        if (receivedState == state::BRAKE) //if current state is coast and reveived state is brake, return brake
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("Brake?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::BRAKE;
        }
        if (receivedState == state::CRAWL) //if current state is coast and received state is crawl, return crawl
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, LOW);
          //Serial.print("Crawl?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::CRAWL;
        }
        break;
      }
    case (state::BRAKE):
      {
        if (receivedState == state::CRAWL) //if current state is brake and received state is crawl, return crawl
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, HIGH);
          Serial.print("Crawl?");


          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::CRAWL;
        }
        if (receivedState == state::STA) //if current state is brake and received state is sta, return brake
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("STA?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::STA;
        }
        break;
      }
    case (state::CRAWL):
      /*  ADD CODE HERE
        1. Send a PWM signal to the motor controller
        2. Determine whether to send an off signal (<166) or on signal (166) based on a timer (avoid using delay)
          Try millis()!
          
          try to make the time on less in this state to simulate the slower motor motion
        
        */
      {
        if (receivedState == state::BRAKE) // if current state is crawl and received state is brake, return brake
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("brake?");
          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::BRAKE;
        }
        break;
      }
    default: // default: return fault

      Serial.print("fault");
      digitalWrite(motor_pin, LOW);
      digitalWrite(brake_pin, LOW);
      Serial.print("Fault?");


      return state::FAULT;
  }
  return currentState;
}

/*
 * Description: Tester function that brints the state's bit data 
 *              for testing. Prints each bit for a different led
 * Input: Integer of the character representation of the state
 * Output: None
 */
void dispState(int s) {
  //  Serial.print("%d %d %d", s&0x1, s&0x2, s&0x4);
  // Print bit data for testing
  digitalWrite(led_0, s & 0x1);
  digitalWrite(led_1, s & 0x2);
  digitalWrite(led_2, s & 0x4);
}

/*
 * Description: Prints a string representation of state s to Serial
 * Parameters: state type as 's'
 * Return: None
 */
void printState(state s)
{
  // Switch statement that checks what state s is equal to in the
  // enum represenation of it. Prints a match to Serial
  switch (s)
  {
    case (state::STA):
      Serial.print("STA");
      break;
    case (state::RTL):
      Serial.print("RTL");
      break;
    case (state::LAUNCH):
      Serial.print("Launch");
      break;
    case (state::COAST):
      Serial.print("Coast");
      break;
    case (state::BRAKE):
      Serial.print("Brake");
      break;
    case (state::CRAWL):
      Serial.print("Crawl");
      break;
    default:
      Serial.print("Fault");
      break;
  }
}
