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
 *         NullNet broadcast example
 * \author
*         Simon Duquennoy <simon.duquennoy@ri.se>
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

/* 3 second window to program nodes */
#define TOKEN_INTERVAL (CLOCK_SECOND * 4)

/* Send node transmits, waiting node receives. */
#define SENDINGNODE 1
#define RECEIVINGNODE 0

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

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

static int node_status = RECEIVINGNODE;
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
/* Function which processes generated token and recieved token smaller
	 token starts communication*/
void process_tokens(int token, int token_rec){
		if (token < token_rec) {
			leds_on(LEDS_BLUE);
			bc_token(token_rec);
			node_status = SENDINGNODE;
			printf("I am pinging\n");
			discovery = 0;				
		} if (token == token_rec) {
			printf("I HAVE DUPE TOKEN I MUST STOP SENDING\n");
			discovery = 0;		
		}
}
/*---------------------------------------------------------------------------*/
/* Function to be called upon packet reception */
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(int)) {
		/* Token recieved */
    int token_rec;
    memcpy(&token_rec, data, sizeof(token_rec));
		/* Debug Messages */
    LOG_INFO("Received %u from ", token_rec);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
		
		/* Process token and recieved token */	
		process_tokens(token,token_rec);
  }

	if(len == sizeof(char)) {
		//
	}
}
/*---------------------------------------------------------------------------*/
/* Function to broadcast token over nullnet */
void bc_token(int token) {
	nullnet_buf = (uint8_t *)&token;
	nullnet_set_input_callback(input_callback);
	LOG_INFO("Sending %d to ", token);
  LOG_INFO_LLADDR(NULL);
  LOG_INFO_("\n");
	memcpy(nullnet_buf, &token, sizeof(token));
	nullnet_len = sizeof(token);
	NETSTACK_NETWORK.output(NULL);
}
/*---------------------------------------------------------------------------*/
/* Function to unicast "Ping" packet over nullnet
void uc_ping() {
	nullnet_buf = 'b';
	nullnet_set_input_callback(input_callback);
	LOG_INFO("Ping");
  LOG_INFO_LLADDR(NULL);
  LOG_INFO_("\n");
	memcpy(nullnet_buf, &token, sizeof(token));
	nullnet_len = sizeof(token);
	NETSTACK_NETWORK.output(NULL);
} */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  static struct etimer periodic_timer;
	
	/* Declare communication token only once*/
	static bool runOnce = false;
				if (runOnce == false) {
            token = random_num();
            runOnce = true;
        }
	
  PROCESS_BEGIN();
	
	
#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */
	
  nullnet_set_input_callback(input_callback);

  etimer_set(&periodic_timer, TOKEN_INTERVAL);
		

  while(discovery) {

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
		printf("token is %d\n",token);

		/* Broadcast communication token */
		bc_token(token);
		
    etimer_reset(&periodic_timer);
		printf("discovery is %d\n",discovery);

  }
	
	
	while(!discovery){

	
		if(node_status == SENDINGNODE){
			printf("I am sending");
			PROCESS_YIELD();
			if(ev == button_hal_press_event){
				PROCESS_WAIT_EVENT_UNTIL(ev == button_hal_release_event);
				
				const char ping = 'p';
				printf("payload is %c",ping);
				//uc_ping();	
			}
		}else {
				printf("I am receiving");
				PROCESS_YIELD();
			}

	}

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
