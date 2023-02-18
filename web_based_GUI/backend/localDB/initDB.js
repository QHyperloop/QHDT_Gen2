module.exports = function (db) {
  db.remove({}, { multi: true }, function (err, numRemoved) {
    db.loadDatabase(function (err) {});
  });

  var doc = {
    _id: "sensorData",
    batteryTemp: 0,
    motorTemp: 0,
    podTemp: 0,
    motorVoltage: 0,
    electronicsVoltage: 0,
    tankPressure: 0,
    vesselPressure: 0,
  };

  db.insert(doc, function (err, newDoc) {
    console.log("Initalized Database with", newDoc);
  });
};
