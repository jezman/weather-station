#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN 4          // NodeMCU D2/GPIO4
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

// Wifi credential
const char* ssid = "SSID";
const char* password = "password";

DHT dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

/*
    Return the JSON string with the received data from the sensor
    Response example:
    {
        "temperature": "23.00",
        "humudity": "40.00",
        "error": ""
    }
*/
String jsonGenerator(float tmp=0.0, float hum=0.0, String err="null") {
    String msg = "{\"temperature\": \"";
    msg += tmp;
    msg += "\",";
    msg += "\"humidity\": \"";
    msg += hum;
    msg += "\",";
    msg += "\"error\": \"";
    msg += err;
    msg += "\"}";

    return msg;
}

void handleDHT() {
    float hum = dht.readHumidity();
    float tmp = dht.readTemperature();
    String err;

    if (isnan(hum) || isnan(tmp)) {
        err = "DHT sensor error";
    }

    server.send(200, "application/json", jsonGenerator(tmp, hum, err));
}

void handleNotFound(){
    server.send(404, "text/plain", "File Not Found");
}

void setup(){
    dht.begin();
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleDHT);

    // Handles wrong request
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");
}

void loop(){
    server.handleClient();
}
