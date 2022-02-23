
function send_via_udp(message) {
  var buf = Buffer.from([parseInt(message)]);
  server.send(buf);
};

exports.emergencyStop = (req, res) => {
    send_via_udp(req.body.status);
}

exports.ready = (req, res) => {
    send_via_udp(req.body.status);
}

exports.safe = (req, res) => {
    send_via_udp(req.body.status);
}

exports.getSensorData = (req, res) => {
    res.status(200);
    res.send(localStorage)
}