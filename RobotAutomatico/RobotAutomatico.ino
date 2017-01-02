/**
 * Basic DC output control.L298D
 * 
 * Just output Pin 8 digital to high for 500 ticks and then
 * put it back to low.
 * 
 * If you connect a LED or something into pin 8, you should see
 * it blinking.
 * 
 */

const int MOTION_PIN = 2; // Pin connected to motion detector
const int LED_PIN = 13; // LED pin - active-high



int const QUIET = 0;
int const STILL = 1;
int const MOVE_FORWARD = 2;
int const MOVE_BACKWARDS = 3;
int const MOVE_RIGHT = 4;
int const MOVE_LEFT = 5;


int IN4 = 6;
int IN3 = 5;
int IN2 = 4;
int IN1 = 3;

int motorstate = QUIET;

void setup() {
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 4
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 5
  pinMode (IN2, OUTPUT);    // Input4 conectada al pin 3
  pinMode (IN1, OUTPUT);    // Input3 conectada al pin 2

  randomSeed(analogRead(0));

  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

unsigned long previousMillis = 0;        // will store last time LED was updated

long interval = 500;           // interval at which to blink (milliseconds)


bool checkproximity()
{
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == LOW) // If the sensor's output goes low, motion is detected
  {
    digitalWrite(LED_PIN, HIGH);
    //Serial.println("Motion detected!");
    return true;
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    //Serial.println(".");
    return false;
  }  
}



void loop() {

  switch (motorstate)
  {
    case MOVE_FORWARD:
      // Move Backward.
      //Serial.println("Moving!");
      digitalWrite (IN4, HIGH);
      digitalWrite (IN3, LOW);
      digitalWrite (IN2, HIGH);
      digitalWrite (IN1, LOW);
      break;
    case MOVE_BACKWARDS:
      // Go ahead and move forward
      //Serial.println("Moving!");
      digitalWrite (IN4, LOW);
      digitalWrite (IN3, HIGH);
      digitalWrite (IN2, LOW);
      digitalWrite (IN1, HIGH);
      break;
    case MOVE_RIGHT:
      // Move the right caterpiller
      //Serial.println("Moving!");
      digitalWrite (IN4, HIGH); // LOW, HIGH for back
      digitalWrite (IN3, LOW);
      digitalWrite (IN2, LOW);
      digitalWrite (IN1, HIGH);
      break;
    case MOVE_LEFT:
      // Move the left Caterpiller
      //Serial.println("Moving!");
      digitalWrite (IN4, LOW);
      digitalWrite (IN3, HIGH);
      digitalWrite (IN2, HIGH);  // LOW, HIGH for back
      digitalWrite (IN1, LOW);
      break;
    default:
      // Stop All Motors
      digitalWrite (IN4, LOW);
      digitalWrite (IN3, LOW);
      digitalWrite (IN2, LOW);
      digitalWrite (IN1, LOW);
      break;
  }
  unsigned long currentMillis = millis();

  if (checkproximity())
  {
    motorstate = MOVE_FORWARD;
    previousMillis = currentMillis;
  }
  
  if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      if (checkproximity()) {
          motorstate = MOVE_FORWARD;
          previousMillis = currentMillis;  
      } else {
        int whattodo = random(5);
  
        if (whattodo == 0) motorstate = MOVE_LEFT;
        else motorstate = QUIET;
      }

  }

  delay(10);

}
