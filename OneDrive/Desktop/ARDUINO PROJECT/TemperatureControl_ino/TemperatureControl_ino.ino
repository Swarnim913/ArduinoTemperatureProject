#include <avr/io.h>
#include <avr/interrupt.h>

// Define pin connections
const int tempPin = A0; // LM35 Temperature sensor connected to analog pin A0
const int ledPin = 13;  // Onboard LED connected to digital pin 13

// Use volatile for variables that are modified within an interrupt service routine
volatile bool ledState = false; // Tracks the current state of the LED (on or off)
const int threshTemp = 30; // Temperature threshold in Celsius
float sensorCorrection = 20; // Correction factor for sensor accuracy

// Variable to hold the calculated temperature
volatile float temperatureC = 0;

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 bps
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  pinMode(tempPin, INPUT); // Set the temperature sensor pin as an input

  // Disable interrupts while setting up Timer1
  noInterrupts();           
  TCCR1A = 0;                // Set entire TCCR1A register to 0
  TCCR1B = 0;                // Set entire TCCR1B register to 0
  TCNT1  = 0;                // Initialize the counter value to 0
  // Set compare match register for 2-second intervals
  OCR1A = 62499;             
  TCCR1B |= (1 << WGM12);    // Turn on CTC (Clear Timer on Compare Match) mode
  // Set CS12 and CS10 bits for 1024 prescaler (timer tick frequency)
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // Enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);   
  interrupts();              // Re-enable interrupts
}

// Interrupt service routine for Timer1 compare match
ISR(TIMER1_COMPA_vect) {  
  int val = analogRead(tempPin); // Read the analog value from the temperature sensor
  // Convert the analog value to millivolts, assuming a 5V supply
  float millivolts = (val / 1024.0) * 5000; 
  // Convert millivolts to Celsius
  temperatureC = millivolts / 10; 
  // Adjust the reading based on the sensor correction factor
  temperatureC = temperatureC + sensorCorrection;
  
  // Determine the blink rate based on the temperature
  if(temperatureC < threshTemp) {
    OCR1A = 7812; // Set timer for approximately 250ms intervals
  } else {
    OCR1A = 15624; // Set timer for approximately 500ms intervals
  }
  // Toggle the LED state
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
}

void loop() {
  // Main code loop does not need to perform any actions for this example
}
