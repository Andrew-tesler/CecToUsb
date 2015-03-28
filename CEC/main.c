/* ----------------------------------------------------------------------------
//	CEC to USB adapter fitHeadless 2
//	andrey.tesler@compulab.co.il
// ----------------------------------------------------------------------------
*/
//***** Header Files **********************************************************
#include <driverlib.h>
#include <stdio.h>
#include "myClocks.h"
#include "myGPIO.h"
#include "myTimers.h"


//***** Prototypes ************************************************************



//***** Defines ***************************************************************
//#define ONE_SECOND  8000000        // Well, it's about a second
//#define HALF_SECOND 4000000

/* Define applicable CEC and I2C address and maximum CEC message size in bytes*/
#define Host_I2C_Addr 0x94                  // Address is 0x4A<<1 for R/W
#define Local_I2C_Addr 0x90                 // Local address is 0x48

#define Bcst_CEC_Addr 0x0D                  // BroadCast address
#define Local_CEC_Addr 0x04                 // Logical CEC address for TV

#define Max_Message_Size 10                 // Maximum number of bytes in a
                                            // CEC message is 15, this can
                                            // be increased for manufacturer
                                            // defined messages

/* CEC Start Bit Timing Defintions,  */ //1051.4285714285714285714285714286
/* Recieve Start   */
#define Sbit_Min_LD 3680                     // Start Bit Minimum Low duration  	 3.5ms
#define Sbit_Max_LD 4100                     // Start Bit Maximum Low duration  	 3.9ms
#define Sbit_Min_TD 4521                     // Start Bit Minimum Total duration	 4.3ms
#define Sbit_Max_TD 4941                     // Start Bit Maximum Total duration	 4.7ms
/* Transmit Start  */
#define Sbit_Nom_LD 3890                     // Start Bit Nominal Low duration  	 3.7ms
#define Sbit_Nom_HD 4731                     // Start Bit Nominal High Duration		 4.5ms

/* CEC Data Bit Timing Defintions,  */ //420
/* Recieve Data    */
#define Dbit_Min_TD 2155                     // Data Bit Minimum Total Duration	  			2.05ms
#define Dbit_Max_TD 2891                     // Data Bit Maximum Total Duration				2.75ms
#define Dbit_Sample 1104                     // Data Bit Sample time						1.05ms

/* Logic 0 times */
#define Dbit_Min_0 1366          			 // The earliest time for high impedance state (logical 0) 1.3ms
#define Dbit_Max_0 1787 					 // The latest time for high impedance state (logical 0)   1.7ms
/* Logic 1 times */
#define Dbit_Min_1 420						 // The earliest time for high impedance state (logical 1) 0.4ms
#define Dbit_Max_1 1051						 // The latest time for high impedance state (logical 1)   0.8ms

#define Dbit_Min_END 2155   		    	 // The earliest time for the start of a following bit.	   2.05ms
#define Dbit_Max_END 2891					 // The latest time for the start of a following bit.	   2.75ms
/* Transmit Data   */
#define Dbit_High_LD 630                     // Data Bit logical 1 Nominal Low duration		0.6ms
#define Dbit_High_HD 2523                    // Data Bit logical 1 Nominal High Duration	2.4ms
#define Dbit_Low_LD 1577                      // Data Bit logical 0 Nominal Low duration	1.5ms
#define Dbit_Low_HD 2523                      // Data Bit logical 0 Nominal High Duration   2.4ms

/* CEC Ack Bit Timing Defintions,*/
#define Ack_HD 84                           // Set High duration after ACK
#define Ack_sample 28                       // The acknowledge is sampled
                                            // after the low duration is
                                            // transmitted.
unsigned char I2C_State = 0;                // code executed within USI ISR
unsigned char CEC_State = 0;                // code executed within Port1 ISR
unsigned char Timer0_State = 0;             // code executed within Timer0 ISR
unsigned char Timer1_State = 0;             // code executed within Timer1 ISR

unsigned int Payload[Max_Message_Size];    // Data from either I2C or CEC is
                                            // stored here
unsigned char Pldcnt = 0;                   // Count # of bytes rx'd or tx'd
                                            // this should be made volatile if
                                            // optimizations are used in
                                            // compiler
unsigned char Pldsize = 0;                  // Records # of bytes received
unsigned char Bit_Count = 0;                // Tracks bit count of CEC message
unsigned char EOM = 0;                      // End of Message Flag
unsigned char Retry = 0;                    // Retry Flag used to retransmit
unsigned int TimerB_State = 0;				// ASK Status timer
unsigned int counter = 0;					// Bit counter

//************Debug fields******************************************************
// Test value
unsigned int i = 0;

unsigned int n = 0;
//int test[100];


// debug struct for debuging the code
struct debug{
	// CEC state value
	int CECState;
	// Counter of bits
	int bitsCounter;
	// Data char
	int deviceData;


};
typedef struct debug debugFild;

debugFild debug;


//***** Global Variables ******************************************************
int current_time = 0;
//int old_time = 0;


//***** Main Function *********************************************************
void main (void)
{


    // Stop watchdog timer
    WDT_A_hold( WDT_A_BASE );

    // Initialize GPIO
    initGPIO();

    // Initialize clocks
    //initClocks();

    // Initialize timers
   initTimers();
   initTimer0_A1();
   initTimerB0();
   //initTimerB0();
   // Initialaztion of time values
//   current_time = TIMER_A_getCounterValue(TIMER_A0_BASE);
//   old_time = current_time;
    //initTimerA0();
    __bis_SR_register( GIE );         // Enable interrupts globally
    printf("Start test\n");


    // Int old_time = 0;
    // Disable timer interupt
    TIMER_A_disableInterrupt(TIMER_A0_BASE);
    TIMER_A_disableCaptureCompareInterrupt(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);

    TIMER_B_disableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
    while(1) {
    	//GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
    	//while (i == 9) {
    	//	if (Payload[9] == 0){
    	//		GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
    	//	}
    	//}
    	//if (Payload[8] == 0)

    //	current_time = TIMER_A_getCounterValue(TIMER_A0_BASE);
    	//if (current_time - old_time> 900) {
    		//GPIO_toggleOutputOnPin(GPIO_PORT_P4,GPIO_PIN7);
    	//	old_time = current_time;
    	//}

    	//GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN1);            // Tougle pin P4.7
    }
}


//*****************************************************************************
// Interrupt Service Routines
//*****************************************************************************
//#pragma vector=PORT1_VECTOR
//__interrupt void pushbutton_ISR (void)
//{
        // Toggle the LED on/off (initial code; moved into switch statement below)
        //GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );

//          switch( __even_in_range( P1IV, 10 )) {
//           case 0x00: break;                    // None
//           case 0x02: break;                    // Pin 0
//           case 0x04:                           // Pin 1
//           GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
//            break;
//           case 0x06: break;                    // Pin 2
//           case 0x08: break;                    // Pin 3
//           case 0x0A: break;                    // Pin 4
//           case 0x0C: break;                    // Pin 5
//           case 0x0E: break;                    // Pin 6
//          case 0x10: break;                    // Pin 7
//           default:   _never_executed();
//        }
//}
/*
#pragma vector=PORT2_VECTOR
__interrupt void pushbutton2_ISR (void)
{
        // Toggle the LED on/off (initial code; moved into switch statement below)
        //GPIO_toggleOutputOnPin( GPIO_PORT_P1, GPIO_PIN0 );
		GPIO_toggleOutputOnPin(GPIO_PORT_P4,GPIO_PIN7);
          switch( __even_in_range( P2IV, 10 )) {
           case 0x00: break;                    // None
           case 0x02: break;                    // Pin 0
           case 0x04:                           // Pin 1
            GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
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
*/


#pragma vector=PORT2_VECTOR
__interrupt void CEC_ISR (void)
{
	int current_time;
	//unsigned char tempa;
	GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7); // Clear flag

          switch( __even_in_range( CEC_State, 12 )) {
           case 0: // Initial Hight to Low transition

        	   // Reset TimerA
        	   TIMER_A_clear(TIMER_A0_BASE);
        	   // Set triger on rising edge.
        	   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION );
        	   // Clear flag after toggling edge
        	   GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7);
        	   // Go to the next stage: Rising Edge
        	   CEC_State = 2;
        	   //__bic_SR_register_on_exit(LPM0_bits);
        	   // Debug fields
        	   debug.CECState = CEC_State;
        	   break;
           case 2: // Test start bit Low time duration test
        	   // Set trigger to falling edge
        	   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION );
        	   // Clear flag after toggling edge
        	   GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7);
        	   // Halt the timer
        	   TIMER_A_stop(TIMER_A0_BASE);
        	   // Get timer value
        	   current_time = TIMER_A_getCounterValue(TIMER_A0_BASE);
        	   // Restart the timer
        	   TIMER_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
        	   //GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN7 );
        	   if ( current_time < Sbit_Min_LD ) {
        		   // disable innterupt ?
        		   CEC_State = 0; // Invalid low duration
        	   }
        	   else {
        		   CEC_State = 4; // Go to next falling edge
        	   }
        	   // Debug fileds
        	   debug.CECState = CEC_State;
        	   break;
           case 4:		// Check the duration of case 2 high timing and diterming if this is valid start bit.
        	   // Disable timer innterupt?

        	   // Halt Timer
        	   TIMER_A_stop(TIMER_A0_BASE);

        	   // Get timer value
        	   current_time =  TIMER_A_getCounterValue(TIMER_A0_BASE);
        	   // Restart the timer
        	   TIMER_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
        	   // Reset TimerA
        	   TIMER_A_clear(TIMER_A0_BASE);
        	   if (current_time < Sbit_Min_TD ) {
        		   CEC_State = 0;
        	   }
        	   else { // Start bit is valid get ready To recive data
        		   // Set trigger to rising edge
        		   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION );
        		   CEC_State = 6;
        	   }
        	   // Debug fields
        	   debug.CECState = CEC_State;
            break;

           case 6: // Check timing of low duration after start bit if  0.4ms > x > 0.8 Logic 1 if 1.3ms > x > 1.7ms logic 0
        	   GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );   //Debug
        	   GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN7 );   //debug
        	   // Clear flag after toggling edge
        	   GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7);
        	   // Halt Timer
        	   TIMER_A_stop(TIMER_A0_BASE);
        	   current_time =  TIMER_A_getCounterValue(TIMER_A0_BASE);
        	   //debug[n].Time0 = TIMER_A_getCounterValue(TIMER_A0_BASE);
        	   // Restart the timer
        	   TIMER_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);

        	   // Test timings resolv if logic 0, logic 1 or noise
        	   if (current_time > Dbit_Min_0 && current_time < Dbit_Max_0) {  // logic 0 case test
        		   CEC_State = 8; // Logic 0
        	   } // end if test logic 0
        	   else if (current_time > Dbit_Min_1 && current_time < Dbit_Max_1) { // logic 1 case test
        		   CEC_State = 10; // Logic 1
        	   } // end if test logic 1
        	   else {  // data noise start over
        		   CEC_State = 0;
        	   } // End abnormal timing state
        	   // Set the CEC GPIO HIGH to LOW intrupt edge
        	   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION );
        	   // Debug fields
        	   debug.CECState = CEC_State;
        	   break;

           case 8:  // data bit end case logic 0
        	   // Halt Timer
        	   TIMER_A_stop(TIMER_A0_BASE);
        	   // Get corrent time
        	   current_time =  TIMER_A_getCounterValue(TIMER_A0_BASE);
        	   // Restart the timer
        	   TIMER_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
        	   // Check timings of
        	   if (current_time > Dbit_Min_END && current_time < Dbit_Max_END) {
        		   // Toggle led P4.7 for debugging
        		   GPIO_setOutputHighOnPin( GPIO_PORT_P4, GPIO_PIN7 );
        		   Payload[counter] = 0;
        		 //  if (counter == 9) {
        		//	   GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN2 );
        		//	   TimerB_State = 0;
        		//	   TIMER_B_enableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
        		//	   TIMER_B_clear(TIMER_B0_BASE);
        		//	   //GPIO_toggleOutputOnPin(GPIO_PORT_P2,GPIO_PIN2);
        		//	  // GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN2 );
        		//	   counter = 0 ;
        		//	   CEC_State = 6;
        		//	   break;
        		//   }
        		   counter++;

        		   // Setup for the next bit
        		   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION );
            	   // Reset TimerA
            	   TIMER_A_clear(TIMER_A0_BASE);
        		   CEC_State = 6;
        	   } // End timing test if.
        	   else { // bad timing
        		   CEC_State = 0;
        	   }
        	   // Clear flag after toggling edge
        	   GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7);
        	   // debug information
        	   debug.CECState = CEC_State;
        	   debug.bitsCounter = counter;
        	   debug.deviceData = debug.deviceData << 1;
        	   break;
           case 10: // Data bit end case logic 1
        	   current_time =  TIMER_A_getCounterValue(TIMER_A0_BASE);

        	   if (current_time > Dbit_Min_END && current_time < Dbit_Max_END) {
        		   // Toggle led P1.0 for debugging
        		   GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
        		   Payload[counter] = 1;
        		   if (counter == 11 ) {
               			   //TimerB_State = 0;
               			   //TIMER_B_enableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
               			   //TIMER_B_clear(TIMER_B0_BASE);
        			  // GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN7 );
        			  // GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN7 );
        			   counter = 0;
        		   }
        		   else {
        			   counter++;
        		   }


        		   // Setup for the next bit
        		  // GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION );
            	   // Reset TimerA
            	   TIMER_A_clear(TIMER_A0_BASE);
            	 //  if (counter == 9) {
            	//	   TimerB_State = 0;
            	//	   TIMER_B_enableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
            	//	   TIMER_B_clear(TIMER_B0_BASE);
            	 //  }
//
        		   CEC_State = 6;
        	   }
        	   else {
        		   CEC_State = 0;
        	   }
        	   // Clear flag after toggling edge
        	   GPIO_clearInterruptFlag(GPIO_PORT_P2,GPIO_PIN7);
        	   break;

           // Pin 4
           case 12:  // End of message case
        	   GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION );

        	   break;

           default:   _never_executed();
        }
      	// Clear GPIO interrupt falg

}
// timer interrupt CEC translate
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timera0_ISR(void)
{
	TIMER_A_clearTimerInterruptFlag(TIMER_A0_BASE);
 // unsigned char data;
//  TACCTL0 &= ~CCIE;                         // Disable interrupt
  switch(__even_in_range(Timer0_State, 6))
  {
    case 0: //Testing Bit timing
            /* failing condition */
//            TACCTL1 &= ~CCIE;               // Disable interrupt
          //  CEC_State = 2;                  // Invalid start, ignore
           // printf("State 0\n");

            GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
            TIMER_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,Sbit_Nom_LD);
            //TIMER_A_clearTimerInterruptFlag(TIMER_A0_BASE);
            //i++;
           //Timer_A_clearTimerInterruptFlag(TIMER_A_CAPTURECOMPARE_REGISTER_0);
            //__bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0
            Timer0_State = 2;
            break;

    case 2:                                 //Transmit start bit
          // setup high duration of start bit
            Timer0_State = 4;
            TIMER_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0,Sbit_Nom_HD-Sbit_Nom_LD);
            GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
            //TACCR0 = CCIE;
            //printf("State 2\n");
            //TIMER_A_clearTimerInterruptFlag(TIMER_A0_BASE);

            break;

    case 4:
    		GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
    		Timer0_State = 6;
    		break;
    case 6:
    		GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
  }
}

// timer interrupt CEC translate
#pragma vector=TIMER0_B0_VECTOR
__interrupt void timerb0_ISR(void)
{
	TIMER_B_clearTimerInterruptFlag(TIMER_B0_BASE);
 // unsigned char data;
//  TACCTL0 &= ~CCIE;                         // Disable interrupt
  switch(__even_in_range(TimerB_State, 6))
  {
    case 0: //Testing Bit timing
            /* failing condition */
//            TACCTL1 &= ~CCIE;               // Disable interrupt
          //  CEC_State = 2;                  // Invalid start, ignore
           // printf("State 0\n");
		    GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN7 );
		    GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN7 );
            //GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN2 );
            TIMER_B_setCompareValue(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0,Dbit_Low_LD);
            //TIMER_B_disableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
            //TIMER_A_clearTimerInterruptFlag(TIMER_A0_BASE);
            //i++;
           //Timer_A_clearTimerInterruptFlag(TIMER_A_CAPTURECOMPARE_REGISTER_0);
            //__bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0
            TimerB_State = 2;
            break;

    case 2:                                 //Transmit start bit
          // setup high duration of start bit
    		TimerB_State = 0;
           // TIMER_B_setCompareValue(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0,Sbit_Nom_HD-Sbit_Nom_LD);
            GPIO_setOutputHighOnPin( GPIO_PORT_P2, GPIO_PIN7 );
            GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P2, GPIO_PIN7 );
            //GPIO_setAsInputPin( GPIO_PORT_P2, GPIO_PIN2 );
            TIMER_B_disableCaptureCompareInterrupt(TIMER_B0_BASE,TIMER_B_CAPTURECOMPARE_REGISTER_0);
            counter = 0; // Reset counter
            //TACCR0 = CCIE;
            //printf("State 2\n");
            //TIMER_A_clearTimerInterruptFlag(TIMER_A0_BASE);

            break;

    case 4:
    		GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );
    		Timer0_State = 6;
    		break;
    case 6:
    		GPIO_setOutputHighOnPin( GPIO_PORT_P1, GPIO_PIN0 );
  }
}


