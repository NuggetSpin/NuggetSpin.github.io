#include "HomeSpan.h"
#include "extras/Stepper_TB6612.h"     // this contains HomeSpan's StepperControl Class for the Adafruit TB6612 driver board
// #include <WiFi.h>
// #include <HTTPClient.h>

// const char* ssid = "pp 13Pro";
// const char* password = "3190102522pyc";

struct DEV_WindowShade : Service::WindowCovering {

  Characteristic::CurrentPosition currentPos{0,true};
  Characteristic::TargetPosition targetPos{0,true};
  // Characteristic::CurrentHorizontalTiltAngle currentTilt{0,true};
  // Characteristic::TargetHorizontalTiltAngle targetTilt{0,true};
  
  StepperControl *mainMotor;          // motor to open/close shade
  // StepperControl *slatMotor;          // motor to tilt shade slats

  DEV_WindowShade(StepperControl *mainMotor) : Service::WindowCovering(){

    this->mainMotor=mainMotor;                          // save pointers to the motors
    // this->slatMotor=slatMotor;
           
    mainMotor->setAccel(10,20);                         // set acceleration parameters for main motor
    mainMotor->setStepType(StepperControl::HALF_STEP);  // set step type to HALF STEP for main motor
        
    LOG0("Initial Open/Close Position: %d\n",currentPos.getVal());
    // LOG0("Initial Slat Position: %d\n",currentTilt.getVal());
    
    mainMotor->setPosition(currentPos.getVal()*20);       // define initial position of main motor
    // slatMotor->setPosition(currentTilt.getVal()*11.47);   // define initial position of slat motor
  }

  ///////////
  
  boolean update(){

    if(targetPos.updated()){
      
      // Move motor to absolute position, assuming 400 steps per revolution and 5 revolutions for full open/close travel,
      // for a total of 2000 steps of full travel. Specify that motor should enter the BRAKE state upon reaching to desired position.
      // Must multiply targetPos, which ranges from 0-100, by 20 to scale to number of motor steps needed
    
      mainMotor->moveTo(targetPos.getNewVal()*20,5,StepperControl::BRAKE);
      LOG1("Setting Shade Position=%d\n",targetPos.getNewVal());
    }

    // if(targetTilt.updated()){
      
    //   // Move motor to absolute position, assuming 2064 steps per revolution and 1/2 revolution for full travel of slat tilt in either direction
    //   // Must multiply targetPos, which ranges from -90 to 90, by 11.47 to scale number of motor steps needed
    //   // Note this driver board for this motor does not support a "short brake" state
    
    //   slatMotor->moveTo(targetTilt.getNewVal()*11.47,5);
    //   LOG1("Setting Shade Position=%d\n",targetTilt.getNewVal());
    // }

    return(true);
  }

  ///////////

  void loop(){

    // If the current window shade position or tilt does NOT equal the target position, BUT the motor has stopped moving,
    // we must have reached the target position, so set the current position equal to the target position
    
    if(currentPos.getVal()!=targetPos.getVal() && !mainMotor->stepsRemaining()){
      currentPos.setVal(targetPos.getVal());
      LOG1("Main Motor Stopped at Shade Position=%d\n",currentPos.getVal());
    }

    // if(currentTilt.getVal()!=targetTilt.getVal() && !slatMotor->stepsRemaining()){
    //   currentTilt.setVal(targetTilt.getVal());
    //   LOG1("Slat Motor Stopped at Shade Tilt=%d\n",currentTilt.getVal());
    // } 
            
  }
  
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }
  // Serial.print("Connected to WiFi:");
  // Serial.println(WiFi.localIP());

  homeSpan.setPairingCode("11122333");
  homeSpan.begin(Category::WindowCoverings,"window1");

  new SpanAccessory();                                                          
    new Service::AccessoryInformation();
      new Characteristic::Identify(); 
    new DEV_WindowShade(new Stepper_TB6612(5,4,6,7));   // instantiate drivers for each board and specify pins used on ESP32

}

void loop() {
  // put your main code here, to run repeatedly:
  homeSpan.poll();
}
