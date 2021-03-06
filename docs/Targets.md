

# Processor and Flight Controller Targets

This codebase currently supports the STM32F411 and STM32405 processors. The `Makefile` 
will define `STM32F405xx` or `STM32F411xE` as appropriate based on the *Flight Controller Target*.

There are a few places in the code that are conditionally compiled based on those definitions.

The following flight controller targets are currently defined:

* `OMNIBUSF4` 	- This uses the STM32F405 processor which runs at 168Mhz
* `NOX`         - This uses the STM32F411 processor which runs at 98Mhz or 100Mhz (we must use 98Mhz for proper USB Virtual Com Port support)

> Note: A *target* is simply the name of a hardware configuration (processor and peripherals) that
was defined for use with Betaflight. Different flight controller boards can be made that
correspond to a given target. 

The `Makefile` defaults to building for the `NOX` target but this can be overriden when invoking
the makefile by specifying the `TARGET` on the command line like so:

```
mingw32-make.exe -j12 flash TARGET=OMNIBUSF4
```

The above command will build the `OMNIBUSF4` target and flash the build onto the flight controller board.
For more details on how to build and flash (and develop) this software read the [Develop](Develop.md) page

> Note: I'm still in the early stages of adding support for the F3 processor and will update this
document when it becomes available. The first F3 based flight controller target I plan to support
will be the OMNIBUS board and then the CRAZYBEEF3 (FlySky) board.


# NOX 

I've been using the "Play F4" flight controller (sometimes described as "JMT Play F4" or "JHEMC Play F4").
This board is a NOX target. This is what it looks like:

![Play F4 Top](images/Play-F4-Top.jpg)
![Play F4 Bottom](images/Play-F4-Bot.jpg)


By examining the Betaflight source code and `target.h` files for any
Betaflight target (as well as using the `resource` and `resource list` commands) we can learn
how the STM32 processor interfaces with the MPU, OSD and other peripherals.

For example, the following pads of the Play F4 board map to the STM32 pins:
 
 On top side of board are:

* 3.3v
* DSM/IBUS/PPM  - Goes directly to PB10 (verified with multimeter)
* SBUS      - Coupled thru a switchable inverter (controlled by PC14) and then (I think) to PA3 (USART2 RX)
* 5v
* GND
* TX1       - Goes directly to PB6 (USART1 TX)

On back (bottom) side of board are:

* RX1       - Goes directly to PB7 (verified with multimeter) (USART1 RX)
* TX2       - Goes directly to PA2 (verified with multimeter) (USART2 TX)
* LED_STRIP - Goes directly to PA0 (verified with multimeter)
* BZ-       - Does not seem to be directly tied to STM32, probably uses a driver transistor to PC13

This information is important if you want to change which pads to use to interface
to your transceiver board (NRF24L01, XN297, XN297L or LT8900). More information can be found
in the [Configuration](Configuration.md) section.


# Adding support for new targets

This section will contain my notes on what is required to add support for new targets.

The project folder structure contains a `Targets` folder that in turn contains subfolders for each of the
targets (such as `NOX`, `OMNIBUSF4`). In turn these folders contain two subfolders containing source code
for the processors and peripherals associated with the target boards, and also a `.ioc` file

* Core - STM32CubeMX generated source files
* Drivers - Various STM32 source files
* `????.ioc` - This is an STM32CubeMX project file used to configure the generated source code

I want to add support for the Mobula6 flight controller which is the "HappyModel Crazybee F4 Lite 1S" board.
In Betaflight it is known as the `MATEKF411RX` target. So I would create a new folder with that name within
our `Targets` folder. 

Using STM32CubeMX you'll want to configure the various pins and perhipherals of the STM32 chip.


Required by Silverware:

* SPI interface for the MPU (4-wire soft spi implementation)
    * SPI_MPU_SS - PA4 (GPIO output)
    * SPI2_CLK  - PA5 (GPIO output)
    * SPI2_MISO - PA6 (GPIO input)
    * SPI2_MOSI - PA7 (GPIO output)
* SPI interface for the OSD
    * This is actually configured by editing `drv_sd_spi.config.h` rather than via Stm32CubeMX
* SWD pins (SWDIO, SWCLK) - If available
    * SWD is not available on `MATEKF411RX`, in fact PA14 and/or PA13 are used for other purposes
* ESC1 - PB10 (GPIO output)
* ESC2 - PB6 (GPIO output)
* ESC3 - PB7 (GPIO output)
* ESC4 - PB8 (GPIO output)

* VOLTAGE_DIVIDER - PB0 (ADC input)
* LED - PC13 (GPIO output)


Additional/Available on this FC board
* CURRENT_METER_ADC_PIN - PB1 (ADC)


RX SPI (FlySky A7105)
* SPI3_SCK_PIN  - PB3 (GPIO output)
* SPI3_MISO_PIN - PB4 (GPIO input)
* SPI3_MOSI_PIN - PB5 (GPIO output)
* RX_NSS_PIN    - PA15 (GPIO output)
* RX_SPI_EXTI_PIN - PA14 (GPIO External interrupt mode)
    * Examining `A7105Init()` in Betaflight source code reveals it is configured for `EXTI_TRIGGER_RISING`
* RX_SPI_LED_PIN - PB9 (GPIO output)
* RX_SPI_BIND_PIN - PB2 (GPIO input), Should we enable pull up or pull down?


Note: For ESC pinouts look inside Betaflight for the corresponding `target.c` and you'll see a table like this:
```
const timerHardware_t timerHardware[USABLE_TIMER_CHANNEL_COUNT] = {
    DEF_TIM(TIM9, CH2, PA3,  TIM_USE_PPM,   0, 0), // PPM/RX2

    DEF_TIM(TIM2, CH3, PB10, TIM_USE_MOTOR, 0, 0), // S1_OUT - DMA1_ST1
    DEF_TIM(TIM4, CH1, PB6,  TIM_USE_MOTOR, 0, 0), // S2_OUT - DMA1_ST0
    DEF_TIM(TIM4, CH2, PB7,  TIM_USE_MOTOR, 0, 0), // S3_OUT - DMA1_ST3
    DEF_TIM(TIM4, CH3, PB8,  TIM_USE_MOTOR, 0, 0), // S4_OUT - DMA1_ST7

    DEF_TIM(TIM5, CH1, PA0,  TIM_USE_LED,   0, 0), // 2812LED - DMA1_ST2

    DEF_TIM(TIM9, CH1, PA2,  TIM_USE_PWM,   0, 0 ), // TX2
    DEF_TIM(TIM1, CH2, PA9,  TIM_USE_PWM,   0, 0 ), // TX1
    DEF_TIM(TIM1, CH3, PA10, TIM_USE_PWM,   0, 0 ), // RX1
};
```

This describes the timer peripherals used on the STM32 and you'll notice that
4 entries are tagged with `TIM_USE_MOTOR`; these are the ESC pins in order
from ESC1 thru ESC4.


## STM32 resources
SystemClock
ADC1                - Battery voltage
TIM1
    NOX
        TIM1_UP:    DMA2, Stream 5, NVIC global interrupt enabled
        TIM1_CH:    DMA2, Stream 1, NVIC global interrupt enabled
        TIM1_CH2:   DMA2, Stream 2, NVIC global interrupt enabled
    OMNIBUSF4
        TIM1_UP:    DMA2, Stream 5, NVIC global interrupt enabled
        TIM1_CH:    DMA2, Stream 1, NVIC global interrupt enabled
        TIM1_CH2:   DMA2, Stream 2, NVIC global interrupt enabled

TIM2 for gettime()

Blackbox
    NOX:        USART2, 2MB, 8N1  
        USART2_TX: DMA1, Stream 6
    OMNIBUSF4:  UART4, 2MB, 8N1
        UART4_TX: DMA1, Stream 4

DSHOT (drv_dshot_bdir and drv_dshot_dma) uses TIM1, DMA2