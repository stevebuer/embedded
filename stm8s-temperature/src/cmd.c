/* 
 * command interpreter 
 */

#include "stm8s_conf.h"
#include "uart.h"
#include "cmd.h"
#include "i2c_bus.h"
#include "aht20.h"

extern aht20_ops_t aht20_ops;

#define CMD_MAXLINE 32

static char linebuf[CMD_MAXLINE];
static unsigned char line_len = 0;
static const char *prompt = "cmd> ";

static void uart_puthex16(uint16_t value)
{
	uart_puthex8((unsigned char)(value >> 8));
	uart_puthex8((unsigned char)(value & 0xFF));
}

static void usage(void)
{
	uart_puts("micro-mon: v0.3\r\n");
	uart_puts("  h|?           help\r\n");
	uart_puts("  i             scan i2c bus\r\n");
	uart_puts("  t             read AHT20\r\n");
	uart_puts("  r <a> <r>     i2c read reg (hex bytes)\r\n");
	uart_puts("  w <a> <r> <v> i2c write reg (hex bytes)\r\n");
	uart_puts("  e <addr> <v>  write byte to stm8 data EEPROM\r\n");
}

static char *next_field(char** p)
{
	char *s = *p;
	char *start;

	while (*s == ' ')
		s++;

	if (*s == '\0') {
		*p = s;
		return 0;
	}

	start = s;

	while (*s && *s != ' ')
		s++;

	if (*s)
		*s++ = '\0';

	*p = s;

	return start;
}

static unsigned char parse_hex(const char *s, int16_t *out)
{
	char c;
	int16_t n = 0;

	if (*s == '\0')
		return 0;

	while ((c = *s++) != '\0') {

		n <<= 4;

		if (c >= '0' && c <= '9')
			n |= (int16_t)(c - '0');
		else if (c >= 'a' && c <= 'f')
			n |= (int16_t)(c - 'a' + 10);
		else if (c >= 'A' && c <= 'F')
			n |= (int16_t)(c - 'A' + 10);
		else
			return 0;
	}
	 
	*out = n;

	return 1;
}

void cmd_auto_report(void)
{
	uint32_t raw_humidity = 0;
	uint32_t raw_temperature = 0;
	aht20_status_t status;

	if (!aht20_ops.initialized) {
		status = aht20_init(&aht20_ops);
		if (status != AHT20_OK) {
			uart_puts("AHT20 init error: ");
			uart_puthex8((unsigned char)status);
			uart_puts("\r\n");
			return;
		}
	}

	status = aht20_read_raw(&aht20_ops, &raw_humidity, &raw_temperature);
	if (status != AHT20_OK) {
		uart_puts("AHT20 read error: ");
		uart_puthex8((unsigned char)status);
		uart_puts("\r\n");
		return;
	}

	uart_puts("AHT20 H=0x");
	uart_puthex32((unsigned long)raw_humidity);
	uart_puts(" T=0x");
	uart_puthex32((unsigned long)raw_temperature);
	uart_puts("\r\n");
}

static void exec_line(char* line)
{
	char *args = 0, *a1, *a2, *a3, *cmd = line;
	unsigned char i2c_value = 0, ok = 0, rv = 0;
	int16_t a = 0, r = 0, v = 0, n = 0;

	while (*cmd == ' ')
		cmd++;

	if (*cmd == '\0')
		return;

	args = cmd;

	while (*args && *args != ' ')
		args++;

	if (*args) {

		*args++ = '\0';

		while (*args == ' ')
			args++;
	}

	if (cmd[0] == 'h' || cmd[0] == '?')
		return usage();

	if (cmd[0] == 'i' && cmd[1] == '\0')
		return i2c_bus_scan();

	if (cmd[0] == 't' && cmd[1] == '\0') {
		cmd_auto_report();
		return;
	}

	if (cmd[0] == 'r' && cmd[1] == '\0') {

		a1 = next_field(&args); a2 = next_field(&args);

		if (!a1 || !a2 || !parse_hex(a1, &a) || !parse_hex(a2, &r))
			return uart_puts("ERR: usage r <addr> <reg>\r\n");

		rv = i2c_bus_read_reg((unsigned char)a, (unsigned char)r, &i2c_value);

		if (rv == 1) {

			uart_puts("  0x");
			uart_puthex8(i2c_value);
			uart_puts("\r\n");

		} else if (rv == 2)
			uart_puts("ERR: i2c timeout\r\n");
		else 
			uart_puts("ERR: i2c no-ack\r\n");
		
		return;
	}

	if (cmd[0] == 'w' && cmd[1] == '\0') {

		a1 = next_field(&args); a2 = next_field(&args); a3 = next_field(&args);

		if (!a1 || !a2 || !a3 || !parse_hex(a1, &a) || !parse_hex(a2, &r) || !parse_hex(a3, &v))
			return uart_puts("ERR: usage w <addr> <reg> <val>\r\n");

		rv = i2c_bus_write_reg((unsigned char) a, (unsigned char) r, (unsigned char) v);

		if (rv == 1)
			uart_puts("ok\r\n");
		else if (rv == 2)
			uart_puts("ERR: i2c timeout\r\n");
		else 
			uart_puts("ERR: i2c no-ack\r\n");
		
		return;
	}

	if (cmd[0] == 'e' && cmd[1] == '\0') {

		a1 = next_field(&args); a2 = next_field(&args);

		if (!a1 || !a2 || !parse_hex(a1, &a) || !parse_hex(a2, &v))
			return uart_puts("ERR: usage e <addr> <val>\r\n");

		if (a < 0x4000 || a > 0x43FF)
			return uart_puts("ERR: eeprom addr out of range [0x4000..0x43FF]\r\n");

		FLASH_Unlock(FLASH_MEMTYPE_DATA);
		FLASH_ProgramByte((uint32_t)a, (uint8_t)v);
		FLASH_Lock(FLASH_MEMTYPE_DATA);

		if (FLASH_ReadByte((uint32_t) a) != (uint8_t) v)
			return uart_puts("ERR: eeprom verify failed\r\n");

		uart_puts("ok\r\n");
		return;
	}

	uart_puts("ERR: unknown command\r\n");
}

void cmd_init(void)
{
	line_len = 0;

	uart_puts("\r\n");

	usage();

	uart_puts(prompt);
}

void cmd_poll(void)
{
	char c;

	while (uart_rx_ready()) {

		c = uart_getc();

		/* newline */

		if (c == '\r' || c == '\n') {

			uart_puts("\r\n");

			linebuf[line_len] = '\0';

			/* execute */

			exec_line(linebuf);

			/* next prompt */

			line_len = 0;

			uart_puts(prompt);

			continue;
		}

		/* backspace */

		if ((c == '\b' || c == 127) && line_len > 0) {

			line_len--;

			uart_puts("\b \b");

			continue;
		}

		/* regular character */

		if (line_len < (CMD_MAXLINE - 1)) {

			linebuf[line_len++] = c;

			uart_putc(c);
		}
	}
}
