#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define UART_RECEIVE_LEN			9
#define RING_BUFFER_SIZE			8

struct uart_device {
	uint8_t rx_lenth;
	uint8_t rx_data[UART_RECEIVE_LEN];
};

struct ring_buffer {
	struct uart_device uart_slots[RING_BUFFER_SIZE];
    volatile uint8_t read;
    volatile uint8_t write;
};

extern void RingBuffer_Init(struct ring_buffer* Rb_valRingBuffer);
extern bool RingBuffer_Put(struct ring_buffer* Rb_valRingBuffer, uint8_t* Rb_valData, uint8_t Rb_valLength);
extern bool RingBuffer_Get(struct ring_buffer* Rb_valRingBuffer, uint8_t* Rb_valData, uint8_t Rb_valLength);

#endif /* RING_BUFFER_H_ */
