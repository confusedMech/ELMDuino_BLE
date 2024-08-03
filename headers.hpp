#pragma once
#include "Arduino.h"
#include "ELMduino.h"

/**
 * A BLE client example to receive many characteristics.
 * There is a lot new capabilities implemented.
 * author unknown
 *  Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
 * updated by chegewara and MoThunderz
 *Changed again by confusedMech thanks to help from https://github.com/mo-thunderz/Esp32BleManyChars/tree/main/Arduino/BLE_many_notify_workaround_server
 */

#include "BLEDevice.h"
// #include "BLEScan.h"

// Define UUIDs:
static BLEUUID serviceUUID(BLEUUID((uint16_t)0xfff0));
static BLEUUID char_RX_UUID(BLEUUID((uint16_t)0xfff1));
static BLEUUID char_TX_UUID(BLEUUID((uint16_t)0xfff2));

// Some variables to keep track on device connected
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

// Define pointer for the BLE connection
static BLEAdvertisedDevice *myDevice;
static BLERemoteCharacteristic *pRemoteChar_TX;
static BLERemoteCharacteristic *pRemoteChar_RX;

static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify);
bool connectToServer();
bool connectCharacteristic(BLERemoteService *pRemoteService, BLERemoteCharacteristic *l_BLERemoteChar);
void sendStartCommand(const uint8_t(*cmd), int cmdSize);

// void sendCommand(const uint8_t cmd);

// Callback function that is called whenever a client is connected or disconnected
class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
  }
  void onDisconnect(BLEClient *pclient)
  {
    connected = false;
    Serial.println("onDisconnect");
  }
};

// Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  // Called for each advertising BLE server.
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

// ELM327 myELM327;

#define ELM_PORT elmWork
static volatile bool readChar_0 = false;
const uint8_t delimit[] = {0x0D};
const uint8_t modInfo[] = {'A', 'T', 'R', 'V'};
const uint8_t engineloads1[] = {'1'};
const uint8_t engineloads2[] = {'1','2'};
const uint8_t engineloads3[] = {'1'};
static String rxValue, txValue;
