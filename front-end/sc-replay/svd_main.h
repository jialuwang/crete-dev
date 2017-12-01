#define QKLEE
#include <stdio.h>
#include <stdint.h>
#include "hw/hw.h"
#include "hw/pci/pci.h"
#include "net/net.h"
#include "net/checksum.h"
#include "hw/loader.h"
#include "sysemu/sysemu.h"
#include "sysemu/dma.h"
#include "qemu/iov.h"
#include "qemu/range.h"

typedef struct _MMIO_REQUEST {
	void *opaque;
	const uint8_t *buf;
	hwaddr addr;
	uint64_t val;
	unsigned size;
	int type;
	int ret;
} MMIO_REQUEST;

typedef struct{
	uint32_t icr;
	uint32_t rdh;
	uint32_t mit_timer_on;
	uint32_t mit_irq_level;
	uint32_t compat_flags;
	uint32_t sum_needed;
	uint32_t vlan_needed;
	uint32_t tucso;
	uint32_t tucss;
	uint32_t tucse;
	uint32_t ipcso;
	uint32_t ipcss;
	uint32_t ipcse;
} INTERRUPT_REGS;

typedef struct{
	uint32_t rdh;
	uint32_t gprc;
	uint32_t tpr;
	uint32_t torl;
	uint32_t torh;
	uint32_t status;
} RECEIVE_REGS;

typedef struct{
	uint32_t lower;
	uint32_t upper;
} DMA_DATA;

void klee_request_copy(MMIO_REQUEST*);
void klee_update_receive_regs(RECEIVE_REGS*);
void klee_update_dma(void*, unsigned int);
void klee_update_interrupt_regs(INTERRUPT_REGS*);
void* transAddr(void* input);

