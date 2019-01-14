/*
 * Copyright (C) 2015, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>

#include "contiki.h"
#include "dev/button-hal.h"
#include "dev/leds.h"

PROCESS(etimer_proc, "etimer process 1");
PROCESS(etimer_proc2, "etimer process 2");

AUTOSTART_PROCESSES(&etimer_proc,&etimer_proc2);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(etimer_proc, ev, data)
{
	static struct etimer etimer1;
  static struct etimer etimer2;

  PROCESS_BEGIN();
  
	etimer_set(&etimer1, CLOCK_SECOND);
	etimer_set(&etimer2, CLOCK_SECOND * 3);
	
	while(1) {
		PROCESS_YIELD();
		if(etimer_expired(&etimer1)){
			leds_off(LEDS_RED);
			printf("etimer expired\n");
			etimer_reset(&etimer1);
		}
		if(etimer_expired(&etimer2)){
			leds_on(LEDS_RED);
			printf("red\n");
			etimer_reset(&etimer2);
		}
		if(ev == button_hal_press_event){
			PROCESS_YIELD_UNTIL(ev == button_hal_release_event);
		}
	}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(etimer_proc2, ev, data)
{
	
  PROCESS_BEGIN();
  while(1){
  	PROCESS_YIELD();
  	if(ev == button_hal_press_event){
			leds_off(LEDS_RED);
			leds_on(LEDS_GREEN);		
		} else if(ev == button_hal_release_event){
				leds_off(LEDS_GREEN);
			}
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
