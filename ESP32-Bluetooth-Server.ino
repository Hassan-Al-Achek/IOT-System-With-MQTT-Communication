#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#include <analogWrite.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


// defining pin numbers
#define selectmenupushbuttonPIN 34 // a5dar
#define nextmenupushbuttonPIN 25  // bene
#define dhtPIN 4
#define potentiometerPIN 36
#define rgbPIN 26

// Services Bluetooth
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_1 "f0964972-6d7f-11ec-90d6-0242ac120003"

// creating needed objects
Adafruit_SH1107 Displayboard = Adafruit_SH1107(64, 128, &Wire);
DHT dht(dhtPIN, DHT22);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, rgbPIN, NEO_GRB + NEO_KHZ800);

// defining data needed
int currentmenuoption = 1;
bool optionselected = false;
String tempunit = "C";
float tempfactor1 = 1;
float tempfactor2 = 0;
String lightstatus = "OFF";
String potaction = "dim";
char color = 'r';
bool nextOption = false;
bool selectOption = false;
BLECharacteristic *pCharacteristic;
BLECharacteristic *pCharacteristic1;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) { 
      if(!pCharacteristic->getValue().compare("HIGH")){
        nextOption= true;
      }
 }
 void onRead(BLECharacteristic *pCharacteristic){
    Serial.println(pCharacteristic->getValue().c_str());
  }

};

class SelectOptionCallBack: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) { 
      if(!pCharacteristic->getValue().compare("HIGH")){
        selectOption= true;
      }
 }
 void onRead(BLECharacteristic *pCharacteristic){
    Serial.println(pCharacteristic->getValue().c_str());
  }

};


void setup()
{
  Serial.begin(115200);

  // Bluethooth Initialization
   BLEDevice::init("IOTGroup19");
    BLEServer *pServer = BLEDevice::createServer();
    
    /*Declare Service*/
    BLEService *pService = pServer->createService(SERVICE_UUID);

    /*Declare Characteristic*/

    /*Characteristic To Change Menu Current Option*/
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE | 
            BLECharacteristic::PROPERTY_READ );

    /*Characteristic To Select An Option*/
    pCharacteristic1 = pService->createCharacteristic(
        CHARACTERISTIC_UUID_1,
            BLECharacteristic::PROPERTY_WRITE | 
            BLECharacteristic::PROPERTY_READ );

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic1->setCallbacks(new SelectOptionCallBack());
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    

  
  // defining inputs and outputs
  pinMode(selectmenupushbuttonPIN, INPUT);
  pinMode(nextmenupushbuttonPIN, INPUT);
  pinMode(dhtPIN, INPUT);
  pinMode(potentiometerPIN, INPUT);

  // initial setup
  dht.begin();
#if defined(__AVR_ATtiny85__)
  if (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
  strip.begin();

  Displayboard.begin(0x3C, true); // Address 0x3C default
  Displayboard.display();
  delay(1000);
  Displayboard.clearDisplay();
  Displayboard.display();
  Displayboard.setRotation(1);

  // text display tests
  Displayboard.setTextSize(1);
  Displayboard.setTextColor(SH110X_WHITE);
  Displayboard.setCursor(0, 0);
  Displayboard.print("display temperature and humidity");
  pCharacteristic->setValue("display temperature and humidity");
  Displayboard.display();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0);
}

void loop()
{
  BLEDevice::startAdvertising();
  if (nextOption)
  {
    delay(200);
    Displayboard.clearDisplay();
    nextoption();
    optionselected = false;
    nextOption = false;
  }
  if (selectOption)
  {
    delay(200);
    Displayboard.clearDisplay();
    optionselected = true;
    selectOption = false;
  }
  if (optionselected == true)
  {
    makeaction();
  }
  potentiometer();
  if (lightstatus == "ON")
  {
    strip.show();
    delay(10);
  }
  else
  {
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    delay(10);
  }
  Displayboard.setCursor(0, 0);
  Displayboard.display();
  delay(10);
}

void nextoption()
{
  switch (currentmenuoption)
  {
  case 1:
    currentmenuoption = 2;
    Displayboard.print("change temperature F°/C°");
    pCharacteristic->setValue("change temperature F°/C°");
    break;
  case 2:
    currentmenuoption = 3;
    Displayboard.print("turn the light on/off");
    pCharacteristic->setValue("turn the light on/off");
    break;
  case 3:
    currentmenuoption = 4;
    Displayboard.print("change potentiometer action: dim/set color");
    pCharacteristic->setValue("change potentiometer action: dim/set color");
    break;
  case 4:
    currentmenuoption = 5;
    Displayboard.print("sleep");
    pCharacteristic->setValue("sleep");
    break;
  case 5:
    currentmenuoption = 1;
    Displayboard.print("display temperature and humidity");
    pCharacteristic->setValue("display temperature and humidity");
    break;
  }
}

void makeaction()
{
  switch (currentmenuoption)
  {
  case 1:
    Displayboard.clearDisplay();
    Displayboard.print("temperature : " + String(dht.readTemperature() * tempfactor1 + tempfactor2) + " " + tempunit + "    humidity : " + String(dht.readHumidity()));
//    String Result = "temperature : " + String(dht.readTemperature() * tempfactor1 + tempfactor2) + " " + tempunit + "    humidity : " + String(dht.readHumidity());
    pCharacteristic1->setValue("Hi Hassan");
    break;
  case 2:
    Displayboard.print("change temperature F°/C°");
    pCharacteristic1->setValue("change temperature F°/C°");
    swapunit();
    optionselected = false;
    break;
  case 3:
    Displayboard.print("turn the light on/off");
    pCharacteristic1->setValue("turn the light on/off");
    changelightstatus();
    optionselected = false;
    break;
  case 4:
    Displayboard.print("change potentiometer action: dim/set color");
    pCharacteristic1->setValue("change potentiometer action: dim/set color");
    changeaction();
    optionselected = false;
    break;
  case 5:
    optionselected = false;
    esp_deep_sleep_start();
    break;
  }
}

void potentiometer()
{
  if (potaction == "dim")
  {
    switch (color)
    {
    case 'r':
      strip.setPixelColor(0, strip.Color(analogRead(potentiometerPIN) / 16, 0, 0));
      break;
    case 'g':
      strip.setPixelColor(0, strip.Color(analogRead(potentiometerPIN) / 16, analogRead(potentiometerPIN) / 16, 0));
      break;
    case 'w':
      strip.setPixelColor(0, strip.Color(analogRead(potentiometerPIN) / 16, analogRead(potentiometerPIN) / 16, analogRead(potentiometerPIN) / 16));
      break;
    }
  }
  else
  {
    if (0 <= analogRead(potentiometerPIN) && analogRead(potentiometerPIN) <= 1300)
    {
      color = 'r';
      strip.setPixelColor(0, strip.Color(255, 0, 0));
    }
    else if (1301 <= analogRead(potentiometerPIN) && analogRead(potentiometerPIN) <= 2600)
    {
      color = 'g';
      strip.setPixelColor(0, strip.Color(255, 255, 0));
    }
    else
    {
      color = 'w';
      strip.setPixelColor(0, strip.Color(255, 255, 255));
    }
  }
}

void swapunit()
{
  if (tempunit == "C")
  {
    tempunit = "F";
    tempfactor1 = 1.8;
    tempfactor2 = 32;
  }
  else
  {
    tempunit = "C";
    tempfactor1 = 1;
    tempfactor2 = 0;
  }
}

void changelightstatus()
{
  if (lightstatus == "OFF")
  {
    lightstatus = "ON";
  }
  else
  {
    lightstatus = "OFF";
  }
}

void changeaction()
{
  if (potaction == "dim")
  {
    potaction = "set color";
  }
  else
  {
    potaction = "dim";
  }
}
