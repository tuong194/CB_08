################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../proj/app/usbaud.c \
../proj/app/usbcdc.c \
../proj/app/usbkb.c \
../proj/app/usbmouse.c 

OBJS += \
./proj/app/usbaud.o \
./proj/app/usbcdc.o \
./proj/app/usbkb.o \
./proj/app/usbmouse.o 


# Each subdirectory must supply rules for building sources it contributes
proj/app/%.o: ../proj/app/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"E:\TELINK\Telink_code\CB08\firmware" -I"E:\TELINK\Telink_code\CB08\firmware\proj\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\mi_api\mijia_ble_api" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\mi_api\libs" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\sdk_src\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\include" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\cfg" -I"E:\TELINK\Telink_code\CB08\firmware\vendor\common\llsync\data_template" -D__PROJECT_MESH__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

