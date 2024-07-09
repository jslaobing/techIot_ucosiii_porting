#include <stdio.h>
#include "gd32f30x_gpio.h"
#include "os.h"




#include <stdio.h>

#include "bsp.h"
#include "os.h"
#include "os_cpu.h"

#define APP_TASK_START_PRIO (OS_CFG_PRIO_MAX - 3)
#define APP_TASK_START_STK_SIZE 2048
static OS_TCB AppTaskStartTCB;
static CPU_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];

void led_init() {
    rcu_periph_clock_enable(RCU_GPIOE);                             // enable group clock
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, BIT(1));  // push-pull output driver
    gpio_bit_reset(GPIOE, BIT(1));
}

void app_startup(void *arg) {
    OS_ERR err;

    CPU_Init();

    OS_CPU_SysTickInitFreq(BSP_CPU_ClkFreq());

    led_init();

    while (1) {
        static uint8_t running_led;
        if (running_led) {
            running_led = 0;
            gpio_bit_set(GPIOE, BIT(1));
        } else {
            running_led = 1;
            gpio_bit_reset(GPIOE, BIT(1));
        }
        OSTimeDly(500, OS_OPT_TIME_DLY, &err);
    }
}

// os entry
int main(void) {
    OS_ERR err;

    // os init
    OSInit(&err);

    // create start task
    OSTaskCreate((OS_TCB *)&AppTaskStartTCB,
                 (CPU_CHAR *)"app_startup",
                 (OS_TASK_PTR)app_startup,
                 (void *)&AppTaskStartTCB,
                 (OS_PRIO)APP_TASK_START_PRIO,
                 (CPU_STK *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY)0,
                 (OS_TICK)0,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    if (err != OS_ERR_NONE) {
        return -1;
    }

    // os start
    OSStart(&err);
    if (err != OS_ERR_NONE) {
        return -1;
    }

    return -1;
}
