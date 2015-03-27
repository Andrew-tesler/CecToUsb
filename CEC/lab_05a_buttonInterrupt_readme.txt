lab_05a_buttonInterrupt

When Lab 5a is complete, you should be able to push the S2 button and toggle 
the LED on/off.

We will begin by importing the solution to Lab 4a. After which we’ll need to 
delete a bit of ‘old’ code and add the following.
  - Setup the interrupt vector
  - Enable interrupts
  - Create an ISR

Basic Steps:
- Import previous project's solution
- Add 'unused_interrupts.c' to project
- Delete the old LED blink code from while{} loop
- Add code to enable interrupts
- Run code checking interrupt IE and IFG registers
  as well as seeing the interrupt occur 
  (using default ISR in unused_interrupts.c)
- Create an ISR and debug it (noticing the IFG
  flag is never cleared)
- Go back and 'upgrade' the ISR, using 
  the PORT1 IV register 
- Run, to watch it work!



Final code ... you can copy from this if you want to save typing time & effort.

// ----------------------------------------------------------------------------
// main.c  (for lab_05a_buttonInterrupt project) ('F5529 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myClocks.h"


//***** Prototypes ************************************************************
void initGPIO(void);


//***** Defines ***************************************************************
#define ONE_SECOND  8000000        // Well, it's about a second
#define HALF_SECOND 4000000


//***** Global Variables ******************************************************


//***** Main Function *********************************************************
void main (void)
{
    // Stop watchdog timer
    WDT_A_hold( WDT_A_BASE );

    // Initialize GPIO
    initGPIO();

    // Initialize clocks
    initClocks();

    __bis_SR_register( GIE );         // Enable interrupts globally

    while(1) {
        __no_operation();             // Placeholder for while loop (not required)
    }
}

//*****************************************************************************
// Initialize GPIO
//*****************************************************************************
void initGPIO(void) {

    // Set pin P1.0 to output direction and turn LED off
    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_PIN0 );                   // Red LED (LED1)
    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );

    // Set P1.1 as input with pull-up resistor (for push button)
    //  configure interrupt on low-to-high transition
    //  and then clear flag and enable the interrupt
    GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P1, GPIO_PIN1 );
    GPIO_interruptEdgeSelect ( GPIO_PORT_P1, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION );
    GPIO_clearInterruptFlag ( GPIO_PORT_P1, GPIO_PIN1 );
    GPIO_enableInterrupt ( GPIO_PORT_P1, GPIO_PIN1 );

}

//*****************************************************************************
// Interrupt Service Routines
//*****************************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void pushbutton_ISR (void)
{
        // Toggle the LED on/off (initial code; moved into switch statement below)
        //GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );

          switch( __even_in_range( P1IV, 10 )) {
           case 0x00: break;                    // None
           case 0x02: break;                    // Pin 0
           case 0x04:                           // Pin 1
            GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );
            break;
           case 0x06: break;                    // Pin 2
           case 0x08: break;                    // Pin 3
           case 0x0A: break;                    // Pin 4
           case 0x0C: break;                    // Pin 5
           case 0x0E: break;                    // Pin 6
           case 0x10: break;                    // Pin 7
           default:   _never_executed();
        }
}

