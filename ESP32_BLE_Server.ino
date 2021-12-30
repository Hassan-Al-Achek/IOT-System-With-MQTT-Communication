#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      Serial.println("Hello from on Write\n");
      digitalWrite(12, LOW);
      if(!pCharacteristic->getValue().compare("Hassan")){
      pCharacteristic->setValue("Hassan");
      Serial.println("Hello from onwrite if");
      std::string value = pCharacteristic->getValue();
      }
 }
 void onRead(BLECharacteristic *pCharacteristic){
    std::string value = pCharacteristic->getValue();
          digitalWrite(12, HIGH);
    Serial.println(pCharacteristic->getValue().c_str());
  }

};

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    pinMode(12, OUTPUT);
    
    BLEDevice::init("IOTGroup19");
    BLEServer *pServer = BLEDevice::createServer();
    
    /*Declare Service*/
    BLEService *pService = pServer->createService(SERVICE_UUID);

    /*Declare Characteristic*/
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE | 
            BLECharacteristic::PROPERTY_READ );

    pCharacteristic->setCallbacks(new MyCallbacks());
    
    pCharacteristic->setValue("Hello World says Neil");

    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    

    
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop()
{
    BLEDevice::startAdvertising();
    delay(2000);
}
