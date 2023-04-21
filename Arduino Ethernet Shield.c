#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
IPAddress server(192, 168, 1, 4);

EthernetClient client;

void setup() {
  Serial.begin(9600);
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, ip);
  }
  
  delay(1000);
}

void loop() {
  if (Serial.available() > 0) {
    int sensorValue = Serial.read();
    
    if (client.connect(server, 80)) {
      Serial.println("Connected to server");
      
      client.print("GET /update?value=");
      client.print(sensorValue);
      client.println(" HTTP/1.1");
      client.println("Host: server.local");
      client.println("Connection: close");
      client.println();
      
      delay(1000);
      
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
      
      Serial.println();
      Serial.println("Closing connection");
      client.stop();
      
      delay(5000);
    } else {
      Serial.println("Failed to connect to server");
      
      delay(5000);
    }
  }
}
