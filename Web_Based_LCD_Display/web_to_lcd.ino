#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "MMN";
const char* password = "Milan@2555";

ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void handleRoot() {
  String message = "";

  if (server.hasArg("msg")) {
    message = server.arg("msg");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.substring(0, 16));

    lcd.setCursor(0, 1);
    if (message.length() > 16) {
      lcd.print(message.substring(16, 32));
    }
  }

  String html = "<html><body>";
  html += "<h2>NodeMCU LCD Control</h2>";
  html += "<form action='/' method='GET'>";
  html += "<input type='text' name='msg' placeholder='Enter text'>";
  html += "<input type='submit' value='Send'>";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");

  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
