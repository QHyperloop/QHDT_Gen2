// const dgram = require("dgram");
require("dotenv").config();
const express = require("express");
const app = express();

// Configure UDP connection
const udpSocket = require("./udp/udpConfig");

const port = process.env.PORT || 5000;

// Initialize local database
require("./udp/initDB");

app.use("/health", (req, res) => {
  res.status(200);
  res.send("Server is running");
});

app.listen(port, () => {
  console.log(`Server is running on port: ${port}`);
});
