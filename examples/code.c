#include "OSAL.h"
#include "OSAL_PwrMgr.h"

#include "OSAL_centralBroadcaster.h"
#include "centralBroadcaster.h"

#include "hal_led.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "uartManager.h"

// Simple FIFO based on Ring buffer
#define FIFO(type,size) struct { type ring[size]; unsigned int rd,wp; unsigned char ne;}

#define FIFO_FLUSH(x) x.wp = x.rd = x.ne = 0
#define FIFO_SIZE(x) (sizeof(x.ring) / sizeof(*(x.ring)))
#define FIFO_NEXT(x,old) (old >= FIFO_SIZE(x)-1 ? 0 : old+1)

#define FIFO_EMPTY(x) (!x.ne)

#define FIFO_RAW(x,i) &x.ring[i]
#define FIFO_FOREACH(x,i) i = x.rd; if(!FIFO_EMPTY(x)) do {
#define FIFO_FOREACH_END(x,i) i = FIFO_NEXT(x,i); } while(i != x.wp);

#define FIFO_PUT(x,el) { \
  x.ring[x.wp] = el; \
  if(x.rd == x.wp && !FIFO_EMPTY(x)) \
    x.rd = FIFO_NEXT(x,x.rd); \
  x.wp = FIFO_NEXT(x,x.wp); \
  x.ne = 1; \
}

#define FIFO_GET(x,el) \
if(!FIFO_EMPTY(x)) { \
  el = &x.ring[x.rd]; \
  x.rd = FIFO_NEXT(x,x.rd); \
  if(x.rd == x.wp) \
    x.ne = 0; \
}

// DJB hash function
uint16 hash_packet(uint8 *payload, int size)
{
    uint16 hash = 5381;
    int i;

    for(i = 0; i < size; i++)
        hash = ((hash << 5) + hash) + payload[i];

    return hash;
}
