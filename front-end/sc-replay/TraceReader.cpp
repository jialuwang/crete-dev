/*
 * TraceReader.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: jialuwang
 */

#include "TraceReader.h"

TraceReader::TraceReader() {
	// TODO Auto-generated constructor stub

}

// req_dump, interrupt_dump, receive_dump are read by this function
void TraceReader::readToVectors() {

	//load req
	std::ifstream req_dump ("../trace_parser/req_dump.txt");
	if(!req_dump.good())
		assert(0);
	if (req_dump.is_open() && req_vec.empty()) {  // only load with empty
		std::string line;
		while (std::getline(req_dump, line)) {
			req_vec.push_back(line);
		}
	}
	// load mo
	std::ifstream mo_dump ("../trace_parser/mo_dump.txt");
	if (mo_dump.is_open() && mo_vec.empty()) {  // only load with empty
		std::string line;
		while (std::getline(mo_dump, line)) {
			mo_vec.push_back(line);
		}
	}

	// load excall
	std::ifstream ret_dump ("../trace_parser/excall_dump.txt");
	if (ret_dump.is_open() && ret_vec.empty()) { // only load with empty
		std::string line;
		while (std::getline(ret_dump, line)) {
			ret_vec.push_back(line);
		}
	}

	// load dma
	// let the execution state keep track of the offset of pci_dma_data to be used
	dma_data_file = fopen("../trace_parser/pci_dma_data", "r");
	fseek(dma_data_file, 0, SEEK_END);
	dma_size = ftell(dma_data_file);
	fseek(dma_data_file, 0, SEEK_SET);
	dma_data = (unsigned char*)malloc(dma_size * sizeof(unsigned char));
	fread(dma_data, 1, dma_size, dma_data_file);


	//load receive regs
	std::ifstream receive_dump ("../trace_parser/receive_dump.txt");
	if (!receive_dump.good()) assert(0);
	if (receive_dump.is_open() && receive_vec.empty()) {
		std::string line;
		while (std::getline(receive_dump, line)) {
			receive_vec.push_back(line);
		}
	}

	//load interrupt
	std::ifstream interrupt_dump ("../trace_parser/interrupt_dump.txt");
	if (!interrupt_dump.good()) assert(0);
	if (interrupt_dump.is_open() && interrupt_vec.empty()) {
		std::string line;
		while (std::getline(interrupt_dump, line)) {
			interrupt_vec.push_back(line);
		}
	}
	std::ofstream global_vars("global_vars.h", std::ios::app);

	if (global_vars.is_open()) {
		global_vars << "#include \"svd_main.h\"" << std::endl;
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}
}
void TraceReader::generateDeclaration() {
/*    std::stringstream dec("");
    dec << "INTERRUPT_REGS* interrupt_arr;" << std::endl;
    dec << "MMIO_REQUEST* req_arr;" << std::endl;
    dec << "const char** excall_name_arr;" << std::endl;
    dec << "long long* excall_val_arr;" << std::endl;
    dec << "RECEIVE_REGS* receive_arr;" << std::endl;

    std::ofstream global_vars("global_vars.h", std::ios::app);
    if (global_vars.is_open()) {
        global_vars << dec.rdbuf();
	global_vars.close();
    } else {
	std::cout << "Unable to open global_vars";
    }*/
}
void TraceReader::generateDMA() {
	std::stringstream arr("");
        if(!dma_size) {
            arr << "unsigned char* dma_arr;" << std::endl;
        } else {
	// create a stringstream as the statement for creating the array of structs in c file.
	    arr << "unsigned char dma_arr[" << dma_size << "] = {" << std::endl;
	    for(unsigned int i = 0; i < dma_size; i++) {
		arr << +dma_data[i];
		if (i == dma_size - 1) {
			arr << "};" << std::endl;
		} else {
			arr << ", ";
		}
	    }
        }   
	std::ofstream global_vars("global_vars.h", std::ios::app);
	if (global_vars.is_open()) {
		global_vars << arr.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}
}

void TraceReader::generateReceive() {
	std::stringstream arr("");
	// get size of the vector
	receive_size = receive_vec.size();
        if(!receive_size) {
          arr << "RECEIVE_REGS* receive_arr;" << std::endl;
        } else {
	    // create a stringstream as the statement for creating the array of structs in c file.
	  arr << "RECEIVE_REGS receive_arr[" << receive_size << "] = {" << std::endl;

	  for(unsigned int i = 0; i < receive_size; i++) {
		std::string regs = receive_vec[i];
		std::stringstream stream(regs);
		RECEIVE_REGS tmp;
		stream >> tmp.rdh;
		stream >> tmp.gprc;
		stream >> tmp.tpr;
		stream >> tmp.torl;
		stream >> tmp.torh;
		stream >> tmp.status;

		arr << "    {";
		arr << tmp.rdh << ",";
		arr << tmp.gprc << "," ;
		arr << tmp.tpr << ",";
		arr << tmp.torl << ",";
		arr << tmp.torh << ",";
		arr << tmp.status;
		if (i == receive_size - 1) {
			arr << "}" << std::endl << "};" << std::endl;
		} else {
			arr << "}," << std::endl;
		}
	  }
        }  
	std::ofstream global_vars("global_vars.h", std::ios::app);
	if (global_vars.is_open()) {
		global_vars << arr.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}

}

void TraceReader::generateInterrupt() {
	std::stringstream arr("");
	// get size of the vector
	interrupt_size = interrupt_vec.size();
        if(!interrupt_size) {
            arr << "INTERRUPT_REGS* interrupt_arr;" << std::endl;   
        } else {
	  // create a stringstream as the statement for creating the array of structs in c file.
	  arr << "INTERRUPT_REGS interrupt_arr[" << interrupt_size << "] = {" << std::endl;

	  for(unsigned int i = 0; i < interrupt_size; i++) {
		std::string regs = interrupt_vec[i];
		std::stringstream stream(regs);
		INTERRUPT_REGS tmp;
		stream >> tmp.icr;
		stream >> tmp.rdh;
		stream >> tmp.mit_timer_on;
		stream >> tmp.mit_irq_level;
		stream >> tmp.compat_flags;
		stream >> tmp.sum_needed;
		stream >> tmp.vlan_needed;
		stream >> tmp.tucso;
		stream >> tmp.tucss;
		stream >> tmp.tucse;
		stream >> tmp.ipcso;
		stream >> tmp.ipcss;
		stream >> tmp.ipcse;


		arr << "    {";
		arr << tmp.icr << ",";
		arr << tmp.rdh << "," ;
		arr << tmp.mit_timer_on << ",";
		arr << tmp.mit_irq_level << ",";
		arr << tmp.compat_flags << ",";
		arr << tmp.sum_needed << ",";
		arr << tmp.vlan_needed << ",";
		arr << tmp.tucso << "," ;
		arr << tmp.tucss << ",";
		arr << tmp.tucse << ",";
		arr << tmp.ipcso << ",";
		arr << tmp.ipcss << ",";
		arr << tmp.ipcse;

		if (i == interrupt_size - 1) {
			arr << "}" << std::endl << "};" << std::endl;
		} else {
			arr << "}," << std::endl;
		}
	  }
        }
	std::ofstream global_vars("global_vars.h", std::ios::app);
	if (global_vars.is_open()) {
		global_vars << arr.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}

}

void TraceReader::generateRequest() {
	std::stringstream arr("");
	// get size of the vector
	req_size = req_vec.size();
        if(!req_size) {
            arr << "MMIO_REQUEST* req_arr;" << std::endl;   
        } else {
	  // create a stringstream as the statement for creating the array of structs in c file.
	  arr << "MMIO_REQUEST req_arr[" << req_size << "] = {" << std::endl;

	  for (unsigned int i = 0; i < req_size; i++) {
		MMIO_REQUEST mmio_request;
		std::string req = req_vec[i];
		std::stringstream stream(req);
		stream >> mmio_request.type;
		switch (mmio_request.type) {
		case 1: {
			long long opaque;
			stream >> opaque;
			mmio_request.opaque = opaque;
			unsigned long long addr;
			stream >> addr;
			mmio_request.addr = addr;
			unsigned long long val;
			stream >> val;
			mmio_request.val = val;
			unsigned size;
			stream >> size;
			mmio_request.size = size;
			break;
		}
		case 2: {
			long long opaque;
			stream >> opaque;
			mmio_request.opaque = opaque;
			unsigned long long addr;
			stream >> addr;
			mmio_request.addr = addr;
			unsigned size;
			stream >> size;
			mmio_request.size = size;
			break;
		}
		case 3: {
			long long opaque;
			stream >> opaque;
			mmio_request.opaque = opaque;
			long buf;
			stream >> buf;
			mmio_request.buf = buf;
			unsigned size;
			stream >> size;
			mmio_request.size = size;
			break;
		}
		case 4: {
			long long opaque;
			stream >> opaque;
			mmio_request.opaque = opaque;
			break;
		}
		case 5: {
			long long opaque;
			stream >> opaque;
			mmio_request.opaque = opaque;
			break;
		}
		default:
			std::cout << "Wrong type of request" << std::endl;
			break;
		}
		arr << "    {";
		arr << "(void*)" << mmio_request.opaque << ", ";
		arr << "(unsigned char*)" <<mmio_request.buf << ", ";
		arr << mmio_request.addr << ", ";
		arr << mmio_request.val << ", ";
		arr << mmio_request.size << ", ";
		arr << mmio_request.type << ", ";
		arr << mmio_request.ret;

		if (i == req_size - 1) {
			arr << "}" << std::endl << "};" << std::endl;
		} else {
			arr << "}," << std::endl;
		}
          }
	}

	std::ofstream global_vars("global_vars.h", std::ios::app);
	if (global_vars.is_open()) {
		global_vars << arr.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}

}

void TraceReader::generateExcallRet() {
	// create a stringstream as the statement for creating the array of function names in c file.
	std::stringstream name_arr("");
	// create a stringstream as the statement for creating the array of function ret vals in c file.
	std::stringstream val_arr("");
	// get size of the vector
	ret_size = ret_vec.size();
        if(!ret_size){
	  name_arr << "const char** excall_name_arr;" << std::endl;
	  val_arr << "long long* excall_val_arr;" <<  std::endl;
        } else {
	  name_arr << "const char* excall_name_arr[" << ret_size << "] = {" << std::endl;
	  val_arr << "long long excall_val_arr[" << ret_size << "] = {" << std::endl;

	  for(unsigned int i = 0; i < ret_size; i++) {
		std::string regs = ret_vec[i];
		std::stringstream stream(regs);
		std::string trans_num, name, val;
		stream >> trans_num;
		stream >> name;
		stream >> val;

		name_arr << "\"" << name << "\"";
		val_arr << val;

		if (i == ret_size - 1) {
			name_arr << "};" << std::endl;
			val_arr << "};" << std::endl;
		} else {
			name_arr << ",";
			val_arr << ",";

		}
          }
	}
	std::ofstream global_vars("global_vars.h", std::ios::app);
	if (global_vars.is_open()) {
		global_vars << name_arr.rdbuf();
		global_vars << val_arr.rdbuf() << std::endl;
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}
}

void TraceReader::generateMOs() {
	// create a stringstream as the statement for allocation of memories in c file.
	std::stringstream mo_addr("");
	std::stringstream mo_val("");
	// get size of the vector
	mo_size = mo_vec.size();
        if(!mo_size) {
            mo_addr << "char** mo_addr;" << std::endl;
            mo_val << "unsigned char* mo_val;" << std::endl;

        } else {

	  std::map<long long, unsigned int> addr_val_map;
	  for(unsigned int i = 0; i < mo_size; i++) {
		std::string regs = mo_vec[i];
		std::stringstream stream(regs);
		long long addr;
		unsigned int val;
		stream >> addr;
		stream >> val;
		// insert every pair of mo into a map so that they are sorted by addr
		addr_val_map.insert(std::pair<long long, unsigned int>(addr, val));

	  }
	  mo_addr << "char* mo_addr[" << mo_size << "] = {";
	  mo_val << "unsigned char mo_val[" << mo_size << "] = {";

	  unsigned int cnt = 0;
	  for(std::map<long long, unsigned int>::iterator it = addr_val_map.begin(); it!=addr_val_map.end(); it++) {
		if(cnt == mo_size - 1) {
			mo_addr << it->first << "};" << std::endl;
			mo_val << it->second << "};" << std::endl;
		} else {
			mo_addr << it->first << ", ";
			mo_val  << it->second << ", ";
		}
		cnt++;
	  }
        }
	std::ofstream global_vars("global_vars.h", std::ios::app);

	if (global_vars.is_open()) {
		global_vars << mo_addr.rdbuf();
		global_vars << mo_val.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}
}
void TraceReader::generateIndices() {
	std::ofstream global_vars("global_vars.h", std::ios::app);
	std::stringstream ind("");
	ind << "unsigned int dma_ind = 0;" << std::endl;
	ind << "unsigned int receive_ind = 0;" << std::endl;
	ind << "unsigned int interrupt_ind = 0;" << std::endl;
	ind << "unsigned int req_ind = 0;" << std::endl;
	ind << "unsigned int excall_ret_ind = 0;" << std::endl;

	ind << "unsigned int dma_size = " << dma_size << ";" << std::endl;
	ind << "unsigned int receive_size = " << receive_size << ";" << std::endl;
	ind << "unsigned int interrupt_size = " << interrupt_size << ";" << std::endl;
	ind << "unsigned int req_size = " << req_size << ";" << std::endl;
	ind << "unsigned int ret_size = " << ret_size << ";" << std::endl;

	if (global_vars.is_open()) {
		global_vars << ind.rdbuf();
		global_vars.close();
	} else {
		std::cout << "Unable to open global_vars";
	}
}



TraceReader::~TraceReader() {
	// TODO Auto-generated destructor stub
}


