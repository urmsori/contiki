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

int sclock_rtimer_set(struct rtimer* rt, struct timestamp *interval, rtimer_callback_t func, void *ptr, struct sclock *sc)
{
  struct timestamp duration_tmp;
  timestamp_cpy(&duration_tmp, interval);
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
    printf("tick: %u, %u\n",tick,RTIMER_ARCH_SECOND);
    rtimer_set(rt, RTIMER_NOW()+tick, 1,
	       (void (*)(struct rtimer *, void *))func, ptr);
    timestamp_init(interval);
    return 0;
  }

}
int stimer_hold_time(struct timestamp *duration)
{
  return 0;
}
