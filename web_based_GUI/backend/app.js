require("dotenv").config();
const express = require("express");
const app = express();

var dgram = require("dgram");

// Configure local database
var Datastore = require("nedb");
var db = new Datastore({ filename: "./localDB/local.db", autoload: true });

// Configure UDP connection
require("./udp/udpConfig")(db);
// Initialize local database
require("./localDB/initDB")(db);

// Launch backend server
const port = process.env.PORT || 5000;
app.listen(port, () => {
  console.log(`Server is running on port: ${port}`);
});

app.use(express.json());

// Health endpoint to test if server is running
app.use("/health", (req, res) => {
  res.status(200);
  res.send("Server is running");
});

// Get request for sensor data
// Data is stored as a single JSON object in the local database
app.get("/sensors", (req, res) => {
  db.find({}, function (err, docs) {
    res.status(200);
    res.send(docs);
  });
});

// Endpoints for sending UDP state updates to Hyperloop pod over UDP
const outgoingUDPConnection = dgram.createSocket("udp4");
const sendUdp = require("./udp/sendUDP");

app.post("/emergencyStop", (req, res) => {
  sendUdp.send_via_udp(0, outgoingUDPConnection);
  res.status(200);
  res.send("Emergency Stop");
});

app.post("/safe", (req, res) => {
  sendUdp.send_via_udp(1, outgoingUDPConnection);
  res.status(200);
  res.send("Safe");
});

app.post("/ready", (req, res) => {
  sendUdp.send_via_udp(2, outgoingUDPConnection);
  res.status(200);
  res.send("Ready");
});
