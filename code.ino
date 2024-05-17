#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "PROJECT-720";
const char* password = "12345678";

const int DHTPin = 4;
const int DHTType = DHT11;

DHT dht(DHTPin, DHTType);

WebServer server(80);

const int maxDataPoints = 40;
float temperatureData[maxDataPoints] = {0};
float humidityData[maxDataPoints] = {0};
int dataIndex = 0;

float currentTemperature = 0;
float currentHumidity = 0;

unsigned long lastUpdateTime = 0;

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  dht.begin();

  server.on("/", handleRoot);
  server.on("/tempGraph", drawTempGraph);
  server.on("/humidityGraph", drawHumidityGraph);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  if (millis() - lastUpdateTime >= 2000) {
    float newTemperature = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    if (!isnan(newTemperature)) {
      temperatureData[dataIndex] = newTemperature;
    }

    if (!isnan(newHumidity)) {
      humidityData[dataIndex] = newHumidity;
    }

    dataIndex = (dataIndex + 1) % maxDataPoints;

    lastUpdateTime = millis();

    currentTemperature = newTemperature;
    currentHumidity = newHumidity;
  }
}

void handleRoot() {
  String html = "<html><head><title>ESP32 Temperature & Humidity</title></head><body>";
  
  // Add the list of names and scores
  html += "<h1>Electronics Instrumentation</h1>";
  html += "<ul>";
  html += "<li><h2>Vijay Wath - A68</h2></li>";
  html += "<li><h2>Yadhnesh vyavahare - A39</h2></li>";

  html += "</ul>";

  html += "<h1 style=\"color: #333;\">Current Temperature: ";
  html += String(currentTemperature);
  html += "°C</h1>";
  html += "<img src=\"/tempGraph\" /><br><br>";
  html += "<h1 style=\"color: #333;\">Current Humidity: ";
  html += String(currentHumidity);
  html += "%</h1>";
  html += "<img src=\"/humidityGraph\" />";
  
  html += "</body></html>";

  server.send(200, "text/html", html);
}


void drawTempGraph() {
  String out = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<polyline points=\"";

  for (int i = 0; i < maxDataPoints; i++) {
    int x = i * (400 / maxDataPoints); 
    int y = map(temperatureData[i], 0, 50, 140, 10); 
    out += String(x) + "," + String(y) + " ";
  }

  out += "\" stroke=\"red\" fill=\"none\" stroke-width=\"2\" />\n";
  out += "</svg>\n";

  server.send(200, "image/svg+xml", out);
}

void drawHumidityGraph() {
  String out = "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(210, 230, 250)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<polyline points=\"";

  for (int i = 0; i < maxDataPoints; i++) {
    int x = i * (400 / maxDataPoints); 
    int y = map(humidityData[i], 0, 100, 140, 10); 
    out += String(x) + "," + String(y) + " ";
  }

  out += "\" stroke=\"blue\" fill=\"none\" stroke-width=\"2\" />\n";
  out += "</svg>\n";

  server.send(200, "image/svg+xml", out);
}
