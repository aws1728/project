#include <Servo.h>

#define Moving_delay 25
#define Address_Max 100
#define servoPin_A 7
#define servoPin_B 6
#define servoPin_C 5
#define servoPin_D 4
#define servoPin_E 3
#define servoPin_F 2

int PosSave [Address_Max + 1][6];
int LastPos [6];
int WriteAddress = 0;
int ReadAddress = 0;
int StepSize = 1;
int StepTime = 0;
int Position_A = 90;
int Position_B = 0; 
int Position_C = 180;
int Position_D = 0;
int Position_E = 90;
int Position_F = 90;
int MovingTime_A = 0;
int MovingTime_B = 0;
int MovingTime_C = 0;
int MovingTime_D = 0;
int MovingTime_E = 0;
int MovingTime_F = 0;
int WaitTime = 0;
int Commed = 0;
char receivedData[50];

Servo servo_A;
Servo servo_B;
Servo servo_C;
Servo servo_D;
Servo servo_E;
Servo servo_F;


void setup() {
  Serial.begin(9600);
  Serial.println("START");

  Servo_on();

  Set_Servo(Position_A, Position_B, Position_C, Position_D, Position_E, Position_F);
  
  delay(10);
  Serial.println("READY");
}

void loop() {

  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', receivedData, sizeof(receivedData));
    receivedData[bytesRead] = '\0';

    // 解析數據
    sscanf(receivedData, "%d,%d,%d,%d,%d,%d,%d", &Position_A, &Position_B, &Position_C, &Position_D, &Position_E, &Position_F, &Commed);
    Serial.print("A:" + String(Position_A) + " ");
    Serial.print("B:" + String(Position_B) + " ");
    Serial.print("C:" + String(Position_C) + " ");
    Serial.print("D:" + String(Position_D) + " ");
    Serial.print("E:" + String(Position_E) + " ");
    Serial.print("F:" + String(Position_F) + " ");
    Serial.println("Commed:" + String(Commed));
  
    if (Commed == 0) {
      //Serial.println("Commed == 0");
      
      //int CurrentPosition_A = servo_A.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_A = LastPos[0];
      moveToTargetPosition_A(CurrentPosition_A, Position_A, StepSize);
      
      //int CurrentPosition_B = servo_B.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_B = LastPos[1];
      moveToTargetPosition_B(CurrentPosition_B, Position_B, StepSize);
    
      //int CurrentPosition_C = servo_C.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_C = LastPos[2];
      moveToTargetPosition_C(CurrentPosition_C, Position_C, StepSize);
    
      //int CurrentPosition_D = servo_D.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_D = LastPos[3];
      moveToTargetPosition_D(CurrentPosition_D, Position_D, StepSize);
    
      //int CurrentPosition_E = servo_E.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_E = LastPos[4];
      moveToTargetPosition_E(CurrentPosition_E, Position_E, StepSize);
    
      //int CurrentPosition_F = servo_F.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_F = LastPos[5];
      moveToTargetPosition_F(CurrentPosition_F, Position_F, StepSize);
      }
    
    else if (Commed == 1) {
      //Serial.println("Commed == 1");
      
      //int CurrentPosition_A = servo_A.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_A = LastPos[0];
      moveToTargetPosition_A(CurrentPosition_A, Position_A, StepSize);
      
      //int CurrentPosition_B = servo_B.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_B = LastPos[1];
      moveToTargetPosition_B(CurrentPosition_B, Position_B, StepSize);
    
      //int CurrentPosition_C = servo_C.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_C = LastPos[2];
      moveToTargetPosition_C(CurrentPosition_C, Position_C, StepSize);
    
      //int CurrentPosition_D = servo_D.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_D = LastPos[3];
      moveToTargetPosition_D(CurrentPosition_D, Position_D, StepSize);
    
      //int CurrentPosition_E = servo_E.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_E = LastPos[4];
      moveToTargetPosition_E(CurrentPosition_E, Position_E, StepSize);
    
      //int CurrentPosition_F = servo_F.read();  //讀取伺服馬達的當前位置
      int CurrentPosition_F = LastPos[5];
      moveToTargetPosition_F(CurrentPosition_F, Position_F, StepSize);
      
      PosSave [WriteAddress][0] = Position_A;
      PosSave [WriteAddress][1] = Position_B;
      PosSave [WriteAddress][2] = Position_C;
      PosSave [WriteAddress][3] = Position_D;
      PosSave [WriteAddress][4] = Position_E;
      PosSave [WriteAddress][5] = Position_F;
      
      Serial.print("Write_Address: " + String(WriteAddress) + " ");
      Serial.print("A:" + String(Position_A) + " ");
      Serial.print("B:" + String(Position_B) + " ");
      Serial.print("C:" + String(Position_C) + " ");
      Serial.print("D:" + String(Position_D) + " ");
      Serial.print("E:" + String(Position_E) + " ");
      Serial.println("F:" + String(Position_F) + " ");
      Commed = 0;
      WriteAddress++;
      }
    else if (Commed == 2 && WriteAddress > 0) {
        //Serial.println("Commed == 2");
        
        for (int i = 0; i < WriteAddress; i++) {
          ReadAddress = i;
          //int CurrentPosition_A = servo_A.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_A = LastPos[0];
          MovingTime_A = moveToTargetPosition_A(CurrentPosition_A, PosSave [ReadAddress][0], StepSize);
          
          //int CurrentPosition_B = servo_B.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_B = LastPos[1];
          MovingTime_B = moveToTargetPosition_B(CurrentPosition_B, PosSave [ReadAddress][1], StepSize);
        
          //int CurrentPosition_C = servo_C.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_C = LastPos[2];
          MovingTime_C = moveToTargetPosition_C(CurrentPosition_C, PosSave [ReadAddress][2], StepSize);
        
          //int CurrentPosition_D = servo_D.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_D = LastPos[3];
          MovingTime_D = moveToTargetPosition_D(CurrentPosition_D, PosSave [ReadAddress][3], StepSize);
        
          //int CurrentPosition_E = servo_E.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_E = LastPos[4];
          MovingTime_E = moveToTargetPosition_E(CurrentPosition_E, PosSave [ReadAddress][4], StepSize);
        
          //int CurrentPosition_F = servo_F.read();  //讀取伺服馬達的當前位置
          int CurrentPosition_F = LastPos[5];
          MovingTime_F = moveToTargetPosition_F(CurrentPosition_F, PosSave [ReadAddress][5], StepSize);
    
          WaitTime = Moving_Max(MovingTime_A, MovingTime_B, MovingTime_C, MovingTime_D, MovingTime_E, MovingTime_F);
          
          Serial.print("Read_Address: " + String(ReadAddress) + " ");
          Serial.print("A:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.print("B:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.print("C:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.print("D:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.print("E:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.print("F:" + String(PosSave [ReadAddress][0]) + " ");
          Serial.println("WaitTime:" + String(WaitTime) + "+" + String(StepTime));
          WaitTime = WaitTime + StepTime;
          delay(WaitTime);
          }
        ReadAddress = 0;
        Commed = 0;
    }  
  }
}

void Servo_on() {
  // 将伺服馬達連接到相對的引脚
  servo_A.attach(servoPin_A);  //將伺服馬達 A 連接到引脚 7
  servo_B.attach(servoPin_B);  //將伺服馬達 B 連接到引脚 6
  servo_C.attach(servoPin_C);  //將伺服馬達 C 連接到引脚 5
  servo_D.attach(servoPin_D);  //將伺服馬達 D 連接到引脚 4
  servo_E.attach(servoPin_E);  //將伺服馬達 E 連接到引脚 3
  servo_F.attach(servoPin_F);  //將伺服馬達 F 連接到引脚 2
}

void Set_Servo(int Position_A, int Position_B, int Position_C, int Position_D, int Position_E, int Position_F) {
  servo_A.write(Position_A);  // 設定初始位置
  LastPos[0] = Position_A;
  servo_B.write(Position_B);  // 設定初始位置
  LastPos[1] = Position_B;
  servo_C.write(Position_C);  // 設定初始位置
  LastPos[2] = Position_C;
  servo_D.write(Position_D);  // 設定初始位置
  LastPos[3] = Position_D;
  servo_E.write(Position_E);  // 設定初始位置
  LastPos[4] = Position_E;
  servo_F.write(Position_F);  // 設定初始位置 
  LastPos[5] = Position_F;
}

int Moving_Max(int MovingTime_A, int MovingTime_B, int MovingTime_C, int MovingTime_D, int MovingTime_E, int MovingTime_F){
  int Time_Max = MovingTime_A;
  if (MovingTime_B > Time_Max) {
    Time_Max = MovingTime_B;
    }
  if (MovingTime_C > Time_Max) {
    Time_Max = MovingTime_C;
    }
  if (MovingTime_D > Time_Max) {
    Time_Max = MovingTime_D;
    }
  if (MovingTime_E > Time_Max) {
    Time_Max = MovingTime_E;
    }
  if (MovingTime_F > Time_Max) {
    Time_Max = MovingTime_F;
    }
  return Time_Max;
}

int moveToTargetPosition_A(int CurrentPosition, int TargetPosition, int StepSize) {
  //CurrentPosition = constrain(CurrentPosition, 0, 180);
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_A.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("AF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_A.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("AR ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[0] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}

int moveToTargetPosition_B(int CurrentPosition, int TargetPosition, int StepSize) {
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_B.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("BF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_B.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("BR ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[1] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}

int moveToTargetPosition_C(int CurrentPosition, int TargetPosition, int StepSize) {
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_C.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("CF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_C.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("CR ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[2] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}

int moveToTargetPosition_D(int CurrentPosition, int TargetPosition, int StepSize) {
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_D.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("DF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_D.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("DR ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[3] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}

int moveToTargetPosition_E(int CurrentPosition, int TargetPosition, int StepSize) {
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_E.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("EF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_E.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("ER ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[4] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}

int moveToTargetPosition_F(int CurrentPosition, int TargetPosition, int StepSize) {
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    servo_F.write(CurrentPosition + StepSize); // 逼近目標位置
    Serial.print("FF ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  else if (CurrentPosition > TargetPosition) {
    servo_F.write(CurrentPosition - StepSize); // 逼近目標位置
    Serial.print("FR ");
    delay(Moving_delay); // 可選的延遲，以控制移動速度
    }
  LastPos[5] = TargetPosition;
  Serial.print(CurrentPosition);
  Serial.print(" ");
  Serial.print(TargetPosition);
  Serial.print(" ");
  Serial.println(Moving);
  return Moving;
}
