#include "contiki.h"
#include "sclock.h"
#include "string.h"
#include "sys/rtimer.h"
#include "sys/pt.h"

#define TEST1 0
#define TEST2 0
#define TEST3 0
#define TEST4 1

static void print_packet(uint8_t *packet, int len)
{
  int i,j;
  uint8_t num, num2;
  uint8_t jisu;

  for(i=0; i<len; i++){
    num = packet[i];
    for(j=7; j>-1; j--){
      jisu = 1 << j;
      num2 = num&jisu;
      num2 = num2 >> j;
      if(num&jisu){
	printf("1");
      }
      else{
	printf("0");
      }
    } 
    printf(" ");
  }
  printf("\n");
}

void hold_time(rtimer_clock_t interval)
{
  rtimer_clock_t rct;
  rct = RTIMER_NOW();
  while(RTIMER_CLOCK_LT(RTIMER_NOW(), rct+interval)){
  }
}

static struct pt pt;
char pt_func(struct rtimer* t, void *ptr){
  static struct sclock sc;
  static struct timestamp duration;

  PT_BEGIN(&pt);
  sclock_init(&sc);
  timestamp_init(&duration);
  printf("ddd\n");
  while(1){
    duration.min = 1;
    duration.sec = 1;
    while(!timestamp_is_empty(&duration)){
      sclock_update(&sc);
      printf("----\n");
      print_timestamp(&(sc.now));
      print_timestamp(&duration);
      
      sclock_rtimer_set(t,&duration,(void (*)(struct rtimer *, void *))pt_func,ptr,&sc);
      PT_YIELD(&pt);
      printf("done\n");
    }
  }
  PT_END(&pt);

}

/*---------------------------------------------------------------------------*/
PROCESS(ts_test, "Timestamp test");
AUTOSTART_PROCESSES(&ts_test);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ts_test, ev, data)
{
  PROCESS_BEGIN();
  PT_INIT(&pt);

#if TEST4
  static struct rtimer rtt;
  rtimer_init();
  pt_func(&rtt, NULL);

#endif

#if TEST3
  static struct sclock sc;
  sclock_init(&sc);
  while(1){
    sclock_update(&sc);
    hold_time(RTIMER_ARCH_SECOND);
  }

#endif
  
#if TEST2
  struct timestamp ts;
  struct timestamp ts2;
  timestamp_init(&ts);
  ts.year = 12;
  ts.month = 12;
  ts.day = 31;
  ts.hour = 23;
  ts.min = 59;
  ts.sec = 59;
  ts.msec = 999;
  
  uint8_t packet[10];
  memset(packet,0,10);
  
  timestamp_to_packet(&ts,packet,0);
  timestamp_from_packet(&ts2,packet,0);
  print_timestamp(&ts2);
#endif

#if TEST1
  struct timestamp ts;
  struct timestamp ts2;
  timestamp_init(&ts);


  while(ts.year <= 1){
    timestamp_init(&ts2);
    ts2.min = 40;
    timestamp_plus(&ts,&ts2);
    print_timestamp(&ts);
  }
   
  int i=0;
  while(i<10000){
    i++;
    timestamp_init(&ts2);
    ts2.msec = 40;
    
    if( timestamp_minus(&ts,&ts2) < 0)
      break;
    print_timestamp(&ts);
    }
#endif



  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
