/*
   To be honest a lot of this code is taken from the internet and 
   modified rather then written by me. As a result, there is quite a 
   bit of code that makes no sense by itself. 
*/

#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int recvPin = 4;
int triggerPin = 2;
volatile boolean f_wdt = 1;
volatile int watchdog_counter = 256;

void setup() {
  setup_watchdog(9); // approximately 8 seconds sleep
  pinMode(recvPin, INPUT);
}

void loop() {
  if (f_wdt == 1) { // wait for timed out watchdog / flag is set when a watchdog timeout occurs
    f_wdt = 0;     // reset flag

    cbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter OFF
    
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();
    oneMinSleep();   // Yes. 10 individual 1min sleep calls.

    sbi(ADCSRA, ADEN);                   // switch Analog to Digitalconverter ON

    // Setup pins after sleeping
    pinMode(triggerPin, OUTPUT);
    

    // Turn on main sensor
    digitalWrite(triggerPin, HIGH);

    // Wait until the sensor board gives us a high signal
    while (digitalRead(recvPin) != HIGH) {
      delay(30);
    }

    // Turn off sensor board and set trigger as input to save power
    digitalWrite(triggerPin, LOW);
    pinMode(triggerPin, INPUT);

  }
}

// This is the only way I've been able to get the device to sleep
// for longer then 8 seconds, it calls the sleep function multiple times
// for a sleep duration of *approx* one minute.
void oneMinSleep(){
  
    system_sleep();
    system_sleep();
    system_sleep();
    system_sleep();
    system_sleep();
    system_sleep();
    system_sleep();
    system_sleep();
  }

// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep() {

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
  
}

// The first argument to this is the clock multiplier
// that determines how long to sleep for, the number is 
// up higher where this function is called.
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii = 9;
  bb = ii & 7;
  if (ii > 7) bb |= (1 << 5);
  bb |= (1 << WDCE);
  ww = bb;

  MCUSR &= ~(1 << WDRF);
  // start timed sequence
  WDTCR |= (1 << WDCE) | (1 << WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt = 1; // set global flag


}

