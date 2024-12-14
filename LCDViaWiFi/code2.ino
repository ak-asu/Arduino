//Fill in your specific template/device info in the following three lines
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Enter your WiFi credentials.
const char* ssid = "";
const char* pass = "";

bool animate = true;
bool updated = true;
int row = 0;
char dataArray[2][17] = { "", "" };
BLYNK_WRITE(V3) {
  String input = param.asString();
  if (input == "c") {
    strcpy(dataArray[row], "");
    updated = false;
  } else if (input == "s") {
    row = 1 - row;
  } else if (input == "a") {
    animate = !animate;
  } else {
    updated = false;
    strncpy(dataArray[row], input.c_str(), 16);
    dataArray[row][16] = '\0';
  }
}
// set the LCD number of columns and rows and lcd address
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup() {
  //Wifi stuff
  WiFi.begin(ssid, pass);
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
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  //clear the lcd screen
  lcd.clear();
  for (int j = 0; j < 2; j++) {
    lcd.setCursor(0, j);
    size_t len = strlen(dataArray[j]);
    for (size_t i = 0; i < len; i++) {
      lcd.print(dataArray[j][i]);
      if (animate && row == j && !updated) {
        delay(300);
      }
    }
  }
  updated = true;
  delay(4000);
}