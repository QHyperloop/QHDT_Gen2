// This file is not used in the project, it is just for testing purposes
// You can run this file with "node testUDP.js" in the terminal

var dgram = require("dgram");
var client = dgram.createSocket("udp4");

const message = "114.294.299.54.298.0.0.0.300";
// ("speed.batteryTemp.motorTemp.batteryVoltage.airSystemTemp.tankPressure.caliper1Pressure.caliper2Pressure.distanceTravelled");

console.log(`Sending UDP message: "${message}"`);

client.send(message, 0, message.length, 8888, "0.0.0.0", function (err, bytes) {
  client.close();
});
