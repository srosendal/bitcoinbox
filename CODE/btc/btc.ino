#include "ThingSpeak.h"
//#include <CoinMarketCapApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <WiFiClientSecure.h>
//#include <ArduinoJson.h>


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   D7 //Connect to D1 on OLED
#define OLED_CLK    D0 //Connect to D0 on OLED 
#define OLED_DC     D1 //Connect to DC on OLED
#define OLED_CS     D8 //Connect to CS on OLED
#define OLED_RESET  D4 //Connect to RES on OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Buttons
#define SELL D3
#define BUY D2
#define RESET D6
#define BUZZER D5

// Variables
int state = 2;           // (-1: short, 2: hold, 1: long)
int waittime = 0;        // (0: no wait, 1: wait)
int selector = 0;         // (1: close short, 2: begin long, 3: close long, 4: begin short)
int reboot = 0;
int first_game = 0;
int i = 0;
int j = 0;
int k = 0;
int l = 0;
int x = 0;
float tot_score = 1.0;

float net_score = 1.0;
float this_score = 1.0;
float cur = 1.0;
float prev = 1.0;
float myprev = 1.0;
float btc = 1.0;
float player = 1.0;

// Timer
long previousMillis = 0;        // will store last time LED was updated
long interval = 30000;

// Player #1
//----------------  Fill in your credentails   ---------------------
char ssid[] = "PLACE YOUR NETWORK SSID HERE";             // your network SSID (name)
char pass[] = "PLACE YOUR NETWORK PASSWORD HERE";         // your network password

unsigned long myChannelNumber = 0;  // Replace the 0 with your channel number
unsigned long counterChannelNumber = 0; // Replace the 0 with your opponent channel number
unsigned long btcChannelNumber = 0; // Replace the 0 with chosen BTC channel number
const char * myWriteAPIKey = "PLACE YOUR CHANNEL WRITE API KEY HERE";    // Paste your ThingSpeak Write API Key between the quotes
const char * myReadAPIKey = "PLACE YOUR CHANNEL READ API KEY HERE";
const char * myCounterReadAPIKey = "PLACE YOUR OPPONENT CHANNEL READ API KEY HERE";
const char * btcReadAPIKey = "PLACE BTC CHANNEL READ API KEY HERE";
//------------------------------------------------------------------

WiFiClient client;
//WiFiClientSecure clientsecure;
//CoinMarketCapApi api(clientsecure);

const unsigned char cbit [] = {
  0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00,
  0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00,
  0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00,
  0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0x00,
  0x01, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0x80, 0x03, 0xFF, 0xFF, 0xFC, 0x38, 0xFF, 0xFF, 0xC0,
  0x07, 0xFF, 0xFF, 0xFC, 0x30, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFC, 0xFC, 0x30, 0xFF, 0xFF, 0xE0,
  0x0F, 0xFF, 0xFC, 0x08, 0x70, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFC, 0x00, 0x70, 0xFF, 0xFF, 0xF0,
  0x1F, 0xFF, 0xF8, 0x00, 0x01, 0xFF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFE, 0x00, 0x00, 0xFF, 0xFF, 0xF8,
  0x3F, 0xFF, 0xFF, 0x80, 0x00, 0x3F, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0x80, 0x00, 0x1F, 0xFF, 0xFC,
  0x3F, 0xFF, 0xFF, 0x80, 0xC0, 0x0F, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0x80, 0xF8, 0x07, 0xFF, 0xFE,
  0x7F, 0xFF, 0xFF, 0x81, 0xFC, 0x03, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0x01, 0xFE, 0x03, 0xFF, 0xFE,
  0x7F, 0xFF, 0xFF, 0x01, 0xFE, 0x03, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0x01, 0xFE, 0x03, 0xFF, 0xFE,
  0xFF, 0xFF, 0xFF, 0x03, 0xFC, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x30, 0x07, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x0F, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xC0, 0x1F, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFC, 0x07, 0xF0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x07, 0xF8, 0x0F, 0xFF, 0xFF,
  0x7F, 0xFF, 0xFC, 0x0F, 0xFC, 0x07, 0xFF, 0xFE, 0x7F, 0xFF, 0xD8, 0x0F, 0xFC, 0x07, 0xFF, 0xFE,
  0x7F, 0xFF, 0x80, 0x0F, 0xFC, 0x07, 0xFF, 0xFE, 0x7F, 0xFF, 0x80, 0x0F, 0xF8, 0x07, 0xFF, 0xFE,
  0x7F, 0xFF, 0x00, 0x01, 0xF0, 0x07, 0xFF, 0xFE, 0x3F, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFC,
  0x3F, 0xFF, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xFC, 0x3F, 0xFF, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0xFC,
  0x1F, 0xFF, 0xFC, 0x38, 0x00, 0x3F, 0xFF, 0xF8, 0x1F, 0xFF, 0xFC, 0x38, 0x00, 0xFF, 0xFF, 0xF8,
  0x0F, 0xFF, 0xFC, 0x30, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFC, 0x30, 0xFF, 0xFF, 0xFF, 0xF0,
  0x07, 0xFF, 0xF8, 0x70, 0xFF, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0x70, 0xFF, 0xFF, 0xFF, 0xE0,
  0x03, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xC0, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00,
  0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00,
  0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00,
  0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
  0x00, 0x00, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x00
};

void setup() {
  digitalWrite(RESET, HIGH);
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  

  pinMode(SELL, INPUT_PULLUP);
  pinMode(BUY, INPUT_PULLUP);
  pinMode(RESET, OUTPUT);
  pinMode(BUZZER, OUTPUT);//buzzer

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Keep Reset button pushed during startup to initialize first game
  if (digitalRead(RESET) == LOW) {
    reboot = 1; // Don't load stored data from thingspeak, upload standard values instead
    delay(100);
    tone(BUZZER, NOTE_E6, 850);
    delay(800);
    noTone(BUZZER);

    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.setTextColor(WHITE);        // Draw white text
    display.println(F("NEW GAME !"));
    display.display();
    Serial.println("");
    Serial.println("************BTC************");
    Serial.println("NEW GAME");
    Serial.println("***************************");

    delay(2000);

    display.clearDisplay();
    delay(1000);
  }
  else {

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    Serial.println("");
    Serial.println("************BTC************");
    Serial.println("CONTINUE GAME");
    Serial.println("***************************");

    tone(BUZZER, NOTE_E6, 125);
    delay(130);
    tone(BUZZER, NOTE_G6, 125);
    delay(130);
    tone(BUZZER, NOTE_E7, 125);
    delay(130);
    tone(BUZZER, NOTE_C7, 125);
    delay(130);
    tone(BUZZER, NOTE_D7, 125);
    delay(130);
    tone(BUZZER, NOTE_G7, 125);
    delay(125);
    noTone(BUZZER);

    display.clearDisplay();
    display.drawBitmap(32, 0, cbit, 64, 64, WHITE);
    display.display();
    delay(5000); // Pause for 5 seconds

    // Clear the buffer
    display.clearDisplay();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    waittime == 1; // Wait while updating, ie. button push has no effect

    // Connect or reconnect to WiFi
    Serial.println("");
    Serial.println("CHECK WIFI ...");
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect to SSID: ");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, pass);
        Serial.print(".");
        delay(5000);
      }
      Serial.println("\nConnected.");
    }
    Serial.println("CHECK WIFI DONE");

    // NORMAL CONDITIONS, REBOOT IS SET TO 0
    // IF REBOOT == 0, GET LAST KNOWN VALUES FROM THINGSPEAK
    // WHEN RESTARTING INTO A NEW GAME, REBOOT IS SET TO 1
    // THEN SKIP THE FOLLOWING DOWNLOAD VALUES FROM THINGSPEAK
    // AND UPLOAD DEFAULT VALUES LATER

    if (reboot == 0) { // Restore last known values stored from thingspeak
      Serial.println("RESTORE VALUES FROM TS ...");
      selector = 0;
      // last known tot_score
      tot_score = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey);
      Serial.println("tot_score");
      Serial.println(tot_score, 6);
      // last known state
      state = ThingSpeak.readFloatField(myChannelNumber, 3, myReadAPIKey);
      Serial.println("state");
      Serial.println(state);
      // last known myprev
      myprev = ThingSpeak.readFloatField(myChannelNumber, 4, myReadAPIKey);
      Serial.println("myprev");
      Serial.println(myprev);
      // last known prev
      prev = ThingSpeak.readFloatField(myChannelNumber, 5, myReadAPIKey);
      Serial.println("prev");
      Serial.println(prev);
      // last known player
      player = ThingSpeak.readFloatField(counterChannelNumber, 2, myCounterReadAPIKey);
      Serial.println("player");
      Serial.println(player, 6);

      while (tot_score == 0 || state == 0 || prev == 0 || myprev == 0 || player == 0) { // Data from thingspeak not loaded properly
        previousMillis = millis(); // reset timer
        Serial.println("THINGSPEAK SERVER ERROR");
        Serial.println("RETRY GET VALUES ...");

        // Display Server Error Information
        display.clearDisplay();
        display.setCursor(15, 10);
        display.setTextSize(2);
        display.setTextColor(WHITE);        // Draw white text
        display.println(F("SRV ERR !"));
        display.setCursor(60, 30);
        display.println(j);
        display.display();

        Serial.println("Server Error !");
        Serial.println(j);

        delay(20000);  // retry after 20 seconds
        previousMillis = millis(); // reset timer
        j += 1;

        // last known tot_score
        tot_score = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey);
        Serial.println("tot_score");
        Serial.println(tot_score, 6);
        // last known state
        state = ThingSpeak.readFloatField(myChannelNumber, 3, myReadAPIKey);
        Serial.println("state");
        Serial.println(state);
        // last known myprev
        myprev = ThingSpeak.readFloatField(myChannelNumber, 4, myReadAPIKey);
        Serial.println("myprev");
        Serial.println(myprev);
        // last known prev
        prev = ThingSpeak.readFloatField(myChannelNumber, 5, myReadAPIKey);
        Serial.println("prev");
        Serial.println(prev);
        // last known player
        player = ThingSpeak.readFloatField(counterChannelNumber, 2, myCounterReadAPIKey);
        Serial.println("player");
        Serial.println(player, 6);

        if (j == 3) {
          // Reconnect to WiFi
          Serial.println("");
          Serial.println("RECONNECT WIFI ...");
          if (WiFi.status() != WL_CONNECTED) {
            Serial.print("Attempting to connect to SSID: ");
            WIFI_Connect();
          } else {
            Serial.println("RECONNECT WIFI DONE");
          }
        }

        if (j == 5) {
          // Reset
          Serial.println("");
          Serial.println("RESET ...");
          digitalWrite(RESET, LOW);
        }
      }

      reboot = 1;
      j = 0;
      Serial.println("RESTORE VALUES FROM TS DONE");
    }

    // Get latest Bitcoin value
    Serial.println("GET BTC VALUE ...");
    cur = getbtc();
    Serial.println("GET BTC VALUE DONE");

    Serial.println("CHECK TRADES ...");
    // Make Trade
    if (selector == 0) { // No action, just calculate score
      if (state == 1) { // Long position active
        Serial.println("LONG ACTIVE");
        net_score = 1 + ((cur - myprev) / (myprev));
        tot_score = tot_score * net_score;
        Serial.println("net_score");
        Serial.println(net_score, 6);
        Serial.println("tot_score");
        Serial.println(tot_score, 6);
        myprev = cur;
      }
      else if (state == -1) { // Short position active
        Serial.println("SHORT ACTIVE");
        net_score = 1 + ((myprev - cur) / (cur));
        tot_score = tot_score * net_score;
        Serial.println("net_score");
        Serial.println(net_score, 6);
        Serial.println("tot_score");
        Serial.println(tot_score, 6);
        myprev = cur;
      }
      else {
        Serial.println("NO POSITIONS");
      }
    }

    if (selector == 1) { // Close Short
      Serial.println("CLOSE SHORT");
      selector = 0;

      // Calculate Score as position is closed
      net_score = 1 + ((myprev - cur) / (cur));
      tot_score = tot_score * net_score;

      this_score = 1 + ((prev - cur) / (cur));

      display.setCursor(0, 50);
      display.setTextSize(2);
      display.println(this_score, 6);
      display.display();

      Serial.println("this_score");
      Serial.println(this_score, 6);

      prev = 1;
      myprev = 1;

      delay(3000);
    }

    if (selector == 2) { // Begin Long
      Serial.println("BEGIN LONG");
      selector = 0;

      // Store current Bitcoin Price, used for later closing the trade
      prev = cur;
      myprev = cur; // Used in calculate live score

      display.setCursor(0, 50);
      display.println(prev);
      display.display();

      Serial.println("prev");
      Serial.println(prev);

      delay(3000);
    }

    if (selector == 3) { // Close Long
      Serial.println("CLOSE LONG");
      selector = 0;

      // Calculate Score as position is closed
      net_score = 1 + ((cur - myprev) / (myprev));
      tot_score = tot_score * net_score;

      this_score = 1 + ((cur - prev) / (prev));

      display.setCursor(0, 50);
      display.setTextSize(2);
      display.println(this_score, 6);
      display.display();

      Serial.println("this_score");
      Serial.println(this_score, 6);

      prev = 1;
      myprev = 1;

      delay(3000);
    }

    if (selector == 4) { // Begin Short
      Serial.println("BEGIN SHORT");
      selector = 0;

      // Store current Bitcoin Price, used for later closing the trade
      prev = cur;
      myprev = cur; // Used in calculating live score

      display.setCursor(0, 50);
      display.println(prev);
      display.display();

      Serial.println("prev");
      Serial.println(prev);

      delay(3000);
    }

    Serial.println("CHECK TRADES DONE");

    Serial.println("UPDATE THINGSPEAK ...");

    // Update ThingSpeak
    ThingSpeak.setField(1, cur);
    Serial.println("cur");
    Serial.println(cur);
    ThingSpeak.setField(2, tot_score);
    Serial.println("tot_score");
    Serial.println(tot_score, 6);
    ThingSpeak.setField(3, state);
    Serial.println("state");
    Serial.println(state);
    ThingSpeak.setField(4, myprev);
    Serial.println("myprev");
    Serial.println(myprev);
    ThingSpeak.setField(5, prev);
    Serial.println("prev");
    Serial.println(prev);

    x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    while (x != 200) {
      previousMillis = millis(); // reset timer
      Serial.println("PROBLEM UPDATING CHANNEL");
      Serial.println(String(x));

      l += 1;

      // Display Server Error Information
      display.clearDisplay();
      display.setCursor(15, 10);
      display.setTextSize(2);
      display.setTextColor(WHITE);        // Draw white text
      display.println(F("SRV ERR !"));
      display.setCursor(60, 30);
      display.println(i);
      display.display();

      Serial.println("Server Error !");
      Serial.println(i);

      delay(20000);
      previousMillis = millis(); // reset timer

      Serial.println("Retry");

      // Update ThingSpeak
      ThingSpeak.setField(1, cur);
      Serial.println("cur");
      Serial.println(cur);
      ThingSpeak.setField(2, tot_score);
      Serial.println("tot_score");
      Serial.println(tot_score, 6);
      ThingSpeak.setField(3, state);
      Serial.println("state");
      Serial.println(state);
      ThingSpeak.setField(4, myprev);
      Serial.println("myprev");
      Serial.println(myprev);
      ThingSpeak.setField(5, prev);
      Serial.println("prev");
      Serial.println(prev);

      x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

      if (l == 3) {
        // Reconnect to WiFi
        Serial.println("");
        Serial.println("RECONNECT WIFI ...");
        if (WiFi.status() != WL_CONNECTED) {
          Serial.print("Attempting to connect to SSID: ");
          WIFI_Connect();
        } else {
          Serial.println("RECONNECT WIFI DONE");
        }
      }

      if (l == 5) {
        // Reset
        Serial.println("");
        Serial.println("RESET ...");
        digitalWrite(RESET, LOW);
      }

    }

    Serial.println("UPDATE THINGSPEAK DONE");
    l = 0;

    Serial.println("GET PLAYER DATA FROM THINGSPEAK ...");

    // Get data from ThingSpeak
    player = ThingSpeak.readFloatField(counterChannelNumber, 2, myCounterReadAPIKey);

    Serial.println("player");
    Serial.println(player, 6);

    Serial.println("GET PLAYER DATA FROM THINGSPEAK DONE ...");

    Serial.println("UPDATE DISPLAY ...");

    // Update Display
    updatedisplay(cur, tot_score, state, prev, player);

    Serial.println("UPDATE DISPLAY DONE");

    Serial.println("");
    Serial.println("***************************");
    Serial.println("BTC in USD: ");
    Serial.println(cur);
    if (state == -1) {
      Serial.println("SHORT");
    }
    else if (state == 1) {
      Serial.println("LONG");
    }
    else {
      Serial.println("OUT!");
    }
    Serial.println(prev);
    Serial.println("tot_score");
    Serial.println(tot_score, 6);
    Serial.println("player_score");
    Serial.println(player, 6);
    Serial.println("***************************");
    Serial.println("");

    waittime = 0; // Wait over
  }


  if (digitalRead(BUY) == LOW && state == -1 && waittime == 0) { // Close Short
    waittime = 1;
    selector = 1; // Close Short
    state = 2;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("CLOS SHORT");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("CLOSE SHORT");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

  if (digitalRead(BUY) == LOW && state == 2 && waittime == 0) { // Begin Long
    waittime = 1;
    selector = 2; // Begin Long
    state = 1;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("GO LONG !");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("GO LONG !");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

  if (digitalRead(SELL) == LOW && state == 1 && waittime == 0) { // Close Long
    waittime = 1;
    selector = 3; // Close Long
    state = 2;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("CLOSE LONG");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("CLOSE LONG");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

  if (digitalRead(SELL) == LOW && state == 2 && waittime == 0) { // Begin Short
    waittime = 1;
    selector = 4; // Begin Short
    state = -1;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("GO SHORT !");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("GO SHORT !");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

  if (digitalRead(SELL) == LOW && state == 1 && waittime == 1 && selector == 2) { // Change from Begin Long to Begin Short
    waittime = 1;
    selector = 4; // Begin Short
    state = -1;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("GO SHORT !");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("GO SHORT !");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

  if (digitalRead(BUY) == LOW && state == -1 && waittime == 1 && selector == 4) { // Change from Begin Short to Begin Long
    waittime = 1;
    selector = 2; // Begin Long
    state = 1;

    // Button Tone
    tone(BUZZER, NOTE_G4, 35);
    delay(35);
    tone(BUZZER, NOTE_G5, 35);
    delay(35);
    tone(BUZZER, NOTE_G6, 35);
    delay(35);
    noTone(BUZZER);

    // Reset Timer
    previousMillis = millis();

    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.println("GO LONG !");
    display.setCursor(0, 30);
    display.println("TRADING...");
    display.display();

    Serial.println("");
    Serial.println("***************************");
    Serial.println("GO LONG !");
    Serial.println("TRADING...");
    Serial.println("***************************");
    Serial.println("");
  }

}

void updatedisplay(float x, float y, int z, float p, float pp) { // updatedisplay(cur, tot_score, state, prev, player);
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 50);            // Start at top-left corner
  display.println(y, 4);

  Serial.println("tot_score");
  Serial.println(y, 6);

  display.setCursor(50, 0);
  if (z == -1) {
    display.println("SHORT");

    Serial.println("State");
    Serial.println("SHORT");
  }
  if (z == 2) {
    display.println("OUT!");

    Serial.println("State");
    Serial.println("OUT!");
  }
  if (z == 1) {
    display.println("LONG");

    Serial.println("State");
    Serial.println("LONG");
  }

  display.setCursor(40, 10);
  if (p != 1) {
    display.println(p);
  }

  Serial.println("prev");
  Serial.println(p);

  display.setCursor(75, 50);
  display.println(pp, 4);

  Serial.println("player");
  Serial.println(pp, 6);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setCursor(10, 25);
  display.println(x);

  Serial.println("cur");
  Serial.println(x);

  display.display();
}

float getbtc() {
  btc = ThingSpeak.readFloatField(btcChannelNumber, 1, btcReadAPIKey);

  // While Server Error, Retry
  while (btc == 0) {
    previousMillis = millis(); // reset timer
    btc = 0;

    // Display Server Error Information
    display.clearDisplay();
    display.setCursor(15, 10);
    display.setTextSize(2);
    display.setTextColor(WHITE);        // Draw white text
    display.println(F("SRV ERR !"));
    display.setCursor(60, 30);
    display.println(i);
    display.display();

    Serial.println("Server Error !");
    Serial.println(i);

    delay(20000);  // retry after 20 seconds
    previousMillis = millis(); // reset timer

    i += 1;

    btc = ThingSpeak.readFloatField(btcChannelNumber, 1, btcReadAPIKey);

    if (i == 3) {
      // Reconnect to WiFi
      Serial.println("");
      Serial.println("RECONNECT WIFI ...");
      if (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        WIFI_Connect();
      } else {
        Serial.println("RECONNECT WIFI DONE");
      }
    }

    if (i == 5) {
      // Reset
      Serial.println("");
      Serial.println("RESET ...");
      digitalWrite(RESET, LOW);
    }

  }

  // BTC price found on server
  i = 0;

  Serial.println("btc");
  Serial.println(btc);

  return btc;
}

void WIFI_Connect()
{
  WiFi.disconnect();
  Serial.println("Disconnect ...");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, pass);
  // Wait for connection
  for (int k = 0; k < 25; k++)
  {
    if ( WiFi.status() != WL_CONNECTED ) {
      delay ( 250 );
      Serial.print ( "." );
      delay ( 250 );
    }
  }
  Serial.println("\nConnected.");
}
