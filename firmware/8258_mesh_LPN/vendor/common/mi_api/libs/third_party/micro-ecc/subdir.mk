################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/common/mi_api/libs/third_party/micro-ecc/uECC.c 

OBJS += \
./vendor/common/mi_api/libs/third_party/micro-ecc/uECC.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/common/mi_api/libs/third_party/micro-ecc/%.o: ../vendor/common/mi_api/libs/third_party/micro-ecc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\TELINK\Telink_code\CB08\firmware" -I"E:\TELINK\Telink_code\CB08\firmware\proj\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\mi_api\mijia_ble_api" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\mi_api\libs" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\sdk_src\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\cfg" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\data_template" -D__PROJECT_MESH_LPN__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


