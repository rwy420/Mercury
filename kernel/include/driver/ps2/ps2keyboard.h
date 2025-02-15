#ifndef __QS__DRIVER__PS2__PS2KEYBOARD_H
#define __QS__DRIVER__PS2__PS2KEYBOARD_H

#include <hardware/port.h>
#include <common/types.h>
#include <common/screen.h>
#include <hardware/interrupts.h>
#include <driver/driver.h>

void ps2_kb_enable();
void ps2_kb_disable();
void ps2_kb_handle_interrupt();

#endif 
