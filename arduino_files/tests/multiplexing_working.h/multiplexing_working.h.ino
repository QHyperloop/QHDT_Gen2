/*  Pressure transducer testing code
    Queen's Hyperloop Design Team
    A0 pin to pressure sensor, using 5V and ground
    Adjust the SLOPE and BIAS to map the data
*/

void mutliSetup (int bit1, int bit2, int bit4, int temp){

  pinMode(bit1, OUTPUT);
  pinMode(bit2, OUTPUT);
  pinMode(bit4, OUTPUT);
  pinMode(temp, INPUT);
  
}

void multilexer() {
  // put your main code here, to run repeatedly:
  for (int x = 0; x < 2; x++)
  {
    digitalWrite(bit4, x);
    for (int y = 0; y < 2; y++)
    {
      digitalWrite(bit2, y);
      for (int z = 0; z < 2; z++)
      {
        digitalWrite(bit1, z);
        //Serial.print(digitalRead(x));
        //Serial.print(digitalRead(y));
        //Serial.print(digitalRead(z));
        Serial.print('\t');
        Serial.print(((analogRead(temp)*5.0/1024)-0.4)/0.0195);
        //Serial.print('\t');
        
      }
    }
  }
  Serial.println();
}
