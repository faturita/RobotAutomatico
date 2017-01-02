/**
 * Basic DC output control.L298D
 * 
 * Just output Pin 8 digital to high for 500 ticks and then
 * put it back to low.
 * 
 * If you connect a LED or something into pin 8, you should see
 * it blinking.
 * 
 * The PIR motion sensor has a three-pin JST connector terminating it. Connect
the wire colors like this:
 * Black: D2 - signal output (pulled up internally)
 * White: GND
 * Red: 5V
 */

const int MOTION_PIN = 11; // Pin connected to motion detector
const int LED_PIN = 13; // LED pin - active-high



int const QUIET = 0;
int const STILL = 1;
int const MOVE_FORWARD = 2;
int const MOVE_BACKWARDS = 5;
int const MOVE_RIGHT = 4;
int const MOVE_LEFT = 3;


int IN4 = 7;
int IN3 = 6;
int IN2 = 5;
int IN1 = 4;

#define trigPin 2
#define echoPin 3

int motorstate = QUIET;

void setup() {
  //Serial.begin (9600);
    
  pinMode(trigPin, OUTPUT_FAST);
  pinMode(echoPin, INPUT_FAST);
  
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 4
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 5
  pinMode (IN2, OUTPUT);    // Input4 conectada al pin 3
  pinMode (IN1, OUTPUT);    // Input3 conectada al pin 2

  randomSeed(analogRead(0));

  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long movementMillis = 0;
  
long interval = 900;           // interval at which to blink (milliseconds)


#define SCANNING 1
#define MOVING    2

int status = SCANNING;

int pathcounter=0;

bool checkproximity(unsigned long mvmMillis)
{
  bool proximityBool = false;
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == LOW) // If the sensor's output goes low, motion is detected
  {
    digitalWrite(LED_PIN, HIGH);
    //Serial.println("Motion detected!");
    proximityBool = true;
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    //Serial.println(".");
    proximityBool = false;
  }  

  if ( ((millis() - mvmMillis)>=4000) && proximityBool)
  {
    return true;
  } else {
    return false;
  }
  
}

bool checkobstacle()
{
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
  //Serial.println(distance);
  if (distance == 0) {
      // ikely some error
      return false;
  } else if (distance < 9) {
      motorstate = MOVE_BACKWARDS;
      previousMillis = millis();  
      movementMillis = millis();
      interval = 200;
      return true;
  } else if (distance < 12) {  
      //Serial.print("OBSTACLE !");Serial.println(distance);
      return true;
  } else {
    return false;
  }
}



void loop() {
  bool obstacle = checkobstacle();

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


  if (checkproximity(movementMillis) && motorstate == QUIET && !obstacle)
  {
    motorstate = MOVE_FORWARD;
    previousMillis = currentMillis;
    movementMillis = currentMillis;
    interval = 900;
    pathcounter=0;
  }
  
  if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;

      if (status == SCANNING) {
        pathcounter++;
        if (checkproximity(movementMillis) && motorstate == QUIET && !obstacle) {
            motorstate = MOVE_FORWARD;
            previousMillis = currentMillis;  
            movementMillis = currentMillis;
            interval = 900;
            pathcounter=0;
        } else {
          motorstate = QUIET;
          interval = 900;   
        }
        if (pathcounter>20)
        {
          pathcounter = 0;
          motorstate = QUIET;
          interval = 900;
          status = MOVING;
        }
      } else {
        pathcounter++;
        if (pathcounter>6)
        {
          pathcounter = 0;
          motorstate = QUIET;
          interval = 900;
          status = SCANNING;
        }
        int action = random(3);
        interval = 200*random(4);
        motorstate = action + 2;

      }

  }

  delay(10);

}
