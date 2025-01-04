#ifndef __MERCURY__DRIVER__PS2__PS2KEYBOARD_H
#define __MERCURY__DRIVER__PS2__PS2KEYBOARD_H

#include <hardware/port.h>
#include <core/types.h>
#include <core/screen.h>
#include <hardware/interrupts.h>
#include <driver/driver.h>

void ps2_kb_enable();
void ps2_kb_disable();
void ps2_kb_handle_interrupt();

#endif 
