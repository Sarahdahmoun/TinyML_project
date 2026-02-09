/*
  inference_vibrations.ino
  - Lit l'accéléromètre (ax, ay, az) du Nano 33 BLE
  - Construit une fenêtre temporelle de 50 échantillons
  - Donne cette fenêtre au modèle TFLite (vibration_model.h)
  - Affiche la classe prédite sur le Serial Monitor
*/

#include <Arduino.h>
#include <Arduino_LSM9DS1.h>

// TensorFlow Lite Micro
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"  // 
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// Le modèle converti en .h 
#include "vibration_model.h"

// =============================
// Paramètres comme au training
// =============================
static const int kWindowSize = 50;          // 50 échantillons ~ 1 seconde à 50 Hz
static const int kNumFeatures = 3;          // ax, ay, az
static const int kNumClasses = 3;           // off / normal / dysfunction
static const int kSamplePeriodMs = 20;      // 50 Hz (20 ms)

// Labels (doivent correspondre à tes labels du notebook)
static const char* kLabels[kNumClasses] = {"off", "normal", "dysfunction"};


constexpr int kTensorArenaSize = 30 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

// =============================
// TFLite Micro: modèle + reporter + interpreter
// =============================
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;


tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;

// =============================
// Buffer de fenêtre (50 x 3)
// =============================
float window_buffer[kWindowSize][kNumFeatures];


int argmax(const float* v, int n) {
  int best_i = 0;
  float best_v = v[0];
  for (int i = 1; i < n; i++) {
    if (v[i] > best_v) {
      best_v = v[i];
      best_i = i;
    }
  }
  return best_i;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Serial.println("=== TinyML Vibration Inference (Nano 33 BLE) ===");

  // IMU
  if (!IMU.begin()) {
    Serial.println("ERROR: IMU.begin() failed");
    while (1) { delay(1000); }
  }
  Serial.println("IMU OK");

  // Charger le modèle TFLite
  model = tflite::GetModel(vibration_model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.print("ERROR: Model schema mismatch. Expected ");
    Serial.print(TFLITE_SCHEMA_VERSION);
    Serial.print(", got ");
    Serial.println(model->version());
    while (1) { delay(1000); }
  }

  // Résolveur d'opérateurs
  static tflite::AllOpsResolver resolver;

  //  Création de l'interpréteur avec la bonne signature (6 args)
  static tflite::MicroInterpreter static_interpreter(
    model,
    resolver,
    tensor_arena,
    kTensorArenaSize,
    error_reporter,
    nullptr  // profiler
  );
  interpreter = &static_interpreter;

  // Allouer les tenseurs
  TfLiteStatus alloc_status = interpreter->AllocateTensors();
  if (alloc_status != kTfLiteOk) {
    Serial.println("ERROR: AllocateTensors() failed");
    while (1) { delay(1000); }
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  // Vérification simple de la forme attendue
  // On s'attend à [1, 50, 3]
  Serial.print("Input dims: ");
  for (int i = 0; i < input->dims->size; i++) {
    Serial.print(input->dims->data[i]);
    if (i < input->dims->size - 1) Serial.print(" x ");
  }
  Serial.println();

  Serial.println("Setup done. Starting inference...");
}

void loop() {
  //  Remplir une fenêtre de 50 échantillons IMU
  for (int i = 0; i < kWindowSize; i++) {
    float ax, ay, az;

    // Attendre que l'IMU ait une mesure dispo
    while (!IMU.accelerationAvailable()) {
      delay(1);
    }

    // Lire accélération (en g)
    IMU.readAcceleration(ax, ay, az);

    window_buffer[i][0] = ax;
    window_buffer[i][1] = ay;
    window_buffer[i][2] = az;

    delay(kSamplePeriodMs);
  }

  // Copier la fenêtre dans le tenseur d'entrée du modèle
  // On doit mettre 50x3 valeurs dans l'ordre.
  int idx = 0;
  for (int i = 0; i < kWindowSize; i++) {
    for (int j = 0; j < kNumFeatures; j++) {
      input->data.f[idx++] = window_buffer[i][j];
    }
  }

  // Lancer l'inférence
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    Serial.println("ERROR: Invoke failed");
    return;
  }

  // Lire la sortie
  float p0 = output->data.f[0];
  float p1 = output->data.f[1];
  float p2 = output->data.f[2];

  int pred = argmax(output->data.f, kNumClasses);

  // Affichage
  Serial.print("Pred: ");
  Serial.print(kLabels[pred]);
  Serial.print(" | probs = [");
  Serial.print(p0, 4); Serial.print(", ");
  Serial.print(p1, 4); Serial.print(", ");
  Serial.print(p2, 4); Serial.println("]");

  
  delay(200);
}
