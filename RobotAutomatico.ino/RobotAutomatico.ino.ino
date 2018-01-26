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

const int TILT_PIN = 8;

int const QUIET = 0;
int const STILL = 1;
int const MOVE_FORWARD = 2;
int const MOVE_BACKWARDS = 3;
int const MOVE_RIGHT = 4;
int const MOVE_LEFT = 5;

int const RED = 9;
int const BLUE = 10;
int const GREEN = 12;


int movesequence[10];
bool sequencing=false;

int IN4 = 7;
int IN3 = 6;
int IN2 = 5;
int IN1 = 4;

#define trigPin 2
#define echoPin 3

int motorstate = QUIET;

int debug = false;

// Compass sensor is too slow.
bool compassenabled = true;

struct sensortype {
  int fps;        // 4
  long distance;  // 4
  int16_t acx;    // 2
  int16_t acy;    // 2
  int16_t acz;    // 2
  int16_t gx;    // 2
  int16_t gy;    // 2
  int16_t gz;    // 2
  int16_t mx;    // 2
  int16_t my;    // 2
  int16_t mz;    // 2
  int16_t counter;  // 2
} sensor; // 28


void setup() {
  Serial.begin (9600);
    
  pinMode(trigPin, OUTPUT_FAST);
  pinMode(echoPin, INPUT_FAST);
  
  pinMode (IN4, OUTPUT);    // Input4 conectada al pin 4
  pinMode (IN3, OUTPUT);    // Input3 conectada al pin 5
  pinMode (IN2, OUTPUT);    // Input4 conectada al pin 3
  pinMode (IN1, OUTPUT);    // Input3 conectada al pin 2

  randomSeed(analogRead(0));

  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  //pinMode(FAN_PIN, OUTPUT);
  //pinMode(TILT_PIN, INPUT);

  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);

  Serial2.begin(9600);

  setupsensors();
}

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long movementMillis = 0;
  
long interval = 900;           // interval at which to blink (milliseconds)


bool checktilted()
{
  int tiltVal = digitalRead(TILT_PIN);
  if (tiltVal == HIGH) {
    Serial.println("Tilted.");
    return true;
  } else {
    Serial.println("Not Tilted.");
    return false;
  }
}

bool checkproximity(unsigned long mvmMillis)
{
  bool proximityBool = false;
  int proximity = digitalRead(MOTION_PIN);
  if (proximity == LOW) // If the sensor's output goes low, motion is detected
  {
    proximityBool = true;
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BLUE, LOW);digitalWrite(RED, LOW);digitalWrite(GREEN, LOW);
    //Serial.println(".");
    proximityBool = false;
  }  

  // The obstacle is detected AFTER movement has stopped.
  if ( (abs(millis() - mvmMillis)>=4000) && proximityBool)
  {
    digitalWrite(BLUE,HIGH);
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,LOW);
    if (false) Serial.println("Motion detected!");
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
  sensor.distance = distance;
  if (distance == 0) {
      // ikely some error
      return false;
  } else if (distance < 8) {
      motorstate = MOVE_BACKWARDS;
      previousMillis = millis();  
      movementMillis = millis();
      interval = 100;
      return true;
  } else if (distance < 12) {  
      //Serial.print("OBSTACLE !");Serial.println(distance);
      return true;
  } else {
    return false;
  }
}


int counter=0;

void loop() {
  int state,controlvalue;
    
  //bool tilted = checktilted();
  bool obstacle = checkobstacle();
  
  checkproximity(movementMillis);

  if (!obstacle) {
    counter++;

    if (counter>100) {
      motorstate = MOVE_FORWARD;
      interval=10;
      counter=0;
    }
  } else {
    counter=0;
  }

  sensor.fps = fps();

  parseCommand(state,controlvalue);
  

  switch (motorstate)
  {
    case MOVE_FORWARD:
      // Move Backward.
      //Serial.println("Moving!");
      digitalWrite (IN4, HIGH);
      digitalWrite (IN3, LOW);
      digitalWrite (IN2, HIGH);
      digitalWrite (IN1, LOW);
            movementMillis = millis();
      break;
    case MOVE_BACKWARDS:
      // Go ahead and move forward
      //Serial.println("Moving!");
      digitalWrite (IN4, LOW);
      digitalWrite (IN3, HIGH);
      digitalWrite (IN2, LOW);
      digitalWrite (IN1, HIGH);
            movementMillis = millis();
      break;
    case MOVE_RIGHT:
      // Move the right caterpiller
      //Serial.println("Moving!");
      digitalWrite (IN4, HIGH); // LOW, HIGH for back
      digitalWrite (IN3, LOW);
      digitalWrite (IN2, LOW);
      digitalWrite (IN1, HIGH);
            movementMillis = millis();
      break;
    case MOVE_LEFT:
      // Move the left Caterpiller
      //Serial.println("Moving!");
      digitalWrite (IN4, LOW);
      digitalWrite (IN3, HIGH);
      digitalWrite (IN2, HIGH);  // LOW, HIGH for back
      digitalWrite (IN1, LOW);
            movementMillis = millis();
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

/**
  if (sensor.acx < 10 )
  {
    motorstate = MOVE_BACKWARDS;
  } else {
    motorstate = MOVE_FORWARD;
  }**/

  if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      
      int rnd = random(100);
  
      if (rnd<=30) {
        motorstate = QUIET;
        interval=10000;
        digitalWrite(BLUE,LOW);
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,LOW);
      } else if (rnd>30 && rnd<=90) {
        motorstate = MOVE_FORWARD;
        interval=500;
        digitalWrite(BLUE,LOW);
        digitalWrite(RED,LOW);
        digitalWrite(GREEN,HIGH);
      } else if (rnd>90 && rnd<=94) {
        motorstate = MOVE_RIGHT;
        interval=500;
      } else if (rnd>94 && rnd<=98) {
        motorstate = MOVE_LEFT;
        interval=500;
      } else if (rnd>98 && rnd<=100) {
        motorstate = MOVE_BACKWARDS;
        interval=500;
      }
        

  }

  loopsensors();
  burstsensors();

  //delay(10);

}
