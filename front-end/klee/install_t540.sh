./configure --with-stp=/home/qin/stp-2.1.0/build \
 --with-llvmsrc=/home/qin/llvm-3.4.2.src/ \
 --with-llvmobj=/home/qin/llvm-3.4.2.src/ \
 --with-llvmcc=/home/qin/llvm-3.4.2.src/Release+Asserts/bin/clang \
 --with-llvmcxx=/home/qin/llvm-3.4.2.src/Release+Asserts/bin/clang++
make -j8
