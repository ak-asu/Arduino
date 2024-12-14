#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Enter your WiFi credentials.
const char* ssid = "";
const char* password = "";
//URL endpoint for API; this is where you will paste the URL from the website
String URL = "https://api.open-meteo.com/v1/forecast?latitude=33.323&longitude=-111.933&current=temperature_2m,relative_humidity_2m,apparent_temperature,rain,cloud_cover,wind_speed_10m&temperature_unit=fahrenheit&wind_speed_unit=mph&precipitation_unit=inch&timezone=America%2FDenver";
// timerDelay is to set how often we get data from the API; we don't want to get banned!; timer set to 5 minutes(300000 ms)
unsigned long timerDelay = 300000;
// lastTime is initially set to one less than the negative of timerDelay so the data is grabbed right at the start of the code
unsigned long lastTime = -timerDelay - 1;
String JSON_Data;
//initialize variables where we will store the data from the API
int temp;
int rh;
int real_feel;
int wind_speed;
float rain;
int cloud_cover;
//this will help us determine if we successfully connected to the API
int success;
// set the LCD number of columns and rows and lcd address
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {
  //Wifi stuff
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.begin(115200);
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  //connect LCD to esp32, initialize LCD and turn on backlight
  Wire.begin(33, 32);
  lcd.init();
  lcd.backlight();
}

void loop() {
  //this first if statement will run the code every X number of seconds
  if (millis() - lastTime > timerDelay) {
    //wait for wifi to connect
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      //Set HTTP Request URL
      http.begin(URL);
      //start connection and send HTTP request
      int httpCode = http.GET();
      //this if statement only works if things are good (i.e., httpCode will be negative if there is error)
      if (httpCode > 0) {
        //Gets the data from the API (as a JSON file)
        JSON_Data = http.getString();
        //deserialize the JSON data; make it usable by the ArduinoJson library
        JsonDocument doc;
        deserializeJson(doc, JSON_Data);
        //Get data from JsonDocument
        JsonObject current = doc["current"];
        JsonObject daily = doc["daily"];
        temp = current["temperature_2m"];
        rh = current["relative_humidity_2m"];
        real_feel = current["apparent_temperature"];
        wind_speed = current["wind_speed_10m"];
        rain = current["rain"];
        cloud_cover = current["cloud_cover"];
        success = 1;  //it worked!
      } else {
        success = 0;  //it didn't work :(
      }
      http.end();
    }
    lastTime = millis();
  }
  //if the esp32 successfully connected to wifi and got data from the API...
  Serial.println(success);
  if (success == 1) {
    //clear the lcd screen
    lcd.clear();
    //start typing on lcd screen in the top left corner
    lcd.setCursor(0, 0);
    lcd.print("Temp = ");
    lcd.print(temp);
    lcd.print(" F");
    //start typing on lcd screen in the bottom left corner
    lcd.setCursor(0, 1);
    lcd.print("RealFeel = ");
    lcd.print(real_feel);
    lcd.print(" F");
    delay(4000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Humidity = ");
    lcd.print(rh);
    lcd.print(" %");
    lcd.setCursor(0, 1);
    lcd.print("Wind = ");
    lcd.print(wind_speed);
    lcd.print(" mph");
    delay(4000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Rain = ");
    lcd.print(rain);
    lcd.print(" in");
    lcd.setCursor(0, 1);
    lcd.print("CloudCover = ");
    lcd.print(cloud_cover);
    lcd.print("%");
    delay(4000);
  }
  //if the esp32 DID NOT connect to wifi and got data from the API...
  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Can't Get Data!");
    delay(300000);  //wait 5 minutes
  }
}