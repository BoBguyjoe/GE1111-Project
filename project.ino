#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#include <TM1637Display.h>
#define CLK 2
#define DIO 3
TM1637Display display = TM1637Display(CLK, DIO);

const int DIFFICULTY = 5;

int leftButton;
int rightButton;
int downButton;
int upButton;
int selectButton;
bool failure;

char index[16][2];

int cursorX;
int cursorY;

int numX[DIFFICULTY];
int numY[DIFFICULTY];
int currentNumber;

int pastTime = 0;
int currentTime = 0;
int timer = 0;

void setup() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 2; j++) {
      index[i][j] = '0';
    }
  }
  
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(5,OUTPUT);
  
  // Display Setup
  lcd.begin(16,2);
  lcd.clear();
  display.setBrightness(7);
  delay(500);

  // Countdown
  lcd.setCursor(0,0);
  lcd.print("Ready");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set");
  delay(1000);
  lcd.setCursor(0,0);
  lcd.clear();
  delay(500);

  // Display the numbers
  bool copy;
  randomSeed(analogRead(A0));
  for (int n = 0; n < DIFFICULTY; n++) {
    numX[n] = random(0,16); // Generate a random spot for the number
    numY[n] = random(0,2);
    copy = false;
    for (int i = 0; i < n; i++) { // Check if that spot is already used for a previous number
      if (numX[i] == numX[n] && numY[i] == numY[n]) {
        n--;
        copy = true;
      }
    }
    if (copy == false) { // Display the number
      index[numX[n]][numY[n]] = 'X';
    }
  }

  lcd.clear();
  for (int n = 0; n < DIFFICULTY; n++) {
    lcd.setCursor(numX[n],numY[n]);
    lcd.print(n+1);
  }
  delay(750);

  // Blot out the numbers
  for (int n = 0; n < DIFFICULTY; n++) {
    lcd.setCursor(numX[n],numY[n]);
    lcd.write(255);
  }

  cursorX = 0;
  cursorY = 0;
  currentNumber = 1;
  failure = false;
}

void loop() {
  leftButton = analogRead(A4);
  rightButton = analogRead(A5);
  downButton = analogRead(A2);
  upButton = analogRead(A3);
  selectButton = analogRead(A1);

  // Timer
  currentTime = millis();
  if ((currentTime - pastTime) >= 1000) {
    timer++;
    pastTime = currentTime;
  }
  display.showNumberDec(timer);

  // Draw the screen
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 2; j++) {
      lcd.setCursor(i,j);
      if (index[i][j] == '0') {
        lcd.print(" ");
      }
      else if (index[i][j] == 'X') {
        lcd.write(255);
      }
      else {
        lcd.write(index[i][j] + 48);
      }
    }
  }

  // Print an 'O' for the cursor
  lcd.setCursor(cursorX,cursorY);
  lcd.print("O");
  lcd.setCursor(cursorX,cursorY);
  
  // Move the cursor around
  if (rightButton <= 10 && cursorX < 15) {
    cursorX++;
  }
  if (leftButton <= 10 && cursorX > 0) {
    cursorX--;
  }
  if (upButton <= 10) {
    cursorY = 0;
  }
  if (downButton <= 10) {
    cursorY = 1;
  }

  //Select button
  if (selectButton <= 10) {
    if (index[cursorX][cursorY] == 'X') {
      if (numX[currentNumber-1] == cursorX && numY[currentNumber-1] == cursorY) {
        index[cursorX][cursorY] = currentNumber;
        currentNumber++;
        digitalWrite(6, LOW); 
        digitalWrite(5, LOW);
        
        digitalWrite(7, HIGH);
        if (currentNumber == DIFFICULTY+1) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Congrats!");
          exit(0);
        }
        else {
          delay(750);
          digitalWrite(6, LOW); 
          digitalWrite(5, LOW);
          digitalWrite(7, LOW);
        }
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Haha, you failed");
        digitalWrite(6, HIGH); 
        digitalWrite(5, LOW);
        digitalWrite(7, LOW);
        exit(0);
      }
    }
  }

  delay(75);
}
