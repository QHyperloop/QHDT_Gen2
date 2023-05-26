#include <Ethernet.h>
#include <EthernetUdp.h>
// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

// network setup
const int LAPTOP_IP = 60;
const int PACKET_SIZE = 60;
unsigned long interval = 1000;
unsigned long previousMillis = 0;

int canID = 0;

const IPAddress ip(192, 168, 1, 177);
//const IPAddress server(192, 168, 8, 1);
const IPAddress computer(192, 168, 1, 60);
const unsigned int localPort = 8888;  // local port to listen on

//byte spaceXPacket[PACKET_SIZE]; //packet to send spaceX
byte telemetry[PACKET_SIZE];  //packet to send to computer
byte sentData[PACKET_SIZE];
char sending[2] = "0";
char ReplyBuffer[] = "acknowledged";

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

unsigned long lastSerialDataTime = 0;
const unsigned long SERIAL_TIMEOUT = 1000;

/*  -------------------------------------------------------------------------
    FUNCTIONS
    -------------------------------------------------------------------------
*/

void sendUDP(byte *packet, int packetSize, IPAddress IP, int port) {
  // send the packet to an address/port of choice
  Udp.beginPacket(IP, port);
  for (int i = 0; i < packetSize; i++) {
    Udp.write(int(packet[i]));
    Serial.print(int(packet[i]));  // debugging
  }
  Serial.println();  // debugging

  Udp.endPacket();
}

void establishInternal() {
  int requestNumber = 0;
  char receivedChar = NULL;

  // Request the state arduino to send a signal, continue to request signal until the 'A' signal is recieved.
  while (receivedChar != 'B') {

    Serial.write('A');
    if (Serial.available()) {
      receivedChar = Serial.read();
    }

    delay(500);
    requestNumber++;
  }
  digitalWrite(13, HIGH);
}


void establishExternal() {
  // TODO, replace this with actual network data
  // if there's data available, read a packet
  int packet_size = Udp.parsePacket();
  int packet = Udp.read();
  bool connection = false;
  while (!connection) {
    if (packet_size == 1 && Udp.remoteIP()[3] == LAPTOP_IP) {
      // A connection has been established with the computer

      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i = 0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.println(Udp.remotePort());

      Serial.write('C');
      digitalWrite(13, LOW);
      connection = true;
    }
    packet_size = Udp.parsePacket();
  }
}


/*  -------------------------------------------------------------------------
    MAIN
    -------------------------------------------------------------------------
*/
void setup() {
  pinMode(13, OUTPUT);

  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields

  // start the Ethernet
  Ethernet.begin(mac, ip);
  // prevent sending data to both IPs from causing effecting message frequency
  Ethernet.setRetransmissionCount(1);
  Ethernet.setRetransmissionTimeout(10);

  Serial.begin(9600);

  // start UDP
  Udp.begin(localPort);

  //establishInternal();  // send a byte to establish contact until receiver responds
  //establishExternal();

  //zeroPacket(spaceXPacket, PACKET_SIZE);
  //zeroPacket(telemetry, PACKET_SIZE);
}




void loop() {
  int packetCheck = Udp.parsePacket();

  //Serial.println(receivedInt);

  // Serial.println(canID);
  //Serial.println(computer);
  String serialBuffer;                           // Define a buffer to store the serial data
  unsigned long lastSerialSendTime = 0;          // Variable to track the last time the serial data was sent
  const unsigned long serialSendInterval = 500;  // Set the interval at which to send the serial data (in milliseconds)

  // ...

  if (Serial.available()) {
    serialBuffer = Serial.readString();  // Store the received serial data in the buffer
  }
  unsigned long currentMillis = millis();
  if (currentMillis - lastSerialSendTime >= serialSendInterval && serialBuffer.length() > 0) {
    // Send the serial data via UDP
    lastSerialDataTime = currentMillis;
    
    Udp.beginPacket(computer, 8888);
    Udp.write(serialBuffer.c_str());
    Udp.endPacket();

    lastSerialSendTime = currentMillis;

    serialBuffer = "";  // Clear the buffer after sending the data
  }
  if (currentMillis - lastSerialDataTime >= 500) {
      Udp.beginPacket(computer, 8888);
      Udp.write(-1);
      Udp.endPacket();
      lastSerialDataTime = currentMillis;
    }

  // if (currentMillis - previousMillis >= 500) {
  //   previousMillis = currentMillis;
  //   if (Serial.available()) {
  //     // for (int i = 0; i < sizeof(sentData); i++) {
  //     //   sentData[i] = Serial.read();
  //     //   //Serial.print(sentData[i]); // convert each byte to a char and store in the array
  //     // }
  //     String message = Serial.readString();
  //     // for (int i = 0; i < sizeof(sentData); i++) {
  //     //   //sentData[i] = Serial.read();
  //     //   Serial.print(sentData[i]);
  //     //   Serial.print(" ") // convert each byte to a char and store in the array
  //     // }
  //     // Serial.println(message);
  //     // lastSerialDataTime = currentMillis;
  //     Udp.beginPacket(computer, 8888);
  //     Udp.write(message.c_str());
  //     Udp.endPacket();
  //   } 
  // }

  if (packetCheck) {
    // Serial.print("Received packet of size ");
    // Serial.println(packetCheck);
    // Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    // for (int i = 0; i < 4; i++) {
    //   //Serial.print(remote[i], DEC);
    //   if (i < 3) {
    //     Serial.print(".");
    //   }
    // }
    // Serial.print(", port ");
    // Serial.println(Udp.remotePort());

    //read the packet into packetBuffer
    Udp.read(telemetry, UDP_TX_PACKET_MAX_SIZE);
    // Serial.println("Contents:");
    // for (int i = 0; i < sizeof(telemetry); i++) {
    //   Serial.print(telemetry[i], HEX);
    //   Serial.print(" ");
    // }
    // Serial.println();
    // sending[0] = telemetry[0];
    Serial.write(telemetry, 1);

    //send a reply to the IP address and port that sent us the packet we received
    // Serial.println(Udp.remoteIP());
    Udp.beginPacket(computer, 8888);
    Udp.write(ReplyBuffer);
    Udp.endPacket();
    delay(15);
    // convert the signal to a byte
  }

  //  if (Serial.available())
  //  {
  //    int bytes = Serial.readBytesUntil((char)0x4, telemetry, PACKET_SIZE);
  //    for (int i = 0; i < 14; i++)
  //    {
  //      // Send team_id through to velocity from telemetry to spaceXpacket
  //      //spaceXPacket[i] = telemetry[i];
  //    }
  //  }
  //  if (telemetry[0] != 0)
  //  {
  //    //sendUDP(telemetry, PACKET_SIZE, server, 3000);
  //    //    Udp.beginPacket(computer, localPort);
  //    //    Udp.write(telemetry);
  //    //    Udp.endPacket();
  //    sendUDP(telemetry, PACKET_SIZE, computer, 8888);
  //  }
}
