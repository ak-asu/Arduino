// Enter your following BLYNK template values here
#define BLYNK_TEMPLATE_ID "XXX"
#define BLYNK_TEMPLATE_NAME "XXX"
#define BLYNK_AUTH_TOKEN "XXX"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Enter your WiFi credentials.
char ssid[] = "";
char pass[] = "";

//this tells the esp32 that the LEDs are connected to these specific pins
int redPin = 27;
int greenPin = 26;
int bluePin = 25;

int redValue;
int greenValue;
int blueValue;

//this sets the Values equal to the number obtained from the web dashboard
// The 255- depends on whether the LED is cathode or anode type
BLYNK_WRITE(V0) {
  redValue = 255 - param.asInt();
}
BLYNK_WRITE(V1) {
  greenValue = 255 - param.asInt();
}
BLYNK_WRITE(V2) {
  blueValue = 255 - param.asInt();
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
