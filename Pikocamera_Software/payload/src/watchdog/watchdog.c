// 2022-10-28

#include "stm32f10x_iwdg.h"
#include "watchdog.h"

/*
  ~~ Watchdog timeout calculations ~~

  t_IWDG = 4 * t_CLK * 2^PR * (RL + 1), s
  RL = -1 + t_IWDG / (4 * t_CLK * 2^PR), s (0-4095)

  where,
    PR = Prescaler value
    RL = Reload value
    t_IWDG = IWDG timeout
    t_CLK = 1/32000 s
*/
void watchdog_init(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(0x03);
  IWDG_SetReload(200);
  IWDG_Enable();
}

void watchdog_reset(void)
{
  IWDG_ReloadCounter();
}
