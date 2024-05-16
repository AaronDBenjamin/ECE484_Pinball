#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library

#define  SD_ChipSelectPin 53             //define CS pin


// Buttons
const int launcher_in = 8; // Button for Launching Ball
const int flipper_l_in = 9; // Button for using Left Flipper
const int flipper_r_in = 10; // Button for using Right Flipper


// Relay Control for moving parts
const int flipper_r_out = 2; // Relay control for Right Flipper
const int flipper_l_out = 3; // Relay control for Left Flipper
const int launcher_out = 5; // Relay control for Ball Launcher


// Variables for tracking things such as time or score
int ballLaunched = 0; //tracking variable for if we launched the ball
unsigned long score = 0;
unsigned long time;
unsigned long prevTime;


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


// Used for modifying the score array
long digit;


// Variables will change:
int OutputState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin


TMRpcm tmrpcm;  //create  an object for speaker library

// Array that is used to send a RS-485 serial command in proper sequence
int send_buffer[] = {0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F};


void setup() {
  pinMode(launcher_in, INPUT);
  pinMode(launcher_out, OUTPUT);


  // set initial launcher state
  digitalWrite(launcher_out, LOW);


  prevTime = 0;
  pinMode(flipper_r_in, INPUT);
  pinMode(flipper_l_in, INPUT);
  pinMode(flipper_r_out, OUTPUT);
  pinMode(flipper_l_out, OUTPUT);


  tmrpcm.speakerPin  = 11;	//define speaker pin pin 11 for mega 

  
  if (!SD.begin(SD_ChipSelectPin)) {	//see  if the card is present and can be initialized
    return;                           //don't  do anything more if not
  }
  
  tmrpcm.setVolume(5);	//0  to 7. Set volume level
  tmrpcm.play("SOUND1.wav");	//the sound file "1"  will play each time the arduino powers up, or is reset

  Serial.begin(9600);
  digitalWrite(13,HIGH); // turn on pin 13 to enable RS-485 converter

	// Send the values that we are using to initialize the array so that it is reset to all 0s to start with
  Serial.write(send_buffer[0]);
	Serial.write(send_buffer[1]);
	Serial.write(send_buffer[2]);
	Serial.write(send_buffer[3]);
	Serial.write(send_buffer[4]);
	Serial.write(send_buffer[5]);
	Serial.write(send_buffer[6]);
	Serial.write(send_buffer[7]);
	Serial.write(send_buffer[8]);
	Serial.write(send_buffer[9]);
	Serial.write(send_buffer[10]);
	Serial.write(send_buffer[11]);
	Serial.write(send_buffer[12]);
	Serial.write(send_buffer[13]);
	delay(2000);
}

void loop() {
  //flipper code
  if(digitalRead(flipper_r_in) == HIGH){
    digitalWrite(flipper_r_out, HIGH);

  } else{
    digitalWrite(flipper_r_out, LOW);
    
  }
  if(digitalRead(flipper_l_in) == HIGH){
    digitalWrite(flipper_l_out, HIGH);
  }else{
    digitalWrite(flipper_l_out, LOW);
  }

  //launcher code:
  // read the state of the switch into a local variable:
  int reading = digitalRead(launcher_in);

  // check to see if you just pressed the button
  // since the last press to ignore any noise:
  
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // fires the launcher if the new button state is HIGH
      
        OutputState = !OutputState;
      
    }
  }

  // fires the launcher:
  digitalWrite(launcher_out, OutputState);
  ballLaunched = 1; // start incrementing score now

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

	// Score handling
	if (ballLaunched == true)
	{
		if ((time - 10\00) > prevTime)
		{
			score = score + 500; // add 500 to score for still being alive/in play
		}
	}
	unsigned long previous_score = score; // hack to store our score value while also iterating through current value to build a serial buffer

	//send_buffer[] = {0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F};
	
	for (int u = 9; u > -1; u--) {
		digit = (int)(score / pow(10,u));
		send_buffer[11 - u] = (int) digit;
		score = score - (digit * (int)pow(10,u));
	}
	Serial.write(send_buffer[0]);
	Serial.write(send_buffer[1]);
	Serial.write(send_buffer[2]);
	Serial.write(send_buffer[3]);
	Serial.write(send_buffer[4]);
	Serial.write(send_buffer[5]);
	Serial.write(send_buffer[6]);
	Serial.write(send_buffer[7]);
	Serial.write(send_buffer[8]);
	Serial.write(send_buffer[9]);
	Serial.write(send_buffer[10]);
	Serial.write(send_buffer[11]);
	Serial.write(send_buffer[12]);
	Serial.write(send_buffer[13]);
  score = previous_score;
}
