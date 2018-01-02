rm -f main.bc new_main.bc 
rm -f *.ll 
rm -f global_vars.h a.out
g++ -g -std=c++11 gen_global_vars.cpp TraceReader.cpp
./a.out
make
#llvm-as-3.4 svd_main_trace_replay.ll exfunc_def.ll
llvm-as-3.4 test.ll   
llvm-as-3.4 replay_vd.ll 
llvm-as-3.4 exfunc_def.ll 
#llvm-link-3.4 svd_main_trace_replay.bc exfunc_def.bc -o main.bc
llvm-link-3.4 test.bc replay_vd.bc exfunc_def.bc -o vd.bc
~/llvm-3.4/Release+Asserts/bin/opt -load ~/llvm-3.4/Release+Asserts/lib/LLVMHello.so -loadstore2func vd.bc -o new_vd.bc 
#~/klee-1.3.0/Release+Asserts/bin/klee new_vd.bc
~/crete/crete-dev/back-end/klee-1.3.0/Release+Asserts/bin/klee new_vd.bc

#lli-3.4 main.bc

