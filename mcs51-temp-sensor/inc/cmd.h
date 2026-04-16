/*
 * cmd.h
 */

extern volatile __bit cmd_flag;

void task_cmd(void);

extern char cmd_buf;
