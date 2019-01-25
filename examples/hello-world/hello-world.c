/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A Contiki application which flashes a sequence of LEDs infinitely. A user 						button press will interrupt the program and hold a green LED. 
 * \author
 *         Lyudmil Popov <i7461612@bournemouth.ac.uk>
 */
 
#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-hal.h"
#include "sys/etimer.h"

#include <stdio.h>

/* RGB definitions */
#define RED 1
#define GREEN 2
#define BLUE 3
#define REDGREEN 4
#define REDBLUE 5
#define GREENBLUE 6

/*---------------------------------------------------------------------------*/
PROCESS(led_sequence, "LED sequence process");
PROCESS(btn_press, "Button press LED process");
AUTOSTART_PROCESSES(&led_sequence, &btn_press);
/*---------------------------------------------------------------------------*/
/* Declare Variables */
static int colour;
static int counter = 1;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(led_sequence, ev, data)
{	
	/* Declare event timer */
	static struct etimer et;
	
	PROCESS_BEGIN();
	
	printf("Lyudmil Popov\n");	
	
	/* Initialize variables*/
	colour = RED;
	counter = 1;
	
	/* Start 1 second timer */
	etimer_set(&et, CLOCK_SECOND);
	
	while(1) {
		
		PROCESS_YIELD();	
		
		/* If delivered event after yield is a timer res then reset LEDs */
		if(ev == PROCESS_EVENT_TIMER){
			leds_off(LEDS_ALL);
		}
		
		/* If event is a button press yield control until release event */
		if(ev == button_hal_press_event){
			PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_release_event);	
		}
		
		/* If etimer expired and colour is RED */
		if(etimer_expired(&et) && colour == RED){
						
			/* Turn on LED */
			leds_on(LEDS_RED);
			
			/* Print control message */
			printf("%d----> red LED: ON\n",counter++);

			/* Set etimer */
			etimer_set(&et, CLOCK_SECOND * 2);
			
			/* Set next colour */
			colour = BLUE;
		}
		
		/* Above repeated for each colour in sequence */	
		if(etimer_expired(&et) && colour == BLUE){
			leds_on(LEDS_BLUE);		
			printf("%d----> blue LED: ON, red LED: OFF\n",counter++);			
			etimer_set(&et, CLOCK_SECOND * 4);
			colour = REDBLUE;
		}
		
		if(etimer_expired(&et) && colour == REDBLUE){
			leds_on(LEDS_RED);
			printf("%d----> red LED: ON, blue LED: ON\n",counter++);
			leds_on(LEDS_BLUE);
			etimer_set(&et, CLOCK_SECOND);
			colour = RED;
		}
	} 
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/* Button press process */
PROCESS_THREAD(btn_press, ev, data)
{

  PROCESS_BEGIN();
  
  while(1){
  
    /* Yield control until button is pressed */
		PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_press_event);		
  	
  	printf("<------Interrupt event------->\n\tgreen LED: ON\n\n");
  	
  	/* Turn all LEDs off and turn on GREEN */
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		
		/* Keep the GREEN LED on until button released */
		PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_release_event);
		leds_off(LEDS_GREEN);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
