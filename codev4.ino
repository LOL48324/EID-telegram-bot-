/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
  
  Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
  Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/
/* The project is done by eid ntu group T34* Luo JunYuan , with reference from Rui Santo , credits goes to Mr Rui Santo*/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Kcream";
const char* password = "12345678";
//esp32 com4 
// Initialize Telegram BOT
#define BOTtoken "1290996505:AAFEEGJO6Qg27WYMnPG_MDIvgt2paUmTBL8"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "-410820022"
#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 0.5 second.
int botRequestDelay = 500;
unsigned long lastTimeBotRan;
// door lock pin at 5 
const int ledPin2 = 2;
bool ledState = LOW;
const int ledPin5 = 5;
const int BUZZER=21; //buzzer pin
// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following buttons to control your outputs.\n\n";
      bot.sendMessage(chat_id, welcome, "");
      String keyboardJson = F("[[");
      keyboardJson += F("{\"text\":\"UNLOCK\",\"callback_data\":\"UNLOCK\"},");
      keyboardJson += F("{\"text\":\"LOCK\",\"callback_data\":\"LOCK\"}],");
      keyboardJson += F("[{\"text\":\"LOCK_STATE\",\"callback_data\":\"LOCK_STATE\"}],");
      keyboardJson += F("[{\"text\":\"NODE_ON\",\"callback_data\":\"NODE_ON\"}],"); // Added a comma here
      keyboardJson += F("[{\"text\":\"NODE_OFF\",\"callback_data\":\"NODE_OFF\"}]"); // Removed an extra comma here
      keyboardJson += F("]");
      bot.sendMessageWithInlineKeyboard(chat_id, "EID project bot ", "", keyboardJson);
      
    }

    if (text == F("UNLOCK")) {
      bot.sendMessage(chat_id, "You unlocked the door ", "");
      ledState = LOW;
      digitalWrite(ledPin2, ledState);
      tone(BUZZER, 300);
      noTone(BUZZER);
    }
    
    if (text == F("LOCK")) {
      bot.sendMessage(chat_id, "You locked the door", "");
      ledState = HIGH;
      digitalWrite(ledPin2, ledState);
    }

    if (text == F("NODE_ON")) {
      bot.sendMessage(chat_id, "You on the node ", "");
      ledState = HIGH;
      digitalWrite(ledPin5, ledState);
    }

    if (text == F("NODE_OFF")) {
      bot.sendMessage(chat_id, "You off the node ", "");
      ledState = LOW;
      digitalWrite(ledPin5, ledState);
    }
    
    if (text == F("LOCK_STATE")) {
      if (digitalRead(ledPin2)){
        bot.sendMessage(chat_id, "The door is locked", "");
      }
      else{
        bot.sendMessage(chat_id, "The door is unlocked", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW); // Ensure the buzzer is off at startup
  noTone(BUZZER); // Ensure no tone is being generated
  Serial.println("Buzzer should be off now.");
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin5, OUTPUT);
  digitalWrite(ledPin2, ledState);
  digitalWrite(ledPin5, ledState);
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "EID Bot started up, please press /start ", "");
  bot.sendMessage(CHAT_ID, WiFi.localIP().toString(), "");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}