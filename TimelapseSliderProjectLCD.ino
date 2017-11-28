#include <LiquidCrystal.h>
#include <AccelStepper.h>

//LCD

int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//Funtions

#define minZeil       0
#define setPhotos     7
#define setSeconds    6
#define setPosition   2
#define setRPM        5
#define timelapse     4
#define film          3
#define setSleep      1
#define maxZeil       8

int zeil = setPhotos;

//PIN

int stepPIN = 13;

int stepDirPIN = 12;

int sleepPIN = 11;

int optoPIN = 3;

//Classes

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
AccelStepper stepper(1, stepPIN, stepDirPIN); //(1,Steps,Direction)

//Variables

int photos = 3;

int seconds = 5;

int rpm = 100;

int stepsPerTurn = 200;        // 1600 steps U Easydriver // A9488 200 steps U

int lengthSlider = 900; // mm

long stepsPerLenght = (lengthSlider / (PI * (12 + 1))) * stepsPerTurn ;    //  (LengthSlider / (PI * (Diameter + Zennrim))) * StepsPerU // (920 / (PI * (12 + 1))) * 1600

long stepsPerPhoto = 0;

int speedStepper = 1000;

int motorStopDuration = 0;

boolean sleep = true;

void setup()
{
  //PC
  Serial.begin(9600);

  //LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("       MX       ");
  lcd.setCursor(0, 1);
  lcd.print("   Timelapse ");
  delay(1000);

  //Slider

  //Stepper
  stepper.setMaxSpeed(1000); // 1000
  stepper.setAcceleration(1000); //3000

  //PIN
  pinMode(stepPIN, OUTPUT);
  pinMode(stepDirPIN, OUTPUT);
  pinMode(sleepPIN, OUTPUT);
  pinMode(optoPIN, OUTPUT);

  //setPIN mode
  digitalWrite(stepPIN, LOW);
  digitalWrite(stepDirPIN, LOW);
  digitalWrite(sleepPIN, LOW);
  digitalWrite(optoPIN, LOW);
}

//Controllers

// execute Steps
void doSteps(long steps)
{ 
  calcSpeedStepper(); 
  digitalWrite(sleepPIN, HIGH);

  stepper.setCurrentPosition(0);
  stepper.move(steps);
  stepper.runToPosition();
  
  if(sleep)digitalWrite(sleepPIN, LOW);
}

// optocoupler Steuerung
void optocoupler()
{
  digitalWrite(optoPIN, HIGH);
  delay(seconds * 1000);
  digitalWrite(optoPIN, LOW);
}

// Calculators
void calcSpeedStepper()
{
  double x = stepsPerTurn/60.0;
  speedStepper = rpm * x;
  stepper.setMaxSpeed(speedStepper);
  stepper.setAcceleration(speedStepper/2); // Speed Stepper
}

void calcStepsPerPhoto()
{
  if(photos == 1) stepsPerPhoto = 0;
  else stepsPerPhoto = stepsPerLenght/(photos-1);
}

void calcMotorStopDuration()
{
  long spinDuration = (stepsPerPhoto/speedStepper) * 1000;
  long runningTime = (( 2 * seconds * 1000 ) - spinDuration);
  if(runningTime > 0) motorStopDuration = runningTime;
  else motorStopDuration = 0; 
}

// read the buttons
int read_LCD_buttons()
{
  adc_key_in = analogRead(0);      // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;

  return btnNONE;  // when all others fail, return this...
}

void paint()
{
  lcd.setCursor(0, 0);           // move cursor to second line "1" and 9 spaces over
  switch (zeil)
  {
    case setPhotos:
      {
        String stringOne = "Pictures     " + String(photos);
        lcd.print(stringOne + "  ");
        break;
      }
    case setSeconds:
      {
        String stringOne = "Delay |s|     " + String((int)seconds);
        lcd.print(stringOne + "          ");
        break;
      }
    case timelapse:
      {
        lcd.print("Timelapse             ");
        break;
      }
    case setPosition:
      {
        lcd.print("Move             ");
        break;
      }
    case film:
      {
        String stringOne = "Film |mm/s|  " + String((int)rpm);
        lcd.print(stringOne);
        break;
      }
    case setRPM:
      {
        String stringOne = "Speed |rpm|  " + String((int)rpm)+"  ";
        lcd.print(stringOne);
        break;
      }
    case setSleep:
      {
        if(sleep) lcd.print("Sleep      true ");
        else lcd.print("Sleep      false");
        break;
      }
    }
}

void tipeIn()
{
  int tast = 200;  // delay after bressing Button
  lcd.setCursor(0, 1);
  lcd_key = read_LCD_buttons();  // read the buttons

  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      {
        //lcd.print("RIGHT           ");
        if (zeil == setPhotos) {
          photos++;
          delay(tast);
        }
        if (zeil == setSeconds) {
          seconds++;
          delay(tast);
        }
        if (zeil == setRPM) {
          rpm++;
          delay(tast);
        }
        if (zeil == setPosition) 
        {
            doSteps(100);
        }
        if (zeil == setSleep)
        { 
          if (sleep) sleep = false;
          else sleep = true;
          doSteps(1);
          delay(tast);
        }
        break;
      }
    case btnLEFT:
      {
        //lcd.print("LEFT            ");
        if (zeil == setPhotos)
        { 
          photos--;
          if (photos < 1) photos++;
          delay(tast);
        }
        if (zeil == setSeconds)
        { 
          seconds--;
          if (seconds < 1) seconds++;
          delay(tast);
        }
        if (zeil == setRPM)
        { 
          rpm--;
          if (rpm < 1) rpm++;
          delay(tast);
        }
        if (zeil == setPosition) 
        {
          doSteps(-100);
          //delay(tast);
        }
        if (zeil == setSleep)
        { 
          if (sleep) sleep = false;
          else sleep = true;
          doSteps(1);
          delay(tast);
        }
        break;
      }
    case btnUP:
      {
        //lcd.print("UP                   ");
        zeil++;
        if (zeil == maxZeil) zeil--;
        delay(tast);
        break;
      }
    case btnDOWN:
      {
        //lcd.print("DOWN                      ");
        zeil--;
        if (zeil == minZeil) zeil++;
        delay(tast);
        break;
      }
    case btnSELECT:
      {
        //lcd.print("SELECT                    ");
        if (zeil == timelapse)
        {
          playTimeLapse();
        }
        if (zeil == film)
        {
          playFilm();
        }
        break;
      }
    case btnNONE:
      {
        lcd.print("                       ");
        break;
      }
  }
}

//Run Timelapse
void playTimeLapse()
{
  calcStepsPerPhoto();
  calcMotorStopDuration();
  
  lcd.print("  Timelapse !        ");
  
  for(int i = 1 ; i < photos ; i++)
    {
       lcd.setCursor(0, 0);
       String stringOne = "Photo " + String((int)i) + " - " + String((int)photos);
       lcd.print(stringOne);
       optocoupler();
       delay(5);                            //Kamera Shutter close
       doSteps(stepsPerPhoto);
       delay(motorStopDuration);
     }

       lcd.setCursor(0, 0);
       String stringOne = "Photo " + String((int)photos) + " - " + String((int)photos) + "   ";
       lcd.print(stringOne);

     optocoupler();                   // Last Photo Stop 
     
     lcd.setCursor(0, 0);
     lcd.print("      End                ");

     lcd.setCursor(0, 1);
     lcd.print("                ");
     
     doSteps((-1) * stepsPerLenght);
     digitalWrite(sleepPIN, LOW);
}

void playFilm()
  {
    calcStepsPerPhoto();
    long pos = stepsPerLenght;
    lcd.setCursor(0, 1);
    lcd.print("   is Filming !               ");
    doSteps(pos);
    doSteps(-pos);
    lcd.setCursor(0, 1);
    lcd.print("                   ");
  }

void loop()
{
  tipeIn();
  paint();
}



