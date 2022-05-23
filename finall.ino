//Group 11: Card door security locker
#include <EEPROM.h>
#include <MFRC522.h>
#include <LiquidCrystal.h> //For the LCD
#include <SPI.h>

#define SS_PIN 53
#define RST_PIN 49

//PINs to define
#define Green_LED 22
#define Red_LED 23
#define RELAY 47

double max_t = 10000;

String status;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int eeAddress = 0;
MFRC522 mfrc522(SS_PIN, RST_PIN);
String uid;

//String UID[100];
//Global variables
String ID[100];
int counter=0;
int fcount=0;
double valid_table[5];

unsigned long firstFailTime; //This is the first failed attempt to unlock the door

void setup() {
  //Partie Mary-Lynn
  valid_table[fcount]=millis();
  // Put your setup code here, to run once:
  byte value;
  Serial.begin(9600);

  lcd.begin(16,2);
  lcd.print("Welcome!");

//Setting up our I/O pins:
pinMode(Green_LED, OUTPUT);  //Green LED
pinMode(Red_LED, OUTPUT);   //Red LED
pinMode(RELAY, OUTPUT);     //Relay that controls the solenoid lock

digitalWrite(RELAY, HIGH);
delay(3000);
digitalWrite(RELAY, LOW);

//Partie Jean
for(int i=0; i<250; i++)
{
  delay(10);
  Serial.println(EEPROM.read(i));
  delay(10);
  if(EEPROM.read(i)!=250)
  {
      ID[counter] = ID[counter] + EEPROM.read(i);
  }
  if(EEPROM.read(i)==250)
  {
    counter = counter+1;
  }
  if(EEPROM.read(i)==255)
  {
    i=250;
  }

}
//Partie Jean
for(int i=0; i<=counter; i++)
{
Serial.println(ID[i]);
}

SPI.begin();
mfrc522.PCD_Init();
delay(5000);
}

void loop() {
  //Partie Jean
  while(fcount<=2)
  {
    mfrc522.PCD_Init();
  if (mfrc522.PICC_IsNewCardPresent())
    {
      if ( mfrc522.PICC_ReadCardSerial())
      {
        Serial.print("Tag UID:");
        for (byte i=0; i< mfrc522.uid.size; i++) 
        {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
          uid= uid + (mfrc522.uid.uidByte[i]);
          //id=(mfrc522.uid.uidByte[i]);
          eeAddress=eeAddress+1;
        }
        Serial.println("uid: "+ uid);
        delay(500);

        for (int cnt=0; cnt < 1; cnt++)
        {
          if(ID[cnt] == uid)
          {
            //Initial null values: Partie Mary-Lynn
            for(int i=0; i<5; ++i)
            {
              valid_table[i]=0;
            }

//Reset failes_attempts counter:
            fcount=0;
            
            Serial.print("Accessible!");
            status="Accessible!";
                      
//Partie Mary-Lynn
            digitalWrite(RELAY, HIGH); //Unlock the solenoid lock
            digitalWrite(Green_LED, HIGH);  //Turn on green LED
            lcd.setCursor(0, 1);
            lcd.print("Access granted!");

            delay(7000);

            //Restore to initial state:
            digitalWrite(RELAY, LOW); //To control solenoid lock
            digitalWrite(Green_LED, LOW); //Turn off green LED
            lcd.clear();
          }
//Partie jean
          if(status != "Accessible!")
          {
              status="Not Valid!";
              lcd.setCursor(0, 1);
//Partie Mary-Lynn
              lcd.print("Not Valid!");
              digitalWrite(Red_LED, HIGH); // Turn on the Red LED
              delay(1000);
              digitalWrite(Red_LED, LOW); //Turn off the Red LED
              lcd.clear();
              valid_table[fcount]=millis();
              fcount++;
          }
        }
              Serial.print(fcount);
              Serial.println(status);
              status="";
              uid="";
        
              delay(5000);
      }
    }
//Partie Jean
       while(fcount>=3)
       {
          if (mfrc522.PICC_IsNewCardPresent())
          {
          if ( mfrc522.PICC_ReadCardSerial())
            {
              Serial.print("Tag UID:");
              for (byte i=0; i< mfrc522.uid.size; i++) 
               {
                    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " ");
                    uid= uid + (mfrc522.uid.uidByte[i]);
                    //id=(mfrc522.uid.uidByte[i]);
                    eeAddress=eeAddress+1;
               }
              Serial.println("uid: " + uid);
              delay(100);
            }

//Partie Mary-Lynn
            //Reset all values: 
            if(valid_table[1]-valid_table[0]>max_t)
            {
              fcount=0; //Reset counter
            }
            
            else
            {
              if(valid_table[2]-valid_table[1]<max_t)
              {
                //Call admin
                if (uid!="208223643")
                {
                  Serial.println("Action Blocked! Call Admin!");
                  lcd.print("Action Blocked! Call Admin!");
                  uid="";
                  fcount++;
                  delay(1500);
                }

               else if (uid="208223643")
                 {
                    Serial.print("Done!");
                    lcd.print("Done!");
                    
                    uid="";
                    delay(500);

                    digitalWrite(RELAY, HIGH); //Unlock the solenoid lock
                    digitalWrite(Green_LED, HIGH);  //Turn on green LED
                    lcd.setCursor(0, 1);
                    lcd.print("Access granted");

                    delay(700);

                     //Restore to initial state:
                     digitalWrite(RELAY, LOW); //To control solenoid lock
                     digitalWrite(Green_LED, LOW); //Turn off green LED
                     lcd.clear();

                    //Reset les valeurs de notre table:
                     for(int i=0; i<5; ++i)
                     {
                        valid_table[i]=0;
                     }
                     fcount=0;
                }
            }
            else
            {
                fcount=0;
              
            }
          }
        
        }
      }
    }
  }
