/**************************************************************************
Wemos D1 mini

L298N Motor Controller

Blynk library here:
 https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

V0 = 
V1 = 
V2 = 

**************************************************************************/
#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <BlynkSimpleEsp8266.h>

WidgetTerminal terminal(V6);

boolean reset_flag = false; //Setting data reset flag (true or false)

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40] = "blynk-cloud.com";
char mqtt_port[6] = "8080";
char blynk_token[34] = "YOUR_BLYNK_TOKEN";
char time_zone[6] = "8";

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// L298N Motor Controller pin settings
int ENA = 4; //D2
int IN1 = 0; //D3
int IN2 = 2; //D4

void setup() {
// set all the motor control pins to outputs
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

Serial.begin(115200);
  Serial.println("");

    if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
    Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
    Serial.println("parsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(blynk_token, json["blynk_token"]);
          strcpy(time_zone, json["time_zone"]);
        } else {
    Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
      Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 32);
  WiFiManagerParameter custom_time_zone("UTC", "Time Zone", time_zone, 6);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

  //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_blynk_token);
    wifiManager.addParameter(&custom_time_zone);

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

  // WiFi connected
    Serial.println("\nWiFi Connected");
    Serial.println(WiFi.localIP());

  //read updated parameters
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    strcpy(blynk_token, custom_blynk_token.getValue());
    strcpy(time_zone, custom_time_zone.getValue());

  //still not connected to the Blynk server yet
  //it will try to connect when it reaches first Blynk.run() or Blynk.connect() call
    Blynk.config(blynk_token, mqtt_server, atoi(mqtt_port));

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["blynk_token"] = blynk_token;
    json["time_zone"] = time_zone;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

//    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
    
  //Setting done
    Serial.println("Success update settings");
    Serial.print("\nTime Zone : +"); Serial.println(time_zone);
    Serial.print("mqtt_server : "); Serial.println(mqtt_server);
    Serial.print("mqtt_port : "); Serial.println(mqtt_port);
    Serial.println("blynk_token :"); Serial.println(blynk_token);
    
    delay(1000);

    
  //Reset WiFi settings with touch switch
    if (reset_flag != false) {
    Serial.print("\nReset WiFi settings now");
    delay(5000);
    wifiManager.resetSettings();
    SPIFFS.format();  //clean FS, for testing
    Serial.print("\nReset WiFi settings Done!!");
    ESP.reset();   //reset and try again
    delay(5000);
    }
  
}


BLYNK_WRITE(V0) {
// turn on motors
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, param.asInt());
  Serial.println(param.asInt());
  terminal.print("Motor value: ");
  terminal.println(param.asInt());
  delay(100);
}



void loop() {
  Blynk.run();
  
  /*
  Serial.println("Test One");
  testOne();   
  delay(1000);   
  Serial.println("Test Two");
  testTwo();   
  delay(5000);
  */
  
}
