/*

Name: Simon Says with NeoPixels!
Author: Alexis Rodriguez Jr.

Derived from: https://create.arduino.cc/projecthub/Arduino_Scuola/a-simple-simon-says-game-25ba99

Hardware needed:
5x Momentary Switches (Tactile Buttons) (SPST - Single Pole Single Throw)
4x NeoPixel LEDs
14x jumpers (5x GND button side, 5x SIGNAL button side, 3x NeoPixel, 1x GND breadboard)
*/

//Include Libraries
#include <Adafruit_NeoPixel.h>

//Define the digital DATA pin for NeoPixel
#define NEOPIXEL_IN 6

//Define number of LEDs in NeoPixel Strip
#define NUM_LIGHTS 4

//Define Simon Says Buttons
#define BUTTON_BLUE A0
#define BUTTON_YELLOW A1
#define BUTTON_RED A2
#define BUTTON_GREEN A3

//Define Simon Says NeoPixel LEDs position
#define NP_BLUE 0
#define NP_YELLOW 1
#define NP_RED 2
#define NP_GREEN 3

//Define Start/Stop button analog input
#define INIT_BUTTON A4

//Baud Rate for Serial
#define BAUD_RATE 115200

//GLOBAL VARIABLES

const int MAX_LEVEL = 100;				//Max level possible for game
uint32_t sequence[2][MAX_LEVEL];		//Stores the random generated sequence to match against
uint32_t your_sequence[MAX_LEVEL];		//Users sequence to match against generated sequence
int level = 1;							//Starting level

int velocity = 1000;					//How quickly LEDs will display pattern

boolean buttonPressed = 0;				//State check for button press to prevent multiple presses on one press

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LIGHTS, NEOPIXEL_IN);	//Initialize NeoPixel object

//Single 32-bit value to determine NeoPixel Color display
//0x00000000
//0xWWRRGGBB
//So to get RED: 0x00FF0000
//However, use low values to limit current draw since these are very bright!
const uint32_t red = 0x00040000;		//Display RED on NeoPixel
const uint32_t green = 0x00000400;	//Display GREEN on NeoPixel
const uint32_t yellow = 0x00040400;	//Display YELLOW on NeoPixel
const uint32_t blue = 0x00000004;	//Display BLUE on NeoPixel
const uint32_t white = 0x00040404;  //Display WHITE on NeoPixel

void setup() {
	
	//Used to debug [Use Serial.println("TEXT")]
	Serial.begin(BAUD_RATE);
	
	//Define all buttons used as inputs with a internal pullup
	pinMode(BUTTON_BLUE, INPUT_PULLUP);
	pinMode(BUTTON_YELLOW, INPUT_PULLUP);
	pinMode(BUTTON_RED, INPUT_PULLUP);
	pinMode(BUTTON_GREEN, INPUT_PULLUP);
	pinMode(INIT_BUTTON, INPUT_PULLUP);
	
	//NeoPixel Setup
	strip.begin();	//Fully initialize the pin and pixel count of object
	delay(100);
	strip.clear();  //Turn off all LEDs and clear frame matrix
	delay(100);
	
	//Show ready pattern
	strip.setPixelColor(NP_BLUE,blue);
	strip.setPixelColor(NP_YELLOW,yellow);
	strip.setPixelColor(NP_RED,red);
	strip.setPixelColor(NP_GREEN,green);
	strip.show();
	
	strip.clear();
	delay(500);
	strip.show();
	
}

void loop() {
	
	if (level == 1) generate_sequence();//generate a sequence;

	if (digitalRead(A4) == LOW || level != 1) { //If start button is pressed or you're winning
		show_sequence();    //show the sequence
		get_sequence();     //wait for your sequence
	}
}

//Display the sequence of LEDs to be inputted
void show_sequence() {
	
	strip.clear();
	delay(100);
	strip.show();

	for (int i = 0; i < level; i++) {
		//Turn on
		strip.setPixelColor(sequence[0][i],sequence[1][i]);
		strip.show();
		delay(velocity);
		
		//Turn off
		strip.clear();
		strip.show();
		delay(200);
	}
}

//Retrieve user input and determine if it was correct or incorrect
void get_sequence() {
	
	int flag = 0; //this flag indicates if the sequence is correct

	for (int i = 0; i < level; i++) {
		
		flag = 0;
		
		//Button press check to see if user has released the button yet to avoid counting a button press twice
		//NOTE: THIS ONLY MASKS THE PROBLEM WITH BUTTONS SLIGHTLY
		while(buttonPressed){
			if (digitalRead(A0) == HIGH && digitalRead(A3) == HIGH && digitalRead(A2) == HIGH && digitalRead(A1) == HIGH ){
				buttonPressed = 0;
			}			
		}
		
		while(flag == 0) {
			//Read if user has pressed any button and hold that state
			if (digitalRead(A0) == LOW || digitalRead(A3) == LOW || digitalRead(A2) == LOW || digitalRead(A1) == LOW ){
				buttonPressed = 1;
			}
			
			//CHECK BLUE
			if (digitalRead(A0) == LOW) {
				strip.clear();
				strip.show();
				
				strip.setPixelColor(NP_BLUE,blue);
				strip.show();
				
				your_sequence[i] = NP_BLUE;
				
				flag = 1;
				
				delay(200);
				
				if (your_sequence[i] != sequence[0][i]) {
					wrong_sequence();
					return;
				}
				strip.clear();
				strip.show();
			}
			
			//CHECK YELLOW
			if (digitalRead(A1) == LOW) {
				strip.clear();
				strip.show();
				
				strip.setPixelColor(NP_YELLOW,yellow);
				strip.show();
				
				your_sequence[i] = NP_YELLOW;
				
				flag = 1;
				
				delay(200);
				
				if (your_sequence[i] != sequence[0][i]) {
					wrong_sequence();
					return;
				}
				strip.clear();
				strip.show();
			}
			
			//CHECK RED
			if (digitalRead(A2) == LOW) {
				strip.clear();
				strip.show();
				
				strip.setPixelColor(NP_RED,red);
				strip.show();
				
				your_sequence[i] = NP_RED;
				
				flag = 1;
				
				delay(200);
				
				if (your_sequence[i] != sequence[0][i]) {
					wrong_sequence();
					return;
				}
				strip.clear();
				strip.show();
			}

			//CHECK GREEN
			if (digitalRead(A3) == LOW) {
				strip.clear();
				strip.show();
				
				strip.setPixelColor(NP_GREEN,green);
				strip.show();
				
				your_sequence[i] = NP_GREEN;
				
				flag = 1;
				
				delay(200);
				
				if (your_sequence[i] != sequence[0][i]) {
					wrong_sequence();
					return;
				}
				strip.clear();
				strip.show();
			}

		}
	}
	right_sequence();
}

//Generates a random pattern to follow for the next level
void generate_sequence() {
	
	randomSeed(millis()); //in this way is really random!!!

	//Generate random numbers between 0-3 (Corresponds to colors) and store into an array up to MAX_LEVEL
	for (int i = 0; i < MAX_LEVEL; i++) {
		sequence[0][i] = random(0,4);
		if (sequence[0][i] == NP_BLUE){
			sequence[1][i] = blue;
		}
		else if (sequence[0][i] == NP_GREEN) {
			sequence[1][i] = green;
		}
		else if (sequence[0][i] == NP_RED) {
			sequence[1][i] = red;
		}
		else if (sequence[0][i] == NP_YELLOW) {
			sequence[1][i] = yellow;
		}
	}
}

//Flashes the LEDs white to show that the incorrect sequence has been inputted
void wrong_sequence() {
	
	strip.clear();
	strip.show();
	delay(250);
	
	strip.setPixelColor(NP_BLUE,white);
	strip.setPixelColor(NP_GREEN,white);
	strip.setPixelColor(NP_YELLOW,white);
	strip.setPixelColor(NP_RED,white);
	strip.show();
	delay(500);
	
	strip.clear();
	strip.show();
	delay(500);
	
	level = 1;
	velocity = 1000;
}

//Flashes the LEDs with their individual colors to show that the correct sequence has been inputted
void right_sequence() {
	
	strip.clear();
	strip.show();
	delay(250);
	
	strip.setPixelColor(NP_BLUE,blue);
	strip.setPixelColor(NP_GREEN,green);
	strip.setPixelColor(NP_YELLOW,yellow);
	strip.setPixelColor(NP_RED,red);
	strip.show();
	delay(500);
	
	strip.clear();
	strip.show();
	delay(500);

	if (level < MAX_LEVEL){
		level++;
	}

	velocity -= 50; //increase difficulty
}