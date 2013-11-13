/*
  Button
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when pressing a pushbutton attached to pin 2. 
 
 
 The circuit:
 * LED attached from pin 13 to ground 
 * pushbutton attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground
 
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 
 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to 
// set pin numbers:
const int button1Pin = 12;     // the number of the pushbutton pin    down
const int button2Pin =  13;      // the number of the pushbutton pin   up
const int MotorP1APin =  9;      // the number of the LED pin
const int MotorP1BPin =  10;      // the number of the LED pin
const int Position1Pin =  0;      // the number of the LED pin
const int Position2Pin =  1;      // the number of the LED pin
const int LED =  7;      // the number of the LED pin

//const int MotorP1DPin =  10;      // the number of the LED pin
// variables will change:
int button1State = 0;         // 往下降的按键的状态

int button2State = 0;         // 往上升的按键的状态

int Button2ChangeCnt = 0;
int Button1ChangeCnt = 0;
int StopFlag = 0;
int Reading1 = 0;
int Reading2 = 0;
int count1 = 0;
long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long time1 = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED, OUTPUT);//测试用
  pinMode(MotorP1APin, OUTPUT);      
  pinMode(MotorP1BPin, OUTPUT);  
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT); 
  pinMode(Position2Pin, INPUT);
  attachInterrupt(Position1Pin, stateChange,FALLING);//镜头缩到最底端的光电传感器中断函数
  attachInterrupt(Position2Pin, Code1, FALLING);//电机的编码器脉冲中断函数

}
void stateChange(){
StopFlag = 1;
}
// 电机的编码器码盘计数中断子程序
void Code1()
{  
  //镜头往外伸的时候count1++，最顶端计数暂时定为210；镜头往回缩的时候count1--；
  if((LOW == button2State)&&(HIGH == button1State)){//sw2 down
  //为了不计入噪音干扰脉冲，
   //当2次中断之间的时间大于5ms时，计一次有效计数
  if((millis()-time1)>5) 
  //当编码器码盘的OUTA脉冲信号下跳沿每中断一次，
  count1 += 1; // 编码器码盘计数加一  
  time1==millis();
  }
  else if((LOW == button1State)&&(HIGH == button2State)){//sw1 down
  //为了不计入噪音干扰脉冲，
   //当2次中断之间的时间大于5ms时，计一次有效计数
  if((millis()-time1)>5) 
  //当编码器码盘的OUTA脉冲信号下跳沿每中断一次，
  count1 -= 1; // 编码器码盘计数加一  
  time1==millis();
  }
}

void loop()
{
  
  // read the state of the pushbutton value:
  // read the state of the switch into a local variable:
  if(1 == StopFlag){
    digitalWrite(LED, 1);
  }
  else{
    digitalWrite(LED, 0);
  }
  Reading1 = digitalRead(button1Pin);
  if(LOW == Reading1){
    if(Button1ChangeCnt<6){
      Button1ChangeCnt++;
    }
    else{
      button1State = Reading1;//连续6次按下，则状态改变
    }
  }
  else{
    Button1ChangeCnt = 0;
    button1State = Reading1;
  }


    Reading2 = digitalRead(button2Pin);
      if(LOW == Reading2){
        if(Button2ChangeCnt<6){
          Button2ChangeCnt++;
          }
          else{
            button2State = Reading2;//连续6次按下，则状态改变
            }
       }
       else{
         Button2ChangeCnt = 0;
         button2State = Reading2;
         }
  
  
    if((LOW == button1State)&&(HIGH == button2State)){//SW1 down
      if(1 == StopFlag){//到达底端则关闭底端光电传感器中断，并停止马达
       detachInterrupt(Position1Pin);//
        analogWrite(MotorP1BPin, 0);
        analogWrite(MotorP1APin, 0);
        count1 = 0;
        }
        else{
          analogWrite(MotorP1BPin, 0);
          analogWrite(MotorP1APin, 80);
          }
    }
      
    else if((LOW == button2State)&&(HIGH == button1State)){//SW2 down
    if((10 <= count1)&&(1 == StopFlag)){//到达过底端，且目前镜头在上升且离开了底端,则打开底端光电传感器中断
    StopFlag = 0;
    attachInterrupt(Position1Pin, stateChange,FALLING);
    }
    else if(210 <= count1){//镜头到达顶端,则停止马达
    analogWrite(MotorP1BPin, 0);
    analogWrite(MotorP1APin, 0);
    }
    else{
      analogWrite(MotorP1BPin, 80);
      analogWrite(MotorP1APin, 0);
      }
      }
    else{
      analogWrite(MotorP1BPin, 0);
      analogWrite(MotorP1APin, 0);
    }       
}
