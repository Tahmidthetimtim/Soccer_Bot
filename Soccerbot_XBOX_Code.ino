#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// Motor pins (Left)
#define LPWM_A  22
#define LPWM_B  23

// Motor pins (Right)
#define RPWM_A  19
#define RPWM_B  21

// Constants
#define MAX_RPM              800
#define FORWARD_LIMIT_RPM    300
#define BACKWARD_LIMIT_RPM   300
#define MAX_PWM              255
#define FORWARD_LIMIT_PWM    ((FORWARD_LIMIT_RPM * MAX_PWM) / MAX_RPM)
#define BACKWARD_LIMIT_PWM   ((BACKWARD_LIMIT_RPM * MAX_PWM) / MAX_RPM)

// Speed values using RPM limits converted to PWM
int forwardSpeed = FORWARD_LIMIT_PWM;
int backwardSpeed = BACKWARD_LIMIT_PWM;

void go_backward() {
  Serial.println("forward");
  ledcWrite(0, forwardSpeed);  // Right forward
  ledcWrite(1, 0);
  ledcWrite(2, forwardSpeed);  // Left forward
  ledcWrite(3, 0);
}
void go_forward() {
  Serial.println("backward");
  ledcWrite(0, 0);
  ledcWrite(1, backwardSpeed);  // Right backward
  ledcWrite(2, 0);
  ledcWrite(3, backwardSpeed);  // Left backward
}
void go_forwardr() {
  Serial.println("turn right");
  ledcWrite(0, MAX_RPM );  // Right forward slower or stop
  ledcWrite(1, 0);
  ledcWrite(2, 0);             // Left backward
  ledcWrite(3, MAX_RPM );
}
void turn_right() {
  Serial.println("turn right");
  ledcWrite(0, forwardSpeed);  // Right forward slower or stop
  ledcWrite(1, 0);
  ledcWrite(2, 0);             // Left backward
  ledcWrite(3, forwardSpeed);
}
void turn_left() {
  Serial.println("turn left");
  ledcWrite(0, 0);             // Right backward
  ledcWrite(1, forwardSpeed);
  ledcWrite(2, forwardSpeed);  // Left forward
  ledcWrite(3, 0);
}
void stopMotors() {
  Serial.println("stop");
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller connected, index=%d\n", i);
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected, index=%d\n", i);
            myControllers[i] = nullptr;
            break;
        }
    }
}

void processGamepad(ControllerPtr ctl) {
    int axisRX = ctl->axisRX();
    int axisRY = ctl->axisRY();

    // Deadzone
    int deadzone = 200;

    bool forward = axisRY > deadzone;
    bool backward = axisRY < -deadzone;
    bool right = axisRX > deadzone;
    bool left = axisRX < -deadzone;

    if (forward && !right && !left) {
        go_forward();
    }
    else if (backward && !right && !left) {
        go_backward();
    }
    else if (right && !forward && !backward) {
        turn_right();
    }
    else if (left && !forward && !backward) {
        turn_left();
    }
    else if (forward && right) {
        Serial.println("forward right");
        ledcWrite(0, forwardSpeed / 2);  // Right slower
        ledcWrite(1, 0);
        ledcWrite(2, forwardSpeed);      // Left full speed
        ledcWrite(3, 0);
    }
    else if (forward && left) {
        Serial.println("forward left");
        ledcWrite(0, forwardSpeed);      // Right full speed
        ledcWrite(1, 0);
        ledcWrite(2, forwardSpeed / 2);  // Left slower
        ledcWrite(3, 0);
    }
    else if (backward && right) {
        Serial.println("backward right");
        ledcWrite(0, 0);
        ledcWrite(1, backwardSpeed / 2);  // Right slower
        ledcWrite(2, 0);
        ledcWrite(3, backwardSpeed);      // Left full speed
    }
    else if (backward && left) {
        Serial.println("backward left");
        ledcWrite(0, 0);
        ledcWrite(1, backwardSpeed);      // Right full speed
        ledcWrite(2, 0);
        ledcWrite(3, backwardSpeed / 2);  // Left slower
    }
    else if (ctl->a()) {
        go_forwardr();

    }
    else {
        stopMotors();
    }

    Serial.printf("axis R: %4d, %4d\n", axisRX, axisRY);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            processGamepad(myController);
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(RPWM_A, OUTPUT);
    pinMode(RPWM_B, OUTPUT);
    pinMode(LPWM_A, OUTPUT);
    pinMode(LPWM_B, OUTPUT);

    ledcSetup(0, 5000, 8);
    ledcSetup(1, 5000, 8);
    ledcSetup(2, 5000, 8);
    ledcSetup(3, 5000, 8);
    // Setup PWM channels
    ledcAttachPin(RPWM_A, 1);  // Right motor forward
    ledcAttachPin(RPWM_B, 0);  // Right motor backward
    ledcAttachPin(LPWM_A, 2);  // Left motor forward
    ledcAttachPin(LPWM_B, 3);  // Left motor backward

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n",
                  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);

    Serial.println("Setup done");
}

void loop() {
    if (BP32.update()) {
        processControllers();
    }
    delay(100);  // Slightly faster response
}