#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const char* ssid = "Siva 4G";
const char* password = "prajantha";

const char* apiKey = "a56ff7e4b5d2d16bbaa91263ec2dcbca";
const char* city = "Nagercoil";

WiFiClient client;
SoftwareSerial mySoftwareSerial(D3, D4); // RX, TX

DFRobotDFPlayerMini myDFPlayer;
#define START_BUTTON D5
#define MENU_BUTTON  D6

bool welcomeShown = false;
bool menuShown = false;
bool checklistShown = false;
bool weatherShown = false;

const char* menuItems[] = {
  "College",
  "Gym",
  "Trip"
};

int menuIndex = 0;

void drawMenu() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0,0);
  display.println("Please Select");
  display.println("Your Destination");
  display.println();

  for(int i=0;i<3;i++){
    if(i==menuIndex) display.print("> ");
    else display.print("  ");
    display.println(menuItems[i]);
  }
  display.display();
}

void showChecklist() {

  display.clearDisplay();
  display.setTextColor(WHITE);

  // -------- Title --------
  display.setTextSize(2);
  display.setCursor(0,0);

  if(menuIndex == 0){

    display.println("College");
    myDFPlayer.play(3);
    delay(1000);
    display.setTextSize(1);
    display.println();
    display.println("> ID Card");
    display.println("> Laptop");
    display.println("> Notebook");

  }
  else if(menuIndex == 1){

    display.println("Gym");
    myDFPlayer.play(4);
    delay(1000);
    display.setTextSize(1);
    display.println();
    display.println("> Gym Bag");
    display.println("> Shoes");
    display.println("> Bottle");

  }
  else{

    display.println("Trip");
    myDFPlayer.play(5);
    delay(1000);
    display.setTextSize(1);
    display.println();
    display.println("> Wallet");
    display.println("> Phone");
    display.println("> Charger");

  }

  display.display();
}
void showWeather() {

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  String url =
      "http://api.openweathermap.org/data/2.5/weather?q=" +
      String(city) +
      "&appid=" +
      String(apiKey) +
      "&units=metric";

  http.begin(client, url);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {

    String payload = http.getString();

    DynamicJsonDocument doc(4096);
    deserializeJson(doc, payload);

    float temp = doc["main"]["temp"];
    String weather = doc["weather"][0]["main"];

    // Play Weather Voice
    myDFPlayer.play(6);
    delay(800);

    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Today's Weather");
    display.drawLine(0,10,128,10,WHITE);

    display.setTextSize(2);
    display.setCursor(5,18);
    display.print(temp);
    display.print(" C");

    display.setTextSize(1);
    display.setCursor(5,45);

    if(weather == "Rain"){

      display.println("Rainy");
      display.println("Carry Umbrella");

    }
    else if(weather == "Clouds"){

      display.println("Cloudy");
      display.println("Enjoy the Weather!");

    }
    else if(weather == "Clear"){

      display.println("Sunny");
      display.println("Use Sun Protection");

    }
    else{

      display.println(weather);
      display.println("Safe Journey!");

    }

    display.display();

    delay(4000);


  }
  else {

    Serial.print("HTTP Error: ");
    Serial.println(httpCode);

  }

  http.end();
}
void showGoodbye() {

  display.clearDisplay();

  display.setTextSize(2);
  display.setCursor(8,18);
  display.println("Have a");
  display.println("Great Journey!");

  display.display();

  myDFPlayer.play(7);

  delay(4500);

  display.clearDisplay();
  display.display();

}

void setup() {

  Serial.begin(115200);

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(MENU_BUTTON, INPUT_PULLUP);

  Wire.begin(D2, D1);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    while(true);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Connecting WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected!");

  display.clearDisplay();
  display.setCursor(0,20);
  display.println("WiFi Connected!");
  display.display();

  delay(1000);

  display.clearDisplay();
  display.display();
    // ---------- DFPlayer Setup ----------
  mySoftwareSerial.begin(9600);

  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer Not Found");
    while (true);
  }

  Serial.println("DFPlayer Ready");

  myDFPlayer.volume(25);

}

void loop() {

  // First START press
  if(!welcomeShown){

    if(digitalRead(START_BUTTON)==LOW){

      delay(20);

      while(digitalRead(START_BUTTON)==LOW);

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(30,10);
      display.println("Hello!");
      display.setCursor(0,30);
      display.println("Welcome to");
      display.setCursor(0,45);
      display.println("Smart Exit Guardian");
      display.display();
      myDFPlayer.play(1); 

      welcomeShown = true;
      delay(3000);
    }

    return;
  }

  // Second START press
  if(!menuShown){

    if(digitalRead(START_BUTTON)==LOW){

      delay(20);

      while(digitalRead(START_BUTTON)==LOW);

      menuShown = true;
      checklistShown = false;
      weatherShown = false;
      drawMenu();
      delay(300);          // Small delay for smooth playback
      myDFPlayer.play(2);  // "Please select your destination"
    }

    return;
  }

  // White button -> Move cursor
  if(digitalRead(MENU_BUTTON)==LOW){

    delay(180);

    while(digitalRead(MENU_BUTTON)==LOW);

    menuIndex++;

    if(menuIndex>2)
      menuIndex=0;

    // Reset state when destination changes
    checklistShown = false;
    weatherShown = false;

    drawMenu();
}

  // Blue button -> Select menu
  // Blue button -> Next Screen
if(digitalRead(START_BUTTON)==LOW){

    delay(180);

    while(digitalRead(START_BUTTON)==LOW);

    if(!checklistShown){

        showChecklist();
        checklistShown = true;
    }

    else if(!weatherShown){

        showWeather();
        weatherShown = true;
    }

    else{

        showGoodbye();

        welcomeShown = false;
        menuShown = false;
        checklistShown = false;
        weatherShown = false;
        menuIndex = 0;
    }

    delay(200);
}

}
