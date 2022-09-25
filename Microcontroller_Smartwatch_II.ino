#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



float Vin = 0.00;//Variables for voltage checker
float R1 = 100000.00; // resistance of R1 (100K)
float R2 = 10000.00; // resistance of R2 (10K)
int val = 0;
int batteryPercentage = 100; //Final result from the voltage checker

int buttonStatus1 = 0;//Pushed/unpushed status of each button
int buttonStatus2 = 0;
int buttonStatus3 = 0;

unsigned long previousMillis = 0;  //Stores the last time amount of seconds increased
const long period = 1000;         //Period at which to increase seconds

int seconds = 0;//Time variables for clock
int minutes = 0;
int hours = 0;
int stopwatchSeconds = 0;//Time variables for stopwatch
int stopwatchMinutes = 0;
int stopwatchHours = 0;
int timerSeconds = 0;//Time variables for timer
int timerMinutes = 0;
int timerHours = 0;
int timerSecondsOrig = 0;
int timerMinutesOrig = 0;
int timerHoursOrig = 0;

bool stopwatchOn = false; //Whether the stopwatch is on or not
bool timerOn = false; //Whether the timer is on or not

int editMode = 0; //Which feature on the screen is being edited
int functionMode = 0; //Which set of data (clock, stopwatch, etc.) is shown on the screen

int secondsCounter = 0; //Iterations of the main loop

void setup()   {
  //delay(250);
  display.begin(i2c_Address, true);//Start I2C connection with OLED
  display.setContrast (0); // dim display
  

  display.clearDisplay();



  pinMode(2, INPUT_PULLUP);//Button inputs
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);

  //pinMode([analog pin], INPUT);//Voltage checker input
  

}

/*
  void VoltageChecker(){
  Vin = ((analogRead(analogInput); * 5.00) / 1024.00;) / (R2/(R1+R2));//Calculates voltage of battery
   if (Vin<0.09)//Read battery voltage aszero if it drops below certain range
   {
     Vin = 0.00;
  }

  batteryPercentage = Vin/4.2*100;
  return(batteryPercentage);
  }
*/



void loop() {
  unsigned long currentMillis = millis(); //Store the current time


  if (currentMillis / 1000 - previousMillis / 1000 >= period / 1000) { //Check if 1000ms has passed and pass all time-related function if so, dividing by 1000 increases accuracy
    previousMillis = currentMillis;   //Save the last time seconds increased
    seconds ++;//Increases times every second
    if (stopwatchOn) {
      stopwatchSeconds ++;
    }
    if (timerOn) {
      timerSeconds --;
    }
  }




  if (seconds >= 60) {//Corrects for out of range times on clock
    seconds = 0;
    minutes++;
  } else {
    //secondsCounter ++;
  }
  if (minutes >= 60) {
    minutes = 0;
    hours++;
  }
  if (hours >= 24) {
    hours = 0;
  }
  if (seconds <= -1) {
    seconds = 59;
    minutes --;
  }
  if (minutes <= -1) {
    minutes = 59;
    hours --;
  }
  if (hours <= -1) {
    hours = 23;
  }

  if (stopwatchSeconds == 60) {//Corrects for out of range times on stopwatch
    stopwatchSeconds = 0;
    stopwatchMinutes++;
  }
  if (stopwatchMinutes == 60) {
    stopwatchMinutes = 0;
    stopwatchHours++;
  }
  if (stopwatchHours == 100) {//Stops stopwatch at 99:59:59
    stopwatchOn = false;
    stopwatchHours = 99;
    stopwatchMinutes = 59;
    stopwatchSeconds = 59;
  }

  if (functionMode == 3 && editMode != 0) {//Corrects for out of range times on timer
    if (timerSeconds == 60) {
      timerSeconds = 0;
      timerMinutes++;
    }
    if (timerMinutes == 60) {
      timerMinutes = 0;
      timerHours++;
    }
    if (timerHours == 99) {
      timerHours = 0;
    }
  }
  if (timerOn) { //Controls how the timer ticks down
    if (timerSeconds == -1) {
      if (timerMinutes > 0 || timerHours > 0) {
        timerSeconds = 59;
        timerMinutes--;
      } else {
        timerSeconds = 0;
        timerOn = false;
      }
      if (timerMinutes == -1) {
        if (timerHours > 0) {
          timerMinutes = 59;
          timerHours --;
        } else {
          timerMinutes = 0;
        }
      }
    }
  }

  display.setTextSize(1);//Using voltage checker function to find and then display battery voltage
  display.setTextColor(SH110X_WHITE);
  display.setCursor(74, 0);
  display.print("Batt:");
  //display.print(VoltageChecker(););
  display.print("%");
  display.setCursor(0, 30);

  int pinValue1 = digitalRead(2);//Intake the status of buttons
  int pinValue2 = digitalRead(3);
  int pinValue3 = digitalRead(4);

  if (buttonStatus1 != pinValue1) {//Determine status of each button via code
    buttonStatus1 = pinValue1;
  }
  if (buttonStatus2 != pinValue2) {
    buttonStatus2 = pinValue2;
  }
  if (buttonStatus3 != pinValue3) {
    buttonStatus3 = pinValue3;
  }

  if (functionMode == 0) {//Clock
    const int clockTime[3] = {hours, minutes, seconds};//Array with clock variables

    display.setTextSize(2);
    display.setCursor(18, 25);
    for (int i = 0; i < 3; i++) {//Prints time with the attribute being edited as highlighted
      if (i == (editMode - 1)) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      }
      if (clockTime[i] < 10) {
        display.print("0");
      }
      display.print(clockTime[i]);
      display.setTextColor(SH110X_WHITE);
      if (i < 2) {
        display.print(":");
      }
    }

    display.setTextSize(1);
    display.setCursor(17, 57);
    if (editMode == 0) {
      display.print("Modu  Func  Conf");
      if (buttonStatus1 == 0) {//Do something with the status of each button
        functionMode ++;
      }
      if (buttonStatus2 == 0) {
        functionMode += 2;
      }
    } else if (editMode == 1) { //Change the amount of hours on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0) {
        hours ++;
      }
      if (buttonStatus2 == 0) {
        hours --;
      }
    } else if (editMode == 2) { //Change the amount of minutes on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0) { //Do something with the status of each button
        minutes ++;
      }
      if (buttonStatus2 == 0) {
        minutes --;
      }
    } else if (editMode == 3) { //Change the amount of seconds on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0) { //Do something with the status of each button
        seconds ++;
      }
      if (buttonStatus2 == 0) {
        seconds --;
      }
    }
    if (editMode > 3) {
      editMode = 0;
    }
    if (buttonStatus3 == 0) {
      editMode ++;
    }

  } else if (functionMode == 1) { //Modudle function select screen
    

    const char* modules[6] = {"WiFiNINA", "Bluetooth", "Alti:", "Pres:", "Temp:", "Humi:"};//A pointer of all the sensors/modules

    display.setCursor(0, 5);
    for (int i = 0; i < 6; i++) { //Prints all the modules to the screen with the selected one higlighted
      if (i == editMode) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      }
      display.println(modules[i]);
      display.setTextColor(SH110X_WHITE);
    }

    if (buttonStatus1 == 0) { //Highlight the next module for selection
      editMode ++;
    }
    if (buttonStatus2 == 0) { //Run the function of each module

    }
    if (buttonStatus3 == 0) { //Go back to main clock screen
      editMode = 0;
      functionMode = 0;
    }
    if (editMode == 6) { //Go back to the beginning of the selection
      editMode = 0;
    }

    display.setCursor(17, 57);
    display.print("Next  Scan  Back");

  } else if (functionMode == 2) { //Stopwatch
    const int stopwatchTime[3] = {stopwatchHours, stopwatchMinutes, stopwatchSeconds};//Array with stopwatch variables

    display.setTextSize(2);
    display.setCursor(18, 25);
    for (int i = 0; i < 3; i++) { //Prints stopwatch time
      if (stopwatchTime[i] < 10) {
        display.print("0");
      }
      display.print(stopwatchTime[i]);
      display.setTextColor(SH110X_WHITE);
      if (i < 2) {
        display.print(":");
      }
    }

    display.setTextSize(1);//Print labels to screen
    display.setCursor(0, 5);
    display.print("Stopwatch:");
    display.setCursor(17, 57);
    display.print("      Func  Rset");
    if (stopwatchOn) {
      display.setCursor(17, 57);
      display.print("Stop");
    } else {
      display.setCursor(17, 57);
      display.print("Strt");
    }

    if (buttonStatus1 == 0 && stopwatchOn) {//Turn stopwatch on/off
      stopwatchOn = false;
    } else if (buttonStatus1 == 0 && !stopwatchOn) {
      stopwatchOn = true;
    }
    if (buttonStatus2 == 0) {//Next function
      functionMode ++;
    }
    if (buttonStatus3 == 0) {//Reset stopwatch
      stopwatchSeconds = 0;
      stopwatchMinutes = 0;
      stopwatchHours = 0;
    }

  } else if (functionMode == 3) {//Timer
    const int timerTime[3] = {timerHours, timerMinutes, timerSeconds};//Array with timer variables

    display.setTextSize(2);
    display.setCursor(18, 25);
    for (int i = 0; i < 3; i++) {//Prints timer time with the attribute being edited as highlighted
      if (i == (editMode - 1)) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      }
      if (timerTime[i] < 10) {
        display.print("0");
      }
      display.print(timerTime[i]);
      display.setTextColor(SH110X_WHITE);
      if (i < 2) {
        display.print(":");
      }
    }

    if (buttonStatus3 == 0) {//Edit next attribute of timer
      editMode ++;
    }
    if (editMode >= 4) {
      timerSecondsOrig = timerSeconds;
      timerMinutesOrig = timerMinutes;
      timerHoursOrig = timerHours;
      editMode = 0;
    }

    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("Timer:");//Print labels to screen

    if (editMode == 0) {
      display.setCursor(17, 57);//Print labels to screen
      display.print("            Conf");

      if (timerOn) {//Print labels to screen
        display.setCursor(17, 57);
        display.print("Stop");
      } else if (!timerOn) {
        display.setCursor(17, 57);
        display.print("Strt");
      }
      display.setCursor(17, 57);
      display.print("      Func");

      if (buttonStatus1 == 0 && timerOn) {//Turn the timer on/off
        timerOn = false;
      } else if (buttonStatus1 == 0 && !timerOn) {
        timerOn = true;
      }
      if (timerSeconds + timerMinutes + timerHours == 0) {//Turn the timer off if the remaining time is zero
        timerOn = false;
      }
      if (buttonStatus2 == 0) {//Next function
        functionMode ++;
      }
    } else if (editMode == 1) {
      if (buttonStatus1 == 0) {//Increase timer hours
        timerHours ++;
      }
    } else if (editMode == 2) {
      if (buttonStatus1 == 0) {//Increase timer minutes
        timerMinutes ++;
      }
    } else if (editMode == 3) {
      if (buttonStatus1 == 0) {//Increase timer seconds
        timerSeconds ++;
      }
    }

    if (editMode >= 1) {
      display.setCursor(17, 57);
      display.print("Incr");
      display.setCursor(17, 57);
      display.print("            Next");

      if (timerSeconds == timerSecondsOrig && timerMinutes == timerMinutesOrig && timerHours == timerHoursOrig) {//Quick exit to timer main screen
        display.setCursor(53, 57);
        display.print("Done");
        if (buttonStatus2 == 0) {
          editMode = 0;
          buttonStatus2 = 1;
        }
      } else {//Reset timer to specified number
        display.setCursor(51, 57);
        display.print("Rset");
        if (buttonStatus2 == 0) {
          timerSeconds = timerSecondsOrig;
          timerMinutes = timerMinutesOrig;
          timerHours = timerHoursOrig;
        }
      }
    }

  } else if (functionMode > 3) {//Return to the clock screen
    functionMode = 0;
  }

  display.display();//Displays every specified aspect to screen
  display.clearDisplay();//Clears screen for next loop iteration
}
