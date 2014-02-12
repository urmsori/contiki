#include "sclock.h"
#include "contiki.h"

void
sclock_init(struct sclock *sc)
{
  timestamp_init(&sc->now);
  sc->slope = SCLOCK_SLOPE_INIT;
  sc->last_tick = RTIMER_NOW();
}

void
sclock_update(struct sclock *sc)
{
  rtimer_clock_t now_tick = RTIMER_NOW();
  rtimer_clock_t interval;
  if(now_tick <= (sc->last_tick)){
    interval = now_tick-(sc->last_tick);
  }
  else{
    interval = (RTIMER_CLOCK_T_MAX - sc->last_tick) + now_tick;
  }

  rtimer_clock_t remain = interval%(sc->slope);
  sc->last_tick = now_tick-remain;

  struct timestamp ts_calc;
  timestamp_init(&ts_calc);
  ts_calc.msec = (interval/(sc->slope))*SCLOCK_UNIT_MSEC;
  timestamp_plus(&(sc->now),&ts_calc);
}

void
sclock_now(struct sclock *sc, struct timestamp *ts)
{
  sclock_update(sc);
  timestamp_cpy(&(sc->now), ts);
}

int sclock_rtimer_set_long(struct sclock *sc, struct rtimer* rt, struct timestamp *interval, rtimer_callback_t func, void *ptr)
{
  struct timestamp amin;
  rtimer_clock_t tick;
  timestamp_init(&amin);
  amin.min = 1;

  if( timestamp_cmp(&amin, interval) <= 0 ){  /* interval >= 1min */
    tick = ((MSEC_MAX+1)*(SEC_MAX+1))*(sc->slope)/SCLOCK_UNIT_MSEC;
    timestamp_minus(interval,&amin);

    rtimer_set(rt, RTIMER_NOW()+tick, 1,
	       (void (*)(struct rtimer *, void *))func, ptr);
    return 1;
  }
  else{
    tick = (interval->msec+(interval->sec*(MSEC_MAX+1)))*(sc->slope)/SCLOCK_UNIT_MSEC;
    rtimer_set(rt, RTIMER_NOW()+tick, 1,
	       (void (*)(struct rtimer *, void *))func, ptr);
    timestamp_init(interval);
    return 0;
  }

}
void sclock_rtimer_set(struct sclock *sc, struct rtimer* rt, struct timestamp *interval, rtimer_callback_t func, void *ptr)
{
  rtimer_clock_t tick;
  tick = (interval->msec+(interval->sec*(MSEC_MAX+1)))*(sc->slope)/SCLOCK_UNIT_MSEC;
  rtimer_set(rt, RTIMER_NOW()+tick, 1,
	     (void (*)(struct rtimer *, void *))func, ptr);
}

void sclock_hold_time(struct sclock *sc, struct timestamp *interval)
{
  rtimer_clock_t tick;
  rtimer_clock_t now = RTIMER_NOW();
  tick = (interval->msec+(interval->sec*(MSEC_MAX+1)))*(sc->slope)/SCLOCK_UNIT_MSEC;
  
  while(RTIMER_CLOCK_LT(RTIMER_NOW(), now+tick)){}
}
