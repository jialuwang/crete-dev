/*
 * exfunc_def.c
 *
 *  Created on: Oct 18, 2017
 *      Author: jialuwang
 */

#include "global_vars.h"

int qklee_ret(int ret) {
	int rc = strcmp(excall_name_arr[excall_ret_ind], "qklee_ret");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
void* qemu_get_nic_opaque(NetClientState *nc){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "qemu_get_nic_opaque");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
Object *object_dynamic_cast_assert(Object *obj, const char *typename,
                                   const char *file, int line, const char *func){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "object_dynamic_cast_assert");
        fprintf(stderr, "%s\n", excall_name_arr[excall_ret_ind]);
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
int64_t qemu_clock_get_ns(QEMUClockType type){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "qemu_clock_get_ns");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
size_t iov_size(const struct iovec *iov, const unsigned int iov_cnt){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "iov_size");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
size_t iov_to_buf(const struct iovec *iov, const unsigned int iov_cnt,
                  size_t offset, void *buf, size_t bytes){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "iov_to_buf");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
size_t iov_from_buf(const struct iovec *iov, unsigned int iov_cnt,
                    size_t offset, const void *buf, size_t bytes){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "iov_from_buf");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
bool address_space_rw(AddressSpace *as, hwaddr addr, uint8_t *buf,
                      int len, bool is_write){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "address_space_rw");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
int memcmp(const void *s1, const void *s2, size_t n){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "memcmp");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
NetClientState *qemu_get_queue(NICState *nic){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "qemu_get_queue");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
uint16_t net_checksum_finish(uint32_t sum){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "net_checksum_finish");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}
uint32_t net_checksum_add_cont(int len, uint8_t *buf, int seq){
	int rc = strcmp(excall_name_arr[excall_ret_ind], "net_checksum_add_cont");
	assert(!rc);
	return excall_val_arr[excall_ret_ind++];
}


/* no return value external call*/
void timer_mod(QEMUTimer *ts, int64_t expire_timer){}
void pci_set_irq(PCIDevice *pci_dev, int level){}
void qemu_flush_queued_packets(NetClientState *nc){}
void qemu_send_packet(NetClientState *nc, const uint8_t *buf, int size){}
void qemu_format_nic_info_str(NetClientState *nc, uint8_t macaddr[6]){}


void klee_request_copy(MMIO_REQUEST* req) {
	fprintf(stderr, "in klee_request_copy, req_ind = %d\n", req_ind);
        if(!req_size) return;
	if (req_ind >= req_size) {
		req->type = 0;
		return;
	}
	*req = req_arr[req_ind++];
}

void klee_update_interrupt_regs(INTERRUPT_REGS* regs) {
	if(!interrupt_size) return;
	assert(interrupt_ind < interrupt_size);
	*regs = interrupt_arr[interrupt_ind++];
}

void klee_update_dma(void* addr, unsigned int size) {
	//fprintf(stderr, "addr = %"PRIu64", dma_arr + dma_ind = %"PRIu64"\n", addr, dma_arr + dma_ind);
	if(!dma_size) return;
	assert(dma_ind < dma_size);

	memcpy(addr, dma_arr + dma_ind, size);
	dma_ind += size;
	//for(unsigned int i = 0; i < size; i++) {
	//	*((char*)addr + i) = dma_arr[dma_ind++];
	//}
}

void klee_update_receive_regs(RECEIVE_REGS* regs) {
	if(!receive_size) return;
        assert(receive_ind < receive_size);
	*regs = receive_arr[receive_ind++];
}

void* transAddr(void* input) {
	void* res = input;
	unsigned int size = sizeof(mo_addr) / sizeof(mo_addr[0]);

	int begin = 0;
	int end = size - 1;
	while(begin <= end) {
		int mid = (begin + end) / 2;
		if(mo_addr[mid] == input) {
			res = mo_val + mid;
			// mo_val + mid is the translated address
			break;
		}
		if(mo_addr[mid] < input) {
			begin = mid + 1;
		} else {
			end = mid - 1;
		}
	}
	return res;
}














