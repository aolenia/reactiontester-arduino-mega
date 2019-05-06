#include <Ultrasonic.h>
#include <LiquidCrystal.h>

//LCD
const int rs = 22, en = 23, d4 = 3, d5 = 24, d6 = 2, d7 = 25;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//UltraSonic
Ultrasonic ultrasonic1(4, 26); // trig, echo
long distance1;
Ultrasonic ultrasonic2(5, 27); // trig, echo
long distance2;
Ultrasonic ultrasonic3(6, 28); // trig, echo
long distance3;
Ultrasonic ultrasonic4(7, 29); // trig, echo
long distance4;
long time = 0;
#define DISTANCE 15

//Buttons
#define STOP_BUTTON_IN 30
#define STOP_BUTTON_OUT 31
#define START_BUTTON_IN 32
#define START_BUTTON_OUT 33
#define RIGHT_LOWER_BUTTON_IN 37
#define RIGHT_LOWER_BUTTON_OUT 36
#define LEFT_LOWER_BUTTON_IN 34
#define LEFT_LOWER_BUTTON_OUT 35
#define BV_MAX 100
#define ANY_SW (digitalRead(STOP_BUTTON_IN) == LOW) || (digitalRead(START_BUTTON_IN) == LOW) || (digitalRead(RIGHT_LOWER_BUTTON_IN) == LOW) || (digitalRead(LEFT_LOWER_BUTTON_IN) == LOW)

int pbPressed = 0; //1 = stop, 2 = start, 3 = left lower, 4 = right lower

//LEDS
#define LED_RED_1 38
#define LED_GREEN_1 39
#define LED_BLUE_1 40

#define LED_RED_2 41
#define LED_GREEN_2 42
#define LED_BLUE_2 43

#define LED_RED_3 44
#define LED_GREEN_3 45
#define LED_BLUE_3 46

#define LED_RED_4 47
#define LED_GREEN_4 48
#define LED_BLUE_4 49
int ledStateTest = 0;

//Menus
int currentMenu = 1; // 1 = base menu, 2 = settings menu

int baseMenuSize = 3;
int baseMenuCurrentPos = 0;
String baseMenu[] = {
  "Game1",        //0
  "Game2",        //1
  "TestFunctions" //2
};

int settingsMenuSize = 8;
int settingsMenuCurrentPos = 0;
String settingsMenu[] = {
  "All red",        //0
  "All green",      //1
  "All blue",       //2
  "All off",        //3
  "Sensor1",        //4
  "Sensor2",        //5
  "Sensor3",        //6
  "Sensor4",        //7
};

int showResultsSize = 10; //same as amount of played rounds i.e sensing amounts
int showResultsCurrentPos = 0;
String resultsMenu[] = {
  "Round1",        //0
  "Round2",      //1
  "Round3",       //2
  "Round4",        //3
  "Round5",        //4
  "Round6",        //5
  "Round7",        //6
  "Round8",        //7
  "Round9",       //8
  "Round10"       //9
};

int gameOn = 0; // 1 = test game with 1 pad, 2 = test game with 4 pads, 3 = do not exist
int gameEnded = 0; // 1 = test game with 1 pad, 2 = test game with 4 pads, 3 = do not exist
long overralStopTime = 0;
long startTime[] =    {0,0,0,0,0,0,0,0,0,0}; // time when led of pad goes on
long sensingTime[] =  {0,0,0,0,0,0,0,0,0,0}; // time when player touches the pad
long reactionTime[]=  {0,0,0,0,0,0,0,0,0,0}; // reactionTime per touch
long averageReactionTime = 0;

int numberOfPad = 0;
int game1NoOfRounds = 0;
int game2NoOfRounds = 0;
int ledIsOn = 0;
int overRide = 0; //test variable to test without 2-4pads

//function that removes the vibration of switch state
void debounce(void) {
  unsigned char bv_counter;
  bv_counter = BV_MAX;       //set max value

  while (bv_counter) {
    if (ANY_SW) {
      bv_counter = BV_MAX;
    } else {
      bv_counter--;
    }
  }
}

//function that reads switches
void read_sw(void) {
  if (digitalRead(STOP_BUTTON_IN) == LOW) {
    debounce();
    pbPressed = 1;
  }
  if (digitalRead(START_BUTTON_IN) == LOW) {
    debounce();
    pbPressed = 2;
  }
  if (digitalRead(LEFT_LOWER_BUTTON_IN) == LOW) {
    debounce();
    pbPressed = 3;
  }
  if (digitalRead(RIGHT_LOWER_BUTTON_IN) == LOW) {
    debounce();
    pbPressed = 4;
  }

}

void updateMenu(void) {
  switch (pbPressed) {
    case 1: //stop pressed
      if (currentMenu == 2) { // if settingsMenu choosed, reverse to baseMenu
        baseMenuCurrentPos = 0;
        settingsMenuCurrentPos = 0;
        currentMenu = 1;
        clearRow(1);
        clearRow(2);
        setAllLedsTo(3);
      }
      if (gameOn != 0) {
        gameOn = 0; // stop game
        initialiseGameVariables();
        setAllLedsTo(3);
      }
      if(gameEnded != 0) {
        gameEnded = 0; // end of showing results
        showResultsCurrentPos = 0;
        clearRow(1);
        clearRow(2);
      }
      pbPressed = 0;
      break;
    case 2: // start pressed
      if (currentMenu == 1 && baseMenuCurrentPos == 2) { //if Settings choosed from baseMenu, set to settingsMenu
        currentMenu = 2;
      }
      if (currentMenu == 1 && baseMenuCurrentPos == 0) {
        gameOn = 1; // start game1
      }
      if (currentMenu == 1 && baseMenuCurrentPos == 1) {
        gameOn = 2; // start game2
      }

      //test function to test without 2-4 pads 
      if (gameOn != 0){
        overRide = 1;
      }
      pbPressed = 0;
      break;
    case 3: //left lower pb pressed
      if (gameOn == 0 && gameEnded == 0){ // game is not on so user can scroll menus
        switch (currentMenu) {
          case 1: // base menu
            if (baseMenuCurrentPos == 0) {
              baseMenuCurrentPos = baseMenuSize - 1;
            } else {
              baseMenuCurrentPos = baseMenuCurrentPos - 1;
            }
            break;
          case 2: // settings menu
            if (settingsMenuCurrentPos == 0) {
              settingsMenuCurrentPos = settingsMenuSize - 1;
            } else {
              settingsMenuCurrentPos = settingsMenuCurrentPos - 1;
            }
            break;
        }
      }else if (gameOn == 0 && gameEnded != 0){
        if (showResultsCurrentPos > 0){
          showResultsCurrentPos = showResultsCurrentPos - 1; 
        }else if (showResultsCurrentPos == 0){
          showResultsCurrentPos = 10;
          //gameEnded = 0; // end of showing results
        }
      }
      pbPressed = 0;
      break;
    case 4:  // right lower pressed
      if (gameOn == 0 && gameEnded == 0){ // game is not on so user can scroll menus
        switch (currentMenu) {
        case 1: //base menu
          if (baseMenuCurrentPos == (baseMenuSize - 1)) {
            baseMenuCurrentPos = 0;
          } else {
            baseMenuCurrentPos = baseMenuCurrentPos + 1;
          }
          break;
        case 2: //settings menu
          if (settingsMenuCurrentPos == (settingsMenuSize - 1)) {
            settingsMenuCurrentPos = 0;
          } else {
            settingsMenuCurrentPos = settingsMenuCurrentPos + 1;
          }
        break;
        } 
      }else if (gameOn == 0 && gameEnded != 0){
        if (showResultsCurrentPos < 10){
          showResultsCurrentPos = showResultsCurrentPos + 1; 
        }else if (showResultsCurrentPos == 10){
          showResultsCurrentPos = 0;
          //gameEnded = 0; // end of showing results
        }
      }
 
    pbPressed = 0;
    break;
  }

  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  switch (currentMenu) {
    case 1:
      lcd.print(baseMenu[baseMenuCurrentPos]);
      break;
    case 2:
      lcd.print(settingsMenu[settingsMenuCurrentPos]);
  }
}

//function that reads switches
int read_us_sensors(void) {
  if (time <  millis()) {
    time = ((millis() + 0));
    distance1 = ultrasonic1.read(CM);
    distance2 = ultrasonic2.read(CM);
    distance3 = ultrasonic3.read(CM);
    distance4 = ultrasonic4.read(CM);
    return 1;
  } else {
    return 0;
  }
}
void printTime(void) {
  lcd.setCursor(15, 3);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}

void setAllLedsTo(int color) { // 0 == red, 1 == green, 2 == blue, 3 == off 
  if (color == 0) {
    digitalWrite(LED_RED_1, HIGH);
    digitalWrite(LED_GREEN_1, LOW);
    digitalWrite(LED_BLUE_1, LOW);
    digitalWrite(LED_RED_2, HIGH);
    digitalWrite(LED_GREEN_2, LOW);
    digitalWrite(LED_BLUE_2, LOW);
    digitalWrite(LED_RED_3, HIGH);
    digitalWrite(LED_GREEN_3, LOW);
    digitalWrite(LED_BLUE_3, LOW);
    digitalWrite(LED_RED_4, HIGH);
    digitalWrite(LED_GREEN_4, LOW);
    digitalWrite(LED_BLUE_4, LOW);
  } else if (color == 1) {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_GREEN_1, HIGH);
    digitalWrite(LED_BLUE_1, LOW);
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_GREEN_2, HIGH);
    digitalWrite(LED_BLUE_2, LOW);
    digitalWrite(LED_RED_3, LOW);
    digitalWrite(LED_GREEN_3, HIGH);
    digitalWrite(LED_BLUE_3, LOW);
    digitalWrite(LED_RED_4, LOW);
    digitalWrite(LED_GREEN_4, HIGH);
    digitalWrite(LED_BLUE_4, LOW);
  } else if (color == 2) {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_GREEN_1, LOW);
    digitalWrite(LED_BLUE_1, HIGH);
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_GREEN_2, LOW);
    digitalWrite(LED_BLUE_2, HIGH);
    digitalWrite(LED_RED_3, LOW);
    digitalWrite(LED_GREEN_3, LOW);
    digitalWrite(LED_BLUE_3, HIGH);
    digitalWrite(LED_RED_4, LOW);
    digitalWrite(LED_GREEN_4, LOW);
    digitalWrite(LED_BLUE_4, HIGH);
  } else if (color == 3) {
    digitalWrite(LED_RED_1, LOW);
    digitalWrite(LED_GREEN_1, LOW);
    digitalWrite(LED_BLUE_1, LOW);
    digitalWrite(LED_RED_2, LOW);
    digitalWrite(LED_GREEN_2, LOW);
    digitalWrite(LED_BLUE_2, LOW);
    digitalWrite(LED_RED_3, LOW);
    digitalWrite(LED_GREEN_3, LOW);
    digitalWrite(LED_BLUE_3, LOW);
    digitalWrite(LED_RED_4, LOW);
    digitalWrite(LED_GREEN_4, LOW);
    digitalWrite(LED_BLUE_4, LOW);
  }
}

void updateTestFunctions(void) {
  if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 0) {
    ledStateTest = 0; //all red
    clearRow(1);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 1) {
    ledStateTest = 1; //all green
    clearRow(1);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 2) {
    ledStateTest = 2; //all blue
    clearRow(1);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 3) {
    ledStateTest = 3; //all off
    clearRow(1);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 4) {
    printUsValues(distance1);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 5) {
    printUsValues(distance2);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 6) {
    printUsValues(distance3);
  } else if (currentMenu == 2 && baseMenuCurrentPos == 2 && settingsMenuCurrentPos == 7) {
    printUsValues(distance4);
  } else {
    ledStateTest = 3; //all off
    clearRow(1);
  }
  setAllLedsTo(ledStateTest);
}
void printUsValues(int distance) {
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.setCursor(0, 1);
  lcd.print(distance);
}
void clearRow(int row) {
  lcd.setCursor(0, row);
  lcd.print("                    ");
}

int setLedToRed(int numberOfPad) {
  int padNumber = 0;
  switch (numberOfPad) {
    case 1:
      digitalWrite(LED_RED_1, HIGH);
      padNumber = 1;
      break;
    case 2:
      digitalWrite(LED_RED_2, HIGH);
      padNumber = 2;
      break;
    case 3:
      digitalWrite(LED_RED_3, HIGH);
      padNumber = 3;
      break;
    case 4:
      digitalWrite(LED_RED_4, HIGH);
      padNumber = 4;
      break;
  }
  return padNumber;
}
int setLedToBlue(int numberOfPad) {
  int padNumber = 0;
  switch (numberOfPad) {
    case 1:
      digitalWrite(LED_BLUE_1, HIGH);
      padNumber = 1;
      break;
    case 2:
      digitalWrite(LED_BLUE_2, HIGH);
      padNumber = 2;
      break;
    case 3:
      digitalWrite(LED_BLUE_3, HIGH);
      padNumber = 3;
      break;
    case 4:
      digitalWrite(LED_BLUE_4, HIGH);
      padNumber = 4;
      break;
  }
  return padNumber;
}

void calculateReactionTimes(int gameOnNo){
  long totalTime = 0;
  for(int i = 0; i < 10; i++){
    reactionTime[i] = sensingTime[i] - startTime[i]; 
    totalTime = reactionTime[i] + totalTime;
//    Serial.print("ReactionTime: ");
//    Serial.println(reactionTime[i]);
  }
  averageReactionTime =  totalTime / 10;
//  Serial.print("Average reactionTime: ");
//  Serial.println(averageReactionTime);
}

void printReactionTimes(int gameOn){
  clearRow(0);
  lcd.setCursor(0,0);
  lcd.print("Game");
  lcd.print(gameOn);
  lcd.print(" end");

  lcd.setCursor(0,1);
  lcd.print("Results:");
  clearRow(2);
  lcd.setCursor(0,2);
  if(showResultsCurrentPos == 10){
    lcd.print("Average:");
    lcd.print(averageReactionTime);
  }else{
    lcd.print(showResultsCurrentPos + 1);
    lcd.print("/10:");
    lcd.print(reactionTime[showResultsCurrentPos]);
    lcd.print("ms"); 
  }
}

void playGame1(void) {
  //first go to else where game1 is initialized
  if (game1NoOfRounds < 10 && gameOn == 1) {
    lcd.setCursor(6,0);
    lcd.print("on ");
    lcd.print(game1NoOfRounds);
    lcd.print(":10");
    if (ledIsOn) {
      read_us_sensors();
      switch (numberOfPad) {
        case 1:
          if (distance1 <= DISTANCE) {
            sensingTime[game1NoOfRounds] = millis();
//            Serial.print("SensingTime: ");
//            Serial.print(sensingTime[game1NoOfRounds]);
//            Serial.print(" of round: ");
//            Serial.println(game1NoOfRounds);
            setLedToBlue(numberOfPad);
            ledIsOn = 0;
            game1NoOfRounds++;
            delay(1000);
          }
          break;
      }
    } else { 
      setAllLedsTo(3); // shutdown all leds
      delay(random(1000,4000));
      numberOfPad = setLedToRed(1); 
      startTime[game1NoOfRounds] = millis();
//      Serial.print("StartTime: ");
//      Serial.print(startTime[game1NoOfRounds]);
//      Serial.print(" of round: ");
//      Serial.println(game1NoOfRounds);
      ledIsOn = 1;
    }
  } else { //game ended
    numberOfPad = 0;
    game1NoOfRounds = 0;
    ledIsOn = 0;
    setAllLedsTo(3); // shutdown all leds
    calculateReactionTimes(gameOn);
    gameOn = 0;
    gameEnded = 1;
//    Serial.println("Game ended");
  }
}

void playGame2(void) {
  //first go to else where game1 is initialized
  if (game2NoOfRounds < 10 && gameOn == 2) {
    lcd.setCursor(6,0);
    lcd.print("on ");
    lcd.print(game2NoOfRounds);
    lcd.print(":10 ");
    if (ledIsOn) {
      read_us_sensors();
      lcd.print(numberOfPad);
      switch (numberOfPad) {
        case 1:
          if (distance1 <= DISTANCE || overRide == 1) {
            sensingTime[game2NoOfRounds] = millis();
//            Serial.print("SensingTime: ");
//            Serial.print(sensingTime[game2NoOfRounds]);
//            Serial.print(" of round: ");
//            Serial.println(game2NoOfRounds);
            setLedToBlue(numberOfPad);
            ledIsOn = 0;
            overRide = 0;
            game2NoOfRounds++;
            delay(1000);
          }
          break;
        case 2:
          if (distance1 <= DISTANCE || overRide == 1) {
            sensingTime[game2NoOfRounds] = millis();
//            Serial.print("SensingTime: ");
//            Serial.print(sensingTime[game2NoOfRounds]);
//            Serial.print(" of round: ");
//            Serial.println(game2NoOfRounds);
            setLedToBlue(numberOfPad);
            ledIsOn = 0;
            overRide = 0;
            game2NoOfRounds++;
            delay(1000);
          }
          break;
        case 3:
          if (distance1 <= DISTANCE || overRide == 1) {
            sensingTime[game2NoOfRounds] = millis();
//            Serial.print("SensingTime: ");
//            Serial.print(sensingTime[game2NoOfRounds]);
//            Serial.print(" of round: ");
//            Serial.println(game2NoOfRounds);
            setLedToBlue(numberOfPad);
            ledIsOn = 0;
            overRide = 0;
            game2NoOfRounds++;
            delay(1000);
          }
          break;
        case 4:
          if (distance1 <= DISTANCE || overRide == 1) {
            sensingTime[game2NoOfRounds] = millis();
//            Serial.print("SensingTime: ");
//            Serial.print(sensingTime[game2NoOfRounds]);
//            Serial.print(" of round: ");
//            Serial.println(game2NoOfRounds);
            setLedToBlue(numberOfPad);
            ledIsOn = 0;
            overRide = 0;
            game2NoOfRounds++;
            delay(1000);
          }
          break;
      }
    } else { 
      setAllLedsTo(3); // shutdown all leds
      delay(random(1000,4000));
      numberOfPad = setLedToRed(random(1,5)); 
      startTime[game2NoOfRounds] = millis();
//      Serial.print("StartTime: ");
//      Serial.print(startTime[game2NoOfRounds]);
//      Serial.print(" of round: ");
//      Serial.println(game2NoOfRounds);
      ledIsOn = 1;
    }
  } else { //game ended
    numberOfPad = 0;
    game2NoOfRounds = 0;
    ledIsOn = 0;
    setAllLedsTo(3); // shutdown all leds
    calculateReactionTimes(gameOn);
    gameOn = 0;
    gameEnded = 2;
//    Serial.println("Game ended");
  }
}
void initialiseGameVariables(void){
    numberOfPad = 0;
    game1NoOfRounds = 0;
    game2NoOfRounds = 0;
    ledIsOn = 0;
}

void setup() {
//  Serial.begin(9600);
  randomSeed(analogRead(0));
  //set buttons outputs to gnd
  pinMode(STOP_BUTTON_OUT, OUTPUT);
  pinMode(START_BUTTON_OUT, OUTPUT);
  pinMode(RIGHT_LOWER_BUTTON_OUT, OUTPUT);
  pinMode(LEFT_LOWER_BUTTON_OUT, OUTPUT);
  digitalWrite(STOP_BUTTON_OUT, LOW);
  digitalWrite(START_BUTTON_OUT, LOW);
  digitalWrite(RIGHT_LOWER_BUTTON_OUT, LOW);
  digitalWrite(LEFT_LOWER_BUTTON_OUT, LOW);
  //set buttons inputs to internal pullups
  pinMode(STOP_BUTTON_IN, INPUT_PULLUP);
  pinMode(START_BUTTON_IN, INPUT_PULLUP);
  pinMode(RIGHT_LOWER_BUTTON_IN, INPUT_PULLUP);
  pinMode(LEFT_LOWER_BUTTON_IN, INPUT_PULLUP);

  pinMode(LED_RED_1, OUTPUT);
  pinMode(LED_GREEN_1, OUTPUT);
  pinMode(LED_BLUE_1, OUTPUT);

  pinMode(LED_RED_2, OUTPUT);
  pinMode(LED_GREEN_2, OUTPUT);
  pinMode(LED_BLUE_2, OUTPUT);

  pinMode(LED_RED_3, OUTPUT);
  pinMode(LED_GREEN_3, OUTPUT);
  pinMode(LED_BLUE_3, OUTPUT);

  pinMode(LED_RED_4, OUTPUT);
  pinMode(LED_GREEN_4, OUTPUT);
  pinMode(LED_BLUE_4, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.setCursor(0, 3);
  lcd.print("ReactionTester");
}

void loop() {
  read_sw();
  updateMenu();
  printTime();

  if (gameOn == 1) {
    playGame1();
  }else if (gameOn == 2) {
    playGame2();
  }else if(gameOn == 0 && gameEnded != 0 ) {
    printReactionTimes(gameEnded);
  }
  
  if (currentMenu == 2) {
    read_us_sensors();
    updateTestFunctions();
  }
}
