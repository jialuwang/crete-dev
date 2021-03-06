#ifndef CRETE_CUSTOM_OPCODE_H
#define CRETE_CUSTOM_OPCODE_H

#define CRETE_INSTR_GENERATE(x, y)            \
    ".byte 0x0F, 0x3F\n"                      \
    ".byte 0x00, 0x" #x ", 0x" #y ", 0x00\n"  \
    ".byte 0x00, 0x00, 0x00, 0x00\n"

#define CRETE_INSTR_CAPTURE_BEGIN_VALUE 0x010000
#define CRETE_INSTR_CAPTURE_BEGIN() CRETE_INSTR_GENERATE(00, 01)

#define CRETE_INSTR_CAPTURE_END_VALUE 0x020000
#define CRETE_INSTR_CAPTURE_END() CRETE_INSTR_GENERATE(00, 02)

#define CRETE_INSTR_INCLUDE_FILTER_VALUE 0x030000
#define CRETE_INSTR_INCLUDE_FILTER() CRETE_INSTR_GENERATE(00, 03)

#define CRETE_INSTR_EXCLUDE_FILTER_VALUE 0x040000
#define CRETE_INSTR_EXCLUDE_FILTER() CRETE_INSTR_GENERATE(00, 04)

#define CRETE_INSTR_QUIT_VALUE 0x050000
#define CRETE_INSTR_QUIT() CRETE_INSTR_GENERATE(00, 05)

#define CRETE_INSTR_DUMP_VALUE 0x060000
#define CRETE_INSTR_DUMP() CRETE_INSTR_GENERATE(00, 06)

#define CRETE_INSTR_MAKE_SYMBOLIC_VALUE 0x070000
#define CRETE_INSTR_MAKE_SYMBOLIC() CRETE_INSTR_GENERATE(00, 07)

#define CRETE_INSTR_ASSUME_BEGIN_VALUE 0x080000
#define CRETE_INSTR_ASSUME_BEGIN() CRETE_INSTR_GENERATE(00, 08)

#define CRETE_INSTR_PRIME_VALUE 0x090000
#define CRETE_INSTR_PRIME() CRETE_INSTR_GENERATE(00, 09)

#define CRETE_INSTR_ASSUME_VALUE 0x0A0000
#define CRETE_INSTR_ASSUME() CRETE_INSTR_GENERATE(00, 0A)

#define CRETE_INSTR_MESSAGE_VALUE 0x0B0000
#define CRETE_INSTR_MESSAGE() CRETE_INSTR_GENERATE(00, 0B)

#define CRETE_INSTR_MAKE_CONCOLIC_VALUE 0x0C0000
#define CRETE_INSTR_MAKE_CONCOLIC() CRETE_INSTR_GENERATE(00, 0C)

#define CRETE_INSTR_DEBUG_PRINT_F32_VALUE 0x0D0000
#define CRETE_INSTR_DEBUG_PRINT_F32() CRETE_INSTR_GENERATE(00, 0D)

#define CRETE_INSTR_DEBUG_PRINT_BUF_VALUE 0x0E0000
#define CRETE_INSTR_DEBUG_PRINT_BUF() CRETE_INSTR_GENERATE(00, 0E)

#define CRETE_INSTR_DEBUG_ASSERT_IS_CONCOLIC_VALUE 0x0F0000
#define CRETE_INSTR_DEBUG_ASSERT_IS_CONCOLIC() CRETE_INSTR_GENERATE(00, 0F)

#define CRETE_INSTR_DEBUG_MONITOR_CONCOLIC_STATUS_VALUE 0x100000
#define CRETE_INSTR_DEBUG_MONITOR_CONCOLIC_STATUS() CRETE_INSTR_GENERATE(00, 10)

#define CRETE_INSTR_MAKE_CONCRETE_VALUE 0x110000
#define CRETE_INSTR_MAKE_CONCRETE() CRETE_INSTR_GENERATE(00, 11)

#define CRETE_INSTR_DEBUG_MONITOR_VALUE_VALUE 0x120000
#define CRETE_INSTR_DEBUG_MONITOR_VALUE() CRETE_INSTR_GENERATE(00, 12)

#define CRETE_INSTR_DEBUG_MONITOR_SET_FLAG_VALUE 0x130000
#define CRETE_INSTR_DEBUG_MONITOR_SET_FLAG() CRETE_INSTR_GENERATE(00, 13)

#define CRETE_INSTR_DEBUG_CAPTURE_VALUE 0x140000
#define CRETE_INSTR_DEBUG_CAPTURE() CRETE_INSTR_GENERATE(00, 14)

#define CRETE_INSTR_CALL_STACK_EXCLUDE_VALUE 0x150000
#define CRETE_INSTR_CALL_STACK_EXCLUDE() CRETE_INSTR_GENERATE(00, 15)

#define CRETE_INSTR_CALL_STACK_SIZE_VALUE 0x160000
#define CRETE_INSTR_CALL_STACK_SIZE() CRETE_INSTR_GENERATE(00, 16)

#define CRETE_INSTR_MAIN_ADDRESS_VALUE 0x170000
#define CRETE_INSTR_MAIN_ADDRESS() CRETE_INSTR_GENERATE(00, 17)

#define CRETE_INSTR_LIBC_START_MAIN_ADDRESS_VALUE 0x180000
#define CRETE_INSTR_LIBC_START_MAIN_ADDRESS() CRETE_INSTR_GENERATE(00, 18)

#define CRETE_INSTR_STACK_DEPTH_BOUNDS_VALUE 0x190000
#define CRETE_INSTR_STACK_DEPTH_BOUNDS() CRETE_INSTR_GENERATE(00, 19)

#define CRETE_INSTR_LIBC_EXIT_ADDRESS_VALUE 0x1A0000
#define CRETE_INSTR_LIBC_EXIT_ADDRESS() CRETE_INSTR_GENERATE(00, 1A)

#define CRETE_INSTR_START_STOPWATCH_VALUE 0x1B0000
#define CRETE_INSTR_START_STOPWATCH() CRETE_INSTR_GENERATE(00, 1B)

#define CRETE_INSTR_STOP_STOPWATCH_VALUE 0x1C0000
#define CRETE_INSTR_STOP_STOPWATCH() CRETE_INSTR_GENERATE(00, 1C)

#define CRETE_INSTR_RESET_STOPWATCH_VALUE 0x1D0000
#define CRETE_INSTR_RESET_STOPWATCH() CRETE_INSTR_GENERATE(00, 1D)

#define CRETE_INSTR_REPLAY_NEXT_PROGRAM_VALUE 0x1E0000
#define CRETE_INSTR_REPLAY_NEXT_PROGRAM() CRETE_INSTR_GENERATE(00, 1E)

#define CRETE_INSTR_READ_PORT_VALUE 0x1F0000
#define CRETE_INSTR_READ_PORT() CRETE_INSTR_GENERATE(00, 1F)

#define CRETE_INSTR_SEND_CONCOLIC_NAME_VALUE 0x200000
#define CRETE_INSTR_SEND_CONCOLIC_NAME() CRETE_INSTR_GENERATE(00, 20)

#endif // CRETE_CUSTOM_OPCODE_H
