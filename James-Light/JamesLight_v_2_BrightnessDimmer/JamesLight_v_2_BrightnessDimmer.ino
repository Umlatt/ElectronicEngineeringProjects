#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define RING_SIZE         24                                                                            // DEFINITION: The amount of LEDs on the ring
#define RING_PIN          6                                                                             // DEFINITION: The pin to which the tx/rx pin for the Neo Pixel is attached
#define BUTTON            4                                                                             // DEFINITION: The input for the pushbutton

const int colour_white[3]       = {255, 255, 255};                                                      // CONSTANT: COLOUR - WHITE
const int colour_orange[3]      = {255, 204, 102};                                                      // CONSTANT: COLOUR - ORANGE
const int colour_red[3]         = {255, 0, 0};                                                          // CONSTANT: COLOUR - WHITE
const int colour_yellow[3]      = {255, 255, 0};                                                        // CONSTANT: COLOUR - YELLOW
const int colour_green[3]       = {0, 50, 0};                                                           // CONSTANT: COLOUR - GREEN
const int colour_blue[3]        = {0, 0, 255};                                                          // CONSTANT: COLOUR - BLUE
const int colour_purple[3]      = {255, 0, 255};                                                        // CONSTANT: COLOUR - BLUE
const int colour_startup[3]     = {255, 200, 0};                                                        // CONSTANT: COLOUR - COLOUR MIX FOR STARTUP

Adafruit_NeoPixel ring = Adafruit_NeoPixel(RING_SIZE, RING_PIN,   NEO_GRB + NEO_KHZ800);                // VARIABLE: A neo pixel ring to be managed
int   delay_value = 20;                                                                                 // VARIABLE: A delay value .
bool  buttonState = LOW;                                                                                // VARIABLE: Initialise a button state variable
int   ringled[24][3] = {0};                                                                             // VARIABLE: A variable to hold the colour state of each LED on a ring
int   R = 0;                                                                                            // VARIABLE: Used for positional arguments to make the 2D array more readable (RED)
int   G = 1;                                                                                            // VARIABLE: Used for positional arguments to make the 2D array more readable (GREEN)
int   B = 2;                                                                                            // VARIABLE: Used for positional arguments to make the 2D array more readable (BLUE)
int   Fade = 3;                                                                                         // VARIABLE: Used for positional arguments to make the 2D array more readable (Brightness)   
int   button_depress_interval_count = 0;                                                                // VARIABLE: Used to indicate the duration that the button was depressed.
int   timer_count = 0;                                                                                  // VARIABLE: An iterator for one second intervals.
int   timer_1second = 1000/delay_value;                                                                 // VARIABLE: A value that reprisents when the count hits 1 second.
int   total_time  = 0;                                                                                  // VARIABLE: A value indicating how many LEDs must be enabled.
int   current_leds = 0;
int   light_duration_delta = 60;                                                                        // VARIABLE: 150s = 2.5 minutes
int   timer = 0;                                                                                        // VARIABLE: A variable to count up
int   fader = 255;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: LED Update
// Push colour data to all LEDs in ring

void update_leds(){                                                                                     // FUNCTION START: Send the colour settings to the LED
    for(int count = 0; count < int(RING_SIZE); count++ ){                                               //  0. [1] Iterate through the LEDs on the ring
      ring.setPixelColor(count, ringled[count][R],ringled[count][G],ringled[count][B]);                 //  1. Send the colour information to the ring buffer
    }                                                                                                   //  0. [1] END
   ring.show();                                                                                         //  0. Update the pixel state based on new settings
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: Set LEDs on
// Set all LEDs on

void set_leds_on(){
  for(int count = 0; count < RING_SIZE; count++ ){                                               
    memcpy(ringled[count],colour_orange, sizeof(ringled[1]));
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: Handle button press
// Check for button press

bool button_pressed(){
  if (digitalRead(BUTTON)){
    button_depress_interval_count++;
    return true;
    }
  else {
    if (button_depress_interval_count != 0) {
      }
    button_depress_interval_count = 0;
  }
  return false;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: Input colours
//On button press, change colours every 5 LEDs

void change_colours(){
        if (current_leds > 20){
        for(int count = 20; count < current_leds; count++ )
        {
          memcpy(ringled[count],colour_purple, sizeof(ringled[1])); 
        }
      } 
      else if (current_leds > 15)
      {
        for(int count = 15; count < current_leds; count++ )
        {
          memcpy(ringled[count],colour_red, sizeof(ringled[1]));         
        }
      } 
      else if (current_leds > 10)
      {
        for(int count = 10; count < current_leds; count++ )
        {
          memcpy(ringled[count],colour_yellow, sizeof(ringled[1]));         
        }
      }
      else if (current_leds > 5)
      {
        for(int count = 5; count < current_leds; count++ )
        {
        memcpy(ringled[count],colour_blue, sizeof(ringled[1]));
        }
      }
      else {
        for(int count = 0; count < current_leds; count++ )
        {
          memcpy(ringled[count],colour_green, sizeof(ringled[1]));         
        }
      }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: Handle user input states
// Handle the user input.

void check_for_user_input(){
  if (button_pressed()){
    if (button_depress_interval_count >= timer_1second){
      ring.setBrightness(255);
      current_leds = (int)floor(button_depress_interval_count/timer_1second);
      change_colours();
    }
  }
  else {
    if (current_leds > 0){
      total_time = current_leds*light_duration_delta;
      set_leds_on();
      button_depress_interval_count = 0;
      current_leds = 0;
    }
  }
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: Handle timeout
// Handle timeout

void fade_led(int led_num){
  ring.setPixelColor(led_num, 255,255,255, fader);
  fader--;
}

void timer_tick(){
  ring.setPixelColor(0, 255,255,255, 255);
  timer++;
  fade_led(0);
  if (timer == timer_1second){
    timer = 0;
  }
  //else (ring.clear()); 
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: INITIALIZATION
// Initialize the Arduino

void setup() {
  pinMode(BUTTON, INPUT);                                                                               // PIN: Switch
  ring.begin();                                                                                         // INITIALIZE NeoPixel strip object (REQUIRED)
  ring.clear();                                                                                         // Turn off all LEDs
  ring.show();                                                                                          // Send the updated pixel colors to the hardware
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> FUNCTIONS: MAIN
// Run main application

void loop() {
  timer_tick();                                                                                         //  0. Count down.
  check_for_user_input();                                                                               //  0. Check and update if user input is applied
  update_leds();                                                                                        //  0. Push the latest colour information to the LEDs
  delay(delay_value);                                                                                   //  0. Delay before next update
}                                                                                                       // FUNCTION END!


// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------> NOTES

// Set Specific Pin         - bigpixels.setPixelColor(0, bigpixels.Color(0,50,0));
// Clear all Pins           - bigpixels.clear();
// Set all Pins             - bigpixels.fill(12582656, 0 ,24);
// Set all Pin Brightness   - bigpixels.setBrightness(10);
// Code Notes               - https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
// COLOURS                  - https://www.shodor.org/stella2java/rgbint.html
