#include "defines.h"
#include "drv_spi.h"
#include "hardware.h"
#include "main.h"

#ifdef SOFTSPI_4WIRE

#define MOSIHIGH gpioset(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin);
#define MOSILOW gpioreset(SPI_MOSI_GPIO_Port, SPI_MOSI_Pin);
#define SCKHIGH gpioset(SPI_CLK_GPIO_Port, SPI_CLK_Pin);
#define SCKLOW gpioreset(SPI_CLK_GPIO_Port, SPI_CLK_Pin);

#define READMISO (SPI_MISO_GPIO_Port->IDR & SPI_MISO_Pin)
// #define DELAY_O3 _NOP_ _NOP_ // necessary when the loop is unrolled i.e. with -O3
#define DELAY_O3

void spi_sendbyte( int data )
{
	for ( int i =7; i >= 0; --i ) {
		SCKLOW
		if ( ( data >> i ) & 1 ) {
			MOSIHIGH
		} else {
			MOSILOW
		}
		SCKHIGH
		DELAY_O3
	}
	SCKLOW
	MOSILOW
}

int spi_sendzerorecvbyte()
{
	int recv = 0;
	MOSILOW
	for ( int i = 7; i >= 0; --i ) {
		recv = recv << 1;
		SCKHIGH
		DELAY_O3
		if ( READMISO ) {
			recv = recv | 1;
		}
		SCKLOW
		DELAY_O3
	}
	return recv;
}

// int spi_sendrecvbyte( int data )
// {
// 	int recv = 0;
// 	for ( int i = 7; i >= 0; --i ) {
// 		recv = recv << 1;
// 		if ( data & ( 1 << 7 ) ) {
// 			MOSIHIGH
// 		} else {
// 			MOSILOW
// 		}
// 		data = data << 1;
// 		SCKHIGH
// 		if ( READMISO ) {
// 			recv = recv | 1;
// 		}
// 		SCKLOW
// 	}
// 	return recv;
// }

// int spi_sendrecvbyte2( int data )
// {
// 	int recv = 0;
// 	for ( int i = 7; i >= 0; --i ) {
// 		if ( data & (1 << 7 ) ) {
// 			MOSIHIGH
// 		} else {
// 			MOSILOW
// 		}
// 		SCKHIGH
// 		data = data << 1;
// 		if ( READMISO ) {
// 			recv = recv | ( 1 << 7 );
// 		}
// 		recv = recv << 1;
// 		SCKLOW
// 	}
// 	recv = recv >> 8;
// 	return recv;
// }

#endif
