#include <LiquidCrystal.h>
LiquidCrystal lcd(13,12,11,10,9,8);
#include <Servo.h>
#include <EEPROM.h>
int objectSensor = 3;
int sealrelay=2;
int hopperrelay=7;
long int count = EEPROM.read(0);;
int servo1 = 4;
Servo myservo1;

void setup() {
  //EEPROM.write(0, 0);
  myservo1.attach(4);  // attaches the servo on pin 9 to the servo object
  myservo1.write(45);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("     PACKING    ");
  lcd.setCursor(0,1);
  lcd.print("      MACHINE   ");
  pinMode(objectSensor,INPUT);
  pinMode(sealrelay,OUTPUT);
  pinMode(hopperrelay,OUTPUT);
  digitalWrite(hopperrelay,LOW);
  delay(1000);
}

void loop() {
  digitalWrite(hopperrelay,HIGH);
  //lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("BAG FILL& PACKED");
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(5,1);
  lcd.print(count);
  if(digitalRead(objectSensor)==HIGH)
  {
    digitalWrite(hopperrelay,LOW);
    while(digitalRead(objectSensor)==HIGH);
    count=count+1;
    EEPROM.write(0, count);
    lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(5,1);
  lcd.print(count);
  delay(2000);
     myservo1.write(0);
     delay(1000);
     delay(1000);
     digitalWrite(sealrelay,HIGH);
     delay(5000);
     digitalWrite(sealrelay,LOW);
     delay(1000);     
     myservo1.write(45);     
  }
  delay(100);
}
