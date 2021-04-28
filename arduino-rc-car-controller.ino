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

StaticJsonDocument<50> joystick;
StaticJsonDocument<50> buttons;

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
    checkButtons(isGreenButtonActive, isRedButtonActive);
    delay(150);
  }

  if (redButton == LOW) {
    Serial.print("Red low!  ");
    Serial.println(isRedButtonActive);
    isRedButtonActive = !isRedButtonActive;
    digitalWrite(RED_BUTTON_LED_PIN, isRedButtonActive ? HIGH : LOW);
    checkButtons(isGreenButtonActive, isRedButtonActive);
    delay(150);
  }

  checkControlInputs();

  delay(50);
}

void checkButtons(int isGreenButtonActive, int isRedButtonActive) {
  buttons["green"] = isGreenButtonActive;
  buttons["red"] = isRedButtonActive;

  char output[128];

  serializeJson(buttons, output);
  radio.write(&output, sizeof(output));

}

void checkControlInputs() {

  int newSw = digitalRead(SW_PIN);
  int newX = analogRead(X_PIN);
  int newY = analogRead(Y_PIN);

  int deltaX = newX - x;
  int deltaY = newY - y;

  if ((abs(deltaX) > 5) || (abs(deltaY) > 5) || newSw != sw) {

    joystick["sw"] = newSw;
    joystick["x"] = newX;
    joystick["y"] = newY;

    char output[128];

    serializeJson(joystick, output);
    radio.write(&output, sizeof(output));

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
}
