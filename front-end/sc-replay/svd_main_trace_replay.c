/*
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
*/
#include "svd_main.h"

unsigned int offset = 0;
bool concolic = false;


#include "e1000_regs.h"
//#include "exec.c"

//#include "qfm_socket.h"
#include "qklee.h"
#include "externalcall.h"

//#include "opcodes.c"
#include "e1000_trace.c"
//#include "fw_replay.c"


int BEGIN = 300000;
int END = 60000;
MMIO_REQUEST request;
bool kvm_allowed = false;
//struct em8051 emu;

int main() {
		//create8051(&emu);
		//reset(&emu, 1);

		//inst_buf = malloc(8000000);
		//int inst_num;

		//qklee_fm_load(code, ext, lower, upper, sfr, inst_buf, &inst_num);
		E1000State * s = request.opaque;

		//qklee_fm_load(emu.mCodeMem, emu.mExtData, emu.mLowerData,
		//		emu.mUpperData, emu.mSFR, inst_buf, &inst_num);
		//fprintf(stderr, "in replay_one_trans : emu = %p\n", &emu);
		//fprintf(stderr, "main:emu_sfrwrite = %p\n", emu.sfrwrite);
		//fprintf(stderr, "main:emu_sfrread = %p\n", emu.sfrread);

		int cycle = 0;
		int counter = 0;

		int ret;
		//klee_load_trace();
		//init_mo();
		klee_request_copy(&request);

		while (counter < BEGIN && request.type != 0) {
			E1000State * s = request.opaque;

			INTERRUPT_REGS inter_regs;
			klee_update_interrupt_regs(&inter_regs);
/*			fprintf(stderr, "inter_regs = %x\n", inter_regs);
            fprintf(stderr, "inter_regs.icr = %d\n", inter_regs.icr);
            fprintf(stderr,"s->mac_reg[ICR] = %d\n",s->mac_reg[ICR]);*/
			s->mac_reg[ICR] = inter_regs.icr;
			s->mac_reg[RDH] = inter_regs.rdh;
			s->mit_timer_on = inter_regs.mit_timer_on;
			s->mit_irq_level = inter_regs.mit_irq_level;
			s->compat_flags = inter_regs.compat_flags;
			s->tx.sum_needed = inter_regs.sum_needed;
			s->tx.vlan_needed = inter_regs.vlan_needed;
			s->tx.tucso = inter_regs.tucso;
			s->tx.tucss = inter_regs.tucss;
			s->tx.tucse = inter_regs.tucse;
			s->tx.ipcso = inter_regs.ipcso;
			s->tx.ipcss = inter_regs.ipcss;
			s->tx.ipcse = inter_regs.ipcse;
			fprintf(stderr, "KLEE-replay::interrupt s->mac_reg[ICR] = %" PRIu32 ", s->mac_reg[RDH] = %" PRIu32 ", s->mit_timer_on = %" PRIu32 ", "
						"s->mit_irq_level = %" PRIu32 ", s->compat_flags = %" PRIu32 ", s->tx.sum_needed = %" PRIu32 ", "
								"s->tx.vlan_needed = %" PRIu32 ", s->tx.tucso = %" PRIu32 ", s->tx.tucss = %" PRIu32 ", "
										"s->tx.tucse = %"PRIu32 ", s->tx.ipcso = %" PRIu32 ", s->tx.ipcss = %" PRIu32 ", "
												"s->tx.ipcse = %" PRIu32 "\n",
								s->mac_reg[ICR], s->mac_reg[RDH], s->mit_timer_on, s->mit_irq_level, s->compat_flags, s->tx.sum_needed, s->tx.vlan_needed,
								s->tx.tucso, s->tx.tucss, s->tx.tucse, s->tx.ipcso, s->tx.ipcss, s->tx.ipcse);

			switch (request.type) {

			case 1:
				e1000_bc_mmio_write(request.opaque, request.addr, request.val, request.size);
				break;
			case 2:
				e1000_bc_mmio_read(request.opaque, request.addr, request.size);
				break;
			case 3:
				ret = e1000_bc_receive(request.opaque, request.buf, request.size);
				break;
			case 4:
				e1000_set_link_status(request.opaque);
				break;
			case 5:
				ret = e1000_bc_can_receive(request.opaque);
				break;
			}
			qklee_ret(ret);
			//fprintf(stderr, "conc trans = %d\n", counter);
			klee_request_copy(&request);
			counter++;
		}
		return 0;
	}

