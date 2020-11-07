#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "states.h"


state getState(Stream &refSer)
{
  if (refSer.available()) {
    char receivedChar = refSer.read();
    Serial.print(receivedChar);
    if ( '0' <= receivedChar && receivedChar <= '0' + state::CRAWL)
    {
      return (state)((int)receivedChar - (int)'0');
    }
  }
  return state::STATE_NONE;
}




state checkState(state receivedState, state currentState, unsigned long *TSI, int motor_pin, int brake_pin)
{
  if (receivedState == state::FAULT && currentState != state::FAULT) {
    currentState = state::FAULT;
    *TSI = millis();
    Serial.print("TSI: ");
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
        if (receivedState == state::STA)
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("STA?");
          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::STA;
        }

        if (receivedState == state::LAUNCH)
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
        
        if (receivedState == state::COAST)
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, HIGH);
          Serial.print("Coast?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::COAST;
        }


        if (receivedState == state::BRAKE)
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
        if (receivedState == state::BRAKE)
        {
          *TSI = millis();

          digitalWrite(motor_pin, LOW);
          digitalWrite(brake_pin, LOW);
          Serial.print("Brake?");

          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::BRAKE;
        }
        if (receivedState == state::CRAWL)
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
        if (receivedState == state::CRAWL)
        {
          *TSI = millis();

          digitalWrite(motor_pin, HIGH);
          digitalWrite(brake_pin, HIGH);
          Serial.print("Crawl?");


          Serial.print("TSI: ");
          Serial.println(*TSI);
          return state::CRAWL;
        }
        if (receivedState == state::STA)
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
        if (receivedState == state::BRAKE)
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
    default:

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
