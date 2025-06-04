#include <Arduino.h>
#include <Wire.h>
#include <vl53l4cd_class.h>
#include <string.h>
#include <stdlib.h>

#define DEV_I2C Wire
#define SerialPort Serial

#ifndef LED_BUILTIN
  #define LED_BUILTIN 13
#endif
#define LedPin LED_BUILTIN

// Components.
VL53L4CD sensor_vl53l4cd_sat(&DEV_I2C,A1);

float h_0=75;
float h;
int k;

void setup() {
  Serial.begin(115200);
  pinMode(LedPin, OUTPUT);

  // Initialize serial for output.
  SerialPort.begin(115200);
  SerialPort.println(F("Initialisation"));

  // Initialize I2C bus.
  DEV_I2C.begin();

  // Configure VL53L4CD satellite component.
  sensor_vl53l4cd_sat.begin();

  // Switch off VL53L4CD satellite component.
  sensor_vl53l4cd_sat.VL53L4CD_Off();

  //Initialize VL53L4CD satellite component.
  sensor_vl53l4cd_sat.InitSensor();

  // Program the highest possible TimingBudget, without enabling the
  // low power mode. This should give the best accuracy
  sensor_vl53l4cd_sat.VL53L4CD_SetRangeTiming(30, 0);

  // Start Measurements
  sensor_vl53l4cd_sat.VL53L4CD_StartRanging();

  Wire.begin();                                     // Start I2C connection
  Serial.println("Connexion réussie");
  k=0;
  delay(100);
}

void loop() {
  capteur();
  if (k<500){
  Serial.println(float(h));
  }
  k=k+1;
  delay(200);
}

void capteur()
{
  static float h_filtree = h_0;
  VL53L4CD_Result_t results;
  uint8_t NewDataReady = 0;
  uint8_t status;

  unsigned long startTime = millis();  // Timestamp du début
  const unsigned long TIMEOUT = 50;  // Timeout en millisecondes

  // Attente de donnée prête, avec timeout
  while (!NewDataReady && (millis() - startTime < TIMEOUT)) {
    status = sensor_vl53l4cd_sat.VL53L4CD_CheckForDataReady(&NewDataReady);
    delay(10);  // Laisser un peu de temps au capteur
  }

  if (!NewDataReady) {
    SerialPort.println("⚠️ Timeout : donnée non disponible !");
    return;  // Sort de la fonction si aucune donnée n'est prête
  }

  digitalWrite(LedPin, HIGH);

  if (!status) {
    sensor_vl53l4cd_sat.VL53L4CD_ClearInterrupt();
    sensor_vl53l4cd_sat.VL53L4CD_GetResult(&results);

    float nouvelle_mesure = results.distance_mm;
    h_filtree = 0.7 * h_filtree + 0.3 * nouvelle_mesure;  // filtre exponentiel
    h = h_filtree;

  } else {
    SerialPort.println("Erreur de communication avec le capteur.");
  }

  digitalWrite(LedPin, LOW);
}