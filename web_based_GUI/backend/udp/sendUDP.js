function send_via_udp(message, server) {
  const messageToString = message.toString();
  const messsageLength = messageToString.length;
  server.send(messageToString, 0, messsageLength, 6000, "0.0.0.0");
}

module.exports = { send_via_udp };
