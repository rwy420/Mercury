#ifndef __MERCURY__FAULTS_H
#define __MERCURY__FAULTS_H

#include <hardware/interrupts.h>

void handle_invalid_opcode(CPUState* cpu);
void handle_general_protection_fault(CPUState* cpu);
void handle_page_fault(CPUState* cpu);

void terminate_or_panic(const char* reason, CPUState* cpu);

#endif
