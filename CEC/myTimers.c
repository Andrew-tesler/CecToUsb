// ----------------------------------------------------------------------------
// myTimers.c  (for lab_06c_timerDirectDriveLed project) ('F5529 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myTimers.h"

//***** Defines ***************************************************************


//***** Global Variables ******************************************************


//***** Functions **************************************************************
void initTimers(void)
{
    // 1. Setup Timer (TAR, TACTL)
    //    TimerA0 in Up mode using ACLK
    //    Toggle LED1 (Red) on/off every second directly from the timers output pin
    //    Toggle LED2 (Green) on/off every second using TA0IFG
    TIMER_A_configureUpMode( TIMER_A0_BASE,             // Set up Timer A0
        TIMER_A_CLOCKSOURCE_SMCLK,                       // Use ACLK (slower clock)
        TIMER_A_CLOCKSOURCE_DIVIDER_1,                  // Input clock = ACLK / 1 = 32KHz
        0xFFFF / 2,                                     // Period (0xFFFF/2):  8000 / 32Khz = 1 second
        TIMER_A_TAIE_INTERRUPT_ENABLE,                  // Enable TAR -> 0 interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,            // No CCR0 interrupt
        TIMER_A_DO_CLEAR                                // Clear TAR & clock divider
    );

    // 2. Setup Capture & Compare features
    TIMER_A_initCompare( TIMER_A0_BASE,
       TIMER_A_CAPTURECOMPARE_REGISTER_0,               // Use CCR2 for compare
       TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,        // Since directly driving LED, interrup not req'd
       TIMER_A_OUTPUTMODE_TOGGLE_RESET,                 // Toggle provides a 1 sec period based on CCR0 and CCR2 values
       0x2000                                           // Compare value: 4000 = 1/2 second
     );

    // 3. Clear/enable flags and start timer
    TIMER_A_clearTimerInterruptFlag( TIMER_A0_BASE );   // Clear TA0IFG

    TIMER_A_startCounter( TIMER_A0_BASE,
        TIMER_A_UP_MODE
    );
}

void initTimerA0(void) {
// Setup TimerA0 in Up mode with CCR0 compare
TIMER_A_configureUpMode( TIMER_A0_BASE,
		TIMER_A_CLOCKSOURCE_SMCLK,
		TIMER_A_CLOCKSOURCE_DIVIDER_1,
		0x4000,
		TIMER_A_TAIE_INTERRUPT_DISABLE,
		TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
		TIMER_A_DO_CLEAR );

TIMER_A_initCompare( TIMER_A0_BASE,
		TIMER_A_CAPTURECOMPARE_REGISTER_0,
		TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
		TIMER_A_OUTPUTMODE_SET_RESET,
		0xBEEF // Compare Value
);

TIMER_A_clearTimerInterruptFlag(
		TIMER_A0_BASE );

TIMER_A_clearCaptureCompareInterruptFlag(
		TIMER_A0_BASE,
		TIMER_A_CAPTURECOMPARE_REGISTER_0 );

TIMER_A_startCounter( TIMER_A0_BASE,
		TIMER_A_UP_MODE);
}

void initTimerB0(void) {

	// (1) Setup timer
    TIMER_B_configureUpMode( TIMER_B0_BASE,			// Setup timer B0
        TIMER_B_CLOCKSOURCE_SMCLK,					// Use SMCLK
        TIMER_B_CLOCKSOURCE_DIVIDER_1,				// Clock devider == 1
        0xFFFF   ,                                  // Period
        TIMER_B_TBIE_INTERRUPT_DISABLE,              // Enable TAR -> 0 interrupt
        TIMER_B_CCIE_CCR0_INTERRUPT_ENABLE,         // Enable CCR0 interrupt
        TIMER_B_DO_CLEAR                            // Clear TAR & clock divider
    );
    // (2) Setup capture and compare
    TIMER_B_initCompare( TIMER_B0_BASE,
       TIMER_B_CAPTURECOMPARE_REGISTER_0,            // Use CCR0 for compare
       TIMER_B_CAPTURECOMPARE_INTERRUPT_ENABLE,      // Ebable interrupt on Capture and compare
       TIMER_B_OUTPUTMODE_TOGGLE_RESET,              // Toggle provides a 1 sec period based on CCR0 and CCR2 values
       0xFFF                                         // Compare value:0xfff
     );

    // (3) Clear/enable flags and start timer
    TIMER_B_clearTimerInterruptFlag( TIMER_B0_BASE );   // Clear TA0IFG
    // (4) Start Timer B0
    TIMER_B_clearCaptureCompareInterruptFlag(
    		TIMER_B0_BASE,
    		TIMER_B_CAPTURECOMPARE_REGISTER_0 );
    TIMER_B_startCounter( TIMER_B0_BASE,
        TIMER_B_UP_MODE
    );
}

void initTimer0_A1(void) {

	// Setup TimerA0_1 in Up mode with CCR2 compare
	TIMER_A_configureUpMode( TIMER_A1_BASE,
	TIMER_A_CLOCKSOURCE_ACLK,
	TIMER_A_CLOCKSOURCE_DIVIDER_1,
	0xffff,
	TIMER_A_TAIE_INTERRUPT_DISABLE,
	TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,
	TIMER_A_DO_CLEAR );

	TIMER_A_initCompare( TIMER_A1_BASE,
	TIMER_A_CAPTURECOMPARE_REGISTER_1,
	TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
	TIMER_A_OUTPUTMODE_TOGGLE_SET,
	0x0250
	);

	TIMER_A_clearTimerInterruptFlag( TIMER_A1_BASE );

	TIMER_A_startCounter( TIMER_A1_BASE,
	        TIMER_A_UP_MODE
	    );


}
//****** Interrupt Service Routines *******************************************

