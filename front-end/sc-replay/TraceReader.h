/*
 * TraceReader.h
 *
 *  Created on: Oct 18, 2017
 *      Author: jialuwang
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <stdlib.h>
#include <map>


#ifndef TRACEREADER_H_
#define TRACEREADER_H_

class TraceReader {
public:
	TraceReader();
	virtual ~TraceReader();
	void readToVectors();
	void generateReceive();
	void generateInterrupt();
	void generateRequest();
	void generateDMA();
	void generateExcallRet();
	void generateIndices();
	void generateMOs();

private:

public:
	typedef struct {
		long long opaque;
		long long buf;
		unsigned long long addr;
		unsigned long long  val;
		unsigned size;
		int type;
		int ret;
	} MMIO_REQUEST;

	typedef struct {
		unsigned int icr;
		unsigned int rdh;
		unsigned int mit_timer_on;
		unsigned int mit_irq_level;
		unsigned int compat_flags;
		unsigned int sum_needed;
		unsigned int vlan_needed;
		unsigned int tucso;
		unsigned int tucss;
		unsigned int tucse;
		unsigned int ipcso;
		unsigned int ipcss;
		unsigned int ipcse;
	} INTERRUPT_REGS;

	typedef struct {
		unsigned int rdh;
		unsigned int gprc;
		unsigned int tpr;
		unsigned int torl;
		unsigned int torh;
		unsigned int status;
	} RECEIVE_REGS;

private:
	std::vector<std::string> req_vec;
	std::vector<std::string> receive_vec;
	std::vector<std::string> interrupt_vec;
	std::vector<std::string> mo_vec;
	std::vector<std::string> ret_vec;
	FILE* dma_data_file;
	unsigned char* dma_data;
	unsigned int dma_size;

	unsigned int req_size;
	unsigned int receive_size;
	unsigned int interrupt_size;
	unsigned int mo_size;
	unsigned int ret_size;

};




#endif /* TRACEREADER_H_ */
