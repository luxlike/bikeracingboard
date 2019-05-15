 /*
  Bike racing board for Bluetooth 
  
  created 2 May 2019
  modified 10 May 2019
  by Steve Kim
  This example code is in the public domain.
 
*/
#include <SoftwareSerial.h>

//#define DEBUG 1

//Bluetooth send constants
#define ACCEL         "w"
#define STEER_LEFT    "a"
#define STEER_RIGHT   "d"
#define NITRO         "n"
#define PAUSE         "q"
#define ENTER         "e"

#define RPM_LIMIT_COUNT 50

SoftwareSerial hm10(8,9); //RX, TX connection 

// set pin numbers for the five buttons:
const int upButton = 12;//bicycle hall sensor
const int downButton = 11;//not used
const int leftButton = 4;
const int rightButton = 5;
const int enterButton = 2;
const int escButton = 3;
const int ledPin = 13;


//Speed variable
int  rotate_count_thresh = 2;//Hall sensor sensitivity
bool hall_sensor_contacted = false;
int  rotate_count = 0;

//Duration measurement variables
float start_time;
float end_time;
float on_time;
float off_time;
float passed_time;

//Button State
bool leftButtonState = false;
bool rightButtonState = false;
bool enterButtonState = false;
bool escButtonState = false;
bool boostKeyState = false;

void debug_serial_print(const char *str)
{
  #ifdef DEBUG
    Serial.print(str);
  #endif
}

void debug_serial_println(const char *str)
{
  #ifdef DEBUG
    Serial.println(str);;
  #endif
}

void setup() { // initialize the buttons' inputs:
  pinMode(upButton, INPUT);
  pinMode(downButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(enterButton, INPUT);
  pinMode(escButton, INPUT);
  pinMode(ledPin,OUTPUT);

  Serial.begin(115200);
  hm10.begin(9600);
}

void loop() { 

  rotate_count = 0;
  hall_sensor_contacted = false;

  start_time = micros();  
  on_time = micros();

  while(true){
    //Button input processing
    ///////////// Left Button //////////////////////////////////////////////////////
    if(digitalRead(leftButton) == HIGH && leftButtonState == false){//left button on
      if(digitalRead(rightButton) == LOW && rightButtonState == false){//left only on
        //Keyboard.press(key_left);
        hm10.write(STEER_LEFT);
        
        debug_serial_println("key_left pressed."); 
        leftButtonState = true;
      }      
    }else if(digitalRead(leftButton) == LOW && leftButtonState == true){//left button off
      //Keyboard.release(key_left);
      debug_serial_println("key_left released.");    
      leftButtonState = false;
      boostKeyState = false;
    }

    ///////////// Right Button //////////////////////////////////////////////////////
    if(digitalRead(rightButton) == HIGH && rightButtonState == false){//right button on
      if(digitalRead(leftButton) == LOW && leftButtonState == false){//right only on
        //Keyboard.press(key_right);
        hm10.write(STEER_RIGHT);
        
        debug_serial_println("key_right pressed.");
        rightButtonState = true;
      }      
    }else if(digitalRead(rightButton) == LOW && rightButtonState == true){//right button off
      //Keyboard.release(key_right);
      debug_serial_println("key_right released.");
      rightButtonState = false;
      boostKeyState = false;
    }

    ///////////// Left & Right On together => Boost /////////////////////////////////////
    if(digitalRead(leftButton) == HIGH  && digitalRead(rightButton) == HIGH && boostKeyState == false){     
          //Keyboard.press(key_boost);
          hm10.write(NITRO);
          
          debug_serial_println("key_boost pressed."); 
          leftButtonState = true;
          rightButtonState = true;
          boostKeyState = true;         
    }

    ///////////// Enter Button //////////////////////////////////////////////////////
    if(digitalRead(enterButton) == HIGH && enterButtonState == false){//enter button on
      //Keyboard.press(key_enter);
      hm10.write(ENTER);
      
      debug_serial_println("key_enter pressed.");
      enterButtonState = true;
    }else if(digitalRead(enterButton) == LOW && enterButtonState == true){//enter button off
      //Keyboard.release(key_enter);
      debug_serial_println("key_enter released.");
      enterButtonState = false;
    }

    ///////////// Esc Button //////////////////////////////////////////////////////
    if(digitalRead(escButton) == HIGH && escButtonState == false){//esc button on
      //Keyboard.press(key_esc);
      hm10.write(PAUSE);
      
      debug_serial_println("key_esc pressed.");
      escButtonState = true;
    }else if(digitalRead(escButton) == LOW && escButtonState == true){//esc button off
      //Keyboard.release(key_esc);
      debug_serial_println("key_esc released.");
      escButtonState = false;
    }
  
   ///////////// Bicycle Sensor Input //////////////////////////////////////////////////////
   if(digitalRead(upButton) == LOW) {//sensor on
        if(hall_sensor_contacted == false){
          debug_serial_println("hall_sensor_contacted 1");
          hall_sensor_contacted = true;
          rotate_count += 1;
          on_time = micros();
        }
    }else{
        hall_sensor_contacted = false;
        off_time = micros();
        passed_time = (off_time - on_time) / 1000000.0;
      
      if(passed_time >= 1){//when rotation stopped
        //debug_serial_println("break 1");
        break;
      }
    }

    if(rotate_count >= rotate_count_thresh){ 
      //debug_serial_println("break 2");
      break;    
    }
  }
  
  end_time = micros();
  passed_time = (end_time - start_time) / 1000000.0;
  float rpm_val = (rotate_count/passed_time)*60.0;

//  Serial.print("Time Passed: ");
//  Serial.print(passed_time);
//  debug_serial_println("s");
//  Serial.print(rpm_val);
//  debug_serial_println(" RPM");
  
  delay(1);        // delay in between reads for stability

  if(rpm_val >= RPM_LIMIT_COUNT){
    //Keyboard.press(key_up);
    hm10.write(ACCEL);
    debug_serial_println("key_up press");
  }else{
    //Keyboard.releaseAll();   
    //debug_serial_println("key release All");
  }  
} 
