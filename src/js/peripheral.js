var events = require('events');
var util = require('util');

function Peripheral(bleCentral, id, address, addressType, connectable, 
                    advertisement, rssi) {
  this._bleCentral = bleCentral;

  this.id = id;
  this.uuid = id;
  this.address = address;
  this.addressType = addressType;
  this.connectable = connectable;
  this.advertisement = advertisement;
  this.rssi = rssi;
  this.services = null;
  this.state = 'disconnected';
}

util.inherits(Peripheral, events.EventEmitter);

Peripheral.prototype.connect = function(callback) {
  if (callback) {
    this.once('connect', function(error) {
      callback(error);
    });
  }

  if (this.state === 'connected') {
    this.emit('connect', new Error('Peripheral already connected'));
  } else {
    this.state = 'connecting';
    this._bleCentral.connect(this.id);
  }
};

Peripheral.prototype.disconnect = function(callback) {
  if (callback) {
    this.once('disconnect', function() {
      callback(null);
    });
  }

  this.state = 'disconnecting';
  this._bleCentral.disconnect(this.id);
};

Peripheral.prototype.updateRssi = function(callback) {
  if (callback) {
    this.once('rssiUpdate', function(rssi) {
      callback(null, rssi);
    });
  }

  this._bleCentral.updateRssi(this.id);
};

Peripheral.prototype.discoverServices = function(uuids, callback) {
  if (callback) {
    this.once('servicesDiscover', function(services) {
      callback(null, services);
    });
  }

  this._bleCentral.discoverServices(this.id, uuids);
};

/*
Peripheral.prototype.discoverServicesAndCharacteristics = 
function(serviceUuids, characteristicsUuids, callback) {
  this.discoverServices(serviceUuids, function(err, services) {
    var numDiscovered = 0;
    var allCharacteristics = [];

    for (var i in services) {
      var service = services[i];

      service.discoverCharacteristics(characteristicsUuids, 
                                      function(error, characteristics) {
        numDiscovered++;
        
        if (error === null) {
          for (var j in characteristics) {
            var characteristic = characteristics[j];

            allCharacteristics.push(characteristic);
          }
        }

        if (numDiscovered === services.length) {
          if (callback) {
            callback(null, services, allCharacteristics);
          }
        }
                                      }.bind(this));
    }
  }.bind(this));
};
*/

Peripheral.prototype.discoverAllservicesAndCharacteristics = 
function(callback) {
  this.discoverSomeServicesAndCharacteristics([], [], callback);
};

Peripheral.prototype.readHandle = function(handle, callback) {
  if (callback) {
//    this.once('handleRead' + handle, function(data) {
      callback(null, data);
    });
  }

  this._bleCentral.readHandle(this.id, handle);
};

Peripheral.prototype.writeHandle = function(handle, data, 
                                            withoutResponse, callback) {
  if(!(data instanceof Buffer)) {
    throw new Error('data must be a Buffer');
  }

  if (callback) {
//    this.once('handleWrite' + handle, function() {
      callback(null);
    });
  }

  this._bleCentral.writeHandle(this.id, handle, data, withoutResponse);
};

module.exports = Peripheral;
