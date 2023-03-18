// This file is not used in the project, it is just for testing purposes
// You can run this file with "node testUDP.js" in the terminal

var dgram = require("dgram");
var client = dgram.createSocket("udp4");

const message = "batteryTemp:60";
const message1 = "motorTemp:80";
const message2 = "podTemp:70";

console.log(`Sending UDP message: "${message}"`);

client.send(message, 0, message.length, 8888, "0.0.0.0", function (err, bytes) {
  client.close();
});
