#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 10);
EthernetServer server(80);
File webFile;
String request;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server started at ");
  Serial.println(Ethernet.localIP());

  // initialize SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;
  }
  Serial.println("SUCCESS - SD card initialized.");

  Serial.println("Searching for website on SD card...");
  if (!SD.exists("index.htm")) {
    Serial.println("ERROR - Can't find index.htm!");
    return;
  }
  Serial.println("SUCCESS - Found file.");
}

void loop() {
  EthernetClient client = server.available();


  if (client) {
    bool currentLineIsBlank = true;
    request = "";
    while (client.connected()) {
      if (client.available()) {
        char reqChar = client.read();  // read 1 byte (character) from client
        request += reqChar;

        if (request == "GET /") {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r");
          // send web page
          webFile = SD.open("index.htm");
          if (webFile) {
            Serial.println("webfile");
            while (webFile.available()) {
              char s = webFile.read();
              client.write(s);  // send web page to client
              Serial.print(s);
            }
            webFile.close();
          }
          // client.println("HTTP/1.1 200 OK\n\rContent-Type: text/css\n\r\n\r");
          // webFile = SD.open("style.css");
          // if (webFile) {
          //   while (webFile.available()) {
          //     client.write(webFile.read());  // send web page to client
          //   }
          //   webFile.close();
          // }
          break;
        }
        // else {
        //   client.println("HTTP/1.1 404 Not Found\n\r"); {
        //   break;
        //   }
        // }

        // every line of text received from the client ends with \r\n
        if (reqChar == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        } else if (reqChar != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }

        //Translate the user request and check to switch on or off the fan
        if (request.indexOf("?switchOn") > 0) {
          //digitalWrite(fan, HIGH);
        } else if (request.indexOf("?standBy") > 0) {
          //digitalWrite(fan, LOW);
        } else if (request.indexOf("?reset") > 0) {
          //digitalWrite(fan, LOW);
        }
      }
    }

    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
    Serial.print("Request: ");
    Serial.println(request);
  }
}
