var dgram = require("dgram");

var client = dgram.createSocket("udp4");

console.log("test");

// client.send("Hello World!", 0, 12, 8888, "192.168.1.177");
// client.send("Hello World!", 0, 12, 8888, "192.168.1.177");
// client.send(
//   "Hello World!",
//   0,
//   12,
//   8888,
//   "192.168.1.177",
//   function (err, bytes) {
//     client.close();
//   }
// );

client.send("Hello World!", 0, 12, 3000, "0.0.0.0");
client.send("Hello World!", 0, 12, 3000, "0.0.0.0");
client.send("Hello Mam!", 0, 10, 3000, "0.0.0.0", function (err, bytes) {
  client.close();
});
