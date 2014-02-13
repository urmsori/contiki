#include "contiki.h"
//#include "dev/leds.h"
#include "ntp.h"
#include "sclock.h"
#include "net/rime.h"
#include "sys/rtimer.h"
#include "sys/pt.h"

#include <stdio.h>
#include <sys/clock.h>

#define HOST 0

static struct sclock sclk;
static struct rtimer rt;
static struct pt pt;
static struct ntp ntp;
//void print_leds();
/*---------------------------------------------------------------------------*/

char loop(struct rtimer* t, void *ptr){
  PT_BEGIN(&pt);
  char packet[TIMESTAMP_BIT];
  int packet_len = 0;

  sclock_init(&sclk);
  ntp_init(&ntp, &sclk);
  rtimer_init();

  static struct timestamp second;
  timestamp_init(&second);
  second.sec = 1;

  while(1) {
    printf("jjj\n");
    sclock_rtimer_set(&sclk, t, &second, (void (*)(struct rtimer *, void *))loop, NULL);
    PT_YIELD(&pt);
    printf("ddd\n");
    
    // leds_invert(LEDS_RED);

#if HOST
    packet_len = ntp_make_request(&ntp, &packet); 
    ntp_send_packet(&packet, packet_len);
#endif
  }

  PT_END(&pt);
}

/*---------------------------------------------------------------------------*/
static void ntp_recv_packet(struct abc_conn *c)
{
  char* recv_str = (char *)packetbuf_dataptr();
  //  ntp_handle(recv_str, ntp);
}
static struct abc_conn abc;
static const struct abc_callbacks abc_call = {ntp_recv_packet};
/*---------------------------------------------------------------------------*/

PROCESS(sync_main, "Sync main");
AUTOSTART_PROCESSES(&sync_main);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sync_main, ev, data)
{
  /* static struct timer tm; */

  PROCESS_EXITHANDLER(abc_close(&abc);)
  PROCESS_BEGIN();
  abc.ptr = (void*) &sclk;
  abc_open(&abc, 128, &abc_call);
  loop(&rt, NULL);
  PT_INIT(&pt);
  
  /*  timer_set(&tm, CLOCK_SECOND);
  while(1) {
    if(timer_expired(&tm)){
      leds_invert(LEDS_RED);
      
      packet = ntp_make_request(&sclk, &packet_len);
      send_packet(packet, packet_len);
      free(packet);
      timer_set(&tm, CLOCK_SECOND);
    }
    }*/
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*void
print_leds(){
  unsigned char ledst = leds_get();
  int green, yellow, red;

  green = ledst&LEDS_GREEN;
  yellow = (ledst&LEDS_YELLOW)>>1;
  red = (ledst&LEDS_RED)>>2;
  
  printf("ledfd:%u%u%u\n", yellow, green, red);
  }*/


