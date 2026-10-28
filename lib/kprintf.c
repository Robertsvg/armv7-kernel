#include <arch/bsp/uart.h>
#include <lib/kprintf.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

static constexpr unsigned int DECIMAL_BASE = 10;
static constexpr unsigned int HEX_BASE	   = 16;
static constexpr unsigned int FIELD_WIDTH  = 8;
static const char	      hex_digits[] = "0123456789abcdef";

static void print_number(unsigned int value, unsigned int base, bool negative, unsigned int width,
			 char padding)
{
	char   digits[sizeof(unsigned int) * CHAR_BIT];
	size_t length = 0;

	do {
		digits[length++] = hex_digits[value % base];
		value /= base;
	} while (value != 0);

	unsigned int printed = (unsigned int)length + (negative ? 1u : 0u);
	if (negative && padding == '0') {
		uart_putc('-');
	}
	while (printed < width) {
		uart_putc(padding);
		printed++;
	}
	if (negative && padding != '0') {
		uart_putc('-');
	}
	while (length > 0) {
		uart_putc(digits[--length]);
	}
}

static bool format_option(char c)
{
	return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == ' ' || c == '#' || c == '.' ||
	       c == '*' || c == 'h' || c == 'l' || c == 'j' || c == 'z' || c == 't' || c == 'L';
}

void kprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	while (*format != '\0') {
		if (*format != '%') {
			uart_putc(*format++);
			continue;
		}
		format++;

		const char *options = format;
		while (format_option(*format)) {
			format++;
		}
		size_t option_length = (size_t)(format - options);
		bool   zero_padding  = option_length == 2 && options[0] == '0' && options[1] == '8';
		bool   has_width     = (option_length == 1 && options[0] == '8') || zero_padding;
		unsigned int width   = has_width ? FIELD_WIDTH : 0;
		char	     padding = zero_padding ? '0' : ' ';

		if (*format == '\0') {
			uart_puts("Unknown conversion specifier");
			break;
		}
		char conversion = *format++;
		bool number	= conversion == 'i' || conversion == 'u' || conversion == 'x';
		if ((option_length != 0 && !has_width) || (has_width && !number)) {
			uart_puts("Unknown conversion specifier");
			continue;
		}

		switch (conversion) {
		case 'i': {
			int	     value     = va_arg(args, int);
			bool	     negative  = value < 0;
			unsigned int magnitude = (unsigned int)value;
			if (negative) {
				// Auch INT_MIN wird ohne vorzeichenbehafteten Ueberlauf behandelt.
				magnitude = 0u - magnitude;
			}
			print_number(magnitude, DECIMAL_BASE, negative, width, padding);
			break;
		}
		case 'u':
			print_number(va_arg(args, unsigned int), DECIMAL_BASE, false, width,
				     padding);
			break;
		case 'x':
			print_number(va_arg(args, unsigned int), HEX_BASE, false, width, padding);
			break;
		case 'p': {
			uintptr_t address = (uintptr_t)va_arg(args, void *);
			uart_puts("0x");
			print_number((unsigned int)address, HEX_BASE, false, FIELD_WIDTH, '0');
			break;
		}
		case 'c':
			uart_putc((char)(unsigned char)va_arg(args, int));
			break;
		case 's': {
			const char *text = va_arg(args, const char *);
			uart_puts(text == NULL ? "(null)" : text);
			break;
		}
		case '%':
			uart_putc('%');
			break;
		default:
			uart_puts("Unknown conversion specifier");
			break;
		}
	}
	va_end(args);
}
