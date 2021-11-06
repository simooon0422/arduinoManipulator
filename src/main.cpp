#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150                // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 550                // This is the 'maximum' pulse length count (out of 4096)
#define SERVOMAX_GRIPPER_DEGREES 60 //Pozycja dla otwartego chwytaka
#define SERVO_FREQ 50               // Analog servos run at ~50 Hz updates
#define IR_PIN 3                    //Pin od odbiornika podczerwieni
#define SIGNAL_1_1 0x1001
#define SIGNAL_1_2 0x1801
#define SIGNAL_2_1 0x1002
#define SIGNAL_2_2 0x1802
#define SIGNAL_3_1 0x1003
#define SIGNAL_3_2 0x1803
#define SIGNAL_4_1 0x1004
#define SIGNAL_4_2 0x1804
#define SIGNAL_UP_1 0x10
#define SIGNAL_UP_2 0x810
#define SIGNAL_DOWN_1 0x11
#define SIGNAL_DOWN_2 0x811
#define SIGNAL_OK_1 0x17
#define SIGNAL_OK_2 0x817

IRrecv irrecv(IR_PIN);  //Konfiguracja odbiornika
decode_results results; //

byte currentServo = 0;            //obecnie sterowane serwo
byte moveServo = 0;               //sterowanie serwem
int servPos[] = {90, 30, 180, 0}; //tablica pozycji początkowych serw

int degreesToPulse(int degrees);               //zamiana stopni na impuls
void writePosition(int channel, int position); //sterowanie pozycją serwa
void checkGripper();                           //sterowanie chwytakiem

void setup()
{
  Serial.begin(9600);

  irrecv.enableIRIn(); //Podłączenie odbiornika podczerwieni

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
  delay(10);

  writePosition(0, servPos[0]);
  delay(1000);
  writePosition(1, servPos[1]);
  delay(1000);
  writePosition(2, servPos[2]);
  delay(1000);
  writePosition(3, servPos[3]);
}

void loop()
{ 
  if (irrecv.decode(&results))
  {
    switch (results.value)
    {
    case SIGNAL_1_1:
    case SIGNAL_1_2:
      currentServo = 0;
      break;

    case SIGNAL_2_1:
    case SIGNAL_2_2:
      currentServo = 1;
      break;

    case SIGNAL_3_1:
    case SIGNAL_3_2:
      currentServo = 2;
      break;

    case SIGNAL_4_1:
    case SIGNAL_4_2:
      currentServo = 3;
      break;

    case SIGNAL_UP_1:
    case SIGNAL_UP_2:
      moveServo = 1;
      break;

    case SIGNAL_DOWN_1:
    case SIGNAL_DOWN_2:
      moveServo = 2;
      break;

    case SIGNAL_OK_1:
    case SIGNAL_OK_2:
      moveServo = 0;
      break;
    }

    Serial.print("Current: ");
    Serial.println(currentServo);
    Serial.print("Move: ");
    Serial.println(moveServo);
    delay(250);
    irrecv.resume();
  }


int old_position = servPos[currentServo];

  switch (moveServo)
  {
  case 0:
    delay(50);
    break;

  case 1:
    if (currentServo == 3)
    {
      if (servPos[currentServo] < SERVOMAX_GRIPPER_DEGREES)
      {
        servPos[currentServo]++;
      }
      Serial.print("Position: ");
      Serial.println(servPos[currentServo]);
      break;
    }
    else if (servPos[currentServo] < 180)
    {
      servPos[currentServo]++;
    }
    Serial.print("Position: ");
    Serial.println(servPos[currentServo]);
    break;

  case 2:
    if (servPos[currentServo] > 0)
    {
      servPos[currentServo]--;
    }
    
    Serial.print("Position: ");
    Serial.println(servPos[currentServo]);
    break;
  }
  if (old_position != servPos[currentServo]) writePosition(currentServo, servPos[currentServo]);
  delay(15);
}

int degreesToPulse(int degrees)
{
  if (degrees >= 0 && degrees <= 180)
  {
    int pulseLenght = map(degrees, 0, 180, SERVOMIN, SERVOMAX);
    return pulseLenght;
  }
  else
    return 0;
}

void writePosition(int channel, int newPosition)
{
  pwm.setPWM(channel, 0, degreesToPulse(newPosition));
}
