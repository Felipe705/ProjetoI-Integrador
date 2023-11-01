#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#define RESET asm ("jmp (0x0000)")
 LiquidCrystal_I2C lcd(0x27, 16, 2);

 int c = 0;

int turnCount = 0;
const int sensorPin = 3;       // Pino digital para o sensor magnético

int currentSensorState;        // Estado atual do sensor
int lastSensorState = 0;  

float inicio = 0;
unsigned long lastPressTime = 0;
unsigned long lastLastPressTime = 0;
bool sensorPressed = false;
unsigned long currentTime = millis();

int buttonErase = 7;  // Pino do botão


void setup() {
 
  
  c= EEPROM.read(100);

    pinMode(sensorPin, INPUT);
    pinMode(buttonErase, INPUT_PULLUP);
    Serial.begin(9600);

    delay(500);

    lcd.init();
    delay(500);
    lcd.backlight();
    delay(500);
    lcd.clear();
    delay(500);
    lcd.setCursor(0, 0);
    delay(500);
    lcd.print("Gaiola 1");
    lcd.setCursor(0, 1);
    lcd.print("Voltas: " + String(turnCount));

      if(c==0) {
     EEPROM.write(100, 1);
     Serial.println("c0");
    RESET;
  } else {
    Serial.println("c1");
     EEPROM.write(100, 0);
  }
    
}

void loop() {
    currentTime = millis();
    currentSensorState = digitalRead(sensorPin);

    if (currentSensorState == HIGH && lastSensorState == LOW) {
        if (!sensorPressed) {
            sensorPressed = true;
            lastLastPressTime = lastPressTime;
            lastPressTime = currentTime;

            turnCount++;

            lcd.setCursor(0, 1);
            lcd.print("Voltas: " + String(turnCount));

            Serial.print("Count: ");
            Serial.println(turnCount);

            if (turnCount == 1) {
                inicio = currentTime;
            }
        }  
    } else {
        lastSensorState = LOW;
    }
  
    if (digitalRead(sensorPin) == HIGH) {
        sensorPressed = false;
        lastSensorState = LOW;
    }

    lastSensorState = currentSensorState;

    //Apagar informações do LCD após 5 segundos
    if (currentTime - lastPressTime >= 5000) {

        if(turnCount != 0){
          limpar();
        }
    }

    // if (digitalRead(buttonErase) == HIGH) {

    //     //Aguardar 10 segundos
    //     delay(10000); 

    //     //Verifica se o botão ainda está pressionado
    //     if (digitalRead(buttonErase) == HIGH) {

    //         if(turnCount != 0){
    //           lcd.clear();
    //           lcd.setCursor(0, 0);
    //           lcd.print("Resetando voltas!");
    //         }

    //         //Envia a informaçao para deletar
    //         Serial.write(0);
    //         limpar();

    //         // Aguarda o botão ser solto antes de continuar
    //         while (digitalRead(buttonErase) == HIGH) {
    //             delay(100);
    //         }
    //     }
    // }
}

void limpar(){

  turnCount = 0;
  lastPressTime = currentTime;
  inicio = currentTime;
  sensorPressed = false;
  Serial.println("Resetting count.");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gaiola 1");
  lcd.setCursor(0, 1);
  lcd.print("Voltas: " + String(turnCount));
}
