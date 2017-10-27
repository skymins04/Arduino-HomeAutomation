/*
  2017.09.19 ~ 2017.10.18 송도중학교 소프트웨어 동아리(소프트아이스크림)
  프로젝트 주제: 평범한 집을 스마트 하우스로 바꾸는 IoT의 마법 - 아두이노 홈오토메이션 시스템 구축
  
  1 팀
  
  팀장: 강민수 (10201)
  팀원: 안준서 (30313)
        장원영 (10417)
        한상현 (20220)

  - H/W 개발 참여: 강민수, 장원영, 한상현
  - S/W 개발 참여: 강민수, 안준서

  -작성자: 강민수-

  아두이노 우노(서브) 용 소스
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//define constant

#define DAT1 2 // 74hc595, 1
#define SCK1 4
#define LAT1 3

#define AIR_COOLER_R 0x80 // 74hc595, 1 - Q
#define AIR_COOLER_B 0x40 
#define WATER 0x20
#define AIR_CLEANER_B 0x10
#define AIR_CLEANER_O 0x08
#define DOOR_R 0x04
#define DOOR_G 0x02

#define DAT2 11 // 74hc595, 2
#define SCK2 13
#define LAT2 12

#define KITCHEN_L 0x80 // 74hc595, 2 - Q
#define LIVING_L 0x40
#define ROOM1_L 0x20
#define ROOM2_L 0x10
#define TOILET_L 0x08
#define ALL_L 0xF8
#define BZ 0x04

#define LIVING_IR 5 // Digital Sensors
#define ROOM1_IR 6
#define ROOM2_IR 7
#define MARGNET 8
#define DHTPIN 9
#define FIRE 10

#define CDS 0 // Analog Sensor

#define MY_ADDR 0x04 // I2C Address

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//include the Header Files

#include <Wire.h>
#include <DHT.h>
#define DHTTYPE DHT11  // DHT타입을 DHT11로 정의한다
DHT dht(DHTPIN, DHTTYPE);  // DHT설정 - dht (디지털2, dht11)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Variables

byte shift1 = 0, shift2 = 0;
byte Sen[8] = {0, };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.begin(9600);
  Serial.println("hello");
  Wire.begin(MY_ADDR);
  Serial.println("world!!");

  Wire.onReceive(readFromMaster);
  Wire.onRequest(sendValue);
  
  // Set Pin Mode
  for(int i = 2; i < 5; i++) { // 74hc595, 1
    pinMode(i, OUTPUT);  
  }
  for(int i = 11; i < 14; i++) { // 74hc595, 2
    pinMode(i, OUTPUT);  
  }
  for(int i = 5; i < 8; i++) { // Sensors
    pinMode(i, INPUT);  
  }
  pinMode(DHTPIN, INPUT);
  pinMode(FIRE, INPUT); // flame sensor  
}

void loop() {
  dht.readTemperature();
  dht.readHumidity(); 
  Shift595out();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Functions

void readFromMaster(int bytes){
  byte x = (byte)Wire.read();    // receive byte as an integer
  byte y = (byte)Wire.read();
  shift1 = x;
  shift2 = y;
}

void sendValue(){
  SenReset();
  for(int i = 0; i < 8; i++){
    Wire.write(Sen[i]);
  }
}

void Shift595out(vifoid){
  digitalWrite(LAT1, LOW); // 74hc595, 1
  shiftOut(DAT1, SCK1, LSBFIRST, shift1);
  digitalWrite(LAT1, HIGH);  
  digitalWrite(LAT2, LOW); // 74hc595, 2
  shiftOut(DAT2, SCK2, LSBFIRST, shift2);
  digitalWrite(LAT2, HIGH);  
}

void SenReset(void){
  Sen[0] = digitalRead(LIVING_IR);
  Sen[1] = digitalRead(ROOM1_IR);
  Sen[2] = digitalRead(ROOM2_IR);
  Sen[3] = digitalRead(MARGNET);
  Sen[4] = dht.readTemperature();
  Sen[5] = dht.readHumidity();  
  Sen[6] = digitalRead(FIRE);   
  Sen[7] = analogRead(CDS);     
}
