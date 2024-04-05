#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int irSensors[6] = {5, 6, 7, 8, 9, 10}; // IR sensor pins for parking slots
int irGateEntry = 11; // IR sensor pin for gate entry
int irGateExit = 12; // IR sensor pin for gate exit
bool slots[6]; // parking slots status
int slot = 6;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 6; i++) {
    pinMode(irSensors[i], INPUT);
    slots[i] = false;
  }
  pinMode(irGateEntry, INPUT);
  pinMode(irGateExit, INPUT);

  myservo.attach(3);
  myservo.write(90);

  lcd.begin(20, 4);
  lcd.setCursor(0, 0); // Set cursor to row 0
  lcd.print("Available Slots:");

  Read_Sensor();

  int total = slots[0] + slots[1] + slots[2] + slots[3] + slots[4] + slots[5];
  slot = slot - total;
}

void loop() {
  Read_Sensor();
  lcd.setCursor(0, 1); // Set cursor to row 1
  for (int i = 0; i < 6; i++) {
    lcd.print("Slot");
    lcd.print(i + 1);
    lcd.print(slots[i] ? ": Filled " : ": Empty ");
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
    if (slots[i] == false) {
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
