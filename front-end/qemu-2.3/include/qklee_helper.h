#ifndef QKLEE_HELPER_H
#define QKLEE_HELPER_H

unsigned char e1000_start_dump = 0;
FILE* vd_trace;
void qklee_start_dump();
void qklee_finish_dump();
void qklee_quit_dump();
void qklee_mark_transaction();
#endif
