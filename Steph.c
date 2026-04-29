#include <math.h>
#include <Bluepad32.h>

GamepadPtr myGamepad;

// ===== LED CONFIG =====
// Status LED pin (GPIO 2 = built-in LED on many ESP32 boards)
const int STATUS_LED_PIN = 2;

// Blink timing
const unsigned long BLINK_INTERVAL = 500;  // ms
unsigned long lastBlinkTime = 0;
bool ledState = false;

void onConnectedGamepad(GamepadPtr gp) {
  myGamepad = gp;
  Serial.println("Gamepad connected");
}

void onDisconnectedGamepad(GamepadPtr gp) {
  myGamepad = nullptr;
  Serial.println("Gamepad disconnected");
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // IMPORTANT

  // Set LED pin
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  Serial.println("Starting Bluepad32...");
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
  Serial.println("Waiting for controller...");
}

void loop() {
  bool newData = BP32.update();
  if (!newData) {
    return;
  }

  // ===== LED STATUS LOGIC =====
  if (myGamepad && myGamepad->isConnected()) {
    // Solid ON when connected
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    // Blink when not connected
    unsigned long now = millis();
    if (now - lastBlinkTime >= BLINK_INTERVAL) {
      lastBlinkTime = now;
      ledState = !ledState;
      digitalWrite(STATUS_LED_PIN, ledState);
    }
  }

  // ===== GAMEPAD DATA =====
  if (myGamepad && myGamepad->isConnected()) {
    Serial.printf("Buttons: %08X\n", myGamepad->buttons());
    delay(300);

    // read gamepad
    float x = myGamepad->axisX();
    float y = myGamepad->axisY();

    Serial.printf("X axis before dz: %f\n", x);
    Serial.printf("Y axis before dz: %f\n", y);

    // deadzone x and y
    if (x >= -10 && x <= 10) {
      x = 0;
    }

    if (y >= -10 && y <= 10) {
      y = 0;
    }

    Serial.printf("X axis after dz: %f\n", x);
    Serial.printf("Y axis after dz: %f\n", y);

    if (x == 0 && y == 0) {
      Serial.println("Direction: None");
      return;
    }

    // convert to degrees
    double degrees = atan2(y, x) * 180.0 / PI;
    Serial.printf("Degrees: %f\n", degrees);

    //add 180 to constrain to 0-360
    degrees += 180;

    if (degrees >= 316 && degrees <= 360 || degrees >= 0 && degrees < 44) {
      Serial.println("Direction: Up");
    } else if (degrees >= 45 && degrees <= 135) {
      Serial.println("Direction: Right");
    } else if (degrees >= 136 && degrees < 224) {
      Serial.println("Direction: Down");
    } else if (degrees >= 225 && degrees < 315) {
      Serial.println("Direction: Left");
    }
  }
}
