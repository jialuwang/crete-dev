rm global_vars.h
g++ -g -std=c++11 gen_global_vars.cpp TraceReader.cpp
./a.out
make
llvm-as-3.4 svd_main_trace_replay.ll
llvm-as-3.4 exfunc_def.ll
llvm-link-3.4 svd_main_trace_replay.bc exfunc_def.bc -o main.bc
~/llvm-3.4/Release+Asserts/bin/opt -load ~/llvm-3.4/Release+Asserts/lib/LLVMHello.so -loadstore2func main.bc -o new_main.bc 
~/klee-1.3.0/Release+Asserts/bin/klee new_main.bc
#lli-3.4 main.bc

