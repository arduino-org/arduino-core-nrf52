// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
// Modified by Arduino.org development team

#ifndef _BLE_PERIPHERAL_H_
#define _BLE_PERIPHERAL_H_

#include "Arduino.h"

#include "BLEBondStore.h"
#include "BLECentral.h"
#include "BLEConstantCharacteristic.h"
#include "BLEDescriptor.h"
#include "BLEDevice.h"
#include "BLEFixedLengthCharacteristic.h"
#include "BLELocalAttribute.h"
#include "BLEProgmemConstantCharacteristic.h"
#include "BLERemoteAttribute.h"
#include "BLERemoteCharacteristic.h"
#include "BLERemoteService.h"
#include "BLEService.h"
#include "BLETypedCharacteristics.h"

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  #include "nRF51822.h"
#else
  #include "nRF8001.h"
#endif

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
  #define BLE_DEFAULT_REQ   -1
  #define BLE_DEFAULT_RDY   -1
  #define BLE_DEFAULT_RST   -1
#elif defined(BLEND_MICRO)
  #define BLE_DEFAULT_REQ   6
  #define BLE_DEFAULT_RDY   7
  #define BLE_DEFAULT_RST   4
#else
  #define BLE_DEFAULT_REQ   10
  #define BLE_DEFAULT_RDY   2
  #define BLE_DEFAULT_RST   9
#endif

enum BLEPeripheralEvent {
  BLEConnected = 0,
  BLEDisconnected = 1,
  BLEBonded = 2,
  BLERemoteServicesDiscovered = 3,
  BLEPasskeyReceived = 4,
  BLEPasskeyRequested = 5,
  BLEScanReceived = 6
};

enum BLEBondingType {
  JUST_WORKS = 0,
  DISPLAY_PASSKEY = 1,
  CONFIRM_PASSKEY = 2,
  LESC = 3,
  LESC_NUM_COMPARISON = 4,
  LESC_DISPLAY_PASSKEY = 5,
  LESC_CONFIRM_PASSKEY = 6
};

typedef void (*BLEPeripheralEventHandler)(BLECentral& central);


class BLEPeripheral : public BLEDeviceEventListener,
                        public BLECharacteristicValueChangeListener,
                        public BLERemoteCharacteristicValueChangeListener
{
  public:
    BLEPeripheral(unsigned char req = BLE_DEFAULT_REQ, unsigned char rdy = BLE_DEFAULT_RDY, unsigned char rst = BLE_DEFAULT_RST);
    virtual ~BLEPeripheral();

    void begin();
    void poll(ble_evt_t *bleEvent = 0);
    void end();

    void setAdvertisedServiceUuid(const char* advertisedServiceUuid);
    void setServiceSolicitationUuid(const char* serviceSolicitationUuid);
    void setManufacturerData(const unsigned char manufacturerData[], unsigned char manufacturerDataLength);
    void setLocalName(const char *localName);

    void setAdvertisingInterval(unsigned short advertisingInterval);
    // connection intervals in 1.25 ms increments,
    // must be between  0x0006 (7.5 ms) and 0x0c80 (4 s), values outside of this range will be ignored
    void setConnectionInterval(unsigned short minimumConnectionInterval, unsigned short maximumConnectionInterval);
    bool setTxPower(int txPower);
    void setConnectable(bool connectable);
    void setBondStore(BLEBondStore& bondStore);
    void enableBond(BLEBondingType type = JUST_WORKS);
    void clearBondStoreData();
    void saveBondData();
	char *getPasskey();
    void sendPasskey(char passkey[]);

    void setDeviceName(const char* deviceName);
    void setAppearance(unsigned short appearance);

    void addAttribute(BLELocalAttribute& attribute);
    void addLocalAttribute(BLELocalAttribute& localAttribute);
    void addRemoteAttribute(BLERemoteAttribute& remoteAttribute);

    void disconnect();

    BLECentral central();
    bool connected();

    void setEventHandler(BLEPeripheralEvent event, BLEPeripheralEventHandler eventHandler);

  protected:
    bool characteristicValueChanged(BLECharacteristic& characteristic);
    bool broadcastCharacteristic(BLECharacteristic& characteristic);
    bool canNotifyCharacteristic(BLECharacteristic& characteristic);
    bool canIndicateCharacteristic(BLECharacteristic& characteristic);

    bool canReadRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool readRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canWriteRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool writeRemoteCharacteristic(BLERemoteCharacteristic& characteristic, const unsigned char value[], unsigned char length);
    bool canSubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool subscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool canUnsubscribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);
    bool unsubcribeRemoteCharacteristic(BLERemoteCharacteristic& characteristic);

    virtual void BLEDeviceConnected(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceDisconnected(BLEDevice& device);
    virtual void BLEDeviceBonded(BLEDevice& device);
    virtual void BLEDeviceRemoteServicesDiscovered(BLEDevice& device);
    virtual void BLEDevicePasskeyReceived(BLEDevice& device);
    virtual void BLEDevicePasskeyRequested(BLEDevice& device);

    virtual void BLEDeviceCharacteristicValueChanged(BLEDevice& device, BLECharacteristic& characteristic, const unsigned char* value, unsigned char valueLength);
    virtual void BLEDeviceCharacteristicSubscribedChanged(BLEDevice& device, BLECharacteristic& characteristic, bool subscribed);

    virtual void BLEDeviceRemoteCharacteristicValueChanged(BLEDevice& device, BLERemoteCharacteristic& remoteCharacteristic, const unsigned char* value, unsigned char valueLength);

    virtual void BLEDeviceAddressReceived(BLEDevice& device, const unsigned char* address);
    virtual void BLEDeviceTemperatureReceived(BLEDevice& device, float temperature);
    virtual void BLEDeviceBatteryLevelReceived(BLEDevice& device, float batteryLevel);

  protected:
    BLEBondStore                   _bleBondStore;

  private:
    void initLocalAttributes();
    void addFieldInPck(uint8_t type, uint8_t len, unsigned char* data);

  private:
    BLEDevice*                     _device;

#if defined(NRF51) || defined(NRF52) || defined(__RFduino__)
    nRF51822                       _nRF51822;
#else
    nRF8001                        _nRF8001;
#endif

    unsigned char                  _advData[32];
    uint8_t                        _advDataLength=0;
    unsigned char                  _scanData[32];
    uint8_t                        _scanDataLength=0;

    BLELocalAttribute**            _localAttributes;
    unsigned char                  _numLocalAttributes;
    BLERemoteAttribute**           _remoteAttributes;
    unsigned char                  _numRemoteAttributes;

    BLEService                     _genericAccessService;
    BLECharacteristic              _deviceNameCharacteristic;
    BLECharacteristic              _appearanceCharacteristic;
    BLEService                     _genericAttributeService;
    BLECharacteristic              _servicesChangedCharacteristic;

    BLERemoteService               _remoteGenericAttributeService;
    BLERemoteCharacteristic        _remoteServicesChangedCharacteristic;

    BLECentral                     _central;
    BLEPeripheralEventHandler      _eventHandlers[6];
};

#endif