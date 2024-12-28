
#include "Config_Pin.h"
#include "../common/blt_soft_timer.h"

static _Bool flag_wakeup_PAD = FALSE;

void RD_Config_Pin(void){
//	uart_gpio_set(GPIO_PD7, GPIO_PA0);
//	uart_reset();
//	uart_init_baudrate(115200, CLOCK_SYS_CLOCK_HZ, PARITY_NONE, STOP_BIT_ONE);
//	uart_dma_enable(1, 0);

//	RD_LOG("start\n");

	gpio_set_func(GPIO_PB7, AS_GPIO);
	gpio_set_output_en(GPIO_PB7, 1);
	gpio_write(GPIO_PB7, 0);

	gpio_set_func(SW1, AS_GPIO);
	gpio_set_input_en(SW1, 1);
	gpio_setup_up_down_resistor(SW1, PM_PIN_UP_DOWN_FLOAT);

	gpio_set_func(SW2, AS_GPIO);
	gpio_set_input_en(SW2, 1);
	gpio_setup_up_down_resistor(SW2, PM_PIN_PULLUP_1M);

	bls_pm_setWakeupSource(PM_WAKEUP_PAD);
    gpio_set_wakeup (SW1, Level_Low, 1);         // nhan nut de wakeup
    cpu_set_gpio_wakeup (SW1, Level_Low, 1);
    gpio_set_wakeup (SW2, Level_Low, 1);         // nhan nut de wakeup
    cpu_set_gpio_wakeup (SW2, Level_Low, 1);

    gpio_core_wakeup_enable_all(0);



}


int rd_test_soft_timer(void){

	static uint8_t tesst = 0;


		//bls_pm_setSuspendMask (SUSPEND_DISABLE); // not enter sleep to on LED
		gpio_set_func(GPIO_PB7, AS_GPIO);
		gpio_set_output_en(GPIO_PB7, 1);
		tesst = !tesst;
		gpio_write(GPIO_PB7, tesst);

    	//bls_pm_setWakeupSource(PM_WAKEUP_PAD);
		//cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time()+5000*1000*sys_tick_per_us);
		//cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
	return 1;
}

void RD_Wakeup_Source(void){
	//cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time()+500*1000*sys_tick_per_us);
	blt_soft_timer_add(&rd_test_soft_timer, 200* 1000);
}

u32 mesh_lpn_wakeup_key_io_get(void)
{
	#define WAKEUP_TRIGGER_LEVEL	0
	if(WAKEUP_TRIGGER_LEVEL == gpio_read (SW1)){
		return SW1;
	}else if(WAKEUP_TRIGGER_LEVEL == gpio_read (SW2)){
		return SW2;
	}
	return 0;
}

void lpn_factory_reset(void){
    u32 detkey = 0;
    //int key_press = 0; // KEY_PRESSED_IDLE
    static u32 long_tick;
    static u32 time_tick = 0;

	gpio_set_func(SW1, AS_GPIO);
	gpio_set_input_en(SW1, 1);
	gpio_setup_up_down_resistor(SW1, PM_PIN_UP_DOWN_FLOAT);

	if(!flag_wakeup_PAD){
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);
		gpio_set_wakeup (SW1, Level_Low, 1);         // nhan nut de wakeup
		cpu_set_gpio_wakeup (SW1, Level_Low, 1);
	}else{
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);
		gpio_set_wakeup (SW1, Level_High, 1);         // nha nut de wakeup
		cpu_set_gpio_wakeup (SW1, Level_High, 1);
	}

        detkey = mesh_lpn_wakeup_key_io_get();
        if(SW1 == detkey){
            if(clock_time_exceed(long_tick, 10*1000)){
            	if(flag_wakeup_PAD == FALSE){
                    time_tick++;
                    long_tick = clock_time();
                	if(time_tick>=100){
                		time_tick = 0;
                		flag_wakeup_PAD = TRUE;
                	}

            	}
            }

        }else{
        	 if(clock_time_exceed(long_tick, 10*1000)){
             	if(flag_wakeup_PAD == TRUE){
                     time_tick++;
                     long_tick = clock_time();
                 	if(time_tick>=100){
                 		time_tick = 0;
                 		flag_wakeup_PAD = FALSE;
                 	}
             	}
        	 }
        }

        if(time_tick == 50 && flag_wakeup_PAD == FALSE){
        	//time_tick = 0;
        	u8 par[2] = {0x01, 0x02};
        	mesh_tx_cmd2normal_primary(0xE1, par, 2, 0x0001, 1);

        }
}

int RD_Messenger_ProcessCommingProcess(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par){

	mesh_tx_cmd2normal_primary(0xE1, par, 1, 0x1b3d, 1);
	return 0;
}


void RD_LOG(const char *format, ...) {
	static char out[256];
	static char *p = &out[0];
	va_list args;
	va_start(args, format);
	print(&p, format, args);
	uart_Csend(out);
}
