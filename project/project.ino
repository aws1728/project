#include <Servo.h>  //引入伺服馬達的程式庫
#include <Adafruit_NeoPixel.h>  //引入LED的程式庫

#define Moving_delay 25  //定義機械手臂每個位置移動的間隔
#define Address_Max 100  //定義機械手臂最大儲存數量
#define servoPin_A 7  //定義伺服馬達A的腳位
#define servoPin_B 6  //定義伺服馬達B的腳位
#define servoPin_C 5  //定義伺服馬達C的腳位
#define servoPin_D 4  //定義伺服馬達D的腳位
#define servoPin_E 3  //定義伺服馬達E的腳位
#define servoPin_F 2  //定義伺服馬達F的腳位
#define RelayPin 52  //定義繼電器的腳位
#define LEDPin 53  //定義LED的腳位
#define LEDs 8  //定義LED的數量

int PosSave [Address_Max + 1][6];  //宣告一個二維整數陣列，以儲存機械手臂的位置
int LastPos [6];  //宣告一個一維整數陣列，以儲存上一個機械手臂位置
int WriteAddress = 0;  //宣告一個整數變數，以儲存當前寫入的位置
int ReadAddress = 0;  //宣告一個整數變數，以儲存當前讀取的位置
int StepSize = 1;  //宣告一個整數變數，以儲存每個 Moving_delay 所移動的大小，建議等於一
int StepTime = 500;  //宣告一個整數變數，以儲存讀取時兩組動作的間隔
int Position_A = 90;  //宣告一個整數變數，以儲存伺服馬達A將要移動至的位置，目前輸入的是啟動時的位置
int Position_B = 0;   //宣告一個整數變數，以儲存伺服馬達B將要移動至的位置，目前輸入的是啟動時的位置
int Position_C = 180;  //宣告一個整數變數，以儲存伺服馬達C將要移動至的位置，目前輸入的是啟動時的位置
int Position_D = 0;  //宣告一個整數變數，以儲存伺服馬達D將要移動的至位置，目前輸入的是啟動時的位置
int Position_E = 90;  //宣告一個整數變數，以儲存伺服馬達E將要移動至的位置，目前輸入的是啟動時的位置
int Position_F = 90;  //宣告一個整數變數，以儲存伺服馬達F將要移動至的位置，目前輸入的是啟動時的位置
int MovingTime_A = 0;  //宣告一個整數變數，以儲存當前伺服馬達A移動至指定位置所需的時間
int MovingTime_B = 0;  //宣告一個整數變數，以儲存當前伺服馬達B移動至指定位置所需的時間
int MovingTime_C = 0;  //宣告一個整數變數，以儲存當前伺服馬達C移動至指定位置所需的時間
int MovingTime_D = 0;  //宣告一個整數變數，以儲存當前伺服馬達D移動至指定位置所需的時間
int MovingTime_E = 0;  //宣告一個整數變數，以儲存當前伺服馬達E移動至指定位置所需的時間
int MovingTime_F = 0;  //宣告一個整數變數，以儲存當前伺服馬達F移動至指定位置所需的時間
int WaitCheck = 0;  //宣告一個整數變數，以確定所有的伺服馬達移動至指定位置
int Commed = 0;  //宣告一個整數變數，以確定當前機械手臂的工作模式
char receivedData[50];  //宣告一個字符組變數，以解析序列埠輸入的值
int LEDMode = 0;  //宣告一個整數變數，以儲存當前工作模式的LED燈號
int State_A = 1;  //宣告一個整數變數，以儲存伺服馬達A是否移動至指定位置的LED燈號
int State_B = 1;  //宣告一個整數變數，以儲存伺服馬達B是否移動至指定位置的LED燈號
int State_C = 1;  //宣告一個整數變數，以儲存伺服馬達C是否移動至指定位置的LED燈號
int State_D = 1;  //宣告一個整數變數，以儲存伺服馬達D是否移動至指定位置的LED燈號
int State_E = 1;  //宣告一個整數變數，以儲存伺服馬達E是否移動至指定位置的LED燈號
int State_F = 1;  //宣告一個整數變數，以儲存伺服馬達F是否移動至指定位置的LED燈號
int State_R = 1;  //宣告一個整數變數，以儲存繼電器是否啟動的LED燈號
int Relay = 0;  //宣告一個整數變數，以儲存繼電器的狀態

//定義伺服馬達
Servo servo_A;
Servo servo_B;
Servo servo_C;
Servo servo_D;
Servo servo_E;
Servo servo_F;

//定義LED模組
Adafruit_NeoPixel strip( LEDs, LEDPin);

//初始化函式
void setup() {
  pinMode(RelayPin, OUTPUT);  //初始化腳位，定設定為輸出腳位
  digitalWrite(RelayPin, HIGH);  //關閉繼電器
  Serial.begin(9600);  //開啟序列埠，設定波特率為9600
  
  Serial.println("START");  //在序列埠輸出[START]並換行，以確定程式開始執行

  strip.begin();  //啟動LED模組
  strip.setBrightness(1);  //設定LED模組的亮度為 (範圍:0~255)
  strip.clear();  //清除LED模組的顯示

  LEDMode = 1;  //指定當前工作模式的LED燈號為一

  Servo_on();  //呼叫副函式，以初始化伺服馬達

  Set_Servo(Position_A, Position_B, Position_C, Position_D, Position_E, Position_F);  //呼叫副函式，以設定所有伺服馬達的初始位置
  
  delay(10);  //等待10毫秒
  Serial.println("READY");  //在序列埠輸出[READY]並換行，以確定程式初始化完成
}

//主函式，將會無限重複執行
void loop() {
  //呼叫LED模組的副函式
  LED(Relay, Commed, MovingTime_A, MovingTime_B, MovingTime_C, MovingTime_D, MovingTime_E, MovingTime_F);
  //如果Relay等於零，繼電器關閉
  if (Relay == 0) {
    digitalWrite(RelayPin, HIGH);
  }
  //如果Relay等於一，繼電器啟動
  else if (Relay == 1) {
    digitalWrite(RelayPin, LOW);
  }
  
  //判斷序列埠是否有數據輸入
  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', receivedData, sizeof(receivedData));
    receivedData[bytesRead] = '\0';

    //解析序列埠的數據
    sscanf(receivedData, "%d,%d,%d,%d,%d,%d,%d,%d", &Position_A, &Position_B, &Position_C, &Position_D, &Position_E, &Position_F, &Commed, &Relay);
    Serial.print("A:" + String(Position_A) + " ");  //在序列埠輸出[A:(Position_A) ]
    Serial.print("B:" + String(Position_B) + " ");  //在序列埠輸出[B:(Position_B) ]
    Serial.print("C:" + String(Position_C) + " ");  //在序列埠輸出[C:(Position_C) ]
    Serial.print("D:" + String(Position_D) + " ");  //在序列埠輸出[D:(Position_D) ]
    Serial.print("E:" + String(Position_E) + " ");  //在序列埠輸出[E:(Position_E) ]
    Serial.print("F:" + String(Position_F) + " ");  //在序列埠輸出[F:(Position_F) ]
    Serial.print("Commed:" + String(Commed) + " ");  //在序列埠輸出[Commed:(Commed)]
    Serial.println("Relay:" + String(Relay));  //在序列埠輸出[Relay:(Relay)]並換行
  }
  
  //如果Commed等於零，普通模式，移動至指定位置
  if (Commed == 0) {

    int CurrentPosition_A = LastPos[0];  //讀取伺服馬達A的當前位置
    moveToTargetPosition_A(CurrentPosition_A, Position_A, StepSize);  //呼叫伺服馬達A的副函式

    int CurrentPosition_B = LastPos[1];  //讀取伺服馬達B的當前位置
    moveToTargetPosition_B(CurrentPosition_B, Position_B, StepSize);  //呼叫伺服馬達B的副函式

    int CurrentPosition_C = LastPos[2];  //讀取伺服馬達C的當前位置
    moveToTargetPosition_C(CurrentPosition_C, Position_C, StepSize);  //呼叫伺服馬達C的副函式

    int CurrentPosition_D = LastPos[3];  //讀取伺服馬達D的當前位置
    moveToTargetPosition_D(CurrentPosition_D, Position_D, StepSize);  //呼叫伺服馬達D的副函式

    int CurrentPosition_E = LastPos[4];  //讀取伺服馬達E的當前位置
    moveToTargetPosition_E(CurrentPosition_E, Position_E, StepSize);  //呼叫伺服馬達E的副函式

    int CurrentPosition_F = LastPos[5];  //讀取伺服馬達F的當前位置
    moveToTargetPosition_F(CurrentPosition_F, Position_F, StepSize);  //呼叫伺服馬達F的副函式
  }
  
  //如果Commed等於零，寫入模式，移動至指定位置並寫入至陣列儲存
  else if (Commed == 1) {

    int CurrentPosition_A = LastPos[0];  //讀取伺服馬達A的當前位置
    moveToTargetPosition_A(CurrentPosition_A, Position_A, StepSize);  //呼叫伺服馬達A的副函式

    int CurrentPosition_B = LastPos[1];  //讀取伺服馬達B的當前位置
    moveToTargetPosition_B(CurrentPosition_B, Position_B, StepSize);  //呼叫伺服馬達B的副函式

    int CurrentPosition_C = LastPos[2];  //讀取伺服馬達C的當前位置
    moveToTargetPosition_C(CurrentPosition_C, Position_C, StepSize);  //呼叫伺服馬達C的副函式

    int CurrentPosition_D = LastPos[3];  //讀取伺服馬達D的當前位置
    moveToTargetPosition_D(CurrentPosition_D, Position_D, StepSize);  //呼叫伺服馬達D的副函式

    int CurrentPosition_E = LastPos[4];  //讀取伺服馬達E的當前位置
    moveToTargetPosition_E(CurrentPosition_E, Position_E, StepSize);  //呼叫伺服馬達E的副函式

    int CurrentPosition_F = LastPos[5];  //讀取伺服馬達F的當前位置
    moveToTargetPosition_F(CurrentPosition_F, Position_F, StepSize);  //呼叫伺服馬達F的副函式
    
    PosSave [WriteAddress][0] = Position_A;  //將伺服馬達A的位置寫入至該行的第一個元素中
    PosSave [WriteAddress][1] = Position_B;  //將伺服馬達B的位置寫入至該行的第二個元素中
    PosSave [WriteAddress][2] = Position_C;  //將伺服馬達C的位置寫入至該行的第三個元素中
    PosSave [WriteAddress][3] = Position_D;  //將伺服馬達D的位置寫入至該行的第四個元素中
    PosSave [WriteAddress][4] = Position_E;  //將伺服馬達E的位置寫入至該行的第五個元素中
    PosSave [WriteAddress][5] = Position_F;  //將伺服馬達F的位置寫入至該行的第六個元素中
    Commed = 0;  //指定 Commed 為零，以回復至普通模式
    WriteAddress++;  //寫入位置加一，以便下次的寫入
  }

  //如果Commed等於零，讀取模式，讀取陣列中所儲存的位置並按照順序進行移動
  else if (Commed == 2 && WriteAddress > 0) {
    
    //WaitCheck 等於零表示有伺服馬達尚未移動至指定位置，將重復執行執直至 WaitCheck 等於一
    if (WaitCheck == 0) {
    int CurrentPosition_A = LastPos[0];  //讀取伺服馬達A的當前位置
    MovingTime_A = moveToTargetPosition_A(CurrentPosition_A, PosSave [ReadAddress][0], StepSize);  //呼叫伺服馬達A的副函式，並讀取其回傳的值

    int CurrentPosition_B = LastPos[1];  //讀取伺服馬達B的當前位置
    MovingTime_B = moveToTargetPosition_B(CurrentPosition_B, PosSave [ReadAddress][1], StepSize);  //呼叫伺服馬達B的副函式，並讀取其回傳的值

    int CurrentPosition_C = LastPos[2];  //讀取伺服馬達C的當前位置
    MovingTime_C = moveToTargetPosition_C(CurrentPosition_C, PosSave [ReadAddress][2], StepSize);  //呼叫伺服馬達C的副函式，並讀取其回傳的值

    int CurrentPosition_D = LastPos[3];  //讀取伺服馬達D的當前位置
    MovingTime_D = moveToTargetPosition_D(CurrentPosition_D, PosSave [ReadAddress][3], StepSize);  //呼叫伺服馬達D的副函式，並讀取其回傳的值

    int CurrentPosition_E = LastPos[4];  //讀取伺服馬達E的當前位置
    MovingTime_E = moveToTargetPosition_E(CurrentPosition_E, PosSave [ReadAddress][4], StepSize);  //呼叫伺服馬達E的副函式，並讀取其回傳的值

    int CurrentPosition_F = LastPos[5];  //讀取伺服馬達F的當前位置
    MovingTime_F = moveToTargetPosition_F(CurrentPosition_F, PosSave [ReadAddress][5], StepSize);  //呼叫伺服馬達F的副函式，並讀取其回傳的值
    }

    //判斷各伺服馬達的副函式的回傳值是否皆等於零，有值不等於零表示有伺服馬達尚未移動至指定位置
    if (MovingTime_A == 0 && MovingTime_B == 0 && MovingTime_C == 0 && MovingTime_D == 0 && MovingTime_E == 0 && MovingTime_F == 0) {
      
      WaitCheck = 1;  //如果各伺服馬達的副函式的回傳值皆等於零，指定 WaitCheck 為一
      
      //判斷當前讀取的位置是否大於已寫入的位置
      if (ReadAddress < WriteAddress - 1) {
        
        ReadAddress = ReadAddress + 1;  //將 ReadAddress 加一，以便讀去下一組位置
        delay(StepTime);  //等待設定的時間
        WaitCheck = 0;  //指定 WaitCheck 為零，以開啟下一輪位置讀取
      }
      //判斷當前讀取的位置是否大於等於已寫入的位置
      else if (ReadAddress >= WriteAddress - 1) {
        
        ReadAddress = 0;  //指定 ReadAddress 為零
        WaitCheck = 0;  //指定 WaitCheck 為零，以開啟下一輪位置讀取
        Commed = 0;   //指定 Commed 為零，以回復至普通模式
      }
    }
  }  
}

//副函式，控制LED
void LED(int Relay, int Commed, int A, int B, int C, int D, int E, int F) {
  //判斷當前繼電器的狀態，以顯示燈號
  if (Relay == 0 && State_R != 0 ) {
    strip.setPixelColor( 7, strip.Color(255,0,0));  //第八個LED顯示紅色
    strip.show();  //使LED進行顯示
    State_R = 0;  //指定 State_R 為零，以確保程式不會一直重複執行
  }
  else if (Relay == 1 && State_R != 1 ) {
    strip.setPixelColor( 7, strip.Color(0,255,0));  //第八個LED顯示綠色
    strip.show();  //使LED進行顯示
    State_R = 1;  //指定 State_R 為一，以確保程式不會一直重複執行
  }
  //判斷當前的模式的狀態，以顯示燈號
  if (Commed == 0 && LEDMode != 0) {
    strip.setPixelColor( 6, strip.Color(255,0,0));  //第七個LED顯示紅色
    strip.show();  //使LED進行顯示
    LEDMode = Commed;  //指定 LEDMode 等於 Commed，以確保程式不會一直重複執行
  }
  else if (Commed == 1 && LEDMode != 1) {
    strip.setPixelColor( 6, strip.Color(0,255,0));  //第七個LED顯示綠色
    strip.show();  //使LED進行顯示
    LEDMode = Commed;  //指定 LEDMode 等於 Commed，以確保程式不會一直重複執行
    delay(500);  //等待500毫秒以確保燈號可以正常顯示
  }
  else if (Commed == 2 && LEDMode != 2) {
    strip.setPixelColor( 6, strip.Color(0,0,255));  //第七個LED顯示藍色
    strip.show();  //使LED進行顯示
    LEDMode = Commed;  //指定 LEDMode 等於 Commed，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達A是否移動至指定位置，以顯示燈號
  if (A == 0 && State_A != 0 ) {
    strip.setPixelColor( 0, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_A = 0;  //指定 State_A 為零，以確保程式不會一直重複執行
  }
  else if (A != 0 && State_A != 1 ) {
    strip.setPixelColor( 0, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_A = 1;  //指定 State_A 為一，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達B是否移動至指定位置，以顯示燈號
  if (B == 0 && State_B != 0 ) {
    strip.setPixelColor( 1, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_B = 0;  //指定 State_B 為零，以確保程式不會一直重複執行
  }
  else if (B != 0 && State_B != 1 ) {
    strip.setPixelColor( 1, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_B = 1;  //指定 State_B 為一，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達C是否移動至指定位置，以顯示燈號
  if (C == 0 && State_C != 0 ) {
    strip.setPixelColor( 2, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_C = 0;  //指定 State_C 為零，以確保程式不會一直重複執行
  }
  else if (C != 0 && State_C != 1 ) {
    strip.setPixelColor( 2, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_C = 1;  //指定 State_C 為一，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達D是否移動至指定位置，以顯示燈號
  if (D == 0 && State_D != 0 ) {
    strip.setPixelColor( 3, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_D = 0;  //指定 State_D 為零，以確保程式不會一直重複執行
  }
  else if (D != 0 && State_D != 1 ) {
    strip.setPixelColor( 3, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_D = 1;  //指定 State_D 為一，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達E是否移動至指定位置，以顯示燈號
  if (E == 0 && State_E != 0 ) {
    strip.setPixelColor( 4, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_E = 0;  //指定 State_E 為零，以確保程式不會一直重複執行
  }
  else if (E != 0 && State_E != 1 ) {
    strip.setPixelColor( 4, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_E = 1;  //指定 State_E 為一，以確保程式不會一直重複執行
  }
  //判斷當前伺服馬達F是否移動至指定位置，以顯示燈號
  if (F == 0 && State_F != 0 ) {
    strip.setPixelColor( 5, strip.Color(0,0,255));
    strip.show();  //使LED進行顯示
    State_F = 0;  //指定 State_F 為零，以確保程式不會一直重複執行
  }
  else if (F != 0 && State_F != 1 ) {
    strip.setPixelColor( 5, strip.Color(255,0,0));
    strip.show();  //使LED進行顯示
    State_F = 1;  //指定 State_F 為一，以確保程式不會一直重複執行
  }
}

//副函式，設定伺服馬達的引腳
void Servo_on() {
  //将伺服馬達連接到相對應的引脚
  servo_A.attach(servoPin_A);  //將伺服馬達 A 連接到引脚
  servo_B.attach(servoPin_B);  //將伺服馬達 B 連接到引脚
  servo_C.attach(servoPin_C);  //將伺服馬達 C 連接到引脚
  servo_D.attach(servoPin_D);  //將伺服馬達 D 連接到引脚
  servo_E.attach(servoPin_E);  //將伺服馬達 E 連接到引脚
  servo_F.attach(servoPin_F);  //將伺服馬達 F 連接到引脚
}

//副函式，設定伺服馬達的初始位置
void Set_Servo(int Position_A, int Position_B, int Position_C, int Position_D, int Position_E, int Position_F) {
  servo_A.write(Position_A);  //設定初始位置
  LastPos[0] = Position_A;  //將初始位置設定至陣列的第一個元素中
  servo_B.write(Position_B);  //設定初始位置
  LastPos[1] = Position_B;  //將初始位置設定至陣列的第二個元素中
  servo_C.write(Position_C);  //設定初始位置
  LastPos[2] = Position_C;  //將初始位置設定至陣列的第三個元素中
  servo_D.write(Position_D);  //設定初始位置
  LastPos[3] = Position_D;  //將初始位置設定至陣列的第四個元素中
  servo_E.write(Position_E);  //設定初始位置
  LastPos[4] = Position_E;  //將初始位置設定至陣列的第五個元素中
  servo_F.write(Position_F);  //設定初始位置 
  LastPos[5] = Position_F;  //將初始位置設定至陣列的第六個元素中
}

//副函式，控制伺服馬達A的移動
int moveToTargetPosition_A(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_A.write(TOGO);  //逼近目標位置
    LastPos[0] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_A.write(TOGO);  //逼近目標位置
    LastPos[0] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}

//副函式，控制伺服馬達B的移動
int moveToTargetPosition_B(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_B.write(TOGO);  //逼近目標位置
    LastPos[1] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_B.write(TOGO);  //逼近目標位置
    LastPos[1] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}

//副函式，控制伺服馬達C的移動
int moveToTargetPosition_C(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_C.write(TOGO);  //逼近目標位置
    LastPos[2] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_C.write(TOGO);  //逼近目標位置
    LastPos[2] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}

//副函式，控制伺服馬達D的移動
int moveToTargetPosition_D(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_D.write(TOGO);  //逼近目標位置
    LastPos[3] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_D.write(TOGO);  //逼近目標位置
    LastPos[3] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}

//副函式，控制伺服馬達E的移動
int moveToTargetPosition_E(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_E.write(TOGO);  //逼近目標位置
    LastPos[4] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_E.write(TOGO);  //逼近目標位置
    LastPos[4] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}

//副函式，控制伺服馬達F的移動
int moveToTargetPosition_F(int CurrentPosition, int TargetPosition, int StepSize) {
  //計算當前的位置移動至目標的位置所需的時間差並宣告一個整數變數儲存在其中
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  //判斷移動方向，順時針移動
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_F.write(TOGO);  //逼近目標位置
    LastPos[5] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }
  //判斷移動方向，逆時針移動
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_F.write(TOGO);  //逼近目標位置
    LastPos[5] = TOGO;  //儲存目前位置，以方便下一步的移動
    delay(Moving_delay);  //可選的延遲，以控制移動速度
  }

  return Moving;  //回傳移動至目標的位置所需的時間
}
