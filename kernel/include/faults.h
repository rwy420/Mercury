#ifndef __MERCURY__FAULTS_H
#define __MERCURY__FAULTS_H

#include <hardware/interrupts.h>

void handle_general_protection_fault(CPUState* cpu);
void handle_stack_fault(CPUState* cpu);

#endif
