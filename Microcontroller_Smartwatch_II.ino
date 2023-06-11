#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_MS8607.h>
#include <Adafruit_Sensor.h>
#include <avr/dtostrf.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <string.h>

Adafruit_MS8607 ms8607;

#define SPIWIFI       SPI
#define SPIWIFI_SS    5  // Chip select pin
#define SPIWIFI_ACK    11   // a.k.a BUSY or READY pin
#define ESP32_RESETN   2   // Reset pin
#define ESP32_GPIO0   -1   // Not connected

#define i2c_Address 0x3c //Define I2C address of OLED
#define i2c_Address_Pressure 0x76 //Define I2C adress of pressure sensor
#define i2c_Adress_Humidity 0x40 //Define I2C adress of humidity sensor

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //  RESET pin not connected
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool button_debounce1 = false;//Basis of functions that only register single button press instead of hold for each button
bool button_debounce2 = false;
bool button_debounce3 = false;
int debounce_reset;//Resets the button_debounce variable when time reached

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
const long secondaryDeboucePer = 500; // period at which the switches will register inputs after they are held for one second

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

int leftButtonPort = 6;
int middleButtonPort = 10;
int rightButtonPort = 7;

bool scanWifi = false; // whether or not wifi is scanned on the current code iteration

void setup()   {
  pinMode(3, INPUT_PULLUP);//using the internal pullup resistors on all unconected pins to eliminate interference
  pinMode(4, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);

  pinMode(leftButtonPort, INPUT_PULLUP);//Setting ports as inputs
  pinMode(middleButtonPort, INPUT_PULLUP);
  pinMode(rightButtonPort, INPUT_PULLUP);

  //pinMode([analog pin], INPUT);//Voltage checker input

  // setup pins for wifi
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);

  delay(250);
  display.begin(i2c_Address, true);//Start I2C connection with OLED
  display.setContrast (0); // dim display
  display.clearDisplay();

  // check for the WiFi module:CHANGE LATER TO BE DISPLAYED ONLY WHEN WIFI FUNCTION IS USED
  while (WiFi.status() == WL_NO_MODULE) {
    display.clearDisplay();//Clears screen for next loop iteration
    display.setCursor(0, 15);
    display.print("Communication with wifi module failed");
    display.display();//Displays every specified aspect to screen
    // don't continue
    delay(1000);
  }

  ms8607.begin();

  ms8607.setHumidityResolution(MS8607_HUMIDITY_RESOLUTION_OSR_8b);//Set humidity sensor resolution
  ms8607.setPressureResolution(MS8607_PRESSURE_RESOLUTION_OSR_256);//Set pressure/temperature sensor resolution


  /*switch (ms8607.getHumidityResolution()){
    case MS8607_HUMIDITY_RESOLUTION_OSR_12b: Serial.println("12-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_11b: Serial.println("11-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_10b: Serial.println("10-bit"); break;
    case MS8607_HUMIDITY_RESOLUTION_OSR_8b: Serial.println("8-bit"); break;
    }*/
  /*switch (ms8607.getPressureResolution()){
    case MS8607_PRESSURE_RESOLUTION_OSR_256: Serial.println("256"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_512: Serial.println("512"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_1024: Serial.println("1024"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_2048: Serial.println("2048"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_4096: Serial.println("4096"); break;
    case MS8607_PRESSURE_RESOLUTION_OSR_8192: Serial.println("8192"); break;
    }*/
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

  sensors_event_t temp, pressure, humidity;//Defines what data will be requested of the sensors

  if (seconds % 5 == 0) {
    ms8607.getEvent(&pressure, &temp, &humidity);//Retrieves the data from  sensors
  }

  if (currentMillis / 1000 - previousMillis / 1000 >= period / 1000) { //Check if 1000ms has passed and pass all time-related function if so; dividing by 1000 increases accuracy
    previousMillis = currentMillis;   //Save the last time seconds increased
    seconds ++; // increases respective times every second
    if (stopwatchOn) {
      stopwatchSeconds ++;
    }
    if (timerOn) {
      timerSeconds --;
    }


    button_debounce1 = false;// results in a button registering inputs every second
    button_debounce2 = false;
    button_debounce3 = false;

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
  display.print("Batt:");//Replace with sprintf
  //display.print(VoltageChecker(););
  display.print("%");
  display.setCursor(0, 30);

  int pinValue1 = digitalRead(leftButtonPort);//Intake the status of buttons
  int pinValue2 = digitalRead(middleButtonPort);
  int pinValue3 = digitalRead(rightButtonPort);
  buttonStatus1 = pinValue1; // assign the status of each button via code
  buttonStatus2 = pinValue2;
  buttonStatus3 = pinValue3;

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
      if (buttonStatus1 == 0 && !button_debounce1) {//Do something with the status of each button
        functionMode ++;
        button_debounce1 = true;
      }
      if (buttonStatus2 == 0 && !button_debounce2) {
        functionMode += 3;
        button_debounce2 = true; // toggle button debounce flag
      }
      if (buttonStatus3 == 0 && !button_debounce3) {
        editMode ++;
        button_debounce3 = true;
      }

    } else if (editMode == 1) { //Change the amount of hours on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0 && !button_debounce1) {
        hours ++;
        button_debounce1 = true; // toggle button debounce flag
      }
      if (buttonStatus2 == 0 && !button_debounce2) {
        hours --;
        button_debounce2 = true; // toggle button debounce flag
      }
    } else if (editMode == 2) { //Change the amount of minutes on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0 && !button_debounce1) { //Do something with the status of each button
        minutes ++;
        button_debounce1 = true; // toggle button debounce flag
      }
      if (buttonStatus2 == 0 && !button_debounce2) {
        minutes --;
        button_debounce2 = true; // toggle button debounce flag
      }
    } else if (editMode == 3) { //Change the amount of seconds on the clock
      display.print("Incr  Decr  Next");
      if (buttonStatus1 == 0 && !button_debounce1) { //Do something with the status of each button
        seconds ++;
        button_debounce1 = true; // toggle button debounce flag
      }
      if (buttonStatus2 == 0 && !button_debounce2) {
        seconds --;
        button_debounce2 = true; // toggle button debounce flag
      }
    }
    if (editMode > 3) {
      editMode = 0;
    }


  } else if (functionMode == 1) { //Module function select screen

    char* modules[3] = {"WiFiNINA", "Bluetooth", "Settings"};//A pointer of all the sensors+data&modules

    display.setCursor(0, 5);
    for (int i = 0; i < 3; i++) { //Prints all the modules to the screen with the selected one higlighted
      if (i == editMode) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      }
      display.println(modules[i]);
      display.setTextColor(SH110X_WHITE);
    }

    if (buttonStatus1 == 0 && !button_debounce1) { //Highlight the next module for selection
      editMode ++;
      button_debounce1 = true;
    }
    if (buttonStatus2 == 0 && !button_debounce2) { //Run the function of each module
      button_debounce2 = true;
      if (editMode == 0) {
        scanWifi = true;
        functionMode++;

      } else if (editMode == 1) {

      } else if (editMode == 2) {

      }
    }
    if (buttonStatus3 == 0 && !button_debounce3) { //Go back to main clock screen
      editMode = 0;
      functionMode = 0;
      button_debounce3 = true;
    }
    if (editMode > 2) { //Go back to the beginning of the selection
      editMode = 0;
    }

    display.setCursor(17, 57);
    display.print("Next  Scan  Back");

  } else if (functionMode == 2) {

    display.clearDisplay();
    display.setCursor(0, 0);



    if (scanWifi) {
      int numSsid = WiFi.scanNetworks();

      if (numSsid == -1) {
        display.print("Connection Failed");
      } else {
        display.println(WiFi.SSID(0));
        display.println(WiFi.SSID(1));
        display.print(WiFi.SSID(2));

      }
    }





    /*if (thisNet == selectedNetwork) {
              display.setTextColor(SH110X_BLACK, SH110X_WHITE);
            } else {
              display.setTextColor(SH110X_WHITE);
            }*/






    display.setCursor(17, 57);
    display.print("Next  Scan  Back");

    if (buttonStatus1 == 0 && !button_debounce1) { //
      //selectedNetwork++;
      button_debounce1 = true;
    }
    if (buttonStatus2 == 0 && !button_debounce2) { //
      button_debounce2 = true;

    }
    if (buttonStatus3 == 0 && !button_debounce3) {

      button_debounce3 = true;
    }




    /*
      char* wifiFunctions[2] = {"Networks", "Chip Data", };//A pointer of the wifi functions

      display.setCursor(0, 5);
      for (int i = 0; i < 3; i++) { //Prints all the wifi functions to the screen with the selected one higlighted
      if (i == editMode) {
        display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      }
      display.println(modules[i]);
      display.setTextColor(SH110X_WHITE);
      }

      display.setCursor(17, 57);
      display.print("Next  Scan  Back");
    */
  }  else if (functionMode == 3) { //Stopwatch

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

    if (buttonStatus1 == 0 && stopwatchOn && !button_debounce1) {//Turn stopwatch on/off
      stopwatchOn = false;
      button_debounce1 = true;
    } else if (buttonStatus1 == 0 && !stopwatchOn && !button_debounce1) {
      stopwatchOn = true;
      button_debounce1 = true;
    }
    /*if (buttonStatus2 == 0) {//Next function
      functionMode ++;
      }*/
    if (buttonStatus2 == 0 && !button_debounce2) {
      functionMode ++;
      button_debounce2 = true; // toggle button debounce flag
    }

    if (buttonStatus3 == 0 && !button_debounce3) {//Reset stopwatch
      stopwatchSeconds = 0;
      stopwatchMinutes = 0;
      stopwatchHours = 0;
      button_debounce3 = true;
    }

  } else if (functionMode == 4) {//Timer
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

    if (buttonStatus3 == 0 && !button_debounce3) {//Edit next attribute of timer
      editMode ++;
      button_debounce3 = true;
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

      if (buttonStatus1 == 0 && timerOn && !button_debounce1) {//Turn the timer on/off
        timerOn = false;
        button_debounce1 = true;
      } else if (buttonStatus1 == 0 && !timerOn && !button_debounce1) {
        timerOn = true;
        button_debounce1 = true;
      }
      if (timerSeconds + timerMinutes + timerHours == 0) {//Turn the timer off if the remaining time is zero
        timerOn = false;
      }
      /*if (buttonStatus2 == 0) {//Next function
        functionMode ++;
        }*/
      if (buttonStatus2 == 0 && !button_debounce2) {
        functionMode ++;
        button_debounce2 = true; // toggle button debounce flag
      }

    } else if (editMode == 1) {
      if (buttonStatus1 == 0 && !button_debounce1) {//Increase timer hours
        timerHours ++;
        button_debounce1 = true;
      }
    } else if (editMode == 2) {
      if (buttonStatus1 == 0 && !button_debounce1) {//Increase timer minutes
        timerMinutes ++;
        button_debounce1 = true;
      }
    } else if (editMode == 3) {
      if (buttonStatus1 == 0 && !button_debounce1) {//Increase timer seconds
        timerSeconds ++;
        button_debounce1 = true;
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
        if (buttonStatus2 == 0 && !button_debounce2) {
          editMode = 0;
          buttonStatus2 = 1;
          button_debounce2 = true;
        }
      } else {//Reset timer to specified number
        display.setCursor(51, 57);
        display.print("Rset");
        if (buttonStatus2 == 0 && !button_debounce2) {
          timerSeconds = timerSecondsOrig;
          timerMinutes = timerMinutesOrig;
          timerHours = timerHoursOrig;
          button_debounce2 = true;
        }
      }
    }

  } else if (functionMode == 5) {//Sensor data

    display.setTextSize(1);
    display.setCursor(0, 5);
    display.print("Sensor Data:");//Print labels to screen

    display.setCursor(17, 57);//Print labels to screen
    display.print("            Conf");
    display.setCursor(17, 57);
    display.print("      Func");
    display.setCursor(17, 57);
    display.print("????");


    /*
      if (buttonStatus2 == 0) {//Next function
       functionMode ++;
      }*/
    if (buttonStatus2 == 0 && !button_debounce2) {
      functionMode ++;
      button_debounce2 = true; // toggle button debounce flag
    }

    char newString2[6];
    dtostrf(pressure.pressure * 0.1, 6, 2, newString2);

    char newString21[6];
    dtostrf(temp.temperature, 6, 2, newString21);

    char newString22[6];
    dtostrf(humidity.relative_humidity, 6, 2, newString22);


    char buffer2[22];
    char buffer21[23];
    char buffer22[26];

    sprintf(buffer2, "Pres: %s kPa", newString2);
    sprintf(buffer21, "Temp: %s degC", newString21);
    sprintf(buffer22, "Humi: %s %%rH", newString22);

    char* modules1[3] = {buffer2, buffer21, buffer22};//A pointer of all the sensors+data&modules

    display.setCursor(0, 25);
    for (int i = 0; i < 3; i++) { //Prints all the modules to the screen with the selected one higlighted
      //if (i == editMode) {
      // display.setTextColor(SH110X_BLACK, SH110X_WHITE);
      //}
      display.println(modules1[i]);
      display.setTextColor(SH110X_WHITE);
    }



  } else if (functionMode > 4) {//Return to the clock screen
    functionMode = 0;
  }

  display.display();//Displays every specified aspect to screen
  display.clearDisplay();//Clears screen for next loop iteration


  if (currentMillis / 1000 - previousMillis / 1000 >= period / 5000) { // allows rapid button presses, but helps prevent skipping when button held down
    if (buttonStatus1 && button_debounce1) {
      button_debounce1 = false;
    }
    if (buttonStatus2 && button_debounce2) {
      button_debounce2 = false;
    }
    if (buttonStatus3 && button_debounce3) {
      button_debounce3 = false;
    }
  }
}
