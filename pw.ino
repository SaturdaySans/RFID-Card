/*
Adapted from a public Arduino + MFRC522 tutorial (see Annex B):
https://www.instructables.com/Arduino-MFRC522-RFID-READER/

What we adapted from the original tutorial:
- Variable and function names were renamed to match our
  pws terminology
- The tutorial's efficient memcmp()-based UID comparison and
  multi-card array structure were kept, since they are a
  cleaner way
*/

/*
Smart Priority Seat System code prototype i guess

Hardware:
  - Arduino 
  - RC522 RFID reader module (13.56 MHz, SPI)
  - 1x LED (amber, with 220 ohm resistor) -> priority seat indicator plaecholder
  - 1x Buzzer -> audio cue


other misc

Wiring:
  SDA: D10
  SCK: D13
  MOSI: D11
  MISO: D12
  IRQ: not connected
  GND: GND
  RST: D9
  3.3V: 3.3V

Other wiring:
  LED long leg (anode)  -> D7, through a 220 ohm resistor
    short leg (cathode) -> GND
  Buzzer +  -> D6
  Buzzer -  -> GND
*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN  10
MFRC522 rfid(SS_PIN, RST_PIN);

#define LED_PIN  7   //LED [priority seat signal]
#define BUZZ_PIN 6   // audio cue

// UIDs of registered cards
byte registeredCards[][4] = {
  {0xDE, 0xAD, 0xBE, 0xEF},   //card 1 (example so ts is hardcoded)
  {0x12, 0x34, 0x56, 0x78}    //card 2
};
const int NUM_REGISTERED_CARDS = 2;

byte scannedUID[4]; // stores the UID of the card currently being read

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);

  Serial.println(F("Registered elderly commuter cards:"));
  for (int i=0; i<NUM_REGISTERED_CARDS; i++) {
    Serial.print(i+1);
    Serial.print("  ");
    for (int j=0; j<4; j++) {
      Serial.print(registeredCards[i][j], HEX);
    }
    Serial.println();
  }

  Serial.println(F("Tap a card on reader"));
}

void loop() {
  if (!readCard()) return;

  bool isRegistered = false;
  for (int i=0; i<NUM_REGISTERED_CARDS; i++) {
    if (!memcmp(scannedUID, registeredCards[i],4)) {
      isRegistered=true;
      break;
    }
  }

  if (isRegistered) {
    Serial.println(F("Registered card"));
    activateunc();
  } else {
    Serial.println(F("Unregistered Card"));
  }
}

// Reads a new card's UID into scannedUID
bool readCard() {
  if (!rfid.PICC_IsNewCardPresent()) return false;
  if (!rfid.PICC_ReadCardSerial()) return false;

  Serial.print(F("UID read: "));
  for (int i=0; i<4; i++) {
    scannedUID[i] = rfid.uid.uidByte[i];
    Serial.print(scannedUID[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  rfid.PICC_HaltA();
  return true;
}

// led and audio thing
void activateunc() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZ_PIN, 700, 200);
  delay(300);
  tone(BUZZ_PIN, 900, 200);
  delay(4000);
  digitalWrite(LED_PIN, LOW);
}




