/*
 Build on code from https://github.com/knolleary/pubsubclient/tree/master/examples/mqtt_esp8266

 Basic ESP8266 MQTT example.
 This sketch demonstrates the capabilities of the pubsub library in combination with the ESP8266 board/library.
 It connects to an MQTT server then:
  - subscribes to the topic "hello/world", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "hello/world" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

  Connect a LED to GPIO 5. This will flash while connecting, and turn on when connected.
  Tested on Amica NodeMcu board.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// ledPin on D2 = GPIO 4
#define ledPin 4
#define wifiPin 5

// Update these with values suitable for your network.

const char* ssid = "ssid_here";
const char* password = "pw_here";
const char* mqtt_server = "ip_adress_here";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifiPin, LOW);
    delay(250);
    Serial.print(".");
    digitalWrite(wifiPin, HIGH);
    delay(250);
  }
  digitalWrite(wifiPin, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, HIGH);   
  } else {
    digitalWrite(ledPin, LOW);  // Turn the LED off by making the voltage LOW
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, resubscribe
      client.subscribe("hello/world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(wifiPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
