#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
int ir_pin = 9;
IRrecv IR(ir_pin);
decode_results results;
int initial = 0;
int inputter;
int counter = 0;
char alphaValues[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int rotor[3][26] = {
  { 5, 20, 19, 8, 7, 0, 21, 4, 3, 23, 11, 10, 25, 24, 15, 14, 22, 18, 17, 2, 1, 6, 16, 9, 13, 12 },
  { 20, 8, 11, 18, 12, 22, 23, 21, 1, 24, 17, 2, 4, 15, 25, 13, 19, 10, 3, 16, 0, 7, 5, 6, 9, 14 },
  { 9, 2, 1, 17, 22, 24, 16, 21, 15, 0, 23, 20, 14, 19, 12, 8, 6, 3, 25, 13, 11, 7, 4, 10, 5, 18 }
};
int reflector[26] = { 9, 11, 8, 22, 14, 7, 21, 5, 2, 0, 20, 1, 17, 16, 4, 25, 13, 12, 23, 24, 10, 6, 3, 18, 19, 15 };
int rotorPos[3] = { 0, 0, 0 };

LiquidCrystal_I2C lcd(0x27, 16,2);
/* Yassa Seifen - 19/9/2022 */
/************************************/
void setup() {
  Serial.begin(9600);
  lcd.init();
  IR.enableIRIn();  
  lcd.backlight();  
  IR.enableIRIn();
}
/************************************/
void clearScrn() {
  lcd.clear();
  counter = 0;
}
/************************************/
void printRotors() {
  for (int i = 2; i >= 0; i--) {
    lcd.setCursor(14 - 3 * i, 0);
    if (rotorPos[i] < 10) {lcd.print("0");}
    lcd.print(rotorPos[i]);
    lcd.print("|");  
  }  
}
/************************************/
int enigma(int inLetterVal) {
  for (int i = 0; i <= 2; i++) {
    inLetterVal += rotorPos[i]; 
    while (inLetterVal >= 26) inLetterVal -= 26; 
    inLetterVal = rotor[i][inLetterVal];
  }
  //Reflector
  inLetterVal = reflector[inLetterVal];
  //Coming back through rotors
  for (int i = 2; i >= 0; i--) {
    inLetterVal = rotor[i][inLetterVal];
    inLetterVal -= rotorPos[i]; 
    while (inLetterVal < 0) inLetterVal += 26; 
  }

  //Clicking Rotors
  rotorPos[0] += 1;
  if (rotorPos[0] >= 26) {
    rotorPos[1] += 1;
    rotorPos[0] = 0;
    if (rotorPos[1] >= 26) {
      rotorPos[2] += 1;
      rotorPos[1] = 0;
      if (rotorPos[2] >= 26) {
        rotorPos[2] = 0;
      }
    }
  }
  
  return inLetterVal;
}
/************************************/

void loop() {
  printRotors();
  
  while(IR.decode(&results)== 0){}
  delay(50);
  IR.resume();

    //Letter Selection
  if(results.value == 0xFF906F){// V+
    initial+=1;
    if(initial==26){
      initial=0;
      }
    }
  if(results.value == 0xFFA857){// V-
    initial-=1;
    if(initial<0){
      initial=25;
      }
    }  
  inputter = initial;
  lcd.setCursor(0, 0);
  lcd.print(alphaValues[inputter]); 
  lcd.print(" ");                   
  if (inputter < 10) {lcd.print("0");} 
  lcd.print(inputter);                 
  if (results.value == 0XFF22DD && counter < 16) {
    lcd.setCursor(counter, 1);
    lcd.print(alphaValues[enigma(inputter)]);
    counter++;
  }

  if (results.value == 0XFF6897) {
    clearScrn();
  }
/***********************/
  if (results.value == 0xFF7A85) {//3
    clearScrn();
  printRotors();
    initial+=1;
    if(initial==26){
      initial=0;
      }
  }
  if(results.value == 0xFF30CF){ //1 
      inputter = initial; 
      lcd.setCursor(14 - 3 * 2, 0); 
      if (inputter < 10) {lcd.print("0");} 
      lcd.print(inputter);                 
      delay(10);
    rotorPos[2] = inputter;
    initial+=1;
    if(initial==26){
      initial=0;
      }
    }
  if(results.value == 0xFF18E7){  //2
      inputter = initial; 
      lcd.setCursor(14 - 3 * 1, 0); 
      if (inputter < 10) {lcd.print("0");} 
      lcd.print(inputter);                 
      delay(10);
    rotorPos[1] = inputter;
    initial+=1;
    if(initial==26){
      initial=0;
      }
    }
  if(results.value == 0xFF7A85){ //3 
      inputter = initial; 
      lcd.setCursor(14 - 3 * 0, 0); 
      if (inputter < 10) {lcd.print("0");} 
      lcd.print(inputter);                 
      delay(10);
    rotorPos[0] = inputter;
    initial+=1;
    if(initial==26){
      initial=0;
      }
    }    
/*************************/
  if (results.value == 0XFF02FD && counter >= 1) {
    counter--;
    lcd.setCursor(counter, 1);
    lcd.print(" ");
    
    //Clicking rotors backwards
    rotorPos[0] -= 1;
    if (rotorPos[0] < 0) {
      rotorPos[1] -= 1;
      rotorPos[0] = 25;
      if (rotorPos[1] < 0) {
        rotorPos[2] -= 1;
        rotorPos[1] = 25;
        if (rotorPos[2] < 0) {
          rotorPos[2] = 25;
        }
      }
    }
  }
}
