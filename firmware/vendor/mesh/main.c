/********************************************************************************************************
 * @file	main.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"

void RD_LOG(const char *format, ...);
int status_cua = 0;

extern void user_init();
extern void main_loop();
void blc_pm_select_none();

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
extern my_fifo_t hci_rx_fifo;

u16 uart_tx_irq = 0, uart_rx_irq = 0;

_attribute_ram_code_ void irq_uart_handle() {
	unsigned char irqS = reg_dma_rx_rdy0;
	if (irqS & FLD_DMA_CHN_UART_RX) //rx
	{
		uart_rx_irq++;
		reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_RX;
		u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num - 1))
				* hci_rx_fifo.size;
		if (w[0] != 0) {
			my_fifo_next(&hci_rx_fifo);
			u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num - 1))
					* hci_rx_fifo.size;
			reg_dma0_addr = (u16) ((u32) p);
		}
	}

	if (irqS & FLD_DMA_CHN_UART_TX) //tx
	{
		uart_tx_irq++;
		reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_TX;
	}
}
#endif

#if IRQ_TIMER1_ENABLE
_attribute_ram_code_ void irq_timer_handle()
{
	u32 src = reg_irq_src;
	static u32 A_debug_irq_cnt =0;
	if(src & FLD_IRQ_TMR1_EN) {
		A_debug_irq_cnt++;
		reg_tmr_sta = FLD_TMR_STA_TMR1;
		gpio_write(GPIO_PA1,A_debug_irq_cnt%2);
	}
}
#endif

#if	IRQ_GPIO_ENABLE
static u32 irq_gpio_user_cnt=0;
void gpio_irq_user_handle()
{
	irq_gpio_user_cnt++;
	return;
}

void gpio_risc0_user_handle()
{
	return;
}

void gpio_risc1_user_handle()
{
	return;
}

void gpio_risc2_user_handle()
{
	return;
}

void irq_gpio_handle()
{
	u32 src = reg_irq_src;
	if(src & FLD_IRQ_GPIO_EN) {
		gpio_irq_user_handle();
		reg_irq_src = FLD_IRQ_GPIO_EN; // clear irq_gpio irq flag
	}

	/************* gpio irq risc0 *************/
	if(src & FLD_IRQ_GPIO_RISC0_EN) {
		gpio_risc0_user_handle();
		reg_irq_src = FLD_IRQ_GPIO_RISC0_EN; // clear irq_gpio irq flag
	}

	/************* gpio irq risc1 *************/
	if(src & FLD_IRQ_GPIO_RISC1_EN) {
		gpio_risc1_user_handle();
		reg_irq_src = FLD_IRQ_GPIO_RISC1_EN; // clear irq_gpio irq flag
	}
#if (!(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278)))
	if(src & FLD_IRQ_GPIO_RISC2_EN) {
		gpio_risc2_user_handle();
		reg_irq_src = FLD_IRQ_GPIO_RISC2_EN;
	}
#endif
}
#endif

_attribute_ram_code_ void irq_handler(void) {
#if ACTIVE_SCAN_ENABLE
	rp_active_scan_req_proc();
#endif
#if DUAL_MESH_ZB_BL_EN
	if(rf_mode == RF_MODE_ZIGBEE) {
		irq_zigbee_sdk_handler();
	} else
#endif
	{
		irq_blt_sdk_handler(); //ble irq proc
	}

#if IRQ_TIMER1_ENABLE
	irq_timer_handle();
#endif

#if (HCI_ACCESS==HCI_USE_UART)
	irq_uart_handle();
#endif

#if	IRQ_GPIO_ENABLE
	irq_gpio_handle();
#endif
}
FLASH_ADDRESS_DEFINE
;
#if(MCU_CORE_TYPE == MCU_CORE_8269)
int main (void) {
	FLASH_ADDRESS_CONFIG;
	cpu_wakeup_init();

	clock_init();
	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

	gpio_init();

	rf_drv_init(CRYSTAL_TYPE);

	user_init ();

	irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
_attribute_ram_code_ int main(void) //must run in ramcode
{
	FLASH_ADDRESS_CONFIG;
#if (PINGPONG_OTA_DISABLE && (0 == FW_START_BY_LEGACY_BOOTLOADER_EN))
	ota_fw_check_over_write(); // must at first for main_
#endif

#if SLEEP_FUNCTION_DISABLE
	blc_pm_select_none();
#else
	blc_pm_select_internal_32k_crystal();
#endif
#if(MCU_CORE_TYPE == MCU_CORE_8258)
	cpu_wakeup_init();
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	cpu_wakeup_init(LDO_MODE,EXTERNAL_XTAL_24M);
#endif

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup(); //MCU deep retention wakeUp

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init(!deepRetWakeUp); //analog resistance will keep available in deepSleep mode, so no need initialize again

	clock_init(SYS_CLK_CRYSTAL);

#if	(PM_DEEPSLEEP_RETENTION_ENABLE)
	if( pm_is_MCU_deepRetentionWakeup() ) {
		user_init_deepRetn ();
	}
	else
#endif
	{
#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
		LOG_USER_MSG_INFO(0, 0, "[mesh] Start from SIG Mesh");
#else
		LOG_USER_MSG_INFO(0, 0, "Start user init...");
#endif
		user_init();

		uart_gpio_set(GPIO_PD7, GPIO_PA0);
		uart_reset();
		uart_init_baudrate(115200, CLOCK_SYS_CLOCK_HZ, PARITY_NONE, STOP_BIT_ONE);
		uart_dma_enable(1, 0);

		gpio_set_func(GPIO_PC0, AS_GPIO);
		gpio_set_input_en(GPIO_PC0, 1);
		gpio_setup_up_down_resistor(GPIO_PC0, PM_PIN_UP_DOWN_FLOAT);

		gpio_set_func(GPIO_PB5, AS_GPIO);
		gpio_set_input_en(GPIO_PB5, 1);
		gpio_setup_up_down_resistor(GPIO_PB5, PM_PIN_UP_DOWN_FLOAT);

	}

	irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop();

		status_cua = gpio_read(GPIO_PB5);
		RD_LOG("trang thai cua: %d\n", status_cua);
		sleep_ms(500);
		wd_clear();
	}
}
#endif

void RD_LOG(const char *format, ...) {
	char out[256];
	char *p = &out[0];
	va_list args;
	va_start(args, format);
	print(&p, format, args);
	uart_Csend(out);
}
