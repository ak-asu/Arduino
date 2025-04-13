#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WebServer.h>
#include <ESP_Mail_Client.h>
#include "time.h"

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

#define SMTP_HOST "smtp.mailersend.net"
#define SMTP_PORT 2525

#define AUTHOR_EMAIL ""
#define AUTHOR_PASSWORD ""
#define RECIPIENT_EMAIL ""

#define BUTTON_PIN 25
#define LED_PIN 27
#define BUZZER_PIN 32
#define BUTTON_MODE_PIN 34

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

int blynkValue = 0;
int blink = 0;
int buzzer = 0;
int messageMode = 0;

WebServer server(80);
bool ledState = false;

SMTPSession smtp;
Session_Config config;
SMTP_Message message;

BLYNK_WRITE(V0) {
  blynkValue = param.asInt();
}
BLYNK_WRITE(V1) {
  blink = param.asInt();
}
BLYNK_WRITE(V2) {
  buzzer = param.asInt();
}

void sendEmailWithReplyLink(String link) {
  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";
  message.sender.name = "AK";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "!!DANGER!!";
  message.addRecipient("User", RECIPIENT_EMAIL);
  String body = "I am in danger. Click this link to confirm: <a href='" + link + "'>Confirm</a>";
  message.text.content = "I am in danger. Click this link to confirm: " + link;
  if (messageMode==1) {
    message.subject = "!!LOST!!";
    body = "I am lost. Click this link to confirm: <a href='" + link + "'>Confirm</a>";
    message.text.content = "I am lost. Click this link to confirm: " + link;
  } else if (messageMode==2) {
    message.subject = "!!HUNGRY!!";
    body = "I am hungry. Click this link to confirm: <a href='" + link + "'>Confirm</a>";
    message.text.content = "I am hungry. Click this link to confirm: " + link;
  }
  message.html.content = body;
  smtp.callback([](SMTP_Status status) {
    Serial.println(status.info());
  });
  if (!smtp.connect(&config)) return;
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Email failed");
  }
  smtp.closeSession();
}

void setMessageMode() {
  messageMode = (messageMode + 1) % 3;
}

void handleReply() {
  digitalWrite(LED_PIN, HIGH);
  ledState = true;
  server.send(200, "text/html", "<h1>Confirmed! LED is now ON.</h1>");
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  server.on("/reply", handleReply);
  server.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  Serial.println("HTTP server started");
}

void loop() {
  Blynk.run();
  server.handleClient();
  if (blynkValue==1) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  if (blink==1) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  if (buzzer==0) {
    noTone(BUZZER_PIN);
  } else {
    tone(BUZZER_PIN, buzzer*100);
  }
  static bool lastButton = HIGH;
  bool current = digitalRead(BUTTON_PIN);
  static bool lastModeButton = HIGH;
  bool currentMode = digitalRead(BUTTON_MODE_PIN);
  if (lastButton == HIGH && current == LOW) {
    String ip = WiFi.localIP().toString();
    String link = "http://" + ip + "/reply";
    sendEmailWithReplyLink(link);
    Serial.println("Email sent with confirmation link");
    delay(1000);
  }
  if (lastModeButton == HIGH && currentMode == LOW) {
    setMessageMode();
    Serial.print("Message mode changed to: ");
    Serial.println(messageMode);
    delay(1000); // Debounce delay
  }
  lastModeButton = currentMode;
  lastButton = current;
}