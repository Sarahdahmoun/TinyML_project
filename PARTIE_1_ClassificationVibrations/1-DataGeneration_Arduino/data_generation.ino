#include <Arduino_LSM9DS1.h>
// Fréquence d'échantillonnage: 50 Hz (1 mesure toutes les 20 ms)
const unsigned long SAMPLE_PERIOD_MS = 20;
unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; } //Attend l'ouverture du port série
  Serial.println("Started");

  //Initialisation de l'IMU
   if (!IMU.begin()) {
    Serial.println("ERROR: IMU.begin() failed");
    while (1) { delay(1000); }
   }

  Serial.println("IMU OK.");
  Serial.println("Format: ACC,t_ms,ax,ay,az");
}

void loop() {
  unsigned long now = millis();
  //Gareder un rythme stable 
  if(now - lastSampleTime < SAMPLE_PERIOD_MS) return;
  lastSampleTime = now;
  //Lecture de l'accéléromètre
  if (IMU.accelerationAvailable()) {
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);
    // Données vers le PC
    Serial.print("ACC,");
    Serial.print(now);
    Serial.print(",");
    Serial.print(ax, 6);
    Serial.print(",");
    Serial.print(ay, 6);
    Serial.print(",");
    Serial.println(az, 6);
  }

}
