'''
Created on May 10, 2016

@author: Qin Wang
'''

if __name__ == '__main__':
    from parser import Parser
    trace = Parser('vd.log')
    trans_id = input("Enter a transaction id: ")
    trace.parse_to(trans_id)
#    assert(len(trace.requests) == trans_id + 1)
    with open('mo_dump.txt', 'w') as out:
        for mo in trace.mos:
	    for elem in mo:
                out.write(format(elem) + ' ')
	    out.write('\n')

    with open('req_dump.txt', 'w') as out:
        for request in trace.requests:
            for arg in request:
                out.write(format(arg))
                out.write(' ')
            out.write('\n')
    with open('req_type.txt', 'w') as out:
	for req_type in trace.req_types:
            out.write(format(req_type) + '\n')

    with open('excall_dump.txt', 'w') as out:
        for excall in trace.excalls:
            for elem in excall:
                out.write(format(elem) + ' ')
            out.write('\n')

    with open('dmaread_dump.txt', 'w') as out:
        for dmaread in trace.dmareads:
            for elem in dmaread:
                out.write(format(elem) + ' ')
            out.write('\n')

    with open('receive_dump.txt', 'w') as out:
        for receive in trace.receives:
            for elem in receive:
                out.write(elem + ' ')
            out.write('\n')

    with open ('interrupt_dump.txt', 'w') as out:
        for interrupt in trace.interrupts:
            for elem in interrupt:
                out.write(elem + ' ')
            out.write('\n')
    print 'Six files generated: mo_dump.txt, req_dump.txt, excall_dump.txt, dmaread_dump.txt, receive_dump.txt, interrupt_dump.txt.'
    print 'DONE.'
