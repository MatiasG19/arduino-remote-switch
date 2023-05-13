#include <Ethernet.h>
#include <SD.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 10);
EthernetServer server(80);

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
  String request;

  if (client) {
    bool currentLineIsBlank = false;
    request = "";
    while (client.connected()) {
      if (client.available()) {
        char reqChar = client.read();  // read 1 byte (character) from client

        if(currentLineIsBlank == true)
          continue;
        else if (reqChar == '\n' && request.length() > 0)  {
          currentLineIsBlank = true;
          continue;
        }  
        request += reqChar;
      }
      else {
        Serial.print("Request: ");
        Serial.println(request);
        if(request.length() > 0) {
          if(request.startsWith("GET /")) {
              request.replace("GET /", "");
              sendResponse(request.substring(0, request.indexOf(" ")), client);
              break;
          }
          else {
            client.println("HTTP/1.1 404 Not Found\n\r"); 
            break;
          }
        }
      }
    }

    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  }
}

void sendResponse(String req, EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  if(req == "") req = "index.htm";

  if(req.endsWith(".htm")) {
    client.println("Content-Type: text/html\n\r\n\r");
  }
  else 
    client.println("\n\r\n\r");

  File webFile = SD.open(req);
  if (webFile) {
    while (webFile.available()) {
      client.write(webFile.read());  // send web page to client
    }
    webFile.close();
  } 
  else {
    if(req == "powerOn") {
      // TODO
    }
    else if(req == "standBy") {
      // TODO
    }
    else if(req == "reset") {
      // TODO
    }
    else {
      client.println("HTTP/1.1 404 Not Found\n\r"); 
    }
  }
}
