/*
  2017.09.19 ~ 2017.10.00 송도중학교 소프트웨어 동아리(소프트아이스크림)
  프로젝트 주제: 평범한 집을 스마트 하우스로 바꾸는 IoT의 마법 - 아두이노 홈오토메이션 시스템 구축
  
  1 팀
  
  팀장: 강민수 (10201)
  팀원: 안준서 (30313)
        장원영 (10417)
        한상현 (20220)

  - H/W 개발 참여: 강민수, 장원영, 한상현
  - S/W 개발 참여: 강민수, 안준서

  -작성자: 강민수-

  아두이노 우노(메인) 용 소스
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//define constant

//Digital
#define BTTX 2 // Blue Tooth
#define BTRX 3 
#define RTC_CLK 4 // RTC
#define RTC_DAT 5
#define RTC_RST 6
#define GAS_SERVO 7 // Servos
#define DOOR_SERVO 8
#define STEP1 9 // StepMotor
#define STEP2 10
#define STEP3 11
#define STEP4 12
#define Bz 13 // Buzzer

//analog
#define UP_BTN 0 // Butten
#define DOWN_BTN 1
#define SELECT_BTN 2
#define LOCK_BTN 3

//I2C
#define LCD_ADDR 0x27
#define SLAVE_ADDR 0x04

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//include the Header File

#include <SoftwareSerial.h>
#include <DS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Stepper.h>

SoftwareSerial BT(2, 3);
DS1302 rtc(RTC_RST, RTC_DAT, RTC_CLK);
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
Servo gas_servo;
Servo door_servo;
int stepsPerRavolution = 2048;
Stepper curtain(stepsPerRavolution, STEP4, STEP2, STEP3, STEP1);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Variables

char START_LCD[2][16] = {
                          {'#','#','S','m','a','r','t','-','-','H','o','u','s','e','#','#'},
                          {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'}
                        };
char PSWD1_LCD[2][16] = {
                          {'P','r','e','s','s',' ','S','e','l','e','c','t','.','.','.',' '},
                          {'-','-','S','o','f','t','I','c','e','c','r','e','a','m','-','-'}
                        };
char PSWD2_LCD[2][16] = {
                          {'P','S','W','D',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
                          {'-','-','S','o','f','t','I','c','e','c','r','e','a','m','-','-'}
                        };
char PSWD3_LCD[2][16] = {
                          {'U','n','l','o','c','k','.','.','.',' ',' ',' ',' ',' ',' ',' '},
                          {'-','-','S','o','f','t','I','c','e','c','r','e','a','m','-','-'}
                        };
                        
int SenValue[8] = {0, };

byte shift1 = 0;
byte shift2 = 0;

int PassKey[4] = {5, 2, 5, 2};

//flags~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int UnLockFlag = 0;
int GasFlag = 0;
int CurtainFlag = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void setup() {
  Serial.begin(9600); // Hardware Serial
  BT.begin(9600); // Software Serial
  
  rtc.halt(false); // RTC
  rtc.writeProtect(false);
  //rtc.setDOW(SUNDAY);
  //rtc.setDate(15, 10, 2017);
  //rtc.setTime(10, 39, 0);

  Wire.begin(); // Arduino to Arduino I2C - master
  
  lcd.begin(); // LCD
  lcd.clear();
  lcd.backlight();

  gas_servo.attach(7); // Servo
  door_servo.attach(8);
  gas_servo.write(0);
  door_servo.write(0);

  curtain.setSpeed(20); // StepMotor

  // Setting Pin Mode~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  for(int i = 9; i < 13; i++){ // StepMotor Pin
    pinMode(i, OUTPUT);  
  }
  //pinMode(Bz, OUTPUT);
  
  //Start system~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Shift595out();
  UnLockFlag = 1;
  
  printLCD(START_LCD);
  delay(2000);
  printLCD(PSWD1_LCD);
}


void loop() {
  callSensorValue();

  ValueMonitoring();
  
  if(UnLockFlag == 0){
    shift1 = 0;
    shift2 = 0;
    bitSet(shift1, 2);
    bitClear(shift1, 1);
    
    door_servo.write(0);
    
    printLCD(PSWD1_LCD);

    if(analogRead(SELECT_BTN) > 300){
      while(analogRead(SELECT_BTN) > 300) {
        //stop  
      } 
      printLCD(PSWD2_LCD);
      int pswd = 0;
      int total[4] = {0, };
      for(int i = 0; i < 4; i++){
        pswd = 0;
        lcd.setCursor(6+i, 0);
        lcd.print(pswd);
        while(1){
          if(analogRead(UP_BTN) > 300){
            while(analogRead(UP_BTN) > 300) {
              delay(50); 
            } 
            delay(10);
            if(pswd == 9){
              pswd = 0;
              lcd.setCursor(6+i, 0);
              lcd.print(pswd);  
            }
            else {
              pswd += 1;
              lcd.setCursor(6+i, 0);
              lcd.print(pswd);  
            }
          } 
          else if(analogRead(DOWN_BTN) > 300){
            while(analogRead(DOWN_BTN) > 300) {
              delay(50); 
            } 
            delay(10);
            if(pswd == 0){
              pswd = 9;
              lcd.setCursor(6+i, 0);
              lcd.print(pswd);  
            }
            else {
              pswd -= 1;
              lcd.setCursor(6+i, 0);
              lcd.print(pswd);  
            }
          } 
          else if(analogRead(SELECT_BTN) > 300){
            while(analogRead(SELECT_BTN) > 300) {
              delay(50); 
            } 
            delay(10);
            total[i] = pswd;
            break;
          } 
        }
        delay(100);
      }
      int check = 0;
      for(int i = 0; i < 4; i++){
        if(total[i] == PassKey[i]) check += 1;
      }
      if(check == 4){
        UnLockFlag = 1;  
      } else {
        UnLockFlag = 0;  
      }
    }
    
    if(SenValue[3] == 0){ // margnet Sensor - true
      while(SenValue[3] != 1){
        callSensorValue();
        tone(Bz, 2000, 0); 
      } 
    } 
    else if(SenValue[3] == 1) { // margnet Sensor - false
      noTone(Bz);
    }

    if(SenValue[0] == 1){
      while(SenValue[0] != 0){
        callSensorValue();
        tone(Bz, 2000, 0); 
      }  
    }
    else if(SenValue[0] == 0){
      noTone(Bz);
    }
    if(SenValue[1] == 1){
      while(SenValue[1] != 0){
        callSensorValue();
        tone(Bz, 2000, 0); 
      } 
    }
    else if(SenValue[1] == 0){
      noTone(Bz);
    }
    if(SenValue[2] == 1){
      while(SenValue[2] != 0){
        callSensorValue();
        tone(Bz, 2000, 0); 
      } 
    }
    else if(SenValue[2] == 0){
      noTone(Bz);
    }

    if(SenValue[6] == 1){
      if(GasFlag == 0){
        gas_servo.write(90);
        GasFlag = 1; 
      }
    }
    Shift595out();
  }
  else if(UnLockFlag == 1){
    printLCD(PSWD3_LCD);
    gas_servo.write(0);
    GasFlag = 0;
    door_servo.write(70);
    bitSet(shift1, 1);
    bitClear(shift1, 2);
    if(SenValue[0] != 255 && SenValue[1] != 255 && SenValue[2] != 255 && SenValue[3] != 255 && SenValue[4] != 255 && SenValue[5] != 255 && SenValue[6] != 255 && SenValue[7] != 255){
      if(SenValue[4] != 0 && SenValue[5] != 0){
        if(SenValue[7] > 30){ // CDS Sensor - Light ture
          for(int i = 3; i < 8; i++){
            bitClear(shift2, i);  
          }
        } else if(SenValue[7] < 30){
          for(int i = 3; i < 8; i++){ // CDS Sensor - Light false
            bitSet(shift2, i);  
          }
        }
        
        if(SenValue[4] >= 22){ // DHT11 Sensor - T Hot
          bitSet(shift1, 6);
          bitClear(shift1, 7);  
        } else if(SenValue[4] < 22 && SenValue[4] > 19){ // DHT11 Sensor - T Normal
          bitClear(shift1, 6);
          bitClear(shift1, 7);
        } else if(SenValue[4] <= 19){ // DHT11 Sensor - T Cool
          bitSet(shift1, 7);
          bitClear(shift1, 6); 
        }
    
        if(SenValue[5] >= 46){ // DHT11 Sensor - H Damp
          bitSet(shift1, 3);
          bitClear(shift1, 4); 
        } else if(SenValue[5] < 46 && SenValue[5] > 42){ // DHT11 Sensor - H Normal
          bitClear(shift1, 3);
          bitClear(shift1, 4);
        } else if(SenValue[5] <= 42){ // DHT11 Sensor - H Dry
          bitSet(shift1, 4);
          bitClear(shift1, 3);
        }

        if(analogRead(LOCK_BTN) > 300){
          delay(1000);
          UnLockFlag = 0;  
        }
        
        Shift595out(); 
      } 
    } 
  }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//functions

void Shift595out(void){ // send to slave, byte data
  Wire.beginTransmission(SLAVE_ADDR);
  Wire.write(shift1);
  Wire.write(shift2);
  Wire.endTransmission();  
}

void callSensorValue(void){
  Wire.requestFrom(SLAVE_ADDR, 8);
  for(int i = 0; i < 8; i++){
    byte value = Wire.read();
    SenValue[i] = (int)value;
  }
}

void printLCD(char c[2][16]){
  lcd.clear();
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 16; j++){
      lcd.setCursor(j, i);
      lcd.print(c[i][j]);  
    }  
  }  
}

void ValueMonitoring(void){
  String Lable[8] = {"거실 인체 : ", "방1 인체 : ", "방2 인체 : ", "현관문 : ", "온도 : ", "습도 : ", "가스레인지 : ", "밝기 : "};
  for(int i = 0; i < 8; i++){
    Serial.print("#  " + Lable[i] + String(SenValue[i]) + "  #  |  ");
  }
  Serial.print("\n");  
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

