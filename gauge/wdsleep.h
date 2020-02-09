#include <avr/sleep.h>
#include <avr/wdt.h>

// http://www.gammon.com.au/forum/?id=11149

// watchdog interrupt
ISR(WDT_vect) {
  wdt_disable();  // disable watchdog
}
void myWatchdogEnable(const byte interval) {  // turn on watchdog timer; interrupt mode every 2.0s
  MCUSR = 0;                          // reset various flags
  WDTCSR |= 0b00011000;               // see docs, set WDCE, WDE
  WDTCSR =  0b01000000 | interval;    // set WDIE, and appropriate delay

  wdt_reset();
  
  byte adcsra_save = ADCSRA;
  byte prr_save = PRR;

  // disable ADC
  ADCSRA = 0;  
  
  // power reduction register
  // Bit 7 - PRTWI: Power Reduction TWI
  // Bit 6 - PRTIM2: Power Reduction Timer/Counter2
  // Bit 5 - PRTIM0: Power Reduction Timer/Counter0
  // Bit 4 - Res: Reserved bit
  // Bit 3 - PRTIM1: Power Reduction Timer/Counter1
  // Bit 2 - PRSPI: Power Reduction Serial Peripheral Interface
  // Bit 1 - PRUSART0: Power Reduction USART0
  // Bit 0 - PRADC: Power Reduction ADC
  
  // turn off various modules
  PRR = 0b11101111;
    
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
  sleep_mode();            // now goes to Sleep and waits for the interrupt
  
  // stop power reduction
  ADCSRA = adcsra_save;
  PRR = prr_save;
}
