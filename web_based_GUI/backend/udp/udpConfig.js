const dgram = require("dgram");

module.exports = function (db) {
  const socket = dgram.createSocket("udp4");

  socket.bind({
    port: process.env.INBOUND_UDP_PORT || 8888,
    address: process.env.INBOUND_UDP_IP || "192.168.1.60",
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

    // Sample message data: 3f 02 9c 78 49 1b 02

    const hexArray = messageData.split(" ");

    const batteryTemp = hexArray[0];
    const motorTemp = hexArray[1];
    const podTemp = hexArray[2];
    const motorVoltage = hexArray[3];
    const electronicsVoltage = hexArray[4];
    const tankPressure = hexArray[5];
    const vesselPressure = hexArray[6];

    db.findOne({ _id: "sensorData" }, async function (err, docs) {
      // for (var key in docs) {
      //   if (key === sensorType) {
      //     docs[key] = sensorValue;
      //   }
      // }
      docs["batteryTemp"] = batteryTemp;
      docs["motorTemp"] = motorTemp;
      docs["podTemp"] = podTemp;
      docs["motorVoltage"] = motorVoltage;
      docs["electronicsVoltage"] = electronicsVoltage;
      docs["tankPressure"] = tankPressure;
      docs["vesselPressure"] = vesselPressure;

      db.update(
        { _id: "sensorData" },
        docs,
        {},
        function (err, docReplaced) {}
      );
    });
  });
};
