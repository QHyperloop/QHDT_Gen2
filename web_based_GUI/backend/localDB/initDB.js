module.exports = function (db) {
  db.remove({}, { multi: true }, function (err, numRemoved) {
    db.loadDatabase(function (err) {});
  });

  let data =
    "speed.batteryTemp.motorTemp.batteryVoltage.airSystemTemp.tankPressure.caliper1Pressure.caliper2Pressure.distanceTravelled";

  var doc = {
    _id: "sensorData",
    speed: 0,
    batteryTemp: 0,
    motorTemp: 0,
    batteryVoltage: 0,
    airSystemTemp: 0,
    tankPressure: 0,
    caliper1Pressure: 0,
    caliper2Pressure: 0,
    distanceTravelled: 0,
  };

  db.insert(doc, function (err, newDoc) {
    console.log("Initalized Database with", newDoc);
  });
};
