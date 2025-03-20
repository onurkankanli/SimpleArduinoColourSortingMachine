#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

#define myButton 2 //push button
boolean buttonPressed = false;

int buzzerPin = 7;

int servoTopPin = 4;
int servoBottomPin = 8;
int ledRed = 13;
int ledGreen = 12;
int ledBlue = 11;
int ledYellow = 10;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //tcs colour sensor object

Servo myServoTop;  // create servo object to control a servo Top
Servo myServoBottom; // create servo object to control a servo Bottom


int colourPos;
boolean TopServoMovedToSensor = false;
boolean readyToMoveBotServo = false;

uint16_t detectColour;
String detectedColour;
boolean detectionDone = false;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
 
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(myButton, INPUT_PULLUP);

  pinMode(buzzerPin, OUTPUT);

  tone(buzzerPin, 1000, 2000);

  myServoTop.attach(servoTopPin);  // attaches the servo on pin 4 to the servo object
  myServoBottom.attach(servoBottomPin); // attaches the servo on pin 8 to the servo object

  if (tcs.begin())
  {
    Serial.println("Sensor Detected!");
  }
  else
  {
    Serial.println("No Colour Sensor(TCS34725) found!, Please check connections!");
    while (1);
  }
}

void loop() 
{
  // declare the colours
  uint16_t r, g, b, c, colorTemp, lux;

  digitalWrite(ledYellow, 1);

  //check if button is pressed
  if(digitalRead(myButton))
  {
      delay(100);
      buttonPressed = true;
      Serial.println("Button pressed!");
  }

  if(buttonPressed == true)
  {
    if(myServoTop.read() == 93)
    {
      controlServoTop();
    }
  }

  // if top servo moved (boolean) read getrawdata
  if(TopServoMovedToSensor == true)
  {
    
    tcs.getRawData(&r, &g, &b, &c);
    colorTemp = tcs.calculateColorTemperature(r, g, b);
    lux = tcs.calculateLux(r, g, b);
      
    colourSorter(r, g, b);

    TopServoMovedToSensor = false;
    
    //wait for top servo to stop moving (not enough current for both to move simulatenously)
    delay(1000);

    readyToMoveBotServo = true;
  }
  
  jobMaker();

  if(readyToMoveBotServo == true)
  {
    delay(2000);
    //call the function to move the bottom servo
    controlServoBottom();
    
    readyToMoveBotServo = false;
  }

  //if sensor detected the ball go in
  if(detectionDone == true)
  {

    delay(1000);
    //drop the ball
    myServoTop.write(0);
    Serial.println("Servo pushed the ball down!");

    detectionDone = false;

    //wait for the ball to drop
    delay(500);
    //return to deafult position
    myServoTop.write(93);
    
    //call function to ring the buzzer to indiate ball is in the cup
    ringBuzzer();
  }
  
}

void controlServoTop()
{
    myServoTop.write(40);
    //wait for sensor to read
    delay(500);
    buttonPressed = false;

    Serial.println("Servo moved to the sensor position!");
    TopServoMovedToSensor = true;
}

void controlServoBottom()
{
  myServoBottom.write(colourPos);  // tell servo to go to position in variable 'pos'
}

uint16_t colourSorter(uint16_t r, uint16_t g, uint16_t b)
{
  detectColour = max(r, g);
  detectColour = max(detectColour, b);
  
  if(detectColour == r)
  {
    Serial.print("It's Red: "); Serial.print("R:"); Serial.print(r, DEC); Serial.print(" G:"); Serial.print(g, DEC); Serial.print(" B:"); Serial.print(b, DEC); Serial.println(" ");
    detectedColour = "Red";
  }
  else if (detectColour == g)
  {
    Serial.print("It's Green: "); Serial.print("R:"); Serial.print(r, DEC); Serial.print(" G:"); Serial.print(g, DEC); Serial.print(" B:"); Serial.print(b, DEC); Serial.println(" ");
    detectedColour = "Green";
  }
  else if (detectColour == b)
  {
    Serial.print("It's Blue: "); Serial.print("R:"); Serial.print(r, DEC); Serial.print(" G:"); Serial.print(g, DEC); Serial.print(" B:"); Serial.print(b, DEC); Serial.println(" ");
    detectedColour = "Blue";
  }

  detectionDone = true;

  return detectColour;
}

void jobMaker()
{
  if(detectedColour == "Red")
  {
    //left side
    colourPos = 45;
    digitalWrite(ledRed, 1);
    digitalWrite(ledGreen, 0);
    digitalWrite(ledBlue, 0);
  }
  else if(detectedColour == "Green")
  {
    //middle
    colourPos = 93;
    digitalWrite(ledGreen, 1);
    digitalWrite(ledRed, 0);
    digitalWrite(ledBlue, 0);
  }
  else if(detectedColour == "Blue")
  {
    //blue
    colourPos = 140;
    digitalWrite(ledBlue, 1);
    digitalWrite(ledRed, 0);
    digitalWrite(ledGreen, 0);
  }
}

void ringBuzzer()
{
  //tone(buzzerPin, 440); // A4
  //delay(1000);

  //tone(buzzerPin, 494); // B4
  //delay(1000);

  tone(buzzerPin, 523); // C4
  delay(1000);

  tone(buzzerPin, 587); // D4
  delay(1000);

  //tone(buzzerPin, 659); // E4
  //delay(1000);

  //tone(buzzerPin, 698); // F4
  //delay(1000);

  //tone(buzzerPin, 784); // G4
  //delay(1000);

  noTone(buzzerPin);
  delay(1000);
}