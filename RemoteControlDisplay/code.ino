#include <IRremote.h>

// Define IR receiver pin
const int IR_RECEIVER_PIN = 11;

// 7-Segment display pin mapping
const int segmentPins[] = {2, 3, 4, 5, 6, 7, 8}; // A, B, C, D, E, F, G
// Digit to 7-segment encoding (common cathode)
const byte digitCodes[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

unsigned long irCodes[10];
int currentMode = 0;       // 0: Initialization mode, 1: Display mode
int currentDigit = 0;      // Tracks the current digit being initialized

void setup() {
  // Initialize IR receiver
  IrReceiver.begin(IR_RECEIVER_PIN);

  // Set 7-segment pins as output
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
  for (int i = 0; i < 10; i++) {
    irCodes[i] = -1;
  }

  Serial.begin(9600);
}

void loop() {
  if (currentMode==0) {
    displayDigit(currentDigit);
  }
  if (IrReceiver.decode()) {
    // Print the received IR code
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    //Serial.println(code, HEX);

    if (currentMode == 0) {
      // Display the current digit on the 7-segment display
      displayDigit(currentDigit);
      if (irCodes[currentDigit] == -1 && code != 0) {
        // Store the received code for the current digit
        irCodes[currentDigit] = code;

        // Notify the user via Serial Monitor
        Serial.print("Stored code for digit ");
        Serial.print(currentDigit);
        Serial.print(": ");
        Serial.println(code);

        // Move to the next digit after storing the current one
        currentDigit++;
        
        // If all digits have been initialized, switch to display mode
        if (currentDigit >= 10) {
          currentMode = 1;  // Switch to display mode
          Serial.println("Initialization complete. Entering display mode.");
        }
      }
    } else {
      Serial.println(code);
      if (code != 0) {
        for (int i = 0; i < 10; i++) {
          if (code == irCodes[i]) {
            // If the received code matches a stored digit code, display the digit
            Serial.print("Digit pressed: ");
            Serial.println(i);
            displayDigit(i);  // Call function to display the digit on your display
            break;
          }
        }
      }
    }

    // Resume the receiver
    IrReceiver.resume();
  }
}

// Function to display a digit on the 7-segment display
void displayDigit(int digit) {
  byte c = digitCodes[digit];
  for (int i = 0; i < 7; i++) {
    //Serial.println(bitRead(code, i));
    if (bitRead(c, i) == 1) {
      digitalWrite(segmentPins[i], HIGH);
    } else {
      digitalWrite(segmentPins[i], LOW);
    }
  }
}
