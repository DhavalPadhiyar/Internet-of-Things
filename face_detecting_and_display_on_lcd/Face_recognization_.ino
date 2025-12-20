#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "abc";
const char* password = "111111111";

ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void handleName() {
  String person = server.arg("person");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Detected:");
  lcd.setCursor(0, 1);
  lcd.print(person);

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("System Ready");

  server.on("/name", handleName);
  server.begin();
}


void loop() {
  server.handleClient();
}
