#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const char* wifi_ssid = "/wifi_ssid.txt";
const char* wifi_pwd = "/wifi_pwd.txt";

String peripheralID;

void getWifiCredentials(){
    StaticJsonDocument<256> doc;
    WiFi.begin("HivePairer");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    if(client.connect("192.168.4.1",8888)){
        Serial.println("Connected!");
        client.print("{\"connection_type\":\"wifi\"}");
        deserializeJson(doc, client);
        client.stop();
    }

    const char* ssid = doc["ssid"];
    const char* password = doc["password"];

    WiFi.disconnect();
    WiFi.begin(ssid,password);

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);
}

void register_peripheral(){
    String registerString = R"({"type":"sensor","parameters":["temperature"],"mac_address":"00:0C:29:60:04:63"})";

    if(client.connect("192.168.1.105",4001)){
        Serial.println("Connected!");
        client.print(registerString);
        peripheralID = client.readString();
        Serial.println(peripheralID);
        client.stop();
    }
}

void connectMQTT(){
    mqtt.setServer("192.168.1.105", 1883);

    while(!mqtt.connected()){
        mqtt.connect("ESP----");
    }
}

void setup() {
    Serial.begin(115200);

    if(!SPIFFS.begin()){
        Serial.println("Error initializing SPIFFS");
    }

    // Verify if wifi_credentials exist

    File ssid = SPIFFS.open(wifi_ssid, "r");
    File pwd = SPIFFS.open(wifi_pwd, "r");

    getWifiCredentials();

    register_peripheral();

    connectMQTT();

/*
    if(!wifi_ssid){ // Doesn't exist
        getWifiCredentials();
    }else{

    }
*/
}

void loop() {
    String topicStr = "/sensor/" + peripheralID + "/temperature/";
    String temp = String(random(30,35));
    mqtt.publish(topicStr.c_str(), temp.c_str());
    mqtt.loop();
    delay(1000);
}