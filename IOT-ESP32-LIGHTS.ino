#include <ArduinoHttpClient.h>
#include <WiFi.h>
#include <Arduino_JSON.h>

# define ON_OFF_PIN 34

String LIGHT_STATUS = "OFF";

char ssid[] = "NETGEAR95";
char pass[] = "purpleunicorn212";

char serverAddress[] = "192.168.1.149";  // server address
int port = 80;



WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

void setup() {
  Serial.begin(115200);

  pinMode(ON_OFF_PIN, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin("NETGEAR95", "purpleunicorn212");
  Serial.print("Connecting to WiFi ..");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}


void loop() {
  if(digitalRead(ON_OFF_PIN) == HIGH ){
    if(LIGHT_STATUS == "OFF"){
        Serial.println("[+] Turn ON the light");
        String putData = "{\"on\":true, \"sat\":254, \"bri\":240,\"hue\":240}";
        String contentType = "text/plain;charset=UTF-8";
        client.put("/api/tg3Q-LxYoUy6nHrAme48pDG5Qgqzz5ORUpTgWQ9X/lights/5/state", contentType, putData);
      
        // read the status code and body of the response
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();
      
        prettyPrint(response);
        
        LIGHT_STATUS = "ON";
    }
    else{
        Serial.println("[+] Turn Off the light");
        String putData = "{\"on\":false, \"sat\":254, \"bri\":240,\"hue\":240}";
        String contentType = "text/plain;charset=UTF-8";
        client.put("/api/tg3Q-LxYoUy6nHrAme48pDG5Qgqzz5ORUpTgWQ9X/lights/5/state", contentType, putData);
      
        // read the status code and body of the response
        int statusCode = client.responseStatusCode();
        String response = client.responseBody();
      
        prettyPrint(response);

        LIGHT_STATUS = "OFF";

      }
//      delay(1000);
  }
  Serial.println("Wait for a second");
  delay(100);

}

void prettyPrint(String response){
  Serial.println("Response Data:");
  JSONVar jsonArray = JSON.parse(response);
  
  // In case an error occurs
  if (JSON.typeof(jsonArray) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

//  Serial.println(jsonArray[0].hasOwnProperty("success"));
   if (jsonArray[0].hasOwnProperty("success")){
      Serial.println("[+] Work Fine");
    }
   
  }
