#ifndef __MERCURY__DRIVER__PS2__PS2KEYBOARD_H
#define __MERCURY__DRIVER__PS2__PS2KEYBOARD_H

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

void ps2_kb_enable();
void ps2_kb_disable();

#endif 
