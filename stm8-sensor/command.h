#ifndef COMMAND_H
#define COMMAND_H

volatile extern char cmd_buf;
void process_cmd(void);
void init_cmd(void);
void Toggle(void);

#endif
