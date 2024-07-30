/**
 * A BLE client example to receive many characteristics.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara and MoThunderz
 *Changed again by confusedMech thanks to help from https://github.com/mo-thunderz/Esp32BleManyChars/tree/main/Arduino/BLE_many_notify_workaround_server
 */

#include "BLEDevice.h"
//#include "BLEScan.h"

// Define UUIDs:
static BLEUUID serviceUUID(BLEUUID((uint16_t)0xfff0));
static BLEUUID char_RX_UUID(BLEUUID((uint16_t)0xfff1));
static BLEUUID char_TX_UUID(BLEUUID((uint16_t)0xfff2));


// Some variables to keep track on device connected
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

// Define pointer for the BLE connection
static BLEAdvertisedDevice* myDevice;
BLERemoteCharacteristic* pRemoteChar_TX;
BLERemoteCharacteristic* pRemoteChar_RX;


volatile bool readChar_0 = false;



// Callback function for Notify function
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  if (pBLERemoteCharacteristic->getUUID().toString() == char_RX_UUID.toString()) {
    readChar_0 = true;
    //Serial.print("readChar: ");
  }
}

// Callback function that is called whenever a client is connected or disconnected
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// Function that is run whenever the server is connected
bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  Serial.println(pClient->setMTU(517));

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  connected = true;
  pRemoteChar_TX = pRemoteService->getCharacteristic(char_RX_UUID);
  pRemoteChar_RX = pRemoteService->getCharacteristic(char_TX_UUID);


  if (connectCharacteristic(pRemoteService, pRemoteChar_TX) == false) {
    connected = false;
    Serial.println("char 0");
  } else if (connectCharacteristic(pRemoteService, pRemoteChar_RX) == false) {
    connected = false;
    Serial.println("char 0");
  }

  if (connected == false) {
    pClient->disconnect();
    Serial.println("At least one characteristic UUID not found");
    return false;
  }
  return true;
}

// Function to check Characteristic
bool connectCharacteristic(BLERemoteService* pRemoteService, BLERemoteCharacteristic* l_BLERemoteChar) {
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  if (l_BLERemoteChar == nullptr) {
    Serial.print("Failed to find one of the characteristics");
    Serial.print(l_BLERemoteChar->getUUID().toString().c_str());
    return false;
  }
  // String rxValue = l_BLERemoteChar->readValue();
  //Serial.println(" - Found characteristic: " + String(l_BLERemoteChar->getUUID().toString().c_str()) + ", with value: " + String(rxValue.c_str()));
  if (l_BLERemoteChar->canNotify()) {
    Serial.println(" NOTIFY");
    l_BLERemoteChar->registerForNotify(notifyCallback);
  } else {
    Serial.println("NO NOTIFY");
  }
  return true;
}

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  //Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  // BLEDevice::setMTU(517);

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}  // End of setup.




const uint8_t notificationOff[] = { 0x41, 0x54, 0x49, 0x0D, 0x0A, 0x00 };

String rxValue, txValue;
void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
      pRemoteChar_RX->writeValue((uint8_t*)notificationOff, 5, true);
    } else {
      Serial.println("ABORT");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    Serial.print("Found TX characteristic with value: ");
    Serial.println(pRemoteChar_TX->readValue());
    delay(5000);

  } else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
}
