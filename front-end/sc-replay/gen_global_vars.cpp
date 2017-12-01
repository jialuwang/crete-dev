/*
 * gen_global_vars.cpp
 *
 *  Created on: Oct 21, 2017
 *      Author: jialuwang
 */


#include "TraceReader.h"

int main() {
	TraceReader tr;
	tr.readToVectors();
	tr.generateReceive();
	tr.generateInterrupt();
	tr.generateRequest();
	tr.generateDMA();
	tr.generateExcallRet();
	tr.generateIndices();
	tr.generateMOs();
	return 0;
}

