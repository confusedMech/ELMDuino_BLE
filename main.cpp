#include <Arduino.h>
#include "headers.hpp"

// Callback function for Notify function
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
  if (pBLERemoteCharacteristic->getUUID().toString() == char_RX_UUID.toString())
  {
    readChar_0 = true;
    // Serial.print("readChar: ");
  }
}

// Function that is run whenever the server is connected
bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  Serial.println(pClient->setMTU(517));

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our Device: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  connected = true;
  pRemoteChar_TX = pRemoteService->getCharacteristic(char_RX_UUID);
  pRemoteChar_RX = pRemoteService->getCharacteristic(char_TX_UUID);

  if (connectCharacteristic(pRemoteService, pRemoteChar_TX) == false)
  {
    connected = false;
    Serial.println("char 0");
  }
  else if (connectCharacteristic(pRemoteService, pRemoteChar_RX) == false)
  {
    connected = false;
    Serial.println("char 0");
  }

  if (connected == false)
  {
    pClient->disconnect();
    Serial.println("At least one characteristic UUID not found");
    return false;
  }
  return true;
}

// Function to check Characteristic
bool connectCharacteristic(BLERemoteService *pRemoteService, BLERemoteCharacteristic *l_BLERemoteChar)
{
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  if (l_BLERemoteChar == nullptr)
  {
    Serial.print("Failed to find one of the characteristics");
    Serial.print(l_BLERemoteChar->getUUID().toString().c_str());
    // return false;
  }
  // String rxValue = l_BLERemoteChar->readValue();
  // Serial.println(" - Found characteristic: " + String(l_BLERemoteChar->getUUID().toString().c_str()) + ", with value: " + String(rxValue.c_str()));
  if (l_BLERemoteChar->canNotify())
  {
    Serial.println(" NOTIFY");
    l_BLERemoteChar->registerForNotify(notifyCallback);
  }
  else
  {
    Serial.println("NO NOTIFY");
  }
  return true;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  // BLEDevice::setMTU(517);

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void loop()
{

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true)
  {
    if (connectToServer())
    {
      Serial.println("We are now connected to the BLE Server.");
      // initalize BLE server

      sendStartCommand(SET_ALL_TO_DEFAULTS, sizeof(SET_ALL_TO_DEFAULTS)); // ATD
      delay(100);

      sendStartCommand(RESET_ALL, sizeof(RESET_ALL)); // ATZ
      delay(100);

      // sendStartCommand(ECHO_OFF, sizeof(ECHO_OFF)); // ATE0
      delay(100);

      //  sendStartCommand(PRINTING_SPACES_OFF, sizeof(PRINTING_SPACES_OFF)); //ATS0
      delay(100);

      sendStartCommand(ALLOW_LONG_MESSAGES, sizeof(ALLOW_LONG_MESSAGES)); // ATAL
      delay(100);
    }
    else
    {
      Serial.println("ABORT");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected)
  {
     delay(100);
    pRemoteChar_RX->writeValue((uint8_t *)modInfo, sizeof(modInfo), true);
    delay(100);
    pRemoteChar_RX->writeValue((uint8_t *)delimit, sizeof(delimit), true);

    delay(3000);
    txValue = pRemoteChar_TX->readValue().c_str();
    Serial.print(" \tFound with value:\t ");
    Serial.println(txValue);
    delay(1000);

    pRemoteChar_RX->writeValue((uint8_t *)engineloads1, sizeof(engineloads1), true);
    delay(100);
        pRemoteChar_RX->writeValue((uint8_t *)engineloads2, sizeof(engineloads2), true);
    delay(100);
        pRemoteChar_RX->writeValue((uint8_t *)engineloads3, sizeof(engineloads3), true);
    delay(100);
    pRemoteChar_RX->writeValue((uint8_t *)delimit, sizeof(delimit), true);

    delay(3000);
    txValue = pRemoteChar_TX->readValue().c_str();
    delay(100);
    Serial.print(" \tFound RPM value:\t ");
    Serial.println(txValue);
    delay(1000);

  }
  else if (doScan)
  {
    BLEDevice::getScan()->start(0); // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
}

void sendStartCommand(const uint8_t(*cmd), int cmdSize)
{

  // pRemoteChar_RX->writeValue((uint8_t *)delimit, sizeof(delimit), true);
  pRemoteChar_RX->writeValue((uint8_t *)cmd, cmdSize, true);
  delay(100);
  pRemoteChar_RX->writeValue((uint8_t *)delimit, sizeof(delimit), true);

  Serial.println("Checking if readable");
  if (pRemoteChar_TX->canRead())
  {
    txValue = pRemoteChar_TX->readValue().c_str();
    Serial.println("is readable");
    Serial.println(" Found TX characteristic with value: " + txValue);
  }

  delay(1000);
}
