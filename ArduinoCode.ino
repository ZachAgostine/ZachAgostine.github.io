/*------------------------------------------------Libraries and Global Variables----------------------------------------------------
Purpose: Declare libraries and global variables
Inputs: No descrete inputs.
Outputs: Allows user to control stepper motor and trigger timed delays.  Creates variables that are used in the code.
Use: Declare stepper and set pins that will output signal used for stepper motor controls.  Note the order - if pins a/b/c/d are used, declare pins as a/c/b/d.  
*/

#include <Stepper.h>
#include <TimerOne.h>

const int stepsPerRevolution = 32; //motor steps //Number of steps for the 28BYJ-48 series motor used. 
Stepper sugarStepper (stepsPerRevolution, 9, 11, 10, 12); //pins entered in a/c/b/d order. 

//variables
 bool haveCoffee;
 bool haveWater;
 bool haveCup;
 bool canBrew;
 bool brewRequest;
 int echoPin; 
 int trigPin;
 bool coffeeReady;
 bool brewStarted;
 bool sugarRequest;
 static int brewTime = 0;
int inputPin;
int outputPin;

/*----------------------------------------------Pin assignments and interrupt initialization----------------------------------------------------
Purpose: Setup pin modes and assignments and initialize the interrupt.
Inputs: No descrete inputs.
Outputs: No descrete outputs.
Use: Assign pins based on your board configuration. 
*/
void setup()
{
  Serial.begin(9600);

  //pinout
  pinMode(A0,INPUT_PULLUP); //cup Sensor photoresistor
  pinMode(2, INPUT_PULLUP); //coffee sensor / switch/ NOPB  
  pinMode(3, INPUT); //water / ultrasonic echo pin
  pinMode(4, OUTPUT);  // water / ultrsonic trig pin
  pinMode(7, INPUT); //receives command from NODEMCU
  pinMode(8, OUTPUT);  //relay trigger
  pinMode(9, OUTPUT); //sugga steppa
  pinMode(10, OUTPUT); //sugga steppa
  pinMode(11, OUTPUT); //sugga steppa
  pinMode(12, OUTPUT); //sugga steppa
  pinMode(13, OUTPUT); // indicator light

  //pin attach
  echoPin = 3; 
  trigPin = 4;
  inputPin = 7;
  outputPin = 8;

  //Initialize Timer interupt
  Timer1.initialize(100000); //trigger once every second
}

/*---------------------------------------------------------------------Main Loop--------------------------------------------------------------------------------
Purpose: Control the coffee maker.
Inputs: 

Pin A0 - photoresistor 
Pin 2 - switch
Pin 3 - receive ultrasonic ping 
Pin 7 - Signal sent by user via ESP8266 & NODE MCU board

Outputs: 
Pin 4 - send ultrasonic ping
Pin 8 - HIGH signal for water heater
Pin 9-12 - Signal for stepper 
Pin 13 - LED

Use and flow: A sensor check is run, which runs 3 functions that take descrete inputs from theultrasonic sensor (checking if there's water), limit switch (checking if there's coffee), and photoresisotr (checking if theres a cup). 
    When true, bool canBrew goes true.  After this, user can request coffee via Blynk, which sends a HIGH signal to 7.
    This runs the main brew cycle.  Function sugarStepper is run, which turns on the stepper motor for 3 rotations.  Brew function is then run, which uses a for loop with a time delay to send a HIGH signal to pin 8, which turns on the water heater.
    Each second, the timed interrupt is run, which checks that the cup is present.  If so, brew function continues.  If not, reset() function is run.
    When the brew() function completes, Arduino remains idle until user takes the cup.  When bool haveCup goes false, reset() is run. The reset() function checks if the cup is present via checkCup(), and resets all variables when FALSE. 
    User can now repeat the program. 
*/
void loop(){
  int lightValue = analogRead(A0);
  Serial.println(lightValue);
  
  //checks if coffee, cup and water are present
  //when all true, this statement is skipped
  if (canBrew == false && brewStarted == false){
    sensorCheck();
  }

  //user initiates coffee request and sensors all return true
  if (digitalRead(inputPin) == 1 && canBrew == true){
    brewRequest = true;
  }

  
  //brew cycle
  //attach interrupt, brew coffee, make sure cup is present
  while (canBrew == true && brewRequest == true && coffeeReady == false){
    Timer1.attachInterrupt(checkCup); 
    brewStarted = true;
    if (haveCup == true){
    addSugar(); //function that turns on stepper to add sugar
    brew(); //turns on water heater if cup is present
    }

    
    while (haveCup == false){
      Serial.print("Have cup returning false");
      digitalWrite(outputPin, LOW); //turn off heater
      reset(); //set everything false 
        }
     
    }
    
  // coffee is ready, which will blink a light and run the reset() function.  Reset occurs when cup is removed. This behavior is coded in the function, not in the loop. 
   while (coffeeReady == true){
    digitalWrite(outputPin, LOW);
    Timer1.detachInterrupt();
    digitalWrite(13, HIGH); //LED that indicates coffee is ready
    delay(700);
    digitalWrite(13, LOW);
    delay(100);
    reset();
    }
}

  
/*------------------------------------------------------------------------Functions-----------------------------------------------------------------*/

//the three sensor functions packaged with serial debug code.  When all three functions return true, canBrew goes true, which is required to initiate brew.
int sensorCheck(){
  checkWater();
  checkCoffee();
  checkCup();

  Serial.print("haveWater is returning ");
  Serial.println(haveWater);

    Serial.print("haveCup is returning ");
  Serial.println(haveCup);

    Serial.print("haveCoffee is returning ");
  Serial.println(haveCoffee);

  
  if(haveWater == true && haveCup == true && haveCoffee == true){
    Serial.println("All sensor checks true.  coffee can be brewed");
    canBrew = true;
  }
  else{
    canBrew = false;
  }
  delay(500);
}


//pings the water level and converts time to distance.  2 inches is roughly a full funnel of water, which is enough for a full cup of coffee.
int checkWater(){
  //send ping and listen
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  int duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  int inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  if (inches < 3){
    haveWater = true;
  }
}





//returns true if PB is pressed. 
int checkCoffee(){
  int checkPB = digitalRead(2);
  if (checkPB == 0){
    haveCoffee = true;
  }
}




//no cup returns ~400. With cup returns 0.  Using less than 10 to create a larger window just in case. 
int checkCup(){
  int checkPhoto = analogRead(A0);
  if (checkPhoto < 10){
    haveCup = true;
    }
  else{
    haveCup = false;
  }
    Serial.println(haveCup);
  }





//turns on stepper motor for 3 full rotations at max speed. 
int addSugar(){ 
  sugarStepper.setSpeed(700);
  sugarStepper.step(2048);
  sugarStepper.step(2048);
  sugarStepper.step(2048);
  Serial.print("Add sugar function is exiting");
}




//Turns water heater on via pin 8 as long as brewTime is less than brewLimit
//When brewLimit is reached, coffeeReady becomes true.
int brew(){
  Serial.println("BREW has started  RUNNING");
  const int brewLimit = 120; //number of seconds water heater will be on, achieved via 'delay(1000)'.
      
  if (brewTime < brewLimit){
    Serial.print("Have cup returning true and brew time is ");
    Serial.println(brewTime);
    digitalWrite(outputPin, HIGH);
    delay(1000);
    brewTime++;
  }

  if (brewTime == brewLimit){
    coffeeReady = true;
    Serial.print("coffeeready is returning ");
    Serial.println(coffeeReady);
  }
      
}



//all variables go false when the user takes the cup
int reset (){
    checkCup();
    if (haveCup == false){
      brewTime = 0;
      canBrew = false;
      brewRequest = false;
      brewStarted = false;
      haveCoffee = false;
      haveWater = false;
      coffeeReady = false;
      Serial.println("reset should be successful");
}
}
