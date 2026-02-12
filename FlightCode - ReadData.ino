const float P0 = 101325.0;
float time = 0.0;
float pressure = 0.0;
float altitude = 0.0;
float maxAltitude = 0.0;
float groundAltitude = 0.0;

const int PRESSURE_AVG_SIZE = 30;
float pressureSamples[PRESSURE_AVG_SIZE];
float pressureSum = 0;
float pressureAverage = 0;
int pressureIndex = 0;

bool baselineSet = false;
float baselinePressure = 0;

bool liftoffDetected = false;
int liftoffDetectionCount = 0;
int REQUIRED_CONSECUTIVE_DETECTIONS = 3;

bool apogeeDetected = false;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // wait for connection
  while (!Serial) {}

  Serial.println("ARDUINO IS READY");

  for (int i=0; i<PRESSURE_AVG_SIZE; i++) {
    pressureSamples[i] = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()) {

      String textTime = Serial.readStringUntil(' ');
      String textPressure = Serial.readStringUntil('\n');

      time = textTime.toFloat();
      pressure = textPressure.toFloat();

      // Print Altitude and Pressure 
      // Serial.print("A: ");
      // Serial.print(altitude, 3); // ln is move to new line
      // Serial.print(" P: ");
      // Serial.println(pressure, 3); // ln is move to new line

      // Update Pressure Average
      pressureSum -= pressureSamples[pressureIndex]; // Remove oldest sample
      pressureSamples[pressureIndex] = pressure; // Replace with newest sample
      pressureSum += pressure; // Add newest pressure to pressureSum
      pressureIndex = (pressureIndex + 1) % PRESSURE_AVG_SIZE; // Advance pressureIndex by 1
      pressureAverage = pressureSum / PRESSURE_AVG_SIZE;

      // Convert back to altitude
      altitude = 44330.0 * (1.0 - pow(pressure / P0, 0.1903));

      // Set a baseline for our pressure using first 30 readings once completed
      if (!baselineSet && pressureIndex == 0) {
        baselinePressure = pressureAverage;
        baselineSet = true;
        Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");
        Serial.print("Baseline pressure set: "); Serial.println(baselinePressure);
        Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");

        // Calculate ground altitude
        groundAltitude = 44330.0 * (1.0 - pow(baselinePressure / P0, 0.1903));
      }

      // After baseline set, keep track of max altitude so far
      if (baselineSet) {
        if (altitude > maxAltitude) {
          maxAltitude = altitude;
        }
      }

      if (true) { // pressureIndex == 0 is placeholder for SD card check / causes less prints
        Serial.print("Time: "); Serial.print(time, 3);
        Serial.print(", Pressure: "); Serial.print(pressure, 3);
        Serial.print(", Temperature: "); Serial.print("n/a"); //Placeholder for temp reading
        Serial.print(", Altitude: "); Serial.println(altitude, 3);
      }

      // Liftoff Check
      if (!liftoffDetected && baselineSet) {
        // Check if rocket 50+ meters off ground
        if ((altitude - groundAltitude) > 50) {
          liftoffDetectionCount++;
          // Need enough checks > REQUIRED_CONSECUTIVE_DETECTIONS
          // Avoids noise, so confirms several times before declaring true
          if (liftoffDetectionCount >= REQUIRED_CONSECUTIVE_DETECTIONS) {
            liftoffDetected = true;
            Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");
            Serial.println("-------Liftoff Detected -------");
            Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");
          }
          
        } else {
          // Need 3 consecutive liftoffs or reset
          liftoffDetectionCount = 0;
        }
      
      }

      // And finally, the Apogee detection
      // NEEDS ADJUSTMENT FOR OUTLIERS, right now bad readings set this off
      if ((!apogeeDetected && altitude < maxAltitude - 10.0) && liftoffDetected) {
        Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");
        Serial.println("-------Apogee Detected -------");
        Serial.print("-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n-\n");
        Serial.print("Apogee: "); Serial.println(maxAltitude - groundAltitude);
        apogeeDetected = true;
      }
  
  }

}
