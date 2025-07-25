#include "ring_buffer.h"

void RingBuffer_Init(struct ring_buffer* Rb_valRingBuffer)
{
	uint8_t i = 0;
	
	Rb_valRingBuffer->read = 0;
	Rb_valRingBuffer->write = 0;

	for(;i < RING_BUFFER_SIZE;i++)
	{
		Rb_valRingBuffer->uart_slots[i].rx_lenth = 0;
		memset(&Rb_valRingBuffer->uart_slots[i].rx_data[0], 0, sizeof(Rb_valRingBuffer->uart_slots[i].rx_data));
	}
}

static bool RingBuffer_IsEmpty(struct ring_buffer* Rb_valRingBuffer)
{
	return Rb_valRingBuffer->read == Rb_valRingBuffer->write;
}

static bool RingBuffer_IsFull(struct ring_buffer* Rb_valRingBuffer)
{
	return ((Rb_valRingBuffer->write + 1) % RING_BUFFER_SIZE) == Rb_valRingBuffer->read;
}

bool RingBuffer_Put(struct ring_buffer* Rb_valRingBuffer, uint8_t* Rb_valData, uint8_t Rb_valLength)
{
	bool Rb_valRet_Lo = false;
	
	if(RingBuffer_IsFull(Rb_valRingBuffer) || (Rb_valLength > UART_RECEIVE_LEN))
		return true;
	
	Rb_valRingBuffer->uart_slots[Rb_valRingBuffer->write].rx_lenth = Rb_valLength;
	
	memcpy(&Rb_valRingBuffer->uart_slots[Rb_valRingBuffer->write].rx_data[0], Rb_valData, Rb_valLength);
	
	Rb_valRingBuffer->write = ((Rb_valRingBuffer->write + 1) % RING_BUFFER_SIZE);
	
	return Rb_valRet_Lo;
}

bool RingBuffer_Get(struct ring_buffer* Rb_valRingBuffer, uint8_t* Rb_valData, uint8_t Rb_valLength)
{
	bool Rb_valRet_Lo = false;
	
	if(RingBuffer_IsEmpty(Rb_valRingBuffer) || (Rb_valLength > UART_RECEIVE_LEN))
		return true;
	
	memcpy(Rb_valData, &Rb_valRingBuffer->uart_slots[Rb_valRingBuffer->read].rx_data[0], Rb_valLength);	
	
	memset(&Rb_valRingBuffer->uart_slots[Rb_valRingBuffer->read].rx_data[0], 0, Rb_valLength);
	Rb_valRingBuffer->uart_slots[Rb_valRingBuffer->read].rx_lenth = 0;
	Rb_valRingBuffer->read = ((Rb_valRingBuffer->read + 1) % RING_BUFFER_SIZE);
	
	return Rb_valRet_Lo;
}
