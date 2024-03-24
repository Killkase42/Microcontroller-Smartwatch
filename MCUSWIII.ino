#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Defined constants for OLED
#define i2c_Address 0x3c //OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //  RESET pin not connected
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long previousMillis = 0;  //Stores the last time amount of seconds increased
const long period = 1000;         //Period at which to increase seconds

bool stopwatchOn = false; //Whether or not respective clocks are ticking down/up
bool timerOn = false;

int editMode = 0; //Which feature on the screen is being edited
int functionMode = 0; //Which set of data (clock, stopwatch, etc.) is shown on the screen

//Variables for voltage checker
int vCheckerPin = 4; // analogue pin A4
float R1 = 4700.00; // CHANGE BASED ON MEASURED VALUES FOR NEW PCB
float R2 = 5090.00; // CHANGE BASED ON MEASURED VALUES FOR NEW PCB
int batteryPercentage; //Final result from voltage checker
int battSecs = 10; //Interval at which to check battery charge

struct clockFace { //Data type of 00:00:00 format
  int hour = 0;
  int minute = 0;
  int second = 0;
  int defHour = 0; //Default times, what time is restored to when timer or stopwatch reset button is pressed
  int defMinute = 0;
  int defSecond = 0;
};

struct clockFace watch;
struct clockFace stopwatch;
struct clockFace timer;

class Screens { //Class that covers the display of the watch, stopwatch, timer, sensor, and wifi module screens
  public:
    void watchScreen() { //Display of the watch screen
      const int watchTime[3] = {watch.hour, watch.minute, watch.second};//Array with clock variables
      display.setTextSize(2);
      display.setCursor(18, 25);
      for (int i = 0; i < 3; i++) {//Prints time with the attribute being edited as highlighted
        if (i == (editMode - 1)) {
          display.setTextColor(SH110X_BLACK, SH110X_WHITE);
        }
        if (watchTime[i] < 10) {
          display.print("0");
        }
        display.print(watchTime[i]);
        display.setTextColor(SH110X_WHITE);
        if (i < 2) {
          display.print(":");
        }
      }
    }
    void stopwatchScreen() { //Display of the stopwatch screen
      display.setTextSize(1);
      display.setCursor(5, 5);
      display.print("Stopwatch");
      const int watchTime[3] = {stopwatch.hour, stopwatch.minute, stopwatch.second};//Array with clock variables
      display.setTextSize(2);
      display.setCursor(18, 25);
      for (int i = 0; i < 3; i++) {//Prints time with the attribute being edited as highlighted
        if (i == (editMode - 1)) {
          display.setTextColor(SH110X_BLACK, SH110X_WHITE);
        }
        if (watchTime[i] < 10) {
          display.print("0");
        }
        display.print(watchTime[i]);
        display.setTextColor(SH110X_WHITE);
        if (i < 2) {
          display.print(":");
        }
      }
    }
    void timerScreen() { //Display of the timer screen
      display.setTextSize(1);
      display.setCursor(5, 5);
      display.print("Timer");
      const int watchTime[3] = {timer.hour, timer.minute, timer.second};//Array with clock variables
      display.setTextSize(2);
      display.setCursor(18, 25);
      for (int i = 0; i < 3; i++) {//Prints time with the attribute being edited as highlighted
        if (i == (editMode - 1)) {
          display.setTextColor(SH110X_BLACK, SH110X_WHITE);
        }
        if (watchTime[i] < 10) {
          display.print("0");
        }
        display.print(watchTime[i]);
        display.setTextColor(SH110X_WHITE);
        if (i < 2) {
          display.print(":");
        }
      }
    }
    void sensorsScreen() { //Display of the sensor screen
      display.setTextSize(1);
      display.setCursor(5, 5);
      display.print("Sensors");
    }
    void modulesScreen() { //Display of the wifi module screen-----------------------------------------------------------------------

    }
};

class Input { //Covers button signal input and debouncing
  private:
    uint8_t btn;
    uint16_t state;
  public:
    void start(uint8_t button) {
      btn = button;
      state = 0;
      pinMode(btn, INPUT_PULLUP);
    }
    bool debounce() {
      state = (state << 1) | digitalRead(btn) | 0xfe00;
      return (state == 0xff00);
    }
};

Input top;
Input mid;
Input bottom;

class Buttons { //Covers button visuals and functionality
  public:
    void func() { //Move on to the next function screen
      display.setTextSize(1);
      display.setCursor(52, 57);
      display.print("Func");
      if (mid.debounce()) {
        if (functionMode < 3) {
          functionMode++;
        } else {
          functionMode = 0;
        }
      }
    }
    void edit() { //Begin to edit attributes of current clock face
      display.setTextSize(1);
      display.setCursor(87, 57);
      display.print("Edit");
      if (bottom.debounce()) {
        editMode = 1;
      }
      timerOn = false; //Prevents stopwatch/timer from ticking up/down when editing
      stopwatchOn = false;
    }
    void conf() { //Confirm current and move on to the next editable attribute of the clock screen being edited
      display.setTextSize(1);
      display.setCursor(87, 57);
      display.print("Conf");
      if (bottom.debounce()) {
        if (editMode < 3) {
          editMode++;
        } else {
          editMode = 0;
          if (functionMode == 2) { //Set timer default time, enabling the timer to be reset
            timer.defHour = timer.hour;
            timer.defMinute = timer.minute;
            timer.defSecond = timer.second;
          }
        }
      }
    }
    void modu() { //Bring up the wifi menu screen
      display.setTextSize(1);
      display.setCursor(17, 57);
      display.print("Modu");
    }
    void incr() { //Button to increase hour/min/sec on the watch and timer 
      display.setTextSize(1);
      display.setCursor(17, 57);
      display.print("Incr");
      if (top.debounce()) {
        switch (functionMode) {
          case 0: //Increase the hour/minute/second of the watch
            switch (editMode) {
              case 1:
                watch.hour++;
                break;
              case 2:
                watch.minute++;
                break;
              case 3:
                watch.second++;
                break;
            }
            break;
          case 2: //Increase the hour/minute/second of the timer
            switch (editMode) {
              case 1:
                timer.hour++;
                break;
              case 2:
                timer.minute++;
                break;
              case 3:
                timer.second++;
                break;
            }
            break;
        }
      }
    }
    void decr() { //Button to decrease hour/min/sec on the watch and timer
      display.setTextSize(1);
      display.setCursor(52, 57);
      display.print("Decr");
      if (mid.debounce()) {
        switch (functionMode) {
          case 0: //Decrease the hour/minute/second of the watch
            switch (editMode) {
              case 1:
                watch.hour--;
                break;
              case 2:
                watch.minute--;
                break;
              case 3:
                watch.second--;
                break;
            }
            break;
          case 2: //Decrease the hour/minute/second of the timer
            switch (editMode) {
              case 1:
                timer.hour--;
                break;
              case 2:
                timer.minute--;
                break;
              case 3:
                timer.second--;
                break;
            }
            break;
        }
      }
    }
    void strt() { //Button to start stopwatch and timer
      display.setTextSize(1);
      display.setCursor(17, 57);
      display.print("Strt");
      if (top.debounce()) {
        switch (functionMode) {
          case 1: //Turn on the stopwatch
            stopwatchOn = true;
            break;
          case 2: //Turn on the timer
            timerOn = true;
            break;
        }
      }
    }
    void stop1() { //Button to stop stopwatch and timer
      display.setTextSize(1);
      display.setCursor(17, 57);
      display.print("Stop");
      if (top.debounce()) {
        switch (functionMode) {
          case 1: //Turn off the stopwatch
            stopwatchOn = false;
            break;
          case 2: //Turn off the timer
            timerOn = false;
            break;
        }
      }
    }
    void rset() { //Resets the stopwatch and timer
      display.setTextSize(1);
      display.setCursor(87, 57);
      display.print("Rset");
      if (bottom.debounce()) {
        switch (functionMode) {
          case 1:
            stopwatch.hour = stopwatch.defHour;
            stopwatch.minute = stopwatch.defMinute;
            stopwatch.second = stopwatch.defSecond;
            break;
          case 2:
            timer.hour = timer.defHour;
            timer.minute = timer.defMinute;
            timer.second = timer.defSecond;
            break;
        }
      }
    }
};

void setup() {
  pinMode(3, INPUT_PULLUP); //For unused pins lacking external pullup resistors
  pinMode(2, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  pinMode(vCheckerPin, INPUT); //Voltage checker pin

  top.start(15); //Initialize buttons
  mid.start(16);
  bottom.start(17);

  delay(250); //Allow OLED to warm up before connecting to it
  display.begin(i2c_Address, true);//Start I2C connection with OLED
  //display.setContrast(0); // dim display
  display.display();
}

void increment_time() { //Increases watch and stopwatch hours/minutes/seconds, decreases for timer, over time
  unsigned long currentMillis = millis(); //Store the current time
  if (currentMillis / 1000 - previousMillis / 1000 >= period / 1000) { //Check if 1000ms has passed; dividing by 1000 increases accuracy
    previousMillis = currentMillis; //Save the last time seconds increased
    watch.second++; //Increments times every second
    battSecs++; //Increment time for which battery charge is checked
    if (stopwatchOn) {
      stopwatch.second++;
    }
    if (timerOn) {
      timer.second--;
    }
  }
  if (watch.second > 59) { //Corrects for out-of-range times on the watch
    watch.minute++;
    watch.second = 0;
  } else if (watch.second < 0) {
    watch.minute--;
    watch.second = 59;
  }
  if (watch.minute > 59) {
    watch.hour++;
    watch.minute = 0;
  } else if (watch.minute < 0) {
    watch.hour--;
    watch.minute = 59;
  }
  if (watch.hour > 24) {
    watch.hour = 0;
  } else if (watch.hour < 0) {
    watch.hour = 24;
  }
  if (timer.second > 59) { //Corrects for out-of-range times on the timer
    timer.minute++;
    timer.second = 0;
  } else if (timer.second < 0) {
    if (timer.minute == 0 && timer.hour == 0) {
      timerOn = false;
      timer.second = 0;
    } else {
      timer.minute--;
      timer.second = 59;
    }
  }
  if (timer.minute > 59) {
    timer.hour++;
    timer.minute = 0;
  } else if (timer.minute < 0) {
    timer.hour--;
    timer.minute = 59;
  }
  if (timer.hour > 99) {
    timer.hour = 0;
  } else if (timer.hour < 0) {
    timer.hour = 24;
  }
  if (stopwatch.second > 59) { //Corrects for out-of-range times on the stopwatch
    stopwatch.minute++;
    stopwatch.second = 0;
  } else if (stopwatch.second < 0) {
    stopwatch.minute--;
    stopwatch.second = 59;
  }
  if (stopwatch.minute > 59) {
    stopwatch.hour++;
    stopwatch.minute = 0;
  } else if (stopwatch.minute < 0) {
    stopwatch.hour--;
    stopwatch.minute = 59;
  }
  if (stopwatch.hour > 99) {
    if (stopwatchOn) {
      stopwatchOn = false;
      stopwatch.hour = 99;
      stopwatch.minute = 59;
      stopwatch.second = 59;
    } else {
      stopwatch.hour = 0;
    }
  } if (stopwatch.hour < 0) {
    stopwatch.hour = 99;
  }
}

float voltage_checker() { //Intakes voltage level from voltage divider and calculates battery voltage
  float vInValue = analogRead(vCheckerPin); // intake value from analogue pin
  float voltage = vInValue * (3.3 / 1023) * ((R1 + R2) / R2); // calculates voltage of battery, 3.3volts corresponds to max sensor value of 1023
  return (voltage);
}

void displayBattPcnt() { //
  if (battSecs == 10) { //Only calls voltage checker function once every 10 seconds to avoid flickering
    batteryPercentage = (voltage_checker() - 3.0) / 1.2 * 100; // calculates battery percentage based on protection IC's cutoff of 3volts
    battSecs = 0; //Resets 10-second voltage checker function call time
  }
  display.setTextSize(1);
  display.setCursor(75, 5);
  display.print("Batt:");
  display.print(batteryPercentage);
  display.print("%");
}

void loop() {
  Screens screen;
  Buttons button;

  increment_time();

  displayBattPcnt();

  switch (functionMode) { //Arrange attributes of each function mode on screen
    case 0: //Watch
      screen.watchScreen();
      switch (editMode) {
        case 0:
          button.modu();
          button.func();
          button.edit();
          break;
        default:
          button.incr();
          button.decr();
          button.conf();
          break;
      }
      break;
    case 1: //Stopwatch
      screen.stopwatchScreen();
      switch (editMode) {
        case 0:
          button.func();
          button.rset();
          if (stopwatchOn) {
            button.stop1();
          } else {
            button.strt();
          }
          break;
        default:
          button.incr();
          button.decr();
          button.conf();
          break;
      }
      break;
    case 2: //Timer
      screen.timerScreen();
      switch (editMode) {
        case 0:
          button.func();
          if (timer.hour == timer.defHour && timer.minute == timer.defMinute && timer.second == timer.defSecond) { //Give option to edit if timer time equals equals default
            button.edit();
          } else { //Give option to reset if the timer time equals its default
            button.rset();
          }
          if (timerOn) {
            button.stop1();
          } else {
            button.strt();
          }
          break;
        default:
          button.incr();
          button.decr();
          button.conf();
          break;
      }
      break;
    case 3: //Sensors
      screen.sensorsScreen();
      button.func();
      break;
  }

  display.display();//Displays every specified aspect to screen
  display.clearDisplay();//Clears screen for next loop iteration
}
