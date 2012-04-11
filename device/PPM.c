#include "PPM.h"
#include "Debug.h"
#include <avr/io.h>

volatile int8_t rc_ch[RC_MAX_CHANNEL];
uint8_t rc_channel_counter;
unsigned int rc_channel_sum;

void PPM_Init()
{
    //Port output
    DDRE = (1<<PPM_PIN);

    //Enable timer interrupt
    TIMSK1 |= (1<<TOIE1);

    //Prescaler 8
    TCCR1B |= (1<<CS11);
    TCNT1 = TIMER_VALUE_MAX - RC_REPETITION_TIME;

    //Set all rc channels to neutral value
    for(int i = 0; i < RC_MAX_CHANNEL; i++)
      rc_ch[i] = 0;
}

ISR(TIMER1_OVF_vect)
{
  unsigned int tmp;
  PPM_PORT ^= (1<<PPM_PIN);
  
  if (!(PPM_PORT & (1<<PPM_PIN)))
  {
    TCNT1 = TIMER_VALUE_MAX - (MS0_5);
  }
  else
  {
    if (rc_channel_counter == RC_MAX_CHANNEL)
    {
      TCNT1 = TIMER_VALUE_MAX - rc_channel_sum;
      rc_channel_sum = RC_REPETITION_TIME - ((RC_MAX_CHANNEL+1) * MS0_5);
      rc_channel_counter = 0;
    }
    else
    {
      if (rc_ch[rc_channel_counter] > RC_MAX_VALUE)
      {
        rc_ch[rc_channel_counter] = RC_MAX_VALUE;
      }
      if (rc_ch[rc_channel_counter] < RC_MIN_VALUE)
      {
        rc_ch[rc_channel_counter] = RC_MIN_VALUE;
      }
      TCNT1 = TIMER_VALUE_MAX - (RC_OFFSET_TIME + (5 * rc_ch[rc_channel_counter]));
      tmp = (RC_OFFSET_TIME + (5 * rc_ch[rc_channel_counter]));
      rc_channel_sum -= tmp;
      rc_channel_counter++;
    }
  }
}
