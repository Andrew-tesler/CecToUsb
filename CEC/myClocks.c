// ----------------------------------------------------------------------------
// myClocks.c  ('F5529 Launchpad)
// ----------------------------------------------------------------------------

//***** Header Files **********************************************************
#include <driverlib.h>
#include "myClocks.h"


//***** Defines ***************************************************************
#define LF_CRYSTAL_FREQUENCY_IN_HZ     32768
#define HF_CRYSTAL_FREQUENCY_IN_HZ     4000000

#define MCLK_DESIRED_FREQUENCY_IN_KHZ  32000
#define MCLK_FLLREF_RATIO              MCLK_DESIRED_FREQUENCY_IN_KHZ / ( UCS_REFOCLK_FREQUENCY/1024 )   // ratio = 250


//***** Global Variables ******************************************************
uint32_t myACLK  = 0;
uint32_t mySMCLK = 0;
uint32_t myMCLK  = 0;


//***** Functions **************************************************************
void initClocks(void) {
     // Set core voltage level to handle 8MHz clock rate
     PMM_setVCore( PMM_CORE_LEVEL_3 );


     // Set XIN (P5.4) and XOUT (P5.5) in clock mode
     GPIO_setAsPeripheralModuleFunctionInputPin(
    		 GPIO_PORT_P5,
    		 GPIO_PIN4 + GPIO_PIN5
     	 	 );

     // Start the XT1 oscilator wait until it running
     UCS_LFXT1Start(
    		 UCS_XT1_DRIVE0,
    		 UCS_XCAP_3
    		 );
     // Start the XT2 oscilator wait until it running
    // UCS_XT2Start(
    //		 UCS_XT2DRIVE_24MHZ_32MHZ
    //		 );


    // Initialize the XT1 and XT2 crystal frequencies being used
    //  so driverlib knows how fast they are
    UCS_setExternalClockSource(
            LF_CRYSTAL_FREQUENCY_IN_HZ,
            HF_CRYSTAL_FREQUENCY_IN_HZ
    );

    // Verify if the default clock settings are as expected
    myACLK  = UCS_getACLK();
    mySMCLK = UCS_getSMCLK();
    myMCLK  = UCS_getMCLK();

    // Setup ACLK to use REFO as its oscillator source
    UCS_clockSignalInit(
            UCS_ACLK,                                    // Clock you're configuring
            UCS_XT1CLK_SELECT,
            //UCS_REFOCLK_SELECT,                          // Clock source
            UCS_CLOCK_DIVIDER_1                          // Divide down clock source by this much
    );

    // Set the FLL's clock reference clock source
    UCS_clockSignalInit(
            UCS_FLLREF,                                  // Clock you're configuring
            UCS_REFOCLK_SELECT,                          // Clock source
            UCS_CLOCK_DIVIDER_1                          // Divide down clock source by this much
    );

    // Configure the FLL's frequency and set MCLK & SMCLK to use the FLL as their source
    UCS_initFLLSettle(
            MCLK_DESIRED_FREQUENCY_IN_KHZ,               // MCLK frequency
            MCLK_FLLREF_RATIO                            // Ratio between MCLK and FLL's reference clock source
    );

    // Verify that the modified clock settings are as expected
    myACLK  = UCS_getACLK();
    mySMCLK = UCS_getSMCLK();
    myMCLK  = UCS_getMCLK();
}

