#include <ArduinoJson.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(2, 3); // CE, CSN

const byte address[6] = "00001";


// Arduino pin numbers
const int SW_PIN = 10; // digital pin connected to switch output
const int X_PIN = 5; // analog pin connected to X output
const int Y_PIN = 4; // analog pin connected to Y output
const int GREEN_BUTTON_PIN = 8;
const int GREEN_BUTTON_LED_PIN = 6;
const int RED_BUTTON_PIN = 9;
const int RED_BUTTON_LED_PIN = 7;

StaticJsonDocument<200> joystick;

unsigned int sw = 0;
volatile unsigned int x = 500;
volatile unsigned int y = 500;

boolean isGreenButtonActive = false;
boolean isRedButtonActive = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(GREEN_BUTTON_LED_PIN, OUTPUT);
  digitalWrite(GREEN_BUTTON_LED_PIN, LOW);

  pinMode(RED_BUTTON_LED_PIN, OUTPUT);
  digitalWrite(RED_BUTTON_LED_PIN, LOW);

  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);

  pinMode(GREEN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(100);
  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}


void loop() {
  int greenButton = digitalRead(GREEN_BUTTON_PIN);
  int redButton = digitalRead(RED_BUTTON_PIN);


  if (greenButton == LOW) {
    Serial.print("Green low! ");
    Serial.println(isGreenButtonActive);
    isGreenButtonActive = !isGreenButtonActive;
    digitalWrite(GREEN_BUTTON_LED_PIN, isGreenButtonActive  ? HIGH : LOW);
    delay(100);
  }

  if (redButton == LOW) {
    Serial.print("Red low!  ");
    Serial.println(isRedButtonActive);
    isRedButtonActive = !isRedButtonActive;
    digitalWrite(RED_BUTTON_LED_PIN, isRedButtonActive ? HIGH : LOW);
    delay(100);
  }

  delay(100);

  const char text[] = "Hello World";
  radio.write(&text, sizeof(text));
  delay(1000);
}

void loop2() {

  int newSw = digitalRead(SW_PIN);
  int newX = analogRead(X_PIN);
  int newY = analogRead(Y_PIN);

  int deltaX = newX - x;
  int deltaY = newY - y;

  if ((abs(deltaX) > 5) || (abs(deltaY) > 5) || newSw != sw) {
    digitalWrite(LED_BUILTIN, HIGH);

    joystick["sw"] = sw;
    joystick["x"] = x;
    joystick["y"] = y;
    serializeJson(joystick, Serial);
    Serial.flush();

    if (false) {
      Serial.println("");
      Serial.print("OldX: ");
      Serial.print(x);
      Serial.print(" newX: ");
      Serial.println(newX);
      Serial.print("Oldy: ");
      Serial.print(y);
      Serial.print(" newY: ");
      Serial.println(newY);
      Serial.print(" deltaX: ");
      Serial.println(deltaX);
      Serial.print(" deltaY: ");
      Serial.println(deltaY);
    }

    sw = newSw;
    y = newY;
    x = newX;
  }
  digitalWrite(LED_BUILTIN, LOW);
}
