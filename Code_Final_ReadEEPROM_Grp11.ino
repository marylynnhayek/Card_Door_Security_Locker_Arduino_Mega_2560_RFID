//Partie Jean
#include <EEPROM.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 49

int eeAddress = 0;
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {

  Serial.begin(9600);
   SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
if (mfrc522.PICC_IsNewCardPresent())
    {
      if ( mfrc522.PICC_ReadCardSerial())
      {
        Serial.print("Tag UID:");
        for (byte i=0; i< mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
          Serial.print(mfrc522.uid.uidByte[i], HEX);
          //id=(mfrc522.uid.uidByte[i]);
          EEPROM.put(eeAddress, mfrc522.uid.uidByte[i]);
          eeAddress=eeAddress+1;
        }
        
        EEPROM.put(eeAddress, 250);
        delay(10000);
      }
    }
}
