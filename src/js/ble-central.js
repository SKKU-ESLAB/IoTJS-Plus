/* Copyiright 2016 Kim, Hyukjoong <wangmir@gmail.com>
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

var EventEmitter = requre('events').EventEmitter;
var util = require ('util');
var assert = require('assert');

var bleCentralBuiltin = process.binding(process.binding.blecentral);

function bleCentral() {
  this.state = 'unknown';
  this.address = 'unknown';

  this._peripherals = {};
  this._services = {};
  this._characteristics = {};
  this._descriptors = {};
  this._discoveredPeripheralUUids = [];

  EventEmitter.call(this);

  this.on('warning', function(message) {
    if(this.listeners('warning').length === 1) {
      console.warn('bleCentral: ' + message);
    }
  }.bind(this));

  bleCentralBuiltin.listen(function(event, args){
    switch (event) {
      case 'stateChange' : 
        this.onstateChange(args[0]).bind(this);
        break;
      case 'addressChange' :
        this.onAddressChange(args[0]).bind(this);
        break;
      case 'scanStart' :
        this.onScanStart(args[0]).bind(this);
        break;
      case 'scanStop' :
        this.onScanStop().bind(this);
        break;
      case 'discover' : 
        this.onDiscover(args[0], args[1], args[2], 
                        args[3], args[4], args[5]).bind(this);
        break;
      case 'connect' :
        this.onConnect(args[0], args[1]).bind(this);
        break;
      case 'disconnect' : 
        this.onDisconnect(args[0]).bind(this);
        break;
      case 'rssiUpdate' : 
        this.onRssiUpdate(args[0], args[1]).bind(this);
        break;
      case 'servicesDiscover' :
        this.onServicesDiscover(args[0], args[1]).bind(this);
        break;
      case 'includedServicesDiscover' :
        this.onIncludedServicesDiscover(args[0], args[1], args[2]).bind(this);
        break;
      case 'characteristicsDiscover' :
        this.onCharacteristicsDiscover(args[0], args[1], args[2]).bind(this);
        break;
      case 'read' :
        this.onRead(args[0], args[1], args[2],
                    args[3], args[4]).bind(this);
        break;
      case 'write' :
        this.onWrite(args[0], args[1], args[2]).bind(this);
        break;
      case 'broadcast' :
        this.onBroadcast(args[0], args[1], args[2], args[3]).bind(this);
        break;
      case 'notify' :
        this.onNotify(args[0], args[1], args[2], args[3]).bind(this);
        break;
      case 'descriptorsDiscover' :
        this.onDescriptorsDiscover(args[0], args[1], 
                                   args[2], args[3]).bind(this);
        break;
      case 'valueRead' :
        this.onValueRead(args[0], args[1], args[2], 
                         args[3], args[4]).bind(this);
        break;
      case 'valueWrite' :
        this.onValueWrite(args[0], args[1], 
                          args[2], args[3]).bind(this);
        break;
      case 'handleRead' :
        this.onHandleRead(args[0], args[1], args[2]).bind(this);
        break;
      case 'handleWrite' :
        this.onHandleWrite(args[0], args[1]).bind(this);
        break;
      case 'handleNotify' :
        this.onHandleNotify(args[0], args[1], args[2]).bind(this);
        break;
    }

  }.bind(this));
}

util.inherits(bleCentral, EventEmitter);

bleCentral.prototype.onStateChange = function(state) {
  
  this.state = state;

  this.emit('stateChange', state);
};

bleCentral.prototype.onAddressChange = function(address) {

  this.address = address;
};

bleCentral.prototype.startScanning = function(serviceUuids, 
                                              allowDuplicates, callback) {
  if(this.stae !== 'poweredOn') {
    var error = new Error('Could not start scanning, state is ' 
                          + this.state + ' (not poweredOn)');

    if(typeof callback === 'function') {
      callback(error);
    } else {
      throw error;
    }
  } else {
    if (callback) {
      this.once('scanStart', callback);
    }

    this._discoveredPeripheralUUids = [];
    this._allowDuplicates = allowDuplicates;

    bleCentralBuiltin.startScanning(serviceUuids, allowDuplicates);
  }
};

bleCentral.prototype.onScanStart = function(filterDuplicates) {
  this.emit('scanStart', filterDuplicates);
};

bleCentral.prototype.stopScanning = function(callback) {
  if(callback){
    this.once('scanStop', callbank);
  }
  bleCentralBuiltin.stopScanning();
};

bleCentral.prototype.onScanStop = function() {
  this.emit('scanStop');
};

bleCentral.prototype.onDiscover = function(
    uuid, address, addressType, connectable, advertisement, rssi) {
  var peripheral = this._peripherals[uuid];

  if(!peripheral) {
    peripheral = new Peripheral(this, uuid, address, addressType, 
                                connectable, advertisement, rssi);

    this._peripherals[uuid] = peripheral;
    this._service[uuid] = {};
    this._characteristics[uuid] = {};
    this._descriptors[uuid] = {};
  } else{
    for(var i in advertisement) {
      if(advertisement[i] !== undefined) {
        peripheral.advertisement[i] = advertisement[i];
      }
    }

    peripheral.rssi = rssi;
  }

  var previouslyDiscoverd = 
      (this._discoveredPeripheralUUids.indexOf(uuid) !== -1);

  if(!previouslyDiscoverd) {
    this._discoveredPeripheralUUids.push(uuid);
  }

  if(this._allowDuplicates || !previouslyDiscoverd) {
    this.emit('discover', peripheral);
  }
};


bleCentral.prototype.connect = function(peripheralUuid) {
  bleCentralBuiltin.connect(peripheralUuid);
};

bleCentral.prototype.onConnect = function(peripheralUuid, error) {
  var peripheral = this._peripherals[peripheralUuid];
  
  if (peripheral) {
    peripheral.state = error ? 'error' : 'connected';
    peripheral.emit('connect', error);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' connected!');
  }
};

bleCentral.prototype.disconnect = function(peripheralUuid) {
  bleCentralBuiltin.disconnect(peripheralUuid);
};

bleCentral.prototype.onDisconnect = function(peripheralUuid) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    peripheral.state = 'disconnected';
    peripheral.emit('disconnect');
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' disconnected!');
  }
};

bleCentral.prototype.updateRssi = function(peripheralUuid) {
  bleCentralBuiltin.updateRssi(peripheralUuid);
};

bleCentral.prototype.onRssiUpdate = function(peripheralUuid, rssi) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    peripheral.rssi = rssi;
    peripheral.emit('rssiUpdate', rssi);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' RSSI update!');
  }
};

bleCentral.prototype.discoverServices = function(peripheralUuid, uuids) {
    bleCentralBuiltin.discoverServices(peripheralUuid, uuids);
};

bleCentral.prototype.onServicesDiscover = function(peripheralUuid, 
                                                   serviceUuids) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    var services = [];

    for (var i = 0; i < serviceUuids.length; i++) {
      var serviceUuid = serviceUuids[i];
      var service = new Service(this, peripheralUuid, serviceUuid);

      this._services[peripheralUuid][serviceUuid] = service;
      this._characteristics[peripheralUuid][serviceUuid] = {};
      this._descriptors[peripheralUuid][serviceUuid] = {};
      services.push(service);
    }

    peripheral.services = services;

    peripheral.emit('servicesDiscover', services);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' services discover!');
  }
};

bleCentral.prototype.discoverIncludedServices = function(
    peripheralUuid, serviceUuid, serviceUuids) {
    bleCentralBuiltin.discoverIncludedServices(peripheralUuid, 
                                             serviceUuid, serviceUuids);
};

bleCentral.prototype.onIncludedServicesDiscover = function(
    peripheralUuid, serviceUuid, includedServiceUuids) {
  var service = this._services[peripheralUuid][serviceUuid];

  if (service) {
    service.includedServiceUuids = includedServiceUuids;

    service.emit('includedServicesDiscover', includedServiceUuids);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid 
              + ', ' + serviceUuid + ' included services discover!');
  }
};

bleCentral.prototype.discoverCharacteristics = function(
    peripheralUuid, serviceUuid, characteristicUuids) {
    bleCentralBuiltin.discoverCharacteristics(
        peripheralUuid, serviceUuid, characteristicUuids);
};

bleCentral.prototype.onCharacteristicsDiscover = function(
    peripheralUuid, serviceUuid, characteristics) {
  var service = this._services[peripheralUuid][serviceUuid];

  if (service) {
    var characteristics_ = [];

    for (var i = 0; i < characteristics.length; i++) {
      var characteristicUuid = characteristics[i].uuid;

      var characteristic = new Characteristic(
          this,
          peripheralUuid,
          serviceUuid,
          characteristicUuid,
          characteristics[i].properties
          );

      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid] 
          = characteristic;
      this._descriptors[peripheralUuid][serviceUuid][characteristicUuid] = {};

      characteristics_.push(characteristic);
    }

    service.characteristics = characteristics_;

    service.emit('characteristicsDiscover', characteristics_);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid 
              + ', ' + serviceUuid + ' characteristics discover!');
  }
};

bleCentral.prototype.read = function(peripheralUuid, 
                                     serviceUuid, characteristicUuid) {
  bleCentralBuiltin.read(peripheralUuid, serviceUuid, characteristicUuid);
};

bleCentral.prototype.onRead = function(
    peripheralUuid, serviceUuid, characteristicUuid, data, isNotification) {
  var characteristic = 
      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid];
  if (characteristic) {
    characteristic.emit('data', data, isNotification);
    characteristic.emit('read', data, isNotification); 
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ' read!');
  }
};

bleCentral.prototype.write = function(
    peripheralUuid, serviceUuid, characteristicUuid, data, withoutResponse) {
  bleCentralBuiltin.write(peripheralUuid, serviceUuid, 
                          characteristicUuid, data, withoutResponse);
};

bleCentral.prototype.onWrite = function(peripheralUuid, 
                                        serviceUuid, characteristicUuid) {
  var characteristic = 
      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid];

  if (characteristic) {
    characteristic.emit('write');
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ' write!');
  }
};

bleCentral.prototype.broadcast = function(
    peripheralUuid, serviceUuid, characteristicUuid, broadcast) {
  bleCentralBuiltin.broadcast(peripheralUuid, 
                              serviceUuid, characteristicUuid, broadcast);
};

bleCentral.prototype.onBroadcast = function(
    peripheralUuid, serviceUuid, characteristicUuid, state) {
  var characteristic = 
      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid];

  if (characteristic) {
    characteristic.emit('broadcast', state);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ' broadcast!');
  }
};

bleCentral.prototype.notify = function(
    peripheralUuid, serviceUuid, characteristicUuid, notify) {
  bleCentralBuiltin.notify(peripheralUuid, 
                           serviceUuid, characteristicUuid, notify);
};

bleCentral.prototype.onNotify = function(
    peripheralUuid, serviceUuid, characteristicUuid, state) {
  var characteristic = 
      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid];

  if (characteristic) {
    characteristic.emit('notify', state);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ' notify!');
  }
};

bleCentral.prototype.discoverDescriptors = function(
    peripheralUuid, serviceUuid, characteristicUuid) {
  bleCentralBuiltin.discoverDescriptors(peripheralUuid, 
                                        serviceUuid, characteristicUuid);
};

bleCentral.prototype.onDescriptorsDiscover = function(
    peripheralUuid, serviceUuid, characteristicUuid, descriptors) {
  var characteristic = 
      this._characteristics[peripheralUuid][serviceUuid][characteristicUuid];

  if (characteristic) {
    var descriptors_ = [];

    for (var i = 0; i < descriptors.length; i++) {
      var descriptorUuid = descriptors[i];

      var descriptor = new Descriptor(
          this,
          peripheralUuid,
          serviceUuid,
          characteristicUuid,
          descriptorUuid
          );

      this._descriptors[peripheralUuid]
          [serviceUuid][characteristicUuid][descriptorUuid] = descriptor;

      descriptors_.push(descriptor);
    }

    characteristic.descriptors = descriptors_;

    characteristic.emit('descriptorsDiscover', descriptors_);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid 
              + ' descriptors discover!');
  }
};

bleCentral.prototype.readValue = function(
    peripheralUuid, serviceUuid, characteristicUuid, descriptorUuid) {
  bleCentralBuiltin.readValue(peripheralUuid, 
                              serviceUuid, characteristicUuid, descriptorUuid);
};

bleCentral.prototype.onValueRead = function(
    peripheralUuid, serviceUuid, characteristicUuid, descriptorUuid, data) {
  var descriptor = this._descriptors[peripheralUuid][
      serviceUuid][characteristicUuid][descriptorUuid];

  if (descriptor) {
    descriptor.emit('valueRead', data);
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ', ' 
              + descriptorUuid + ' value read!');
  }
};

bleCentral.prototype.writeValue = function(
    peripheralUuid, serviceUuid, characteristicUuid, descriptorUuid, data) {
  bleCentralBuiltin.writeValue(
      peripheralUuid, serviceUuid, characteristicUuid, descriptorUuid, data);
};

bleCentral.prototype.onValueWrite = function(
    peripheralUuid, serviceUuid, characteristicUuid, descriptorUuid) {
  var descriptor = this._descriptors[peripheralUuid]
      [serviceUuid][characteristicUuid][descriptorUuid];

  if (descriptor) {
    descriptor.emit('valueWrite');
  } else {
    this.emit('warning', 'unknown peripheral ' + peripheralUuid + ', ' 
              + serviceUuid + ', ' + characteristicUuid + ', ' 
              + descriptorUuid + ' value write!');
  }
};

bleCentral.prototype.readHandle = function(peripheralUuid, handle) {
  bleCentralBuiltin.readHandle(peripheralUuid, handle);
};

bleCentral.prototype.onHandleRead = function(peripheralUuid, handle, data) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    peripheral.emit('handleRead' + handle, data);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' handle read!');
  }
};

bleCentral.prototype.writeHandle = function(peripheralUuid, 
                                            handle, data, withoutResponse) {
  bleCentralBuiltin.writeHandle(peripheralUuid, handle, data, withoutResponse);
};

bleCentral.prototype.onHandleWrite = function(peripheralUuid, handle) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    peripheral.emit('handleWrite' + handle);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' handle write!');
  }
};

bleCentral.prototype.onHandleNotify = function(peripheralUuid, handle, data) {
  var peripheral = this._peripherals[peripheralUuid];

  if (peripheral) {
    peripheral.emit('handleNotify', handle, data);
  } else {
    this.emit('warning', 'unknown peripheral ' 
              + peripheralUuid + ' handle notify!');
  }
};

module.exports = bleCentral;

