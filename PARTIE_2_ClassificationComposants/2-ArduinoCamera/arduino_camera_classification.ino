

#include <Arduino.h>

// Header Edge Impulse
#include <ClassificationComposants_inferencing.h>

// Caméra OV767x
#include <Arduino_OV767X.h>   // Bibliothèque caméra (OV7675)

#include <ArduinoBLE.h>


static const int SERIAL_BAUD = 115200;


static const bool IGNORE_BACKGROUND = true;

// Seuil de confiance minimum pour envoyer une détection
static const float MIN_CONFIDENCE = 0.70f;

// Intervalle entre inférences (ms)
static const uint32_t INFER_INTERVAL_MS = 600;


static const char* BLE_DEVICE_NAME = "ei-components";
static const char* BLE_SERVICE_UUID = "19B10000-E8F2-537E-4F6C-D104768A1214";
static const char* BLE_CHAR_UUID    = "19B10001-E8F2-537E-4F6C-D104768A1214";

BLEService eiService(BLE_SERVICE_UUID);
BLEStringCharacteristic eiChar(BLE_CHAR_UUID, BLERead | BLENotify, 40);


// Taille d’entrée du modèle EI (ex: 48x48)
static const int EI_W = EI_CLASSIFIER_INPUT_WIDTH;
static const int EI_H = EI_CLASSIFIER_INPUT_HEIGHT;

// features = frame size (grayscale) attendu par EI DSP
static float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];

// Frame caméra : on prend 160x120 en RGB565 
static const int CAM_W = 160;
static const int CAM_H = 120;
static uint8_t camFrame[CAM_W * CAM_H * 2];   



// Convert RGB565 -> grayscale [0..255]
static inline uint8_t rgb565_to_gray(uint16_t p) {
  // Extract RGB (5/6/5 bits)
  uint8_t r = (p >> 11) & 0x1F;
  uint8_t g = (p >> 5)  & 0x3F;
  uint8_t b = (p >> 0)  & 0x1F;

  
  uint8_t r8 = (r * 255) / 31;
  uint8_t g8 = (g * 255) / 63;
  uint8_t b8 = (b * 255) / 31;

  
  uint16_t y = (uint16_t)(77 * r8 + 150 * g8 + 29 * b8) >> 8;
  return (uint8_t)y;
}


static void fill_features_from_camera_rgb565() {
  // On suppose camFrame = RGB565
  // features[] attend des floats correspondant aux pixels (0..255) en grayscale.
  // EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE doit être pour grayscale image block.

  int idx = 0;
  for (int y = 0; y < EI_H; y++) {
    int srcY = (y * CAM_H) / EI_H;
    for (int x = 0; x < EI_W; x++) {
      int srcX = (x * CAM_W) / EI_W;
      int srcIndex = (srcY * CAM_W + srcX) * 2;

      uint16_t pix = (uint16_t)camFrame[srcIndex] | ((uint16_t)camFrame[srcIndex + 1] << 8);
      uint8_t gray = rgb565_to_gray(pix);

      features[idx++] = (float)gray;
    }
  }
}

// Edge Impulse signal callback
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

// Trouver the best label
static void get_top_prediction(const ei_impulse_result_t &result, const char* &label, float &score) {
  label = "unknown";
  score = 0.0f;

  for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
    const char* l = result.classification[ix].label;
    float s = result.classification[ix].value;

    if (s > score) {
      label = l;
      score = s;
    }
  }
}

static bool is_background_label(const char* label) {

  return (strcmp(label, "background") == 0);
}



static void ble_init_if_possible() {
  if (!BLE.begin()) {
    Serial.println("[BLE] init failed -> BLE disabled");
    return;
  }
  BLE.setLocalName(BLE_DEVICE_NAME);
  BLE.setDeviceName(BLE_DEVICE_NAME);

  BLE.setAdvertisedService(eiService);
  eiService.addCharacteristic(eiChar);
  BLE.addService(eiService);

  eiChar.writeValue("ready");
  BLE.advertise();

  Serial.println("[BLE] advertising OK");
}

static bool camera_init() {
  if (!Camera.begin(QQVGA, RGB565, 1)) {
    Serial.println("[CAM] Camera.begin failed");
    return false;
  }

  
  Serial.println("[CAM] OV767x started (QQVGA RGB565)");
  return true;
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1500);

  Serial.println("=== EI CAMERA CLASSIFICATION START ===");
  Serial.print("EI input: ");
  Serial.print(EI_W);
  Serial.print("x");
  Serial.println(EI_H);

  
  ble_init_if_possible();

  // Caméra
  if (!camera_init()) {
    Serial.println("FATAL: camera init failed");
    while (1) delay(1000);
  }
}

// -------------------- Loop --------------------

void loop() {
  if (BLE.connected()) {
    BLE.poll();
  }

  // Capture frame
 Camera.readFrame(features);


  // Build signal + infer
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
  signal.get_data = &get_signal_data;

  ei_impulse_result_t result = {0};
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);

  if (err != EI_IMPULSE_OK) {
    Serial.print("{\"err\":\"run_classifier\",\"code\":");
    Serial.print((int)err);
    Serial.println("}");
    delay(INFER_INTERVAL_MS);
    return;
  }

  // Top prediction
  const char* label;
  float score;
  get_top_prediction(result, label, score);

  // Filtrage
  bool send_it = true;
  if (score < MIN_CONFIDENCE) send_it = false;
  if (IGNORE_BACKGROUND && is_background_label(label)) send_it = false;

  // Output JSON line, format simple pour Node-RED: {"label":"led","score":0.92}
  if (send_it) {
    Serial.print("{\"label\":\"");
    Serial.print(label);
    Serial.print("\",\"score\":");
    Serial.print(score, 5);
    Serial.println("}");

    // BLE notify (texte)
    if (BLE.connected()) {
      char msg[40];
      snprintf(msg, sizeof(msg), "%s:%.2f", label, score);
      eiChar.writeValue(msg);
    }
  } 
    

  delay(INFER_INTERVAL_MS);
}

