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
void process_sequence(int a[][2], int alen){
	
	/* Variables declared locally to function instead of loop for readability.
	Efficiency is equal.*/
	int colour;
	int time;
	
	/* Print table for testing purposes */
	for ( int i = 0; i < alen; i++ ){
 		for (  int j = 0; j < 2; j++ ){
    	printf("a[%d][%d] = %d\n", i,j, a[i][j] );
  	}
  }
  
  /* Loop through inner elements and store in variables */ 
	for(int i = 0; i < alen; i++){
		for(int j = 0; j < 1; j++){
			colour = a[i][j];
			time =  a[i][j+1];
			flash_sequence(colour,time);
			/* Print and compare with table for testing */
			printf("colour: %d time: %d\n",colour,time);
		}
	}
	
}

void flash_sequence(int colour, int time){
	switch(colour) {
		//Red	
		case 1:
		 		printf("RED for %d seconds",time);
		    break; 
		 //Green
		 case 20:
		 		printf("GREEN for %d seconds",time);
		    break; 
		 //Blue   
		 case 30:
		 		printf("BLUE for %d seconds",time);
		    break; 
		 //Red & Green   
		 case 21:
		 		printf("RED & GREEN for %d seconds",time);
		    break;    
		 //Red & Blue   
		 case 31:
		 		printf("RED & BLUE for %d seconds",time);
		    break;   
			//Green & Blue
			case 50:
		 		printf("GREEN & BLUE for %d seconds",time);
		    break; 
		    
		 	default :
		 		printf("No colour input");
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(led_sequence, ev, data)
{
	static struct etimer et1;
  static struct etimer et2;
  
  /* Declare multidimensional array with sequences */
  int sequence[][2] = { {RED,2}, {BLUE,4}, {REDBLUE,1}, {GREEN,1}, {GREENBLUE,1},
  {REDGREEN,1}, {BLUE,21} };
  
  /* Compute amount of sequence inner arrays as cant be done in function */
  int sequence_len = sizeof(sequence)/sizeof(sequence[0]);
  PROCESS_BEGIN();
	
	printf("Lyudmil Popov\n");	
	
	/* Call function with sequence and length arguments */
	process_sequence(sequence, sequence_len);
	
	etimer_set(&et1, CLOCK_SECOND);
	etimer_set(&et2, CLOCK_SECOND * 3);
	
	while(1) {
		PROCESS_YIELD();
		
		if(ev == button_hal_press_event){
			PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_release_event);	
		}
		if(etimer_expired(&et1)){
			leds_off(LEDS_RED);
			printf("etimer expired\n");
			etimer_reset(&et1);
		}
		if(etimer_expired(&et2)){
			leds_on(LEDS_RED);
			printf("red\n");
			etimer_reset(&et2);
		}
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
