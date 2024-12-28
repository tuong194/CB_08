/*
 * Config_Pin.h
 *
 *  Created on: Dec 6, 2024
 *      Author: PC5
 */

#ifndef CONFIG_PIN_H_
#define CONFIG_PIN_H_

#include "tl_common.h"
#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/mesh_lpn.h"

#define SW1   GPIO_PC0  //(SDA tren mach)
#define SW2   GPIO_PB4  // SW2

#define RD_OPCODE_TYPE_SEND				(0xE0)
#define RD_OPCODE_TYPE_RSP				(0xE1)
#define RD_OPCODE_FAST_CREATE_ROOM		(0xE2)
#define RD_OPCODE_RSP_FOR_E2			(0xE3)


int RD_Messenger_ProcessCommingProcess(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
void RD_LOG(const char *format, ...);
void RD_Config_Pin(void);
u32 mesh_lpn_wakeup_key_io_get(void);
void lpn_factory_reset(void);

int rd_test_soft_timer(void);
void RD_Wakeup_Source(void);

#endif /* CONFIG_PIN_H_ */
