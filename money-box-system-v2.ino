// EEPROM STORAGE:
// Addresses 0, 1, 2, 3 - store pin characters 1-4 respectively
// Address 4 - signifies whether PIN is to be set up -
//             if 0, program will prompt user to set up their pin,
//             if 1, user already has pin, so will go straight to
//             balance view.
// Address 5, 6, 7, 8, 9 - stores num of 20s, 10s, 5s, 2s and 1s respectively

#include <Servo.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD
int rs=7;
int en=8;
int d4=9;
int d5=10;
int d6=11;
int d7=12;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

// SERVO
Servo boxLock;
// 1, 2, 5, 10, 20

// INTERRUPTER
int interupt1Pin = 6;
int interupt2Pin = 5;
int interupt3Pin = 4;
int interupt4Pin = 3;
int interupt5Pin = 2;

int interupt1ValOld = 1;
int interupt2ValOld = 1;
int interupt3ValOld = 1;
int interupt4ValOld = 1;
int interupt5ValOld = 1;
int interupt1ValNew;
int interupt2ValNew;
int interupt3ValNew;
int interupt4ValNew;
int interupt5ValNew;

// BUTTONS
int btn1Pin = A0;
int btn2Pin = A1;
int btn3Pin = A2;
int btn4Pin = A3;
int btn5Pin = A4;

int btn1ValOld = 1;
int btn2ValOld = 1;
int btn3ValOld = 1;
int btn4ValOld = 1;
int btn5ValOld = 1;
int btn1ValNew;
int btn2ValNew;
int btn3ValNew;
int btn4ValNew;
int btn5ValNew;

int btnPressed;

// DATA
int pinEntry[4];
int charCount = 0;
char pinDisplay[4] = {'_', '_', '_', '_'};
int coinCount = 0;
int num20s;
int num10s;
int num5s;
int num2s;
int num1s;
int balance;
String balanceDisplay = "";

// FLAGS
bool isNewUser;
bool isStartingUp = true;
bool balanceUpdated = true;
bool onBalanceScreen;
bool isEnteringPin;
bool accessGranted;
bool boxOpen = false;
bool coinsRemoved;
bool twentyCoinsCounted = false;
bool tenCoinsCounted = false;
bool fiveCoinsCounted = false;
bool twoCoinsCounted = false;
bool oneCoinsCounted = false;

bool scanning;

// £ SYMBOL
byte poundSign[] = {
  B01110,
  B10010,
  B10000,
  B11100,
  B10000,
  B10000,
  B11110,
  B00000
};

// TIMINGS
int processDelay = 2500;
int btnDelay = 100;
int lockDelay = 1000;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  Serial.begin(9600);
  boxLock.attach(13);
  
  pinMode(interupt1Pin, INPUT);
  pinMode(interupt2Pin, INPUT);
  pinMode(interupt3Pin, INPUT);
  pinMode(interupt4Pin, INPUT);
  pinMode(interupt5Pin, INPUT);

  pinMode(btn1Pin, INPUT);
  pinMode(btn2Pin, INPUT);
  pinMode(btn3Pin, INPUT);
  pinMode(btn4Pin, INPUT);
  pinMode(btn5Pin, INPUT);

  digitalWrite(btn1Pin, HIGH);
  digitalWrite(btn2Pin, HIGH);
  digitalWrite(btn3Pin, HIGH);
  digitalWrite(btn4Pin, HIGH);
  digitalWrite(btn5Pin, HIGH);

  // GETS COIN NUMS
  num20s = EEPROM.read(5);
  num10s = EEPROM.read(6);
  num5s = EEPROM.read(7);
  num2s = EEPROM.read(8);
  num1s = EEPROM.read(9);

  // CALCULATES BALANCE
  balance = (num20s*20) +
            (num10s*10) +
            (num5s*5) +
            (num2s*2) +
            (num1s*1);
  
  lcd.createChar(0, poundSign);
  boxLock.write(150);
  Serial.println(balance);
}

int btnScan() {
  int btn = 0;
  scanning = true;
  while(scanning) {
    btn1ValNew = digitalRead(btn1Pin);
    btn2ValNew = digitalRead(btn2Pin);
    btn3ValNew = digitalRead(btn3Pin);
    btn4ValNew = digitalRead(btn4Pin);
      
    while (btn1ValNew == 1 && btn1ValOld == 1 && 
           btn2ValNew == 1 && btn2ValOld == 1 &&
           btn3ValNew == 1 && btn3ValOld == 1 &&
           btn4ValNew == 1 && btn4ValOld == 1) {
      btn1ValNew = digitalRead(btn1Pin);
      btn2ValNew = digitalRead(btn2Pin);
      btn3ValNew = digitalRead(btn3Pin);
      btn4ValNew = digitalRead(btn4Pin);
    }
    
    if (btn1ValNew == 1 && btn1ValOld == 0) {
      btn = 1;
      scanning = false;
    } else if (btn2ValNew == 1 && btn2ValOld == 0) {
      btn = 2;
      scanning = false;
    } else if (btn3ValNew == 1 && btn3ValOld == 0) {
      btn = 3;
      scanning = false;
    } else if (btn4ValNew == 1 && btn4ValOld == 0) {
      btn = 4;
      scanning = false;
    }
    delay(btnDelay);
    btn1ValOld = btn1ValNew;
    btn2ValOld = btn2ValNew;
    btn3ValOld = btn3ValNew;
    btn4ValOld = btn4ValNew;
  }
  return btn;
}

void loop() {
  // put your main code here, to run repeatedly:
  while (isStartingUp) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Smart Money");
    lcd.setCursor(0,1);
    lcd.print("Box!");
    delay(processDelay);

    if (EEPROM.read(4) == 0) {
      isNewUser = true;
    } else {
      isNewUser = false;
    }
    isStartingUp = false;
    onBalanceScreen = true;
  } // isStartingUp
  
  while (isNewUser) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Choose your PIN:");
    lcd.setCursor(0,1);
    lcd.print(pinDisplay);

    btnPressed = btnScan();

    if (btnPressed == 1) {
      pinEntry[charCount] = 1;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 2) {
      pinEntry[charCount] = 2;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 3) {
      pinEntry[charCount] = 3;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 4) {
      pinEntry[charCount] = 4;
      pinDisplay[charCount] = '*';
      charCount++;
    }

    if (charCount == 4) {
      EEPROM.write(0, pinEntry[0]);
      EEPROM.write(1, pinEntry[1]);
      EEPROM.write(2, pinEntry[2]);
      EEPROM.write(3, pinEntry[3]);
      EEPROM.write(4, 1);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PIN saved.");
      delay(processDelay);

      // resets temporary PIN stored
      pinEntry[0] = 0;
      pinEntry[1] = 0;
      pinEntry[2] = 0;
      pinEntry[3] = 0;

      // resets placeholder text
      pinDisplay[0] = '_';
      pinDisplay[1] = '_';
      pinDisplay[2] = '_';
      pinDisplay[3] = '_';

      // resets count
      charCount = 0;
      isNewUser = false;
      onBalanceScreen = true;
    }
  } // isNewUser

  while (!isNewUser && onBalanceScreen) {
    
    if (balanceUpdated) {
      balance = (EEPROM.read(5)*20) +
                (EEPROM.read(6)*10) +
                (EEPROM.read(7)*5) +
                (EEPROM.read(8)*2) +
                (EEPROM.read(9)*1);
      balanceDisplay = createBalance(balance);
      balanceUpdated = false;
    }

    lcd.clear();
    lcd.home();
    lcd.print("Balance: ");
    lcd.write(byte(0));
    Serial.print(balanceDisplay);
    lcd.print(balanceDisplay);
    lcd.setCursor(0,1);
    lcd.print("Any btn to open.");
    delay(btnDelay);

    interupt1ValNew = digitalRead(interupt1Pin);
    interupt2ValNew = digitalRead(interupt2Pin);
    interupt3ValNew = digitalRead(interupt3Pin);
    interupt4ValNew = digitalRead(interupt4Pin);
    interupt5ValNew = digitalRead(interupt5Pin);
    btn1ValNew = digitalRead(btn1Pin);
    btn2ValNew = digitalRead(btn2Pin);
    btn3ValNew = digitalRead(btn3Pin);
    btn4ValNew = digitalRead(btn4Pin);
    btn5ValNew = digitalRead(btn5Pin);

    while (interupt1ValNew == 1 && interupt1ValOld == 1 && 
           interupt2ValNew == 1 && interupt2ValOld == 1 &&
           interupt3ValNew == 1 && interupt3ValOld == 1 &&
           interupt4ValNew == 1 && interupt4ValOld == 1 &&
           interupt5ValNew == 1 && interupt5ValOld == 1 && 
           btn1ValNew == 1 && btn1ValOld == 1 && 
           btn2ValNew == 1 && btn2ValOld == 1 &&
           btn3ValNew == 1 && btn3ValOld == 1 &&
           btn4ValNew == 1 && btn4ValOld == 1 &&
           btn5ValNew == 1 && btn5ValOld == 1) {
      interupt1ValNew = digitalRead(interupt1Pin);
      interupt2ValNew = digitalRead(interupt2Pin);
      interupt3ValNew = digitalRead(interupt3Pin);
      interupt4ValNew = digitalRead(interupt4Pin);
      interupt5ValNew = digitalRead(interupt5Pin);

      btn1ValNew = digitalRead(btn1Pin);
      btn2ValNew = digitalRead(btn2Pin);
      btn3ValNew = digitalRead(btn3Pin);
      btn4ValNew = digitalRead(btn4Pin);
      btn5ValNew = digitalRead(btn5Pin);
    }

    if (interupt1ValNew == 1 && interupt1ValOld == 0) {
      num20s += 1;
      EEPROM.write(5, num20s);
      createBalance(balance);
      balanceUpdated = true;
    } else if (interupt2ValNew == 1 && interupt2ValOld == 0) {
      num10s += 1;
      EEPROM.write(6, num10s);
      createBalance(balance);
      balanceUpdated = true;
    } else if (interupt3ValNew == 1 && interupt3ValOld == 0) {
      num5s += 1;
      EEPROM.write(7, num5s);
      createBalance(balance);
      balanceUpdated = true;
    } else if (interupt4ValNew == 1 && interupt4ValOld == 0) {
      num2s += 1;
      EEPROM.write(8, num2s);
      createBalance(balance);
      balanceUpdated = true;
    } else if (interupt5ValNew == 1 && interupt5ValOld == 0) {
      num1s += 1;
      EEPROM.write(9, num1s);
      createBalance(balance);
      balanceUpdated = true;
    } 
    
    if ((btn1ValNew == 1 && btn1ValOld == 0) || 
        (btn2ValNew == 1 && btn2ValOld == 0) ||
        (btn3ValNew == 1 && btn3ValOld == 0) ||
        (btn4ValNew == 1 && btn4ValOld == 0)) {
          onBalanceScreen = false;
          isEnteringPin = true;
        }

    interupt1ValOld = interupt1ValNew;
    interupt2ValOld = interupt2ValNew;
    interupt3ValOld = interupt3ValNew;
    interupt4ValOld = interupt4ValNew;
    interupt5ValOld = interupt5ValNew;

    btn1ValOld = btn1ValNew;
    btn2ValOld = btn2ValNew;
    btn3ValOld = btn3ValNew;
    btn4ValOld = btn4ValNew;
  }

  while (isEnteringPin) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter your PIN:");
    lcd.setCursor(0,1);
    lcd.print(pinDisplay);

    btnPressed = btnScan();
  
    if (btnPressed == 1) {
      pinEntry[charCount] = 1;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 2) {
      pinEntry[charCount] = 2;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 3) {
      pinEntry[charCount] = 3;
      pinDisplay[charCount] = '*';
      charCount++;
    } else if (btnPressed == 4) {
      pinEntry[charCount] = 4;
      pinDisplay[charCount] = '*';
      charCount++;
    }

    if (charCount == 4) {
      lcd.clear();
      lcd.setCursor(0,0);

      if (pinEntry[0] == EEPROM.read(0) &&
          pinEntry[1] == EEPROM.read(1) &&
          pinEntry[2] == EEPROM.read(2) &&
          pinEntry[3] == EEPROM.read(3)) {
        lcd.print("Access granted.");
        accessGranted = true;
        boxOpen = true;
      } else {
        lcd.print("Access denied.");
        accessGranted = false;
      }

      delay(processDelay);
      charCount = 0;
      pinDisplay[0] = '_';
      pinDisplay[1] = '_';
      pinDisplay[2] = '_';
      pinDisplay[3] = '_';
      isEnteringPin = false;

      while (boxOpen) {
        boxLock.write(0);
        delay(lockDelay);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Press back");
        lcd.setCursor(0,1);
        lcd.print("when finished.");
        
        btn5ValNew = digitalRead(btn5Pin);

        while (btn5ValNew == 1 && btn5ValOld == 1) {
          btn5ValNew = digitalRead(btn5Pin);
        }

        if (btn5ValNew == 1 && btn5ValOld == 0) {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Locking...");
          delay(lockDelay);
          boxLock.write(150);
          delay(lockDelay);
          boxOpen = false;
        }
        btn5ValOld = btn5ValNew;
      }

      while (accessGranted) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Coins removed?");
        lcd.setCursor(0,1);
        lcd.print("1 - Yes, 2 - No");

        btnPressed = btnScan();

        if (btnPressed == 1) {
          coinsRemoved = true;
        } else if (btnPressed == 2) {
          coinsRemoved = false;
          accessGranted = false;
          btn2ValNew = 1;
          btn2ValOld = 1; // sets up for balance screen
        }

        if (coinsRemoved) {
          while (!twentyCoinsCounted) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num. 20p taken:");
            lcd.setCursor(0,1);
            lcd.print(coinCount);

            btnPressed = btnScan();
            
            if (btnPressed == 1) {
              coinCount++;
            } else if (btnPressed == 2 && coinCount > 0) {
              coinCount--;
            } else if (btnPressed == 3) {
              twentyCoinsCounted = true;
              num20s = num20s - coinCount;
              num20s = preventCycle(num20s);
              coinCount = 0;
              EEPROM.write(5, num20s);
            }
          }

          while (!tenCoinsCounted) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num. 10p taken:");
            lcd.setCursor(0,1);
            lcd.print(coinCount);

            btnPressed = btnScan();
            
            if (btnPressed == 1) {
              coinCount++;
            } else if (btnPressed == 2 && coinCount > 0) {
              coinCount--;
            } else if (btnPressed == 3) {
              tenCoinsCounted = true;
              num10s = num10s - coinCount;
              num10s = preventCycle(num10s);
              coinCount = 0;
              EEPROM.write(6, num10s);
            }
          }

          while (!fiveCoinsCounted) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num. 5p taken:");
            lcd.setCursor(0,1);
            lcd.print(coinCount);

            btnPressed = btnScan();
            
            if (btnPressed == 1) {
              coinCount++;
            } else if (btnPressed == 2 && coinCount > 0) {
              coinCount--;
            } else if (btnPressed == 3) {
              fiveCoinsCounted = true;
              num5s = num5s - coinCount;
              num5s = preventCycle(num5s);
              coinCount = 0;
              EEPROM.write(7, num5s);
            }
          }

          while (!twoCoinsCounted) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num. 2p taken:");
            lcd.setCursor(0,1);
            lcd.print(coinCount);

            btnPressed = btnScan();
            
            if (btnPressed == 1) {
              coinCount++;
            } else if (btnPressed == 2 && coinCount > 0) {
              coinCount--;
            } else if (btnPressed == 3) {
              twoCoinsCounted = true;
              num2s = num2s - coinCount;
              num2s = preventCycle(num2s);
              coinCount = 0;
              EEPROM.write(8, num2s);
            }
          }

          while (!oneCoinsCounted) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Num. 1p taken:");
            lcd.setCursor(0,1);
            lcd.print(coinCount);

            btnPressed = btnScan();
            
            if (btnPressed == 1) {
              coinCount++;
            } else if (btnPressed == 2 && coinCount > 0) {
              coinCount--;
            } else if (btnPressed == 3) {
              oneCoinsCounted = true;
              accessGranted = false;
              num1s = num1s - coinCount;
              num1s = preventCycle(num1s);
              coinCount = 0;
              balanceUpdated = true;
              EEPROM.write(9, num1s);
            }
          }
        }
        twentyCoinsCounted = false;
        tenCoinsCounted = false;
        fiveCoinsCounted = false;
        twoCoinsCounted = false;
        oneCoinsCounted = false;
        coinsRemoved = false;

        delay(btnDelay);
      }
      onBalanceScreen = true;
      isEnteringPin = false;
    }
  }
}

String createBalance(int x) {
  String a = String(x);
  String b = "";
  if (x < 100 && x >= 10) {
    b.concat('0');
    b.concat('.');
    b.concat(a[0]);
    b.concat(a[1]);
  } else if (x < 10) {
    b.concat('0');
    b.concat('.');
    b.concat('0');
    b.concat(a[0]);
  } else {
    for (int i = 0; i < a.length() - 2; i++) {
      b.concat(a[i]);
    }
    b.concat('.');
    b.concat(a[a.length() - 2]);
    b.concat(a[a.length() - 1]);
  }
  Serial.println(b);
  return b;
}

int preventCycle(int coinNum) {
  if (coinNum < 0) {
    return 0;
  } else {
    return coinNum;
  }
}