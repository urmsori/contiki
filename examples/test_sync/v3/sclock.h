#ifndef SCLOCK_H
#define SCLOCK_H

#include "timestamp.h"
#include "contiki.h"
#include "sys/rtimer.h"
#include "sys/pt.h"

/* #define SLOPE_INIT CLOCK_SECOND */
#define SCLOCK_UNIT_MSEC 10
#define SCLOCK_SLOPE_INIT ((RTIMER_ARCH_SECOND/1000)*SCLOCK_UNIT_MSEC)
#define stimer_callback_t rtimer_callback_t
#define RTIMER_CLOCK_T_MAX 65535

/* sclock */
struct sclock{
  struct timestamp now;
  rtimer_clock_t slope; /* ticks per unit time */
  rtimer_clock_t last_tick;
};
void sclock_update(struct sclock *sc);
void sclock_init(struct sclock *sc);
void sclock_now(struct sclock *sc, struct timestamp *ts);
void sclock_rtimer_set(struct sclock *sc, struct rtimer* rt, struct timestamp *interval, rtimer_callback_t func, void *ptr);
int sclock_rtimer_set_long(struct sclock *sc, struct rtimer* rt, struct timestamp *interval, rtimer_callback_t func, void *ptr);
void sclock_hold_time(struct sclock *sc, struct timestamp *interval);
/* sclock_rtimer_set README
 * 
 * interval < 1min : sclock_rtimer_set, stimer_hold_time : interval not changed
 * interval > 1min : sclock_rtimer_set_long              : interval changed
 * 
 * < Example >
 * PT_BEGIN(&pt);
 * while(1){
 *  while(timestamp_is_empty(&interval)){
 *   if( sclock_rtimer_set_long(sc,rt,interval,(void (*)(struct rtimer *, void *))func,ptr) > 0 ){
 *   PT_YIELD(&pt);
 *  }
 * }
 * PT_END(&pt);
 */
#endif /* SCLOCK_H */
