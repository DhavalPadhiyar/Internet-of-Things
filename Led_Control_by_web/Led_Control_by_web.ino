#include <ESP8266WiFi.h>

const char* ssid = "MMN";
const char* password = "Milan@2555";

WiFiServer server(80);
int ledPin = D1;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);
    }
    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(ledPin, LOW);
    }

    // HTTP Response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<style>");
    client.println("button { font-size: 24px; padding: 15px 30px; margin: 10px; }");
    client.println(".on { background-color: #4CAF50; color: white; }");
    client.println(".off { background-color: #f44336; color: white; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>ESP8266 LED Control</h1>");
    client.println("<p><a href=\"/LED=ON\"><button class=\"on\">TURN LED ON</button></a></p>");
    client.println("<p><a href=\"/LED=OFF\"><button class=\"off\">TURN LED OFF</button></a></p>");
    client.println("</body>");
    client.println("</html>");

    delay(2000);
   client.stop();
  }
}
