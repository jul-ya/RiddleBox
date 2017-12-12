
#include <LiquidCrystal.h>
#include <Keypad.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int leds[] = {53,51,49,47,45,43,41,39};
const int btns[] = {38,40,42,44,46,48,50,52};

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'5','1','2','K'},
  {'8','3','*','7'},
  {'M','#','4','9'},
  {'6','W','R','0'}
};
byte rowPins[ROWS] = {24, 34, 28, 30};
byte colPins[COLS] = {36, 32, 26, 22};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  
  // lcd setup
  lcd.begin(16, 2);
  lcd.print("RiddleBox 3000");

  // led and switch setup
  for(int i = 0; i < 8; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(btns[i], INPUT_PULLUP);
  }
}

void loop() {
  // leds are turned on with switches
  for(int i = 0; i < 8; i++){
    if(digitalRead(btns[i])){
      digitalWrite(leds[i], HIGH);
    }else{
      digitalWrite(leds[i], LOW);
    }
  }

  // print the pressed key
  char key = kpd.getKey();
  if(key){
        Serial.println(key);
  }

  /* simple display test
  lcd.noDisplay();
  delay(500);
  lcd.display();
  delay(500);*/
}

