function send_via_udp(message, server) {
  const messageToString = message.toString();
  const messsageLength = messageToString.length;
  server.send(messageToString, 0, messsageLength, 8888, "192.168.1.177");
}

module.exports = { send_via_udp };
