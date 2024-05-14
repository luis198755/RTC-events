#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

RTC_DS3231 rtc;

const int eventHours[8]   = {1, 4, 7, 10, 12, 12, 19, 22}; // Event times in 24-hour format
const int eventMinutes[8] = {0, 0, 0,  0, 35, 40,  0,  0}; // Event minutes
const int eventSeconds[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Event seconds
bool eventTriggered[8] = {false, false, false, false, false, false, false, false};

void handleEvent(int eventIndex);
void printTimestamp(DateTime now);

void setup() {
  Serial.begin(9600);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // Uncomment and set the date and time for the first run
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the date and time to the time this sketch was compiled
    // rtc.adjust(DateTime(2024, 5, 15, 12, 0, 0)); // Set the date and time manually: Year, Month, Day, Hour, Minute, Second
  }

  // Retrieve the last triggered event index from EEPROM
  int lastEventIndex = EEPROM.read(0);
  if (lastEventIndex >= 0 && lastEventIndex < 8) {
    eventTriggered[lastEventIndex] = true;
  }

  for (int i = 0; i<8; i++) {
    Serial.println(eventTriggered[i]);
    if (eventTriggered[i] == true) {
      handleEvent(i);
    }
  }
}

void loop() {
  DateTime now = rtc.now();

  for (int i = 0; i < 8; i++) {
    if (now.hour() == eventHours[i] && now.minute() == eventMinutes[i] && now.second() == eventSeconds[i] && !eventTriggered[i]) {
      handleEvent(i);
      eventTriggered[i] = true;
      // Store the last triggered event index in EEPROM
      EEPROM.write(0, i);
    }
  }

  // Reset eventTriggered array at midnight
  if (now.hour() == 0 && now.minute() == 0 && now.second() == 0) {
    for (int j = 0; j < 8; j++) {
      eventTriggered[j] = false;
    }
    // Clear the last event index from EEPROM at midnight
    EEPROM.write(0, 255); // 255 is an invalid event index
  }

  delay(1000);
}

void handleEvent(int eventIndex) {
  //DateTime now = rtc.now();
  //printTimestamp(now);

  switch (eventIndex) {
    case 0:
      Serial.println("Event 0: It's 1:00 AM, start your day with a positive thought!");
      break;
    case 1:
      Serial.println("Event 1: It's 4:00 AM, time for an early morning workout!");
      break;
    case 2:
      Serial.println("Event 2: It's 7:00 AM, breakfast is the most important meal of the day.");
      break;
    case 3:
      Serial.println("Event 3: It's 10:00 AM, take a short break and stretch.");
      break;
    case 4:
      Serial.println("Event 4: It's 1:00 PM, time for lunch!");
      break;
    case 5:
      Serial.println("Event 5: It's 4:00 PM, an afternoon coffee might be nice.");
      break;
    case 6:
      Serial.println("Event 6: It's 7:00 PM, start winding down your day.");
      break;
    case 7:
      Serial.println("Event 7: It's 10:00 PM, time to get ready for bed.");
      break;
    default:
      Serial.println("Unknown event.");
      break;
  }
}

void printTimestamp(DateTime now) {
  Serial.print("Event at: ");
  Serial.print(now.year(), DEC);
  Serial.print("-");
  Serial.print(now.month(), DEC);
  Serial.print("-");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.println();
}
