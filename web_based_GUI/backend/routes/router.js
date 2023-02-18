var express = require("express");
var router = express.Router();

const controller = require("../backend/controller/sendUDP");

/* GET home page. */
router.get("/", function (req, res, next) {
  res.render("index", { sensorData: localStorage });
});

router.get("/sensors", controller.getSensorData);

router.post("/emergency-stop", controller.emergencyStop);
router.post("/ready", controller.ready);
router.post("/safe", controller.safe);

module.exports = router;
