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
 	PROGRAM DESC..
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-hal.h"
#include "sys/etimer.h"

#include <stdio.h>

/* Magic number RGB definitions*/
#define RED 1
#define GREEN 20
#define BLUE 30
#define REDGREEN 21
#define REDBLUE 31
#define GREENBLUE 50

/*---------------------------------------------------------------------------*/
PROCESS(led_sequence, "LED sequence process");
PROCESS(btn_press, "Button press LED process");
AUTOSTART_PROCESSES(&led_sequence, &btn_press);
/*---------------------------------------------------------------------------*/
void flash_colour(int colour, int time){
	switch(colour) {
		/* Red */
		case RED:
				leds_on(LEDS_RED);
		 		printf("RED for %d second(s)\n",time);
		    break; 
		 /* Green */
		 case GREEN:
		 		leds_on(LEDS_GREEN);
		 		printf("GREEN for %d second(s)\n",time);
		    break; 
		 /* Blue */   
		 case BLUE:
		 		leds_on(LEDS_BLUE);
		 		printf("BLUE for %d second(s)\n",time);
		    break; 
		 /* Red & Green */   
		 case REDGREEN:
		 		leds_on(LEDS_RED);
		 		leds_on(LEDS_GREEN);		
		 		printf("RED & GREEN for %d second(s)\n",time);
		    break;    
		 /* Red & Blue */   
		 case REDBLUE:
		 		leds_on(LEDS_RED);
		 		leds_on(LEDS_BLUE);
		 		printf("RED & BLUE for %d second(s)\n",time);
		    break;   
			/* Green & Blue */
			case GREENBLUE:
				leds_on(LEDS_GREEN);
		 		leds_on(LEDS_BLUE);
		 		printf("GREEN & BLUE for %d second(s)\n",time);
		    break;
		  /* no colour */   
		 	default :
		 		printf("No colour input");
	}
}
/*---------------------------------------------------------------------------*/
void process_sequence(int a[][2], int arrlen){
	
	int colour;
	int time;
	
  /* Loop through inner elements and store in variables */ 
	for(int i = 0; i < arrlen; i++){
		for(int j = 0; j < 1; j++){
			colour = a[i][j];
			time =  a[i][j+1];

			flash_colour(colour, time);
			clock_wait(CLOCK_SECOND * time);
			leds_off(LEDS_ALL);
		}
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(led_sequence, ev, data)
{ 
  /* Declare multidimensional array with sequences */
  int sequence[][2] = { {RED,2}, {BLUE,4}, {REDBLUE,1}, {GREEN,1}, {GREENBLUE,2} };
  
  /* Compute length of sequence */
  int sequence_len = sizeof(sequence)/sizeof(sequence[0]);
  
  PROCESS_BEGIN();
	
	printf("Lyudmil Popov\n");	
	
	while(1) {
		/* Call function with sequence and length */
		process_sequence(sequence, sequence_len);
	}
		
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(btn_press, ev, data)
{
  PROCESS_BEGIN();
  while(1){
		PROCESS_YIELD();
  	if(ev == button_hal_press_event){
			leds_off(LEDS_RED);
			leds_on(LEDS_GREEN);		
		}else if(ev == button_hal_release_event){
			leds_off(LEDS_GREEN);
		}
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
