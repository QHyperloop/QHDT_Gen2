// This file is not used in the project, it is just for testing purposes
// You can run this file with "node testUDP.js" in the terminal

var dgram = require("dgram");
var client = dgram.createSocket("udp4");

const message = "podTemp:60";

console.log(`Sending UDP message: "${message}"`);

client.send(message, 0, message.length, 6000, "0.0.0.0", function (err, bytes) {
  client.close();
});
