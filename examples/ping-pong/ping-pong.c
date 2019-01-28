/*
 * Copyright (c) 2017, RISE SICS.
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
 *         A simple ping-pong application using nullnet broadcast for two motes with 
					 cc2538 microprocessors. After a brief moment exchanging tokens, a user may switch
					 the mote which holds the LED, similar to a game of ping pong.	
 * \author
*         Lyudmil Popov <i7461612@bournemouth.ac.uk>
 *
 */

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/leds.h"
#include "dev/button-hal.h"

/* Headers for radio module of mcu */
#include "dev/rfcore.h"
#include "dev/cc2538-rf.h"
#include "dev/soc-adc.h"
#include "dev/sys-ctrl.h"
#include "reg.h"
/* End mcu headers*/

#include <string.h>
#include <stdio.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO


#define TOKEN_INTERVAL (CLOCK_SECOND * 4)

/* half second window to receive ping */
#define RECIEVE_INTERVAL (CLOCK_SECOND / 2)

/* Waiting to send */
#define SENDINGNODE 1 

/* Waiting to recieve */
#define RECEIVINGNODE 0 

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_example_process);
/*---------------------------------------------------------------------------*/

/* Prototype for token broadcast function*/
void bc_token(int token);

/* Initialize token for communication */
static int token;

/* Initialize and declare discovery variable */
static int discovery = 1;

/* Initialize and declare node_status variable */
static int node_status = RECEIVINGNODE;

/* Set ping pong output variable  */
static int pingpong = 1;
/*---------------------------------------------------------------------------*/
/* Function generating random number using LSFR */
unsigned short random_num(void)
{
  uint32_t rv;

  /* Clock the RNG LSFR once */
  REG(SOC_ADC_ADCCON1) |= SOC_ADC_ADCCON1_RCTRL0;

  rv = REG(SOC_ADC_RNDL) | (REG(SOC_ADC_RNDH) << 8);
  return ((unsigned short)rv);
}
/*---------------------------------------------------------------------------*/
/* Function which processes generated token and recieved token. 
	Smaller of the two initiates communication. */
void process_tokens(int token, int token_rec){

	/* If token is smaller than the recieved token turn on LED */
	if (token < token_rec) {
		leds_on(LEDS_BLUE);
		
		/* Initiator mote has even pingpong number to produce same outut each time*/
		pingpong = 2;

		/* Broadcast the received token back over nullnet to announce that mote is now sender */
		bc_token(token_rec);
		
		/* This mote now becomes sender */
		node_status = SENDINGNODE;
		
		/* Token pairing is now done. Get out of discovery loop in process*/
		discovery = 0;				
		} 

	/* If token is the same as recieved it means other node has assumed sender responsibility */
	if (token == token_rec) {
		discovery = 0;		
		}
}
/*---------------------------------------------------------------------------*/
/* Function to process recieved ping message */
void process_ping(char c) {
	
	/* If packet payload is p (ping) turn on LED and assume sender role */
	if (c == 'p'){
		leds_on(LEDS_BLUE);
		node_status = SENDINGNODE;
	}
}
/*---------------------------------------------------------------------------*/
/* Function to be called upon packet reception */
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
	
	/* If packet has size of int */
  if(len == sizeof(int)) {

		/* Recieved token from other mote */
    int token_rec;
    memcpy(&token_rec, data, sizeof(token_rec));
    LOG_INFO("Token received %u from ", token_rec);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
		
		/* Process both token and recieved token */	
		process_tokens(token,token_rec);
  }

	/* If packet has size of char */
	if(len == sizeof(char)) {
		int c;
		memcpy(&c, data, sizeof(c));
		
		/* Process char message */
		process_ping(c);	
	}
}
/*---------------------------------------------------------------------------*/
/* Function to broadcast token over nullnet */
void bc_token(int token) {
	nullnet_buf = (uint8_t *)&token;
	nullnet_set_input_callback(input_callback);
	LOG_INFO("Sending token %d to ", token);
  LOG_INFO_LLADDR(NULL);
  LOG_INFO_("\n");
	memcpy(nullnet_buf, &token, sizeof(token));
	nullnet_len = sizeof(token);
	NETSTACK_NETWORK.output(NULL);
}
/*---------------------------------------------------------------------------*/
/* Function to broadcast "ping" packet over nullnet */
void bc_ping(char c) {
	nullnet_buf = (uint8_t *)&c;		
	nullnet_set_input_callback(input_callback);
	memcpy(nullnet_buf, &c, sizeof(c));
	nullnet_len = sizeof(c);
	NETSTACK_NETWORK.output(NULL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{

	/* Ping pong string declarations for output */

	char ping[] = "Ping";
	char pong[] = "Pong";		
	
	/* Initialise event timers */
  static struct etimer et1;
	static struct etimer et2;
	
	/* Declare communication token only once */
	static bool runOnce = false;
				if (runOnce == false) {
            token = random_num();
            runOnce = true;
        }
	
  PROCESS_BEGIN();
	
	/* Set callback function for when packets are received */
  nullnet_set_input_callback(input_callback);
	
	/* Set first timer to token generation interval */
  etimer_set(&et1, TOKEN_INTERVAL);
		
	/* While motes are in token discovery mode */
  while(discovery) {

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));

		/* Broadcast communication token */
		bc_token(token);
		
    etimer_reset(&et1);
  }
	
	/* When motes are out of discovery mode (ping-pong mode) */
	while(!discovery){	
		
		/* Sender mode */
		if(node_status == SENDINGNODE){

			/* On button press */
			PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_press_event);
			
			/* Define ping message to be broadcast */
			const char ping_message = 'p';
			
			/* Turn off LED and go into receive mode */	
			leds_off(LEDS_BLUE);
			node_status = RECEIVINGNODE;

			/* Broadcast ping message */
			bc_ping(ping_message);	

			/* Console output ping/pong */
			printf("%s\n", (pingpong % 2 == 0) ? ping : pong);
			pingpong += 2;
		} 
	
		/* Receiver mode */
		else {

				/* Empty event timer in order to loop back here frequently and not cause delay in ping transmission.
				One second delay period exists before user can send another ping */
				etimer_set(&et2, CLOCK_SECOND);
				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
				etimer_reset(&et2);
			}
	}
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
