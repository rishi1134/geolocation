#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
 
String apiKey = "WM8RPHCC499OWC91";     //  Enter your Write API key from ThingSpeak
int crash = 1;
const char *ssid =  "Rizz";     // replace with your wifi ssid and wpa2 key
const char *pass =  "directx11";
const char* server = "api.thingspeak.com";
 
//Credentials for Google GeoLocation API...
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
 
// --- Get a google maps ap key here : https://developers.google.com/maps/documentation/geolocation/intro
String key = "AIzaSyDC_knmxwS697JCNNW4EsD5kVNf4SDCrAI";
 
int status = WL_IDLE_STATUS;
String jsonString = "{\n";
 
double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1; // set to 1 for more debug output
 

 
void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid,pass);

  Serial.println();
  Serial.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }


  //Serial.println("Wifi Connected Success!");
  //Serial.print("NodeMCU IP Address : ");
  //Serial.println(WiFi.localIP() );
  
}
 
void loop() 
{
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  //Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
 // Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");
 
    if (more_text) {
       //Print out the formatted json...
      Serial.println("{");
      Serial.println("\"homeMobileCountryCode\": 234,");  // this is a real UK MCC
      Serial.println("\"homeMobileNetworkCode\": 27,");   // and a real UK MNC
      Serial.println("\"radioType\": \"gsm\",");          // for gsm
      Serial.println("\"carrier\": \"Airtel\",");       // associated with Vodafone
      Serial.println("\"cellTowers\": [");                // I'm not reporting any cell towers
      Serial.println("],");
      Serial.println("\"wifiAccessPoints\": [");
     for (int i = 0; i < n; ++i)
     {
       Serial.println("{");
       Serial.print("\"macAddress\" : \"");
       Serial.print(WiFi.BSSIDstr(i));
       Serial.println("\",");
       Serial.print("\"signalStrength\": ");
       Serial.println(WiFi.RSSI(i));
       if (i < n - 1)
       {
         Serial.println("},");
       }
       else
       {
         Serial.println("}");
       }
     }
     Serial.println("]");
     Serial.println("}");
   }
   Serial.println(" ");
 }
 // now build the jsonString...
 jsonString = "{\n";
 jsonString += "\"homeMobileCountryCode\": 404,\n"; // this is a real UK MCC
 jsonString += "\"homeMobileNetworkCode\": 40,\n";  // and a real UK MNC
 jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
 jsonString += "\"carrier\": \"Airtel\",\n";      // associated with Airtel
 jsonString += "\"wifiAccessPoints\": [\n";
 for (int j = 0; j < n; ++j)
 {
   jsonString += "{\n";
   jsonString += "\"macAddress\" : \"";
   jsonString += (WiFi.BSSIDstr(j));
   jsonString += "\",\n";
   jsonString += "\"signalStrength\": ";
   jsonString += WiFi.RSSI(j);
   jsonString += "\n";
   if (j < n - 1)
   {
     jsonString += "},\n";
   }
   else
   {
     jsonString += "}\n";
   }
 }
 jsonString += ("]\n");
 jsonString += ("}\n");
 //--------------------------------------------------------------------
 
 Serial.println("");
 
 WiFiClientSecure client;
 
 //Connect to the client and make the api call
 Serial.print("Requesting URL: ");
 // ---- Get Google Maps Api Key here, Link: https://developers.google.com/maps/documentation/geolocation/intro
 Serial.println("https://" + (String)Host + thisPage + "PUT-YOUR-GOOGLE-MAPS-API-KEY-HERE");
 Serial.println(" ");
 if (client.connect(Host, 443)) {
   Serial.println("Connected");
   client.println("POST " + thisPage + key + " HTTP/1.1");
   client.println("Host: " + (String)Host);
   client.println("Connection: close");
   client.println("Content-Type: application/json");
   client.println("User-Agent: Arduino/1.0");
   client.print("Content-Length: ");
   client.println(jsonString.length());
   client.println();
   client.print(jsonString);
   delay(500);
 }
 
 //Read and parse all the lines of the reply from server
 while (client.available()) {
   String line = client.readStringUntil('\r');
   if (more_text) {
     Serial.print(line);
   }
   JsonObject& root = jsonBuffer.parseObject(line);
   if (root.success()) {
     latitude    = root["location"]["lat"];
     longitude   = root["location"]["lng"];
     accuracy   = root["accuracy"];
   }
 }
 
 Serial.println("closing connection");
 Serial.println();
 client.stop();
 
 Serial.print("Latitude = ");
 Serial.println(latitude, 6);
 Serial.print("Longitude = ");
 Serial.println(longitude, 6);
 Serial.print("Accuracy = ");
 Serial.println(accuracy);
 
 delay(10000);
 
 Serial.println();
 //Serial.println("Restarting...");
 Serial.println();

//-------------------------------------------------------------------------------------------------------------------------------------------
  WiFiClient client1;
  if (client1.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
{                              
  String postStr = apiKey;
  postStr +="&field1=";
  postStr += String(latitude);
  postStr +="&field2=";
  postStr += String(longitude);
  postStr += "\r\n\r\n";
   
  client1.print("POST /update HTTP/1.1\n");
  client1.print("Host: api.thingspeak.com\n");
  client1.print("Connection: close\n");
  client1.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
  client1.print("Content-Type: application/x-www-form-urlencoded\n");
  client1.print("Content-Length: ");
  client1.print(postStr.length());
  client1.print("\n\n");
  client1.print(postStr);
}
else
Serial.print("Not conected");
client1.stop();
Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates, i've set it to 30 seconds
  delay(2000);
















  /*Serial.print("arduino input :-");
  if (client.connect(server,80))
  {
    String data1="GET /update?api_key="+apiKey+"&field1=";
    String data2="GET /update?api_key="+apiKey+"&field2=";
    String data3="GET /update?api_key="+apiKey+"&field3=";
    int crash = 1;             ////analog read value
    data1=data1+String(crash);
    data1=data1+"HTTP/1.1\r\n"+"Host:"+server+"\r\n"+"Connection:close\r\n\r\n";
    data2=data2+String(latitude);
    data2=data2+"HTTP/1.1\r\n"+"Host:"+server+"\r\n"+"Connection:close\r\n\r\n";
    data3=data3+String(longitude);
    data3=data3+"HTTP/1.1\r\n"+"Host:"+server+"\r\n"+"Connection:close\r\n\r\n";
    Serial.println(data2);              
    client.print(data1);
    client.print(data2);
    client.print(data3);
    delay(5000);

    Serial.print("Crash: ");
    Serial.println(crash); 
    */

  /*else
  Serial.print("Not conected");
  client.stop();
  delay(15000);*/
}
