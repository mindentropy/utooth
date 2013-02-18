#include "cq.h"
#include "hal_usb.h"
#include <string.h>
#include <msp430.h>

uint16_t tmptxcnt = 0;
uint16_t tmprxcnt = 0;

extern struct cq rx_q;
extern struct cq tx_q;


void cq_init(struct cq * const cq) {
	cq->start = cq->end = 0;
	cq->curfreesize = MODSIZE;
}

void cq_reset(struct cq * const cq) {
	cq->start = cq->end = 0;
	cq->curfreesize = MODSIZE;
}

uint8_t cq_is_empty(struct cq * const cq) {

	if(cq->curfreesize == MODSIZE) {
		return 1;
	} else {
		return 0;
	}
}


uint8_t cq_is_full(struct cq * const cq) {
	if(cq->curfreesize == 0) {
		return 1;
	} else {
		return 0;
	}
}

void cq_add(struct cq * const cq,uint8_t ch) {
	cq->buff[cq->end] =  ch;
	(cq->end)++;

	if(((cq->end)) == BUFFSIZE) {
		cq->end = 0;
	}
	
	(cq->curfreesize)--;

	if(cq == &tx_q)
		tmptxcnt++;
}

uint8_t cq_del(struct cq * const cq) {
	uint8_t ch;

	ch = cq->buff[cq->start];
	(cq->start)++;

	if(((cq->start)) == BUFFSIZE)
		(cq->start) = 0;

	(cq->curfreesize)++;

	if(cq == &tx_q)
		tmprxcnt++;

	return ch;
}

/* 
 * Note : No checks done for bounds. Responsibility lies with the
 * user of the function 
 */
void cq_discard(struct cq * const cq,uint16_t size) {
	if(((cq->start) + size) >= BUFFSIZE) 
		(cq->start) = size - (BUFFSIZE - (cq->start));
	 else 
		(cq->start) += size;

	(cq->curfreesize) += size;
}

uint8_t cq_peek(struct cq * const cq,uint16_t index) {
	if(((cq->start) + index) >= BUFFSIZE) { //If there is wrapping. Wrap to 0 and 
											//then index.
		index -= (BUFFSIZE - (cq->start));
		return cq->buff[index];
	} else {	
		return cq->buff[(cq->start) + index];
	}
//	return cq->buff[(cq->start) + index];
}

uint16_t cq_freesize(struct cq * const cq) {
	return (cq->curfreesize);
}

uint16_t cq_used_size(struct cq * const cq) {
	return BUFFSIZE - ((cq->curfreesize)+1);
}

