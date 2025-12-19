#include <ESP8266WiFi.h>

const char* ssid = "My wifi name";
const char* password = "My Password";

WiFiServer server(80); // it is a standerd http port that browser  uses
int ledPin = D1;

void setup() {
  Serial.begin(9600); // open serial communication to see on serial monitor
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
// let's connect wifi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi..."); // it will get printed in serial monitor while wifi is not connected
  // configure the loop which wil continuously watch the wi fi status till it get connected 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!"); // when wifi will get connected it will print it on serial monitor
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());// to get the ip adress which we will use to open web page and control led
  server.begin();// start the server
}

void loop() {
  WiFiClient client = server.available();// this line is to check if the server is connected to device if not then return empty else it will return wifi client
  if (client) {
    String request = client.readStringUntil('\r'); //tis par is to send request of led to urn on or off (like GET /LED=ON HTTP/1.1)
    client.flush(); // it will clear extra stuff from line and send only reqested stuff(turn on led)
// to request led to turn on // here index of( will find the (led=on) in data if it found it it will turn on(by returning the value )
    if (request.indexOf("/LED=ON") != -1) {
      digitalWrite(ledPin, HIGH);
    }
    // same case as turning on but here it will find (led=off) but if dont find then it will return -1
    if (request.indexOf("/LED=OFF") != -1) {
      digitalWrite(ledPin, LOW);
    }

    // HTTP Response 
    client.println("HTTP/1.1 200 OK"); // it tells browser browser every thing is ok and it will send the page
    client.println("Content-Type: text/html"); // it tells browser the we are sending a html file
    client.println("Connection: close");// after connection we will release the connection
    client.println("");// empty line which indicates that header is finished

    // it is our web page in html
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
    client.println("<p><a href=\"/LED=ON\"><button class=\"on\">TURN LED ON</button></a></p>");// butoon which has written turn on in it and is green
    client.println("<p><a href=\"/LED=OFF\"><button class=\"off\">TURN LED OFF</button></a></p>");// // butoon which has written turn off in it and is red
    client.println("</body>");
    client.println("</html>");

    delay(1); // it will wait for 1ms before realising the connection
   client.stop();
  }
}
