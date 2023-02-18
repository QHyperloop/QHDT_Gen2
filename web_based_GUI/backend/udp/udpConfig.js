const dgram = require("dgram");

module.exports = function (db) {
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

  socket.on("message", async (message, r) => {
    const messageData = message.toString();
    console.log(`UDP message: ${messageData}`);

    const sensorType = messageData.split(":")[0];
    const sensorValue = messageData.split(":")[1];

    db.findOne({ _id: "sensorData" }, async function (err, docs) {
      for (var key in docs) {
        if (key === sensorType) {
          docs[key] = sensorValue;
        }
      }
      db.update(
        { _id: "sensorData" },
        docs,
        {},
        function (err, docReplaced) {}
      );
      console.log("Updated database with", docs);
    });
  });
};
