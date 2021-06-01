#include <Arduino.h>
#include <WiFiClient.h>
#include <painlessMesh.h>
#include <meshFunctions.h>

#define   MESH_PREFIX     "hivemesh"
#define   MESH_PASSWORD   "admin123"
#define   MESH_PORT       5555

WiFiClient wifiClient;
painlessMesh  mesh;

void setup() {
    Serial.begin(115200);

    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
     
    mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT );
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    Serial.println(mesh.getNodeId());
}

void loop() {
    mesh.update();
    delay(100);
}