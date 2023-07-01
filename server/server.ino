#include <Ethernet.h>
#include <SD.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 10);
EthernetServer server(80);
long delayStart;

const int IN_PIN_POWER_LED = 7;
const int OUT_PIN_POWER = 8, OUT_PIN_RESET = 9;
int POWER_LED_OFF_DELAY = 3000; // ms

// Commands
bool powerOn, standBy, reset, kill;
// States
bool powerLed;

void setup() {
  // PINs and serial
  pinMode(IN_PIN_POWER_LED, INPUT_PULLUP);
  pinMode(OUT_PIN_POWER, OUTPUT);
  pinMode(OUT_PIN_RESET, OUTPUT);
  Serial.begin(9600);

  // Server
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server started at ");
  Serial.println(Ethernet.localIP());
  delayStart = millis();

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
}

void loop() {
  // Read inputs
  if(digitalRead(IN_PIN_POWER_LED) == 0) {
    powerLed = true;
    delayStart = millis();
  }
  else if(millis() - delayStart > POWER_LED_OFF_DELAY)
    powerLed = false;

  // Server
  EthernetClient client = server.available();
  String request;

  if (client) {
    bool currentLineIsBlank = false;
    request = "";
    while (client.connected()) {
      if (client.available()) {
        char reqChar = client.read();  // Read 1 byte (character) from client

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

    delay(1);      // Give the web browser time to receive the data
    client.stop(); 
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
    Serial.println("kill");
    controlOutput(OUT_PIN_POWER, 5000);
    kill = false;
  }
}

void sendResponse(String req, EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  if(req == "") req = "index.htm";

  // Send file to client
  if(req.endsWith(".htm")) {
    client.println("Content-Type: text/html\n\r\n\r");
    File webFile = SD.open(req);
    if (webFile) {
      while (webFile.available()) {
        client.write(webFile.read());  
      }
      webFile.close();
    } 
  }
  // Other actions
  else {
    Serial.println("Other");
    client.println("\n\r\n\r");
    if(req == "powerStatus") {
      if(powerLed) client.write("powerStatus:on");
      else client.write("powerStatus:off");
    }
    else if(req == "powerOn") {
      if (!powerLed) powerOn = true;
    }
    else if(req == "standBy") {
      if (powerLed) standBy = true;
    }
    else if(req == "reset") {
      if (powerLed) reset = true;
    }
    else if(req == "kill") {
      if (powerLed) kill = true;
    }
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
