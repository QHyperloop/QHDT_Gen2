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
    // const messageData = "speed.batteryTemp.motorTemp.batteryVoltage.airSystemTemp.tankPressure.caliper1Pressure.caliper2Pressure.distanceTravelled";

    console.log(`UDP message: ${messageData}`);

    const messsageArray = messageData.split(".");

    db.findOne({ _id: "sensorData" }, async function (err, docs) {
      docs["speed"] = messsageArray[0];
      docs["batteryTemp"] = messsageArray[1];
      docs["motorTemp"] = messsageArray[2];
      docs["batteryVoltage"] = messsageArray[3];
      docs["airSystemTemp"] = messsageArray[4];
      docs["tankPressure"] = messsageArray[5];
      docs["caliper1Pressure"] = messsageArray[6];
      docs["caliper2Pressure"] = messsageArray[7];
      docs["distanceTravelled"] = messsageArray[8];

      db.update(
        { _id: "sensorData" },
        docs,
        {},
        function (err, docReplaced) {}
      );
    });
  });
};
