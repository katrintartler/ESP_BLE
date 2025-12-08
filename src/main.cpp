#include <Arduino.h>
#include <NimBLEDevice.h>
#include <Stepper.h> 
 
const int stepsPerRevolution = 2048; 
const int IN1 = 14; 
const int IN2 = 27; 
const int IN3 = 26; 
const int IN4 = 25; 
Stepper myStepper(stepsPerRevolution, IN1, IN2, IN3, IN4); 

bool rotate_flag = false; 

class CommandCallback : 
  public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& coninfo) override {
      std::string cmd = c->getValue();
      //Serial.print("Received BLE command: ");
      //Serial.println(cmd.c_str());

    if (cmd == "rotate_cw") {
      rotate_flag = true; 
    }
  }
}; 

void setup() {
  Serial.begin(115200);
  myStepper.setSpeed(10); 

  // BLE init 
  NimBLEDevice::init("Bby"); 
  Serial.print("Name vergeben"); 
  
  NimBLEAddress addr = NimBLEDevice::getAddress();
  Serial.print("BLE MAC:");
  Serial.print(addr.toString().c_str()); 

  NimBLEServer * server = NimBLEDevice::createServer();
  NimBLEService * service = server->createService("12345678-1234-1234-1234-1234567890ab"); 

   NimBLECharacteristic* characteristic =
      service->createCharacteristic(
          "abcd1234-ab12-34cd-56ef-1234567890ab",
          NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
      );

  characteristic->setCallbacks(new CommandCallback());
  characteristic->setValue("ready");

  service->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(service->getUUID());
  adv->start();

  Serial.println("advertising");
}
void loop() {
  if (rotate_flag) {
        rotate_flag = false;
        myStepper.step(stepsPerRevolution);
    }
}

