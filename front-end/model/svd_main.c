#define QKLEE

#include "e1000.c"
#include <stdio.h>

typedef struct _MMIO_REQUEST {
  void *opaque;
  const uint8_t *buf;
  hwaddr addr;
  uint64_t val;
  unsigned size;
  int type;
  int ret;
} MMIO_REQUEST;


MMIO_REQUEST request;

int main()
{
//	printf("Place to call fm_replay_mainin replay\n");
	qklee_request_cpy(&request, sizeof(MMIO_REQUEST));
//	qklee_correlate("request", &request);
       

	int ret;
//	set_request();
//      preload_e1000_state();
	E1000State* s = request.opaque;
        if(qklee_check_dump()) {
            fprintf(vd_trace, "KLEE-replay::interrupt s->mac_reg[ICR] = %" PRIu32 ", s->mac_reg[RDH] = %" PRIu32 ", s->mit_timer_on = %" PRIu32 ", "
			"s->mit_irq_level = %" PRIu32 ", s->compat_flags = %" PRIu32 ", s->tx.sum_needed = %" PRIu32 ", "
					"s->tx.vlan_needed = %" PRIu32 ", s->tx.tucso = %" PRIu32 ", s->tx.tucss = %" PRIu32 ", "
							"s->tx.tucse = %"PRIu32 ", s->tx.ipcso = %" PRIu32 ", s->tx.ipcss = %" PRIu32 ", "
									"s->tx.ipcse = %" PRIu32 "\n",
					s->mac_reg[ICR], s->mac_reg[RDH], s->mit_timer_on, s->mit_irq_level, s->compat_flags, s->tx.sum_needed, s->tx.vlan_needed,
					s->tx.tucso, s->tx.tucss, s->tx.tucse, s->tx.ipcso, s->tx.ipcss, s->tx.ipcse);
        }
	switch (request.type) {
	    case 1 : {
//		E1000State *s = request.opaque;
//		qklee_sendMessage(s->nskt);
//		qklee_recvMessage(s->fp);
		e1000_bc_mmio_write(request.opaque, request.addr, request.val, request.size);
		break;
	    }
	    case 2 :
		ret = e1000_bc_mmio_read(request.opaque, request.addr, request.size);	
		break;
	    case 3 :
		ret = e1000_bc_receive(request.opaque, request.buf, request.size);
		break;
	    case 4 : 
		e1000_set_link_status(request.opaque);
		break;
	    case 5 :
//		fprintf(stderr, "svd_main.c: e1000_bc_can_receive\n");
        ret = e1000_bc_can_receive(request.opaque);
        break;

	}

    qklee_ret(ret);
    //fclose(vd_trace);
    return 0;
}

