/*
 * initGPIO.c
 *
 *  Created on: Mar 4, 2015
 *      Author: andrey
 */
//***** Header Files **********************************************************
#include <driverlib.h>
#include "myClocks.h"
#include "myGPIO.h"


void initGPIO(void) {

    // Set pin P1.0 to output direction and turn LED off
    GPIO_setAsOutputPin( GPIO_PORT_P1, GPIO_PIN0 );                   // Red LED (LED1)
    GPIO_setOutputLowOnPin( GPIO_PORT_P1, GPIO_PIN0 );

    // Set pin P4.7 to output and turn LED off
    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN7 );
    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN7 );

    // Set pin P4.3 to output
    GPIO_setAsOutputPin( GPIO_PORT_P4, GPIO_PIN3 );
    GPIO_setOutputLowOnPin( GPIO_PORT_P4, GPIO_PIN3 );

    GPIO_setAsPeripheralModuleFunctionOutputPin( GPIO_PORT_P1, GPIO_PIN3 );

    // Set P1.1 as input with pull-up resistor (for push button)
    //  configure interrupt on low-to-high transition
    //  and then clear flag and enable the interrupt
    GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P1, GPIO_PIN1 );
    GPIO_interruptEdgeSelect ( GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION );
    GPIO_clearInterruptFlag ( GPIO_PORT_P1, GPIO_PIN1 );
    GPIO_enableInterrupt ( GPIO_PORT_P1, GPIO_PIN1 );


   // GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P2, GPIO_PIN1 );
   // GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN1, GPIO_LOW_TO_HIGH_TRANSITION );
   // GPIO_clearInterruptFlag ( GPIO_PORT_P2, GPIO_PIN1 );
   // GPIO_enableInterrupt ( GPIO_PORT_P2, GPIO_PIN1 );


    // Set P2.7 As input with Pull-ip resistor CEC Input pin
    // Configure innterupr from Hight to Low transition
    // And then clear the flag and enable interrupt
    GPIO_setAsInputPin( GPIO_PORT_P2, GPIO_PIN7 );
    GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P2, GPIO_PIN7 );
    GPIO_interruptEdgeSelect ( GPIO_PORT_P2, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION );
    GPIO_clearInterruptFlag ( GPIO_PORT_P2, GPIO_PIN7 );
    GPIO_enableInterrupt ( GPIO_PORT_P2, GPIO_PIN7 );


    // Test
    GPIO_setAsInputPin( GPIO_PORT_P4, GPIO_PIN2 );
    GPIO_setAsInputPinWithPullUpresistor( GPIO_PORT_P4, GPIO_PIN2 );
    GPIO_interruptEdgeSelect ( GPIO_PORT_P4, GPIO_PIN2, GPIO_HIGH_TO_LOW_TRANSITION );
    GPIO_clearInterruptFlag ( GPIO_PORT_P4, GPIO_PIN2 );
    GPIO_enableInterrupt ( GPIO_PORT_P4, GPIO_PIN2 );

    // Debug pin for status of timer
    GPIO_setAsPeripheralModuleFunctionOutputPin( GPIO_PORT_P2, GPIO_PIN0 );

    // Debug led for ASK bit
    GPIO_setAsOutputPin( GPIO_PORT_P2, GPIO_PIN2 );
    GPIO_setOutputLowOnPin( GPIO_PORT_P2, GPIO_PIN2 );

}


