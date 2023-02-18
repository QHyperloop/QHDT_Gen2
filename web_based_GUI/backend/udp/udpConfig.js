const dgram = require("dgram");

const socket = dgram.createSocket("udp4");

socket.bind({
  port: process.env.UDP_PORT || 3000,
  address: process.env.UDP_IP || "192.168.0.20",
});

socket.on("listening", () => {
  const addr = socket.address();
  console.log(`Listening for UDP packets at ${addr.address}:${addr.port}`);
});

socket.on("error", (err) => {
  console.error(`UDP error: ${err.stack}`);
});

socket.on("message", (message, r) => {
  const messageData = message.toString();
  console.log(`UDP message: ${messageData}`);
});
