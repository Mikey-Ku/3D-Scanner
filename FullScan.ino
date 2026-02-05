#include <Servo.h>

Servo Base;     // Horizontal servo (X-axis)
Servo Sensor;   // Vertical servo (Y-axis)

const int sensorPin = A1;              // IR sensor analog input
const uint16_t Sensor_interval = 150;  // Time between movements (ms)

uint32_t move_time_sensor;

int angle_horizontal = 0;  // Left to right (Base)
int angle_vertical = 0;   // Up to down (Sensor)
bool first_reading = true; // Track first reading in a line

const int MAX_VERTICAL_ANGLE = 100;
const int MIN_VERTICAL_ANGLE = 0;
const int MAX_HORIZONTAL_ANGLE = 120;

void setup() {
  Serial.begin(115200);
  Sensor.attach(3);       // Vertical axis (up/down)
  Base.attach(11);        // Horizontal axis (left/right)
 
  // Initialize positions
  Base.write(angle_horizontal);
  Sensor.write(angle_vertical);
  delay(750); // Give servos time to reach top position
 
  move_time_sensor = millis();
  first_reading = true;
}

void loop() {
  if (millis() - move_time_sensor >= Sensor_interval) {
    move_time_sensor = millis();

    // Read IR sensor and calculate distance
    int raw = analogRead(sensorPin);
    float voltage = raw * (5.0 / 1023.0);

    float distance;
    if (voltage <= 0.2317) {
      distance = 120.0; // Out of range
    } else {
      distance = 49.3511 / (voltage - 0.2317); // Your calibration
    }
    if (distance > 30.0) {
      distance = 120.0; // Thresholded "no object"
    }

    // Output distance data in CSV format
    if (!first_reading) {
      Serial.print(",");
    }
    Serial.print(distance, 1); // 1 decimal place
    first_reading = false;

    // --- Vertical Scanning (Top to Bottom Only) ---
    angle_vertical++;
   
    if (angle_vertical > MAX_VERTICAL_ANGLE) {
      // End of vertical sweep - reset to top and move horizontal
      angle_vertical = MIN_VERTICAL_ANGLE;
      Serial.println(); // End of this column
      first_reading = true;
     
      // Move to next horizontal position
      angle_horizontal++;
      if (angle_horizontal > MAX_HORIZONTAL_ANGLE) {
        angle_horizontal = 0; // Reset horizontal position
      }
      Base.write(angle_horizontal);
    }

    // Update vertical servo position
    Sensor.write(angle_vertical);
  }
}

