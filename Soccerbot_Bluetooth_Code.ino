#include "BluetoothSerial.h"


// Motor pins (Left)
#define LPWM_A  21
#define LPWM_B  19

// Motor pins (Right)
#define RPWM_A  23
#define RPWM_B  22




// Constants
#define MAX_RPM        800
#define LIMIT_RPM      300
#define MAX_PWM        255
#define LIMIT_PWM      ((LIMIT_RPM * MAX_PWM) / MAX_RPM)


BluetoothSerial serialBT;
char BT;
int Speed = LIMIT_PWM;


void setup() {
  Serial.begin(115200);
  serialBT.begin("Soccerbot IUT");
  pinMode(RPWM_A, OUTPUT);
  pinMode(RPWM_B, OUTPUT);
  pinMode(LPWM_A, OUTPUT);
  pinMode(LPWM_B, OUTPUT);

  ledcAttach(RPWM_A, 5000, 8); 
  ledcAttach(RPWM_B, 5000, 8);  
  ledcAttach(LPWM_A, 5000, 8);  
  ledcAttach(LPWM_B, 5000, 8);
}

void loop() {
  while (serialBT.available()) {
    BT = serialBT.read();

    if ('0'<=BT<='4') Speed = LIMIT_PWM;
    else Speed = MAX_PWM;


    if (BT == 'F') go_forward();
    else if (BT == 'B') go_backward();
    else if (BT == 'L') go_left();
    else if (BT == 'R') go_right();
    else if (BT == 'S') stopMotors();
    else if (BT == 'I') forward_right();
    else if (BT == 'J') backward_right();
    else if (BT == 'G') forward_left();
    else if (BT == 'H') backward_left();
    else stopMotors();
  }
}




void go_forward() {
  ledcWrite(RPWM_A, LIMIT_PWM);  
  ledcWrite(RPWM_B, 0);      
  ledcWrite(LPWM_A, LIMIT_PWM); 
  ledcWrite(LPWM_B, 0);      
}
void go_backward() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, Speed);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, Speed);
}
void go_right() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, LIMIT_PWM);
  ledcWrite(LPWM_A, LIMIT_PWM);
  ledcWrite(LPWM_B, 0);
}
void go_left() {
  ledcWrite(RPWM_A, LIMIT_PWM);
  ledcWrite(RPWM_B, 0);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, LIMIT_PWM);
}
void stopMotors() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, 0);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, 0);
}
void forward_right() {
  ledcWrite(RPWM_A, LIMIT_PWM);
  ledcWrite(RPWM_B, 0);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, 0);
}
void backward_right() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, Speed);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, 0);
}
void forward_left() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, 0);
  ledcWrite(LPWM_A, LIMIT_PWM);
  ledcWrite(LPWM_B, 0);
}
void backward_left() {
  ledcWrite(RPWM_A, 0);
  ledcWrite(RPWM_B, 0);
  ledcWrite(LPWM_A, 0);
  ledcWrite(LPWM_B, Speed);
}