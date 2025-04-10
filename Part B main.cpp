#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// IMU object
LSM6DSO myIMU;  

BLECharacteristic *pCharacteristic;

int stepCount = 0;
float threshold = 1.5; // Calibrate this
bool stepDetected = false;


void setup() {
  Serial.begin(115200);

  Wire.begin();  // Initialize I2C with defined pins
  delay(10);

  Serial.println("Trying to initialize IMU...");
  if (myIMU.begin()) {
    Serial.println("IMU initialized!");
  } else {
  Serial.println("IMU not initialized");
  }

  myIMU.initialize(BASIC_SETTINGS);

  BLEDevice::init("StepCounter");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->setValue("Zero Steps"); // Initialize with 0
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true); // Optional: Add scan response data
  pAdvertising->start();

  Serial.println("BLE advertising started!");
}

void loop() {
  static unsigned long lastRead = 0;

  if (millis() - lastRead >= 20) {
    lastRead = millis();

    float accX = myIMU.readFloatAccelX();
    float accY = myIMU.readFloatAccelY();

    float rms = sqrt(accX * accX + accY * accY);

    if (rms > threshold && !stepDetected) {
      stepCount++;
      stepDetected = true;

      Serial.print("Step detected! Total: ");
      Serial.println(stepCount);

      pCharacteristic->setValue(String(stepCount).c_str());
      pCharacteristic->notify();
    }

    if (rms < threshold - 0.2) {
      stepDetected = false;
    }
  }
  delay(10); // Small delay to prevent excessive CPU usage
}
