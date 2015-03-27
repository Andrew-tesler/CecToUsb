//******************************************************************************
//  MSP430F201x CEC to I2C Translator 
//
//  Description:  The MSP430 bridges a CEC and I2C bus.  The communication is
//                is half-duplex: either CEC to I2C or I2C to CEC.  
//                The MSP430 receives and store bytes of data from the I2C and 
//                retransmit data on the single wire CEC bus or recieve 
//                data on the CEC bus and retransmit on the I2C bus.  For 
//                testing the MSP430F2013 was used, but this code is also
//                usable on the MSP430F2012.
//                
//
//                MSP430F2013                     HOST
//                                               
//              -----------------           -----------------
//         CEC-|P1.0     P1.7/SDA|<-|---+->|SDA              |
//             |                 |  |      |                 |
//             |                 |  |      |                 |
//             |         P1.6/SCL|<-+----->|SCL              |
//             |                 |         |                 |
//
// Internal Pullups are used on P1.6 and P1.7 as well as P1.0 when not 
// not actively driving low
//
// Developed on V3.42A/W32 [Kickstart] (3.42.1.3)
// Texas Instruments
// C. Sterzik 11/5/2007
//
// THIS PROGRAM IS PROVIDED "AS IS". TI MAKES NO WARRANTIES OR
// REPRESENTATIONS, EITHER EXPRESS, IMPLIED OR STATUTORY,
// INCLUDING ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
// COMPLETENESS OF RESPONSES, RESULTS AND LACK OF NEGLIGENCE.
// TI DISCLAIMS ANY WARRANTY OF TITLE, QUIET ENJOYMENT, QUIET
// POSSESSION, AND NON-INFRINGEMENT OF ANY THIRD PARTY
// INTELLECTUAL PROPERTY RIGHTS WITH REGARD TO THE PROGRAM OR
// YOUR USE OF THE PROGRAM.
//
// IN NO EVENT SHALL TI BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// CONSEQUENTIAL OR INDIRECT DAMAGES, HOWEVER CAUSED, ON ANY
// THEORY OF LIABILITY AND WHETHER OR NOT TI HAS BEEN ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGES, ARISING IN ANY WAY OUT
// OF THIS AGREEMENT, THE PROGRAM, OR YOUR USE OF THE PROGRAM.
// EXCLUDED DAMAGES INCLUDE, BUT ARE NOT LIMITED TO, COST OF
// REMOVAL OR REINSTALLATION, COMPUTER TIME, LABOR COSTS, LOSS
// OF GOODWILL, LOSS OF PROFITS, LOSS OF SAVINGS, OR LOSS OF
// USE OR INTERRUPTION OF BUSINESS. IN NO EVENT WILL TI'S
// AGGREGATE LIABILITY UNDER THIS AGREEMENT OR ARISING OUT OF
// YOUR USE OF THE PROGRAM EXCEED FIVE HUNDRED DOLLARS
// (U.S.$500).
//
// Unless otherwise stated, the Program written and copyrighted
// by Texas Instruments is distributed as "freeware".  You may,
// only under TI's copyright in the Program, use and modify the
// Program without any charge or restriction.  You may
// distribute to third parties, provided that you transfer a
// copy of this license to the third party and the third party
// agrees to these terms by its first use of the Program. You
// must reproduce the copyright notice and any other legend of
// ownership on each copy or partial copy, of the Program.
//
// You acknowledge and agree that the Program contains
// copyrighted material, trade secrets and other TI proprietary
// information and is protected by copyright laws,
// international copyright treaties, and trade secret laws, as
// well as other intellectual property laws.  To protect TI's
// rights in the Program, you agree not to decompile, reverse
// engineer, disassemble or otherwise translate any object code
// versions of the Program to a human-readable form.  You agree
// that in no event will you alter, remove or destroy any
// copyright notice included in the Program.  TI reserves all
// rights not specifically granted under this license. Except
// as specifically provided herein, nothing in this agreement
// shall be construed as conferring by implication, estoppel,
// or otherwise, upon you, any license or other right under any
// TI patents, copyrights or trade secrets.
//******************************************************************************

#include  <msp430x20x3.h>
/* Define applicable CEC and I2C address and maximum CEC message size in bytes*/
#define Host_I2C_Addr 0x94                  // Address is 0x4A<<1 for R/W
#define Local_I2C_Addr 0x90                 // Local address is 0x48 
#define Bcst_CEC_Addr 0x0F                  // BroadCast address
#define Local_CEC_Addr 0x00                 // Logical CEC address for TV
#define Max_Message_Size 15                 // Maximum number of bytes in a 
                                            // CEC message is 15, this can
                                            // be increased for manufacturer
                                            // defined messages

/* CEC Start Bit Timing Defintions, based upon TimerA = SMCLK/2 = 62.5kHz */
/* Recieve Start   */
#define Sbit_Min_LD 218                     // Start Bit Minimum Low duration
#define Sbit_Max_LD 244                     // Start Bit Maximum Low duration
#define Sbit_Min_TD 268                     // Start Bit Minimum Total duration                       
#define Sbit_Max_TD 294                     // Start Bit Maximum Total duration
/* Transmit Start  */
#define Sbit_Nom_LD 231                     // Start Bit Nominal Low duration 
#define Sbit_Nom_HD 50                      // Start Bit Nominal High Duration 

/* CEC Data Bit Timing Defintions, based upon TimerA = SMCLK/2 = 62.5kHz */
/* Recieve Data    */
#define Dbit_Min_TD 128                     // Data Bit Minimum Total Duration
#define Dbit_Max_TD 172                     // Data Bit Maximum Total Duration
#define Dbit_Sample 65                      // Data Bit Sample time
/* Transmit Data   */
#define Dbit_High_LD 37                     // Data Bit Nominal Low duration
#define Dbit_High_HD 112                    // Data Bit Nominal High Duration
#define Dbit_Low_LD 93                      // Data Bit Nominal Low duration
#define Dbit_Low_HD 56                      // Data Bit Nominal High Duration

/* CEC Ack Bit Timing Defintions, based upon TimerA = SMCLK/2 = 62.5kHz */
#define Ack_HD 84                           // Set High duration after ACK
#define Ack_sample 28                       // The acknowledge is sampled 
                                            // after the low duration is
                                            // transmitted.

                                            // State variable determines 
unsigned char I2C_State = 0;                // code executed within USI ISR 
unsigned char CEC_State = 0;                // code executed within Port1 ISR
unsigned char Timer0_State = 0;             // code executed within Timer0 ISR     
unsigned char Timer1_State = 0;             // code executed within Timer1 ISR

unsigned char Payload[Max_Message_Size];    // Data from either I2C or CEC is
                                            // stored here
unsigned char Pldcnt = 0;                   // Count # of bytes rx'd or tx'd
                                            // this should be made volatile if 
                                            // optimizations are used in 
                                            // compiler
unsigned char Pldsize = 0;                  // Records # of bytes received
unsigned char Bit_Count = 0;                // Tracks bit count of CEC message
unsigned char EOM = 0;                      // End of Message Flag
unsigned char Retry = 0;                    // Retry Flag used to retransmit

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO, to 1Mhz
  DCOCTL = CALDCO_1MHZ;
  P1DIR = 0xFE;                             // Unused pins as outputs
                                            // P1.0 is an input for CEC
  
  while(1)
  {
    /* Set Port1.0 to trigger interrupt when there is a high-to-low      */
    /* transition. This would be the the first edge of the CEC start bit */
    P1IES |= BIT0;                           
    P1IFG &= ~BIT0;
    P1OUT &= ~BIT0;
    /* Initialize State variables and byte counter */
    I2C_State = 0;                           
    CEC_State = 0;                           
    Pldcnt = 0;                              
    BCSCTL2 |= DIVS_1;                      // SMCLK is DCO/2, 500kHz
    /* Setup USI:I2C as Slave recieve to trigger an interrupt in the event
       of a start condition. */
    USICTL0 = USIPE6+USIPE7+USISWRST;       // Port & USI mode setup
    USICTL1 = USII2C;                       // Set to I2C mode
    USICKCTL = USICKPL;                     // Setup clock pol 
    USICNT |= USIIFGCC;                     // Disable automatic clear control 
    USICTL0 &= ~USISWRST;                   // Enable USI
    USICTL1 &= ~(USISTP+USIIFG+USISTTIFG);  // Clear Flags
    __disable_interrupt();                  //  Prevent interrupt from occuring 
                                            // between enable and LPM4
    USICTL1 |= USIIE+USISTTIE;              // Enable I2C
    P1IE |= BIT0;                           // Enable CEC port interrupt
    __bis_SR_register(LPM4_bits+GIE);       // Wait in LPM4 until interrupt for 
                                            // I2C or CEC receive
    __no_operation();                       // For debug purposes
    while(Pldcnt)                            
    { /* If Pldcnt is true, then this is the I2C to CEC case */    
      if (USICTL1 & USISTP)                 // Check for the Stop flag
      { /* Finished I2C data reception and moving on to CEC transmit */
        USICTL1 &= ~(USIIE+USISTTIE);       // Disable USI interrupts
        USICNT &= 0xE0;                     // Clear USI bit count
        P1DIR |= BIT0;                      // Set output Low to generate the
                                            // falling edge of the start bit
        TACTL |= TACLR;                     // Reset timerA
        TACTL = TASSEL_2 + ID_3 + MC0;      //SMCLK/8=62.5kHz, Up mode
        TACCR0 = Sbit_Nom_LD;
        Timer0_State = 2;
        Pldsize = Pldcnt;                   // Record the number of bytes rxd
        Pldcnt=0;                           // Initialize variables
        Retry = 0;
        TACCTL0 = CCIE;                     // Enable TimerA0 Interrupt
      }
    }
    /* Hold CPU here until the transmission of the message is completed. */
    /* Upon completion, or failure to acknowledge reset interrupts and   */
    /* return to LPM4                                                    */
    __bis_SR_register(LPM0_bits);
    __no_operation();                       // For Debug
    if (I2C_State == 0)             
    /* This section is entered when transitioning from CEC recieve to I2C
      transmit. */  
    {
      BCSCTL2 &= ~DIVS_1;                   // SMCLK is 1MHz
      USICTL0 = USIPE6+USIPE7+USIMST+USISWRST; // Port & USI mode setup
      USICKCTL = USIDIV_3 + USISSEL_2 + USICKPL; // Setup clock
                                            // Polarity, SMCLK/8, 125kHz
      USICTL0 &= ~USISWRST;                 // Enable USI 
      USICTL1 |= USII2C+USIIFG;             // Set flag
      __disable_interrupt();                // Prevent interrupt from occuring 
                                            // between enable and LPM0
      USICTL1 |= USIIE;                     // Enable I2C mode & USI interrupts
      __bis_SR_register(LPM0_bits+GIE);                                  
      __no_operation();                     // For Debug
    }
  }
}

//*****************************************************************************
// USI interrupt service routine
//*****************************************************************************
#pragma vector = USI_VECTOR
__interrupt void USI_TXRX (void)
{
  if (USICTL1 & USISTTIFG)                  // Start entry?
  {
    P1IE &= ~BIT0;                          // Disable interrupt
    I2C_State =8;                           // Enter 8th state on start
  }
  switch(__even_in_range(I2C_State,16))
  {
//******************************************************************************
// Transmit I2C states: 0-6
// Enter case 0 from Timer0_State = 2
//******************************************************************************
    case 0: // Send Address
            USISRL = 0x00;                  // Generate Start Condition
            USICTL0 |= USIGE+USIOE;
            USICTL0 &= ~USIGE;          
            USISRL = Host_I2C_Addr;         // transmit address, R/W = 0
            USICNT = (USICNT & 0xE0) + 0x08;// Bit counter = 8, TX Address
            I2C_State = 2;                  // Go to next state: receive ack
            Pldcnt = 0;                     // Initialize Pldcnt
            USICTL1 &= ~USISTTIFG;
/* The master transmit causes the USISTTIFG flag to be set.  Since this flag */ 
/* is checked at the top of the ISR for slave receive it needs to be cleared.*/            
            break;
                            
    case 2: // Receive N/Ack bit
            USICTL0 &= ~USIOE;              // SDA = input
            USICNT |= 0x01;                 // Bit counter = 1, receive N/Ack
            I2C_State = 4;                  // Go to next state: check N/Ack
            break;
                            
    case 4: // Check Ack/Nack
            USICTL0 |= USIOE;
            if (USISRL & 0x01)
            {
              // Nack, Send stop...
              USISRL = 0x00;
              USICNT |= 0x01;               // Bit counter=1, SCL high,SDA low
              I2C_State = 6;                // Go to next state:generate stop
            }
            else if (Pldcnt <= Pldsize)
            {
              USISRL = Payload[Pldcnt];     // Load data byte
              USICNT |=  0x08;              // Bit counter = 8, start TX
              Pldcnt++;                     // increment counter
              I2C_State = 2;                // Go to next state:receive N/Ack
            }
            else
            {
              // End of message, Send stop...
              USISRL = 0x00;
              USICNT |= 0x01;               // Bit counter=1,SCL high,SDA low
              I2C_State = 6;                // Go to next state: generate Stop
            }
            break;
      
    case 6: // Generate Stop Condition
            USISRL = 0x0FF;                 // USISRL = 1 to release SDA
            USICTL0 |= USIGE;               // Transparent latch enabled
            USICTL0 &= ~(USIGE+USIOE);      // Latch/SDA output disabled
            if ((Pldcnt <= Pldsize) && !Retry)
            {                               //Error occurred, retransmit 
              Retry=1;
              I2C_State = 0;                // Special case of retransmit
            }  
            else
            {                               // Done 
              USICTL1 &= ~(USIIE);          // Disable USI interrupt
              __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
            }
            break;            
 //*****************************************************************************
 // Receive I2C, states 8-16                                             
 //                                                                        
 //*****************************************************************************
    case 8:                                 // RX Address
            USICNT = (USICNT & 0xE0) + 0x08;// Bit counter = 8, RX address
            USICTL1 &= ~USISTTIFG;
            I2C_State = 10;                 // Go to next state:check address
            break;

    case 10:                                // Check Address and send (N)Ack
            USICTL0 |= USIOE;               // SDA = output
            if (USISRL == Local_I2C_Addr)   // Address match?
            {
              USISRL = 0x00;                // Send Ack
              I2C_State = 12;               // Next state:Receive data
            }
            else
            {
              USISRL = 0xFF;                // Send NAck
              I2C_State = 16;               // 
            }
            USICNT |= 0x01;                 // Bit counter = 1, send (N)Ack bit
            break;

    case 12:                                // Receive data byte
            USICTL0 &= ~USIOE;              // SDA = input
            I2C_State = 14;                 // Next state: Load data, TX ack
            USICNT |=  0x08;                // Bit counter = 8, RX data
            break;

    case 14:                                // Load Data & TX Ack
            Payload[Pldcnt] = USISRL;       // Store data
            USICTL0 |= USIOE;               // SDA = output
            Pldcnt++;                       // Increment Payload count
            I2C_State = 12;             
            USISRL = 0x00;                  // Send Ack
            USICNT |= 0x01;                 // Bit counter = 1,send Ack
            __bic_SR_register_on_exit(LPM4_bits);     // Exit LPM4
            break;                  
              
    case 16:
            USICTL0 &= ~USIOE;              // SDA = input
            P1IE |= BIT0;                   // Enable CEC interrupt
            break;                          // Still in LPM4 and waiting
  }
  /* END CASE STATEMENT */
  USICTL1 &= ~USIIFG;                       // Clear Flag
  if (I2C_State == 0)
  {                                         // Special case of retransmit
    USICTL1 |= USIIFG;                      // Set Flag
  }
}

//******************************************************************************
// Port 1 interrupt service routine
// CEC Recieve
//******************************************************************************
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR(void)
{
  int current_time;
  unsigned char tempa;
  P1IFG &= ~BIT0;                           // Clear flag
  switch(__even_in_range(CEC_State, 8))
  {
    case 0:                                 // Receive First Falling edge, 
                                            // verify start bit criteria
            USICTL1 &= ~(USIIE+USISTTIE);   // Disable USI interrupts
            USICTL1 &= ~USIIFG;             // Clear Flag
            P1IES &= ~BIT0;                 // Set to trigger on rising edge
            P1IFG &= ~BIT0;                 // Clear flag after toggling edge
            Timer0_State = 0;               // Using timer to test bit times
            Timer1_State = 0;
            TACTL |= TACLR;                 // Reset timerA
            TACTL = TASSEL_2 + ID_3 + MC0;  //SMCLK/8=62.5kHz, U&p mode
            /* Start bit needs to meet requirements for low duration and */
            /* total duration.                                           */
            TACCR1 = Sbit_Max_LD;           // Low duration cannot exceed        
            TACCR0 = Sbit_Max_TD;           // Total Duration cannot exceed
            TACCTL0 |= CCIE;           
            TACCTL1 |= CCIE;           
            CEC_State = 2;                  // Go to next state: Rising Edge
            __bic_SR_register_on_exit(LPM4_bits);     // Exit LPM4
            break;
              
    case 2:                                 // Recieve Rising Edge of Start bit
            TACCTL1 &= ~CCIE;               // Turn off fault timer
            P1IES |= BIT0;                  // Set to trigger on falling edge
            P1IFG &= ~BIT0;                 // Clear flag after toggling edge
            TACTL &= ~MC0;                  //Halt the timer  
            current_time = TAR;
            TACTL |= MC0;                   // Restart Timer
            if ( current_time < Sbit_Min_LD )
            {
              TACCTL0 &= ~CCIE;             // Disable interrupt
              CEC_State = 0;                // Invalid low duration
              __bic_SR_register_on_exit(LPM0_bits);  // start process over
            }
            else
            {
              CEC_State = 4;                 // Go to next falling edge
            }
            break;
              
    case 4:                                  // Recieve Second Falling Edge of 
                                             //Start bit
            TACCTL0 &= ~CCIE;                // Disable timer interrupt
            TACCR0 = 0;                      // Halt timer 
            current_time = TAR;
            if ( current_time < Sbit_Min_TD )
            {
              CEC_State = 0;                 // Invalid total duration
              __bic_SR_register_on_exit(LPM0_bits);
            }
            else
            {
              /* Start bit is valid, get ready to receive data */
              Timer1_State = 2;              //Using timer1 to sample
              Timer0_State = 0;
              CEC_State = 6;              
              TACCR1 = Dbit_Sample;         // Setup sample time
              TACCR0 = Dbit_Max_TD;         // Setup maximum bit time
              TACCTL0 |= CCIE;               
              TACCTL1 |= CCIE;           
              Bit_Count = 9;
              Pldcnt = 0;
              Payload[Pldcnt]=0;            //Clear memory location
            }
            break;
              
    case 6:                                 // Recieve/Ack Data Falling Edge
            TACCTL0 &= ~CCIE;               // Disable timer interrupt
            TACCR0 = 0;                     // Halt timer
            current_time = TAR;
            /* The CEC address should only be checked once, Pldcnt=0, after all 
               of the bits have been received, Bit_Count=0, from the header
               block.  The address must match either the broadcast or the local
               CEC address.*/
            tempa = Payload[0] & 0xF;       // Only check destination bits
            if (tempa == Bcst_CEC_Addr)
              tempa = 0;
            else if (tempa == Local_CEC_Addr)
              tempa = 0;
            else tempa=1;
            tempa = tempa && !Bit_Count;
            tempa = tempa && !Pldcnt;
            if ( current_time < Dbit_Min_TD || tempa )
            {
              CEC_State = 0;                // Invalid bit time, ignore
              I2C_State = 2;                // Be sure to return to top of main
                                            // While loop
              __bic_SR_register_on_exit(LPM0_bits);
              break;            
            }
            if (Bit_Count)                  // 9-2 data, 1 EOM, 0 ACK
            {
              Timer0_State = 0;                
              Timer1_State = 2;             // Using timer1 to sample
              TACCR1 = Dbit_Sample;         // Setup sample time
              TACCTL1 |= CCIE;              
              TACCR0 = Dbit_Max_TD;         // Test for maximum bit time
            }
            else
            {
              P1DIR |= BIT0;                // Set to Output Low, ACK
              Timer0_State = 0;                
              Timer1_State = 4;             // Using timer1 to release ACK
              TACCR1 = Dbit_Low_LD;         // Setup time held low for ACK
              TACCTL1 |= CCIE;              
              TACCR0 = Dbit_Max_TD;         // Test for maximum bit time
            }                  
            TACCTL0 |= CCIE;             
            break;
              
    case 8:                                  
            // Start for next byte, bit immediately after the ack
            TACCTL0 &= ~CCIE;                // Disable timer interrupt
            TACCR0 = 0;                      // Halt timer 
            current_time = TAR;
            if ( current_time < Dbit_Min_TD )
            {
              CEC_State = 0;                 // Invalid bit time
              __bic_SR_register_on_exit(LPM0_bits);
            }
            else
            {
              Timer1_State = 2;               // Using timer1 to sample
              TACCR1 = Dbit_Sample;           // Setup sample time
              Timer0_State = 0;
              TACCR0 = Dbit_Max_TD;           // Test for maximum bit time
              CEC_State = 6;
              TACCTL0 |= CCIE;            
              TACCTL1 |= CCIE;    
            }
            break;      
   }                                        // END CASE STATEMENT
   TACCTL0 &= ~CCIFG;                       // Clear Flag
   TACCTL1 &= ~CCIFG;                       // Clear Flag  
}
//******************************************************************************
// Timer A0 interrupt service routine
//******************************************************************************
#pragma vector=TIMERA0_VECTOR
__interrupt void timera0_ISR(void)
{           
  unsigned char data;
  TACCTL0 &= ~CCIE;                         // Disable interrupt
  switch(__even_in_range(Timer0_State, 18))
  {
    case 0: //Testing Bit timing
            /* failing condition */
            TACCTL1 &= ~CCIE;               // Disable interrupt
            CEC_State = 0;                  // Invalid start, ignore
            __bic_SR_register_on_exit(LPM0_bits);  // Exit LPM0
            break;
                            
/* Transmit CEC information, Case 2 is entered when a stop condition is     */
/* received on the I2C bus or when a retry occurs. Timer has counted up to  */
/* TACCR0, re-started at 0 and therefor does not need to be reset.          */
            
    case 2:                                 //Transmit start bit
            P1DIR &= ~BIT0;                 // Set to input
            TACCR0 = Sbit_Nom_HD;           // setup high duration of start bit
            Timer0_State = 4;            
            Bit_Count = 7;
            EOM = 0;
            TACCTL0 = CCIE;            
            break;
              
    case 4: //2nd falling edge, start data 
            P1DIR |= BIT0;                  // Set to output
            data = Payload[Pldcnt];
            data = data >> Bit_Count;       // msb first
            data = data & 0x01;
            if(data) 
              TACCR0 = Dbit_High_LD;        // Set low duration of data bit
            else TACCR0 = Dbit_Low_LD;                
            Timer0_State = 6;                
            TACCTL0 = CCIE;             
            break;
                   
    case 6: //Transmit data bit, setup falling
            P1DIR &= ~BIT0;                 // Set to input
            if (TACCR0 == Dbit_High_LD)
              TACCR0 = Dbit_High_HD;
            else TACCR0 = Dbit_Low_HD;      // Set high duration of data bit        
            if (Bit_Count == 0)
            {
              Pldcnt++;
              Timer0_State = 8;         
            }
            else 
            {
              Timer0_State = 4;
              Bit_Count--;
            }
            TACCTL0 = CCIE;            
            break;  
              
    case 8: //falling edge of last bit of data, start EOM
            P1DIR |= BIT0;                  // Set to output
            if(Pldcnt == Pldsize)
            {
              TACCR0 = Dbit_High_LD;        // Transition for EOM bit 
                                            //rising edge
              EOM =1;                            
            }
            else 
            {
              EOM = 0;
              TACCR0 = Dbit_Low_LD;         //  94 -> 93
            }
            Timer0_State = 10;                   
            TACCTL0 = CCIE;                      
            break;
              
    case 10: //
            P1DIR &= ~BIT0;                 // Set to input
            if(EOM)
              TACCR0 = Dbit_High_HD;        // Set High duration for EOM bit 
            else TACCR0 = Dbit_Low_HD;           
            Timer0_State = 12;              // Recieve ack
            TACCTL0 = CCIE;             
            break;
                    
    case 12:                                // Setup for ACK
            P1DIR |= BIT0;                  // Set to output
            TACCR0 = Dbit_High_LD;          // Transmitter sets low duration
            Timer0_State = 14;              // before recieveing Ack
            TACCTL0 = CCIE;             
            break;
              
    case 14:                                // After completion of the low 
            P1DIR &= ~BIT0;                 // the CEC transmitter releases
            TACCR0 = Ack_sample;            // Time for sampling ACK
            Timer0_State = 16;              // Recieve ack
            TACCTL0 = CCIE;             
            break;
                      
    case 16:                                // Process (N)ACK
            data = P1IN;                    // Low is an ACK
            data &= BIT0;
            TACCR0 = Ack_HD;                // Set High duration after Ack
            if (data && !Retry)
	        {                               // Nack Received
              Timer0_State = 18;
              TACCTL0 = CCIE;
            }
            else if (!data && !EOM)
            { 
              Timer0_State = 4;                
              Bit_Count = 7;                //  Get the next byte
              TACCTL0 = CCIE;
            }
            else
            {
              CEC_State = 0;
              __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
            }
            break;
              
    case 18:  
            P1DIR |= BIT0;                  // Set to output
            TACCR0 = Sbit_Nom_LD;           // Transition for start bit
            Timer0_State = 2;           
            TACCTL0 = CCIE;           
            Pldcnt = 0;
            Retry = 1;  
            break;
  }
}

//******************************************************************************
// Timer A1 interrupt service routine
//******************************************************************************
#pragma vector=TIMERA1_VECTOR
__interrupt void timera1_ISR(void)
{
  unsigned char data;
  TACCTL1 &= ~CCIE;                         // Disable interrupt
  TACCTL1 &= ~CCIFG;                        // Clear Flag
  switch(__even_in_range(Timer1_State, 4))
  {
    case 0:                                 //Failing Bit timing
            TACCTL0 &= ~CCIE;               // Disable interrupt
            TACCTL0 &= ~CCIFG;              // Clear Flag
            __bic_SR_register_on_exit(LPM0_bits);     // Exit LPM0
            break;
              
    case 2:                                 //Sample recieved data
            data = P1IN;                    // Bit of interest is at location 1
            data &= BIT0;
            if (Bit_Count == 1)
            {
              EOM = (data);
            }
            else
            {
              data = data << (Bit_Count-2);
              Payload[Pldcnt] |= data;
            }
            Bit_Count--;
            break;
         
    case 4: //Finish Ack
            P1DIR &= ~BIT0;                 // Bit0 as input
            if (EOM)
            {
              TACCTL0 &= ~CCIE;             // Disable interrupt
              TACCTL0 &= ~CCIFG;            // Clear Flag
              I2C_State = 0;                // CEC successful TX I2C
              Pldsize = Pldcnt;             // Store the # of bytes recieved 
              Retry = 0;
              P1IE &= ~BIT0;
              __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0 and start I2C
            }
            else 
            { // continue to next byte
              CEC_State = 8;                // Go to Falling edge of ack, start
              Bit_Count = 9;                // of next byte
              Pldcnt++;                     // increment byte counter
              Payload[Pldcnt]=0;            //Clear memory location
            }
            break;

  }
}
