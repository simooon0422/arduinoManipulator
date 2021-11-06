#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150                // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 550                // This is the 'maximum' pulse length count (out of 4096)
#define SERVOMAX_GRIPPER_DEGREES 60 //Pozycja dla otwartego chwytaka
#define SERVO_FREQ 50               // Analog servos run at ~50 Hz updates
#define BUTTON 12                   //Pin od przycisku od chwytaka

bool buttonState = 0; //stan przycisku od chwytaka

int degreesToPulse(int degrees);               //zamiana stopni na impuls
void writePosition(int channel, int position); //sterowanie pozycją serwa
void checkGripper();                           //sterowanie chwytakiem

void setup()
{
  Serial.begin(9600);

  pinMode(BUTTON, INPUT_PULLUP); //konfiguracja przycisku

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
  delay(10);
}

void loop()
{
  //pobierz wartości z pinów analogowych i zmapuj do przedziału 0-180
  int servPos1 = map(analogRead(A2), 0, 1023, 0, 180);
  int servPos2 = map(analogRead(A3), 0, 1023, 0, 180);
  int servPos3 = map(analogRead(A0), 0, 1023, 0, 180);

  //ustaw serwa na odczytane pozycje
  writePosition(0, servPos1);
  writePosition(1, servPos2);
  writePosition(2, servPos3);

  checkGripper(); //ustawienie chwytaka

  delay(10);
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

void checkGripper()
{
  if (buttonState == 1) //jeśli wciśnięto przycisk to otwórz chwytak
  {
    writePosition(3, SERVOMAX_GRIPPER_DEGREES);
    if (digitalRead(BUTTON) == LOW)
    {
      buttonState = 0;
      delay(500);
    }
  }
  else //jeśli nie, lub wciśnięto go ponownie to zamknij chwytak
  {
    writePosition(3, 0);
    if (digitalRead(BUTTON) == LOW)
    {
      buttonState = 1;
      delay(500);
    }
  }
}