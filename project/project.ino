#include <Servo.h>  //引入伺服馬達的程式庫
#include <Adafruit_NeoPixel.h>

#define Moving_delay 25  //定義機械手臂每個位置移動的間隔
#define Address_Max 100  //定義機械手臂最大儲存數量
#define servoPin_A 7  //定義伺服馬達A的腳位
#define servoPin_B 6  //定義伺服馬達B的腳位
#define servoPin_C 5  //定義伺服馬達C的腳位
#define servoPin_D 4  //定義伺服馬達D的腳位
#define servoPin_E 3  //定義伺服馬達E的腳位
#define servoPin_F 2  //定義伺服馬達F的腳位
#define RelayPin 52
#define LEDPin 53
#define LEDs 8

int PosSave [Address_Max + 1][6];  //創建一個二維整數陣列，以儲存機械手臂的位置
int LastPos [6];  //創建一個一維整數陣列，以儲存上一個機械手臂位置
int WriteAddress = 0;  //創建一個整數變數，以儲存當前寫入的位置
int ReadAddress = 0;  //創建一個整數變數，以儲存當前讀取的位置
int StepSize = 1;  //創建一個整數變數，以儲存每個 Moving_delay 所移動的大小，建議等於一
int StepTime = 500;  //創建一個整數變數，以儲存讀取時兩組動作的間隔
int Position_A = 90;  //創建一個整數變數，以儲存伺服馬達A將要移動至的位置，目前輸入的是啟動時的位置
int Position_B = 0;   //創建一個整數變數，以儲存伺服馬達B將要移動至的位置，目前輸入的是啟動時的位置
int Position_C = 180;  //創建一個整數變數，以儲存伺服馬達C將要移動至的位置，目前輸入的是啟動時的位置
int Position_D = 0;  //創建一個整數變數，以儲存伺服馬達D將要移動的至位置，目前輸入的是啟動時的位置
int Position_E = 90;  //創建一個整數變數，以儲存伺服馬達E將要移動至的位置，目前輸入的是啟動時的位置
int Position_F = 90;  //創建一個整數變數，以儲存伺服馬達F將要移動至的位置，目前輸入的是啟動時的位置
int MovingTime_A = 0;  //創建一個整數變數，以儲存當前伺服馬達A移動至指定位置所需的時間
int MovingTime_B = 0;  //創建一個整數變數，以儲存當前伺服馬達B移動至指定位置所需的時間
int MovingTime_C = 0;  //創建一個整數變數，以儲存當前伺服馬達C移動至指定位置所需的時間
int MovingTime_D = 0;  //創建一個整數變數，以儲存當前伺服馬達D移動至指定位置所需的時間
int MovingTime_E = 0;  //創建一個整數變數，以儲存當前伺服馬達E移動至指定位置所需的時間
int MovingTime_F = 0;  //創建一個整數變數，以儲存當前伺服馬達F移動至指定位置所需的時間
int WaitCheck = 0;  //創建一個整數變數，以確定所有的伺服馬達移動至指定位置
int Commed = 0;  //創建一個整數變數，以確定當前機械手臂的工作模式
char receivedData[50];  //創建一個字符組變數，以解析序列埠輸入的值
int Relay = 0;

//定義伺服馬達
Servo servo_A;
Servo servo_B;
Servo servo_C;
Servo servo_D;
Servo servo_E;
Servo servo_F;

//初始化函式
void setup() {
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);Serial.begin(9600);  //開啟序列埠，設定波特率為9600
  Serial.println("START");  //在序列埠輸出[START]並換行，以確定程式開始執行

  Servo_on();  //呼叫副函式，以初始化伺服馬達

  Set_Servo(Position_A, Position_B, Position_C, Position_D, Position_E, Position_F);  //呼叫副函式，以設定所有伺服馬達的初始位置
  
  delay(10);  //等待10毫秒
  Serial.println("READY");  //在序列埠輸出[READY]並換行，以確定程式初始化完成
}

//主函式，將會無限重複執行
void loop() {
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
    Serial.print("Commed:" + String(Commed) + " ");  //在序列埠輸出[Commed:(Commed)]並換行
    Serial.println("Relay:" + String(Relay));
  }
  
  if (Relay == 0) {
    digitalWrite(RelayPin, HIGH);
  }
  else if (Relay == 1) {
    digitalWrite(RelayPin, LOW);
  }

  //普通模式，移動至指定位置
  if (Commed == 0) {
    //Serial.println("Commed == 0");  //在序列埠輸出[Commed == 0]

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
  
  //寫入模式，移動至指定位置並寫入至陣列儲存
  else if (Commed == 1) {
    //Serial.println("Commed == 1");  //在序列埠輸出[Commed == 1]

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
    /*
    Serial.print("Write_Address: " + String(WriteAddress) + " ");  //在序列埠輸出[Write_Address: (WriteAddress) ]
    Serial.print("A:" + String(Position_A) + " ");  //在序列埠輸出[A:(Position_A) ]
    Serial.print("B:" + String(Position_B) + " ");  //在序列埠輸出[B:(Position_B) ]
    Serial.print("C:" + String(Position_C) + " ");  //在序列埠輸出[C:(Position_C) ]
    Serial.print("D:" + String(Position_D) + " ");  //在序列埠輸出[D:(Position_D) ]
    Serial.print("E:" + String(Position_E) + " ");  //在序列埠輸出[E:(Position_E) ]
    Serial.println("F:" + String(Position_F) + " ");  //在序列埠輸出[F:(Position_F) ]並換行
    */
    Commed = 0;  //指定 Commed 為零，以回復至普通模式
    WriteAddress++;  //寫入位置加一，以便下次的寫入
  }

  //讀取模式，讀取陣列中所儲存的位置並按照順序進行移動
  else if (Commed == 2 && WriteAddress > 0) {
    //Serial.println("Commed == 2");  //在序列埠輸出[Commed == 2]
    
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
    /*
    Serial.print("Read_Address: " + String(ReadAddress) + " ");  //在序列埠輸出[Read_Address: (ReadAddress) ]
    Serial.print("A:" + String(PosSave [ReadAddress][0]) + " ");  //在序列埠輸出[A:(PosSave [ReadAddress][0]) ]
    Serial.print("B:" + String(PosSave [ReadAddress][1]) + " ");  //在序列埠輸出[B:(PosSave [ReadAddress][1]) ]
    Serial.print("C:" + String(PosSave [ReadAddress][2]) + " ");  //在序列埠輸出[C:(PosSave [ReadAddress][2]) ]
    Serial.print("D:" + String(PosSave [ReadAddress][3]) + " ");  //在序列埠輸出[D:(PosSave [ReadAddress][3]) ]
    Serial.print("E:" + String(PosSave [ReadAddress][4]) + " ");  //在序列埠輸出[E:(PosSave [ReadAddress][4]) ]
    Serial.print("F:" + String(PosSave [ReadAddress][5]) + " ");  //在序列埠輸出[F:(PosSave [ReadAddress][5]) ]
    Serial.println("Check:" + String(WaitCheck));  //在序列埠輸出[Check:(WaitCheck)]並換行
    */
    }

    //判斷各伺服馬達的副函式的回傳值是否皆等於零，有值不等於零表示有伺服馬達尚未移動至指定位置
    if (MovingTime_A == 0 && MovingTime_B == 0 && MovingTime_C == 0 && MovingTime_D == 0 && MovingTime_E == 0 && MovingTime_F == 0) {
    WaitCheck = 1;  //如果各伺服馬達的副函式的回傳值皆等於零，指定 WaitCheck 為一

    //Serial.println("NEXT");  //在序列埠輸出[NEXT]並換行
    }
    
   //判斷各伺服馬達至否移動至指定位，並確定將要讀取的位置是否小於目前的寫入地址
   if (WaitCheck == 1 && ReadAddress < WriteAddress - 1) {
      ReadAddress = ReadAddress + 1;  //將 ReadAddress 加一，以便讀去下一組位置
      delay(StepTime);  //等待設定的時間
      WaitCheck = 0;  //指定 WaitCheck 為零，以開啟下一輪位置讀取

      //Serial.println("NEXT_POS");  //在序列埠輸出[NEXT_POS]並換行
    }
    
    //判斷各伺服馬達至否移動至指定位，並確定將要讀取的位置是否大於等於目前的寫入地址
    if (WaitCheck == 1 && ReadAddress >= WriteAddress - 1) {
      ReadAddress = 0;  //指定 ReadAddress 為零
      WaitCheck = 0;  //指定 WaitCheck 為零，以開啟下一輪位置讀取
      Commed = 0;   //指定 Commed 為零，以回復至普通模式
      
      //Serial.println("FINISH");  //在序列埠輸出[FUNISH]並換行
    }
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
  //Serial.print("A ");  //
在序列埠輸出[A ]
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_A.write(TOGO);  //逼近目標位置
    LastPos[0] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_A.write(TOGO);  //逼近目標位置
    LastPos[0] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }

  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  return Moving;
}

//副函式，控制伺服馬達B的移動
int moveToTargetPosition_B(int CurrentPosition, int TargetPosition, int StepSize) {
  //Serial.print("B ");  //在序列埠輸出[B ]

  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_B.write(TOGO);  //逼近目標位置
    LastPos[1] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_B.write(TOGO);  //逼近目標位置
    LastPos[1] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  return Moving;
}

//副函式，控制伺服馬達C的移動
int moveToTargetPosition_C(int CurrentPosition, int TargetPosition, int StepSize) {
  //Serial.print("C ");  //在序列埠輸出[C ]
  
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_C.write(TOGO);  //逼近目標位置
    LastPos[2] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }

  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_C.write(TOGO);  //逼近目標位置
    LastPos[2] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }

  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  return Moving;
}

//副函式，控制伺服馬達D的移動
int moveToTargetPosition_D(int CurrentPosition, int TargetPosition, int StepSize) {
  //Serial.print("D ");  //在序列埠輸出[D ]
  
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_D.write(TOGO);  //逼近目標位置
    LastPos[3] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_D.write(TOGO);  //逼近目標位置
    LastPos[3] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  return Moving;
}

//副函式，控制伺服馬達E的移動
int moveToTargetPosition_E(int CurrentPosition, int TargetPosition, int StepSize) {
  //Serial.print("E ");  //在序列埠輸出[E ]
  
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_E.write(TOGO);  //逼近目標位置
    LastPos[4] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_E.write(TOGO);  //逼近目標位置
    LastPos[4] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }

  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(Moving);  //在序列埠輸出[(Moving)]
    Serial.print(" ");  //在序列埠輸出[ ]
    */
  }
  return Moving;
}

//副函式，控制伺服馬達F的移動
int moveToTargetPosition_F(int CurrentPosition, int TargetPosition, int StepSize) {
  //Serial.print("F ");  //在序列埠輸出[F ]
  
  int Moving = CurrentPosition - TargetPosition;
  Moving = abs(Moving);
  Moving = Moving * Moving_delay;
  Moving = Moving / StepSize;
  
  if (CurrentPosition < TargetPosition) {
    int TOGO = CurrentPosition + StepSize;
    servo_F.write(TOGO);  //逼近目標位置
    LastPos[5] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    
    /*
    Serial.print("F ");  //在序列埠輸出[F ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.println(Moving);  //在序列埠輸出[(Moving)]並換行
    */
  }
  
  else if (CurrentPosition > TargetPosition) {
    int TOGO = CurrentPosition - StepSize;
    servo_F.write(TOGO);  //逼近目標位置
    LastPos[5] = TOGO;
    delay(Moving_delay);  //可選的延遲，以控制移動速度
    /*
    Serial.print("R ");  //在序列埠輸出[R ]
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.println(Moving);  //在序列埠輸出[(Moving)]
    */
  }
  
  else if (CurrentPosition == TargetPosition && Moving ==0) {
    /*
    Serial.print(CurrentPosition);  //在序列埠輸出[(CurrentPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.print(TargetPosition);  //在序列埠輸出[(TargetPosition)]
    Serial.print(" ");  //在序列埠輸出[ ]
    Serial.println(Moving);  //在序列埠輸出[(Moving)]
    */
  }
  return Moving;
}
