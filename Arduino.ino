#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set up the LCD's number of columns and rows

int irSensors[6] = {5, 6, 7, 8, 9, 10}; // IR sensor pins for parking slots
int irGateEntry = 11; // IR sensor pin for gate entry
int irGateExit = 12; // IR sensor pin for gate exit
bool slots[6]; // parking slots status
int slot = 6;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  for (int i = 0; i < 6; i++) {
    pinMode(irSensors[i], INPUT);
    slots[i] = false;
  }
  pinMode(irGateEntry, INPUT);
  pinMode(irGateExit, INPUT);

  myservo.attach(3);
  myservo.write(90);

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0); // Set cursor to row 0
  lcd.print("Avail Slots:");

  Read_Sensor();

  int total = 0;
  for (int i = 0; i < 6; i++) {
    total += slots[i];
  }
  slot -= total;
}

void loop() {
  Read_Sensor();
  lcd.setCursor(0, 1); // Set cursor to row 1
  lcd.clear(); // Clear the display
  for (int i = 0; i < 6; i++) {
    lcd.print("S");
    lcd.print(i + 1);
    lcd.print(slots[i] ? ":F" : ":E");
    lcd.print("  ");
    if (i == 2) {
      lcd.setCursor(0, 1); // Move to the next row after 3 slots
    }
  }

  // Send parking slots status to NodeMCU
  for (int i = 0; i < 6; i++) {
    Serial.print(slots[i] ? "Filled" : "Empty");
    if (i < 5) Serial.print(",");
  }
  Serial.println();
  delay(1000);

  // Control gate based on parking slots status and car presence at the gate
  bool allSlotsFilled = true;
  for (int i = 0; i < 6; i++) {
    if (!slots[i]) {
      allSlotsFilled = false;
      break;
    }
  }
  if (allSlotsFilled || (digitalRead(irGateEntry) == LOW && digitalRead(irGateExit) == HIGH)) {
    myservo.write(0); // Close gate
  } else {
    myservo.write(90); // Open gate
  }
}

void Read_Sensor() {
  for (int i = 0; i < 6; i++) {
    slots[i] = digitalRead(irSensors[i]) == LOW;
  }
}
