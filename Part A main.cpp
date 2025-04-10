#include <Arduino.h>
#include <BLEDevice.h>

#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define LED_PIN 2
 
class MyCallbacks: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("Received: ");
      Serial.println(value.c_str());

      // Convert to lowercase for safety
      String command = String(value.c_str());
      command.toLowerCase();

      if (command == "on") {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");
      } else if (command == "off") {
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");
      } else {
        Serial.println("unknown command");
      }

      Serial.println("*********");
    }
  }
};
 
void setup() {
 Serial.begin(115200);
 pinMode(LED_PIN, OUTPUT);
 digitalWrite(LED_PIN, LOW);  

 
 Serial.println("1- Download and install an BLE scanner app in your phone");
 Serial.println("2- Scan for BLE devices in the app");
 Serial.println("3- Connect to MyESP32");
 Serial.println("4- Go to CUSTOM CHARACTERISTIC in CUSTOM SERVICE and write something");
 Serial.println("5- See the magic =)");
 
 BLEDevice::init("SDSUCS");
 BLEServer *pServer = BLEDevice::createServer();
 
 BLEService *pService = pServer->createService(SERVICE_UUID);
 
 BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        BLECharacteristic::PROPERTY_READ |
                                        BLECharacteristic::PROPERTY_WRITE
                                      );
 
 pCharacteristic->setCallbacks(new MyCallbacks());
 
 pCharacteristic->setValue("Send 'on' or 'off' ");
 pService->start();
 
 BLEAdvertising *pAdvertising = pServer->getAdvertising();
 pAdvertising->start();
}
 
void loop() {
 delay(2000);
}
