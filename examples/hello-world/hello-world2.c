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

PROCESS(ctimer_proc, "ctimer process 1");

AUTOSTART_PROCESSES(&ctimer_proc);

/*---------------------------------------------------------------------------*/
static struct ctimer timer_ctimer;
static struct ctimer timer_ctimer2;
/*---------------------------------------------------------------------------*/
void
ctimer_callback(void *ptr)
{
  /* rearm the ctimer */
  leds_off(LEDS_RED);
  ctimer_reset(&timer_ctimer);
  printf("CTimer callback called\n");
}
/*---------------------------------------------------------------------------*/
void
ctimer_callback2(void *ptr)
{
  /* rearm the ctimer */
  ctimer_reset(&timer_ctimer2);
	leds_on(LEDS_RED);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ctimer_proc, ev, data)
{
	
  PROCESS_BEGIN();
	
	while(1) {
		ctimer_set(&timer_ctimer, CLOCK_SECOND, ctimer_callback, NULL);
		ctimer_set(&timer_ctimer2	, CLOCK_SECOND * 3, ctimer_callback2, NULL);
		
		PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_press_event);
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_release_event);
		leds_off(LEDS_GREEN);
	}
	
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
