'''
Created on May 9, 2016

@author: Qin Wang
'''
import re

class Parser(object):
    '''
    A parser for the dumped trace
    The dumped trace includes the following information:
        1. pointers/base addresses for e1000state and its containing pointers
        2. addresses for the visited/manipulated bytes in QEMU/KLEE address space during each transaction
        3. transactions in chronological order
        4. function parameters for request, eg. addr and size for mmio_read
        5. external function call return values
    '''
    start_point = '=====KLEE-replay=====KLEE-replay==='

    def __init__(self, trace_file):
        '''
        Constructor
        self.lines: all lines in the trace file
        self.base: base addresses for state pointers
        '''
        with open(trace_file, 'r') as f:
            self.lines = f.readlines()
        self.transaction_id = 0
        self.mos = []
        self.requests = []
        self.excalls = []
        self.dmareads = []
        self.receives = []
        self.interrupts = []
        self.req_types  = []
        self.marked_trans = []

    def parse_to(self, transaction_id):
        self.transaction_id = transaction_id
#        self.start_point = 'KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id - 1)
        self.load_memory_object()
        self.load_request()
        self.load_excall()
        self.load_dmaread()
        self.load_receive()
        self.load_interrupt()
        self.load_marked_trans()
        
    def load_memory_object(self):
        switch = False
        addrs = []
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::MO addr (\d+), value (\d+)', line)
                if match:
                    addr = match.group(1)
                    value = match.group(2)
                    if addr not in addrs:
                        addrs.append(addr)
                        self.mos.append([int(addr), int(value)])
        return
    
    def load_request(self): 
        switch = False
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::REQ::(\w+)', line)
                if match:
                    request = match.group(1)
                    
                    if request == 'mmio_write':
                        args = re.match('KLEE-replay::REQ::\w+ opaque (0x\w+), addr (\d+), val (\d+), size (\d+)', line)
                        opaque = int(args.group(1), 16)
                        addr = int(args.group(2))
                        val = int(args.group(3))
                        size = int(args.group(4))
                        self.requests.append([1, opaque, addr, val, size])
                        self.req_types.append(1)
                    elif request == 'mmio_read':
                        args = re.match('KLEE-replay::REQ::\w+ opaque (0x\w+), addr (\d+), size (\d+)', line)
                        opaque = int(args.group(1), 16)
                        addr = int(args.group(2))
                        size = int(args.group(3))
                        self.requests.append([2, opaque, addr, size])
                        self.req_types.append(2)
                    elif request == 'receive':
                        args = re.match('KLEE-replay::REQ::\w+ opaque (0x\w+), buf (0x\w+), size (\d+)', line)
                        opaque = int(args.group(1), 16)
                        buf = int(args.group(2), 16)
                        size = int(args.group(3))
                        self.requests.append([3, opaque, buf, size])
                        self.req_types.append(3)
                    elif request == 'set_link_status':
                        args = re.match('KLEE-replay::REQ::\w+ opaque (0x\w+)', line)
                        opaque = int(args.group(1), 16)
                        self.requests.append([4, opaque])
                        self.req_types.append(4)
                    elif request == 'can_receive':
                        args = re.match('KLEE-replay::REQ::\w+ opaque (0x\w+)', line)
                        opaque = int(args.group(1), 16)
                        self.requests.append([5, opaque])
                        self.req_types.append(5)
                    elif request == 'embedded_receive':
                        continue
                    else:
                        raise ValueError('don\'t match: ' + line)
        return

    def load_excall(self):
        switch = False
        transaction = 0
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                start = re.match('KLEE-replay::TRS start executorRun (\d+)', line)
                if start:
                    transaction = int(start.group(1))
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::ExCall (\w+) return (\d+)', line)
                if match:
                    function_name = match.group(1)
                    if function_name == 'fprintf' or function_name == 'fopen' or function_name == 'fwrite' or function_name == 'fclose':
                        continue
                    if function_name == 'recvMessage' or function_name == 'sendMessage':
                        continue
                    if function_name == 'qklee_check_dump':
                        continue
                    function_value = int(match.group(2))
                    self.excalls.append([transaction, function_name, function_value])
        return

    def load_dmaread(self):
        switch = False
        transaction = 0
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::TD desc.lower.data = (\d+), desc.upper.data = (\d+)', line)
                if match:
                    self.dmareads.append([match.group(1), match.group(2)])
        return

    def load_receive(self):
        switch = False
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::receive_regs e1000_receive s->mac_reg\[RDH\] = (\d+), s->mac_reg\[GPRC\] = (\d+), s->mac_reg\[TPR\] = (\d+), s->mac_reg\[TORL\] = (\d+), s->mac_reg\[TORH\]= (\d+), s->mac_reg\[STATUS\] = (\d+)',line)
                if match:
                    self.receives.append([match.group(1), match.group(2), match.group(3), match.group(4), match.group(5), match.group(6)])
        return


    def load_interrupt(self):
        switch = False
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                if re.match('KLEE-replay::TRS\s+------>done executorRun %s'%str(self.transaction_id), line):
                    break
                match = re.match('KLEE-replay::interrupt s->mac_reg\[ICR\] = (\d+), s->mac_reg\[RDH\] = (\d+), s->mit_timer_on = (\d+), s->mit_irq_level = (\d+), s->compat_flags = (\d+), s->tx.sum_needed = (\d+), s->tx.vlan_needed = (\d+), s->tx.tucso = (\d+), s->tx.tucss = (\d+), s->tx.tucse = (\d+), s->tx.ipcso = (\d+), s->tx.ipcss = (\d+), s->tx.ipcse = (\d+)', line)
                if match:
                    self.interrupts.append([match.group(1), match.group(2), match.group(3), match.group(4), match.group(5), match.group(6), match.group(7), match.group(8), match.group(9), match.group(10), match.group(11), match.group(12), match.group(13)])
        return

    def load_marked_trans(self):
        switch = False
        for line in self.lines:
            if re.match(self.start_point, line):
                switch = True
                continue
            if switch:
                match = re.match('KLEE-replay::Marked transaction (\d+)', line)
                if match:
                    self.marked_trans.append(match.group(1))
        return
                    
