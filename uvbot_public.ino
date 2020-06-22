/*           _           _   
 _   ___   _| |__   ___ | |_      Daniele Facco
| | | \ \ / / '_ \ / _ \| __|
| |_| |\ V /| |_) | (_) | |_      FarSideVirtual@airmail.cc
 \__,_| \_/ |_.__/ \___/ \__|

*/

//ARDUINOJSON VERSIONE 15.13.5
#include <WiFiClientSecure.h>         
#include <ESP8266WiFi.h>              
#include <UniversalTelegramBot.h>     //VERSIONE 1.0.0
#include <string.h>

const char* ssid = "XXXXXX";     // your network SSID (name)
const char* password = "YYYYYY"; // your network key

#define BOTtoken "ZZZZZZ"  // your Bot Token (Get from Botfather)
WiFiClientSecure client;  // Initialize Wifi connection to the router
UniversalTelegramBot bot(BOTtoken, client); // Initialize Telegram BOT

int uvSensorIn = A0;
long Bot_lasttime=millis();   //last time messages' scan has been done
int Bot_mtbs = 200; //mean time between scan messages RIMETTERE A 15000
float uvIntensity;

void setup() {
  Serial.begin(9600);
  pinMode(uvSensorIn, INPUT);
  Serial.println("Inizio test");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname("wifi");
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //client.setInsecure();
  const uint8_t fingerprint[20] = { 0xF2, 0xAD, 0x29, 0x9C, 0x34, 0x48, 0xDD, 0x8D, 0xF4, 0xCF, 0x52, 0x32, 0xF6, 0x57, 0x33, 0x68, 0x2E, 0x81, 0xC1, 0x90 };
  client.setFingerprint(fingerprint);
}

void loop() {
  int uvValue = analogRead(uvSensorIn);
  float outputvoltage = 3.3/1024*uvValue;
  uvIntensity = mapfloat(outputvoltage, 1.07, 2.8, 0.0, 15.0);
  Serial.println("UV Intensity: " + String(uvIntensity) + "(mW/cm^2)");
  if (millis() > Bot_lasttime + Bot_mtbs)  {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        Serial.println(numNewMessages);
        while(numNewMessages-1) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);  //TELEGRAM
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        Bot_lasttime = millis();
  }
  delay(5000);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void handleNewMessages(int numNewMessages){
  Serial.println("gestione messaggio");
    for (int i=0; i<numNewMessages; i++) {
        String text = bot.messages[i].text;
        String chatid = String(bot.messages[i].chat_id);
        if (text == "/uv") {
          String message = "";
          message += "UV intensity: ";
          message += String(uvIntensity);
          message += " (mW/cm²)\n";
          String uvinfo = "Exposure level: ";
          if(uvIntensity < 3){
            uvinfo += "Low\nTime to burn: 60 minutes\nActions to take: Apply SPF 30+ sunscreen; wear sunglasses on bright days";
          }
          if(uvIntensity >= 3 && uvIntensity < 6){
            uvinfo += "Moderate\nTime to burn: 45 minutes\nActions to take: Apply SPF 30+ sunscreen every 2 hours; wear a hat and sunglasses; seek shade during midday hours (10 a.m. to 4 p.m.), when the sun’s rays are most intense";
          }
          if(uvIntensity >= 6 && uvIntensity < 8){
            uvinfo += "High\nTime to burn: 30 minutes\nActions to take: Apply SPF 30+ sunscreen every 2 hours; wear a wide-brimmed hat, sunglasses, and a long-sleeved shirt and pants if practical; seek shade during midday hours (10 a.m. to 4 p.m.), when the sun’s rays are most intense";
          }
          if(uvIntensity >= 8 && uvIntensity < 11){
            uvinfo += "Very high\nTime to burn: 15 minutes\nActions to take: Apply SPF 30+ sunscreen every 2 hours; wear a wide-brimmed hat, sunglasses, and a long-sleeved shirt and pants if practical; seek shade during midday hours (10 a.m. to 4 p.m.), when the sun’s rays are most intense; limit time outdoors";
          }
          if(uvIntensity >= 11){
            uvinfo += "Extreme\nTime to burn: 10 minutes\nActions to take: Apply SPF 30+ sunscreen every 2 hours; wear a wide-brimmed hat, sunglasses, and a long-sleeved shirt and pants if practical; seek shade during midday hours (10 a.m. to 4 p.m.), when the sun’s rays are most intense; limit time outdoors";
          }
          bot.sendMessage(chatid, message + uvinfo);
        }
    }
}

//e.o.f.
