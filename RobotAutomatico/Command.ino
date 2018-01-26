void parseCommand(int &state, int &controlvalue)
{
  if (Serial.available() > 0) 
  {

    char syncbyte = Serial.read();

    switch (syncbyte) 
    {
      case 'I':
        Serial.println("ROBT");
        break;
      case 'D':
        debug=!debug;
        break;
      case 'S':
        startburst();
        break;
      case 'X':
        stopburst();
        break;
      case 'R':
        digitalWrite(BLUE,LOW);
        digitalWrite(RED,HIGH);
        digitalWrite(GREEN,LOW);
        break;
      case 'G':
        digitalWrite(BLUE,LOW);
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,HIGH);
        break;
      case 'B':
        digitalWrite(BLUE,HIGH);
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,LOW);
        break;
      default:
        break;
    }

  }

}

