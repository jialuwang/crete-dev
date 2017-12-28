void qklee_start_dump(){
    vd_trace = fopen("home/jialuwang/crete/crete-dev/frot-end/trace_parser/vd.log", "ab");
    e1000_start_dump = 1;
    fprintf(vd_trace, "=====KLEE-replay=====KLEE-replay=====KLEE-replay=====KLEE-replay=====KLEE-replay=====\n");
}

void qklee_finish_dump(){
    fclose(vd_trace);
    e1000_start_dump = 0;
}
