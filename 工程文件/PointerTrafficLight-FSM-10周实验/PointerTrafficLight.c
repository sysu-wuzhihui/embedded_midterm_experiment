// PointerTrafficLight.c
// Runs on LM4F120/TM4C123
// Use a pointer implementation of a Moore finite state machine to operate
// a traffic light.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1 Program 6.8, Example 6.4
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2 Program 3.1, Example 3.1

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east facing red light connected to PB5
// east facing yellow light connected to PB4
// east facing green light connected to PB3
// north facing red light connected to PB2
// north facing yellow light connected to PB1
// north facing green light connected to PB0
// north facing car detector connected to PE1 (1=car present)
// east facing car detector connected to PE0 (1=car present)


#include <stdint.h>
#include "PLL.h"
#include "SysTick.h"
#include "inc/tm4c123gh6pm.h"

#define LIGHT                   (*((volatile uint32_t *)0x400051FC))
#define GPIO_PORTB_OUT          (*((volatile uint32_t *)0x400051FC)) // bits 6-0
#define GPIO_PORTE_IN           (*((volatile uint32_t *)0x4002401C)) // bits 2-0
#define SENSOR                  (*((volatile uint32_t *)0x4002401C))

struct State {
  uint32_t Out;            // 7-bit output
  uint32_t Time;           // 10 ms
  const struct State *Next[8];};// depends on 3-bit input
typedef const struct State STyp;
#define goN   &FSM[0]
#define waitN &FSM[1]
#define goE   &FSM[2]
#define waitE &FSM[3]
#define Emer  &FSM[5]		//FSM[5],FSM[6],FSM[7] are all Emer
STyp FSM[8]={
 {0x21,300,{goN,waitN,goN,waitN, Emer, Emer, Emer,Emer}},
 {0x22, 50,{goE,goE,goE,goE, Emer, Emer, Emer,Emer}},
 {0x0C,300,{goE,goE,waitE,waitE, Emer, Emer, Emer,Emer}},
 {0x14, 50,{goN,goN,goN,goN, Emer, Emer, Emer,Emer}},	
 {0x40, 300, {goN,goN,goN,goN, Emer, Emer, Emer,Emer}},
 {0x40, 300, {goN,goN,goN,goN, Emer, Emer, Emer,Emer}},
 {0x40, 300, {goN,goN,goN,goN, Emer, Emer, Emer,Emer}},
 {0x40, 300, {goN,goN,goN,goN, Emer, Emer, Emer,Emer}},
};

int main(void){
  STyp *Pt;  // state pointer
  uint32_t Input;
  volatile uint32_t delay;
  PLL_Init();                  // configure for 50 MHz clock
  SysTick_Init();              // initialize SysTick timer
  // activate port B and port E
  SYSCTL_RCGCGPIO_R |= 0x12;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTB_DIR_R |= 0x7F;    // make PB6-0 out
  GPIO_PORTB_AFSEL_R &= ~0x7F; // disable alt func on PB6-0
  GPIO_PORTB_DEN_R |= 0x7F;    // enable digital I/O on PB6-0
                               // configure PB6-0 as GPIO
  GPIO_PORTB_PCTL_R &= ~0x0FFFFFFF;
  GPIO_PORTB_AMSEL_R &= ~0x7F; // disable analog functionality on PB6-0
  GPIO_PORTE_DIR_R &= ~0x07;   // make PE2-0 in
  GPIO_PORTE_AFSEL_R &= ~0x07; // disable alt func on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // enable digital I/O on PE2-0
                               // configure PE2-0 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFF000)+0x00000000;
  GPIO_PORTE_AMSEL_R &= ~0x07; // disable analog functionality on PE2-0
  Pt = goN;                    // initial state: Green north; Red east
  while(1){
    LIGHT = Pt->Out;           // set lights to current state's Out value
    SysTick_Wait10ms(Pt->Time);// wait 10 ms * current state's Time value
    Input = SENSOR;            // get new input from car detectors
    Pt = Pt->Next[Input];      // transition to next state
  }
}

