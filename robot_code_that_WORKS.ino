#include <IRremote.hpp>

// motor driver pins
#define ENA 9
#define IN1 12
#define IN2 11
#define ENB 6
#define IN3 7
#define IN4 8

// ultrasonic + LED pins
const int trigPin = 10;
const int echoPin = 4;

// object follow sensor pins
const int leftSensor  = 2;
const int rightSensor = 5;

// IR receiver pin
const int irPin = 3;

// ir pins
const int ir1 = A0;  // far left
const int ir2 = A1;  // mid left
const int ir3 = A2;  // center
const int ir4 = A3;  // mid right
const int ir5 = A4;  // far right

//current state constants
const int STC = 0; //state center
const int STL = 1; //state turn left
const int SHL = 2; //state turn hard left
const int STR = 3; //state turn right
const int SHR = 4; //state turn hard right
const int STOP = 5; //stop

const int TURN_SPEED       = 190; //turn speed constants for IR
const int BACK_TURN_SPEED  = 190;
const int SHARP_TURN_SPEED = 150;

int pressMode = 9;  // holds values 1-8; start in avoid mode

// IR hex codes
const unsigned long BUTTON_1 = 0xBA45FF00;      // line following
const unsigned long BUTTON_2 = 0xB946FF00;      // obstacle avoidance
const unsigned long BUTTON_3 = 0xB847FF00;      // dance
const unsigned long BUTTON_4 = 0xBB44FF00;      // object follow
const unsigned long BUTTON_UP = 0xE718FF00;     // forward
const unsigned long BUTTON_RIGHT = 0xA55AFF00;  // right
const unsigned long BUTTON_DOWN = 0xAD52FF00;   // backward
const unsigned long BUTTON_LEFT = 0xF708FF00;   // left
const unsigned long BUTTON_OK = 0xE31CFF00;     // stop / sleep

#define SPEED 190         // motor speed
#define TURN_TIME 800     // turn duration in ms
#define STOP_DISTANCE 20  // obstacle distance in cm

void setup() {
  // motor pins
  pinMode(ENA, OUTPUT); //sets ENA pin to output
  pinMode(IN1, OUTPUT); //sets IN1 pin to output
  pinMode(IN2, OUTPUT); //sets IN2 pin to output
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // object following sensors
  pinMode(leftSensor,  INPUT);
  pinMode(rightSensor, INPUT);

  // serial for distance debugging
  Serial.begin(9600);

  // object following sensors
  pinMode(leftSensor,  INPUT);
  pinMode(rightSensor, INPUT);

  IrReceiver.begin(irPin, ENABLE_LED_FEEDBACK);
  delay(500);
}

void moveForward() {  // drive forward  
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBack() {  // drive backward
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void lineturnRight() {  // pivot right for TURN_TIME
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

}

void turnRight() {  // pivot right for TURN_TIME
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(TURN_TIME);
}

void backLeft() { //actually backwards but sensors are on other side of bot
  // Left motor backward, right motor stopped
  // -> front of robot rotates LEFT (toward an object on the LEFT side)
  analogWrite(ENA, BACK_TURN_SPEED);
  analogWrite(ENB, BACK_TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

}

void backRight() {
  // Right motor backward, left motor stopped
  // -> front of robot rotates RIGHT (toward an object on the RIGHT side)
  analogWrite(ENA, BACK_TURN_SPEED);
  analogWrite(ENB, BACK_TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

}

void turnLeft() {  // pivot left for TURN_TIME
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(TURN_TIME);
}

void lineturnLeft() {  // pivot left for TURN_TIME
  analogWrite(ENA, TURN_SPEED);
  analogWrite(ENB, TURN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
}


void turnsharpRight() {  // pivot right for TURN_TIME
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
 
}

void turnsharpLeft() {  // pivot left for TURN_TIME
  analogWrite(ENA, SHARP_TURN_SPEED);
  analogWrite(ENB, SHARP_TURN_SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

}

void moveStop() {  // stop all motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}



void danceMode() {
  for (int i = 0; i < 2; i++) {  // dance movement
    moveForward();
    delay(500);
    moveBack();
    delay(500);
    moveStop();
    turnRight();
    delay(500);
    moveForward();
    delay(500);
    moveBack();
    delay(500);
    moveStop();
    turnLeft();
    delay(500);
  }
  
  turnRight();  // final spin
  delay(5000);
  moveStop();
  delay(2000);
}

int checkDistance() {  // measure distance in cm
  long duration;
  int distanceCm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);  // read echo pulse
  distanceCm = duration / 58;         // convert time to distance
  return distanceCm;
}

void avoidMode() {  // obstacle avoidance
  int distanceCm = checkDistance();

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  if (distanceCm <= STOP_DISTANCE) {  // obstacle ahead
    moveStop();
    delay(1000);
    moveBack();
    delay(200);
    turnRight();  // already turns for TURN_TIME
    moveStop();
    delay(200);
  }
  else {  // path is clear
    moveForward();
  }
}

void objectFollow() {
  int IRLeft  = digitalRead(leftSensor);
  int IRRight = digitalRead(rightSensor);

  Serial.print("L: 0");
  Serial.print(IRLeft);
  Serial.print("R: 1");
  Serial.println(IRRight);

  if(IRLeft == LOW && IRRight == LOW) {
    moveBack();
  }
  else if(IRLeft == HIGH && IRRight == HIGH) {
    moveStop();
  }
  else if(IRLeft == LOW  && IRRight == HIGH) {
    backRight(); //object on left
  }
  else if(IRLeft == HIGH && IRRight == LOW) {
    backLeft(); //object on right
  }

  delay(50);
}

void lineFollow() {
  int currentState = STC;
  int lastDirection = 0;  // -1 = left, 0 = center, +1 = right

  int s1 = digitalRead(ir1);
  int s2 = digitalRead(ir2);
  int s3 = digitalRead(ir3);
  int s4 = digitalRead(ir4);
  int s5 = digitalRead(ir5);

  Serial.print(s1);
  Serial.print(s2);
  Serial.print(s3);
  Serial.print(s4);
  Serial.println(s5);

  if (s3 == LOW) {
    currentState = STC;
    lastDirection = 0;
  }
  else if (s2 == LOW) {
    currentState = STL;
    lastDirection = -1;
  }
  else if (s1 == LOW) {
    currentState = SHL;
    lastDirection = -1;
  }
  else if (s4 == LOW) {
    currentState = STR;
    lastDirection = +1;
  }
  else if (s5 == LOW) {
    currentState = SHR;
    lastDirection = +1;
  }
  else {
    currentState = STOP;
  }

  switch(currentState) {
    case STC:
      moveForward();
      break;
      
    case STL:
      lineturnLeft();
      break;
      
    case SHL:
      turnsharpLeft();
      break;
      
    case STR:
      lineturnRight();
      break;
      
    case SHR:
      turnsharpRight();
      break;
      
    case STOP:
      if (lastDirection = -1) {
        turnsharpLeft();
        moveForward();
      }
      else if (lastDirection = +1) {
        turnsharpRight();
        moveForward();
      }
      else {
        moveStop();
      }
      //stop
      break;
  }
}

void loop() {
  // read the remote and update the mode
  if (IrReceiver.decode()) {
    unsigned long buttonCode = IrReceiver.decodedIRData.decodedRawData;

    switch (buttonCode) {
      case BUTTON_1: 
        pressMode = 1; 
        break;
      case BUTTON_2: 
        pressMode = 2; 
        break;
      case BUTTON_3: 
        pressMode = 3; 
        break;
      case BUTTON_4: 
        pressMode = 4; 
        break;
      case BUTTON_UP: 
        pressMode = 5; 
        break;
      case BUTTON_RIGHT: 
        pressMode = 6; 
        break;
      case BUTTON_DOWN: 
        pressMode = 7; 
        break;
      case BUTTON_LEFT: 
        pressMode = 8; 
        break;
      case BUTTON_OK:
        pressMode = 9;
        break;
    }
    IrReceiver.resume();
  }
  else {
    moveStop();
  }

  // run the selected mode
  switch (pressMode) {
    case 1:  // line following (stub - not yet implemented)
      Serial.println("line following mode");
      moveStop();
      lineFollow();
      break;

    case 2:  // obstacle avoidance
      Serial.println("Obstacle Avoid mode");
      avoidMode();
      break;

    case 3:  // dance once, then idle
      Serial.println("dance dance mode");
      moveStop();
      danceMode();
      pressMode = 4;  // stop after one routine
      break;

    case 4:  // object follow
      moveStop();
      Serial.println("object follow");
      objectFollow();
      break;

    case 5:  // forward
      moveStop();
      Serial.println("forward women!");
      moveForward();
      break;

    case 6:  // right
      moveStop();
      Serial.println("right");
      turnRight();
      break;

    case 7:  // backward
      moveStop();
      Serial.println("RETREAT!");
      moveBack();
      break;

    case 8:  // left
      moveStop();
      Serial.println("left");
      turnLeft();
      break;
    
    case 9:  // sleep / stop
      Serial.println("eppyyy");
      moveStop();
      break;
  }

  delay(100);
}
