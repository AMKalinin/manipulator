#include <GyverPID.h>                	// подключение библиотеки пид регулятора
#include <Ultrasonic.h> 		// подключение библиотеки для работы с ультразвуковым дальномером

#include <Servo.h>			// подключение библиотеки для работы с сервоприводом


#define BMX055_DISABLE_BMM
#include <iarduino_Position_BMX055.h>	// подключение библиотеки для работы с гироскопом, акселлерометром, магнитометром

iarduino_Position_BMX055 sensorG(BMG);
iarduino_Position_BMX055 sensorX(BMG);


GyverPID pid(0.3, 0.9, 0.05);
GyverPID pid1(1, 0.05, 0.1);

Servo servo1;
Servo servo2;

const uint8_t pinH1   = 7;                                   // Создаём константу указывая номер вывода H1 MotorShield (он управляет направлением 1 мотора)
const uint8_t pinE1   = 6;                                   // Создаём константу указывая номер вывода E1 MotorShield (он управляет скоростью    1 мотора)
const uint8_t pinE2   = 5;                                   // Создаём константу указывая номер вывода E2 MotorShield (он управляет скоростью    2 мотора)
const uint8_t pinH2   = 4;                                   // Создаём константу указывая номер вывода H2 MotorShield (он управляет направлением 2 мотора)

char inChar;
int ind;
int period;
float kurs;
float timer;


Ultrasonic ultrasonic(12, 13);

void setup() {
 sensorG.begin();
 sensorX.begin();           
  
 pinMode(pinH1, OUTPUT); digitalWrite(pinH1, LOW);          // Конфигурируем вывод pinH1 как выход и устанавливаем на нём уровень логического «0»
 pinMode(pinE1, OUTPUT); digitalWrite(pinE1, LOW);          // Конфигурируем вывод pinE1 как выход и устанавливаем на нём уровень логического «0»
 pinMode(pinE2, OUTPUT); digitalWrite(pinE2, LOW);          // Конфигурируем вывод pinE2 как выход и устанавливаем на нём уровень логического «0»
 pinMode(pinH2, OUTPUT); digitalWrite(pinH2, LOW);          // Конфигурируем вывод pinH2 как выход и устанавливаем на нём уровень логического «0»


 Serial.begin(9600); // Инициализация Serial - порта



// установка начальных значений

 pid.setDirection(NORMAL);             
 pid.setpoint = 45;

 pid1.setDirection(NORMAL);
 pid1.setpoint = 0;

 period = 50;

 
 servo1.attach(3);
 servo2.attach(11); 

 servo1.write(140);
 servo2.write(40);

 

}

void loop() 
{
  // считывание сигналов с raspberry pi
  if (Serial.available() > 0)
  {
    inChar = Serial.read();

  }


  if (inChar == 'R')
  {
    digitalWrite(pinH1, LOW);  
    digitalWrite(pinH2, HIGH);
    povorot('R');
  }
  else if (inChar == 'L')
  {
    digitalWrite(pinH1, HIGH);  
    digitalWrite(pinH2, LOW);
    povorot('L');
  }
  else if (inChar == 'S')
  {
    analogWrite(pinE1, LOW);
    analogWrite(pinE2, LOW);

  }
  else if (inChar == 'B')
  {
    digitalWrite(pinH1, HIGH);  
    digitalWrite(pinH2, HIGH);
    analogWrite(pinE1, 65);
    analogWrite(pinE2, 85); 
    

  }
  else if (inChar == 'F')
  { 
    
    float dist_cm = ultrasonic.Ranging(CM);
    if (dist_cm < 16)                         // условие захвата
    {

      analogWrite(pinE1, LOW);
      analogWrite(pinE2, LOW);
      Serial.println('A');
      delay(500);
      while(Serial.available() > 0)
      {
        Serial.read();
      }
      zahvat();
      
      timer = millis();
      while((millis()-timer) < 4000)
      { 
        digitalWrite(pinH1, LOW);  
        digitalWrite(pinH2, HIGH);
        povorot('R');
      }

      analogWrite(pinE1, LOW);
      analogWrite(pinE2, LOW);
      inChar = 'S';
    }
    else
    {
      digitalWrite(pinH1, LOW);  
      digitalWrite(pinH2, LOW);

      if (ind == 0)
      { 
        analogWrite(pinE1, 0);
        analogWrite(pinE2, 0);
        delay(500);
        sensorX.read();
        kurs = sensorX.axisZ;
        ind = 1;
        analogWrite(pinE1, 100);
        analogWrite(pinE2, 100);
      }
      vpered();
    }
   
  }

}


void povorot(char napr)      // поворот вокруг оси с заданной скоростью
{
  static uint32_t tmr; 
  
  
  
  if ((millis()-tmr)>= period)
  {
    tmr = millis();
    sensorG.read();

    if (napr=='R') {pid.input = -sensorG.axisZ;}
    else {pid.input = sensorG.axisZ;}
  
    pid.getResult();
    analogWrite(pinE1, pid.output);
    analogWrite(pinE2, pid.output);

  }
}



void vpered()          // движение вперед
{
  static uint32_t tmr; 
  
  if ((millis()-tmr)>= 20)
  {
    tmr = millis();
    sensorX.read();
    pid1.input = -abs(sensorX.axisZ);
    pid1.getResult();

    if ((sensorX.axisZ)>0) 
    {
      analogWrite(pinE1, 60+pid1.output);
      analogWrite(pinE2, 75);
    }
    if ((sensorX.axisZ)<0) 
    {
      analogWrite(pinE1, 60);
      analogWrite(pinE2, 75+pid1.output);
    } 
  }
}



void zahvat()
{
  servo1.write(30);
  delay(1000);
  servo2.write(180);
  delay(1000);
}

void poloj()
{
  servo2.write(50);
  delay(1000);
  servo1.write(140);
  delay(1000);
  
}