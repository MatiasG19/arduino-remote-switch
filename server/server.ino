#include <Ethernet.h>
#include <SD.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 10);
EthernetServer server(80);

const int IN_PIN_POWER_LED = 7;
const int OUT_PIN_POWER = 8, OUT_PIN_RESET = 9;

// Commands
bool powerOn, standBy, reset, kill;
// States
bool powerLed;

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server started at ");
  Serial.println(Ethernet.localIP());

  // SD card
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

  // PINs
  pinMode(IN_PIN_POWER_LED, INPUT_PULLUP);
  pinMode(OUT_PIN_POWER, OUTPUT);
  pinMode(OUT_PIN_RESET, OUTPUT);
}

void loop() {
  // Read inputs
  if(digitalRead(IN_PIN_POWER_LED) > 0) 
    powerLed = true;
   else 
    powerLed = false;

  // Server
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

  // Control outputs
  if(powerOn) {
    controlOutput(OUT_PIN_POWER, 500);
    powerOn = false;
  } else if(standBy) {
    controlOutput(OUT_PIN_POWER, 500);
    standBy = false;
  } else if(reset) {
    controlOutput(OUT_PIN_RESET, 500);
    reset = false;
  } else if(kill) {
    controlOutput(OUT_PIN_POWER, 4500);
    kill = false;
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

  // Send file to client
  File webFile = SD.open(req);
  if (webFile) {
    while (webFile.available()) {
      client.write(webFile.read());  
    }
    webFile.close();
  } 
  // Other actions
  else {
    if(req == "powerStatus")
      client.write(powerLed);
    else if(req == "powerOn" && !powerLed) 
      powerOn = true;
    else if(req == "standBy" && powerLed) 
      standBy = true;
    else if(req == "reset" && powerLed) 
      reset = true;
    else if(req == "kill" && powerLed) 
      kill = true;
    else 
      client.println("HTTP/1.1 404 Not Found\n\r"); 
  }
}

void controlOutput(int output, int delayTime) {
  digitalWrite(output, HIGH); 
  delay(delayTime);              
  digitalWrite(output, LOW);
  delay(3000); 
}
