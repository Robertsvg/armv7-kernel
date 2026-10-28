#include <kernel/tests.h>
#include <lib/kprintf.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

static void test_number(int value)
{
	kprintf("i: [%i] [%8i] [%08i]\n", value, value, value);
	kprintf("u: [%u] [%8u] [%08u]\n", (unsigned int)value, (unsigned int)value,
		(unsigned int)value);
	kprintf("x: [%x] [%8x] [%08x]\n", (unsigned int)value, (unsigned int)value,
		(unsigned int)value);
}

void test_kprintf(void)
{
	static const int  values[] = { 71, 2, 1, 0x416c6c67, INT_MIN, 0, INT_MAX, -12345 };
	static const char long_text[] =
		"Dies ist eine laengere Zeichenkette fuer den Stringtest. "
		"Sie wird Zeichen fuer Zeichen ausgegeben und benoetigt deshalb keinen "
		"festen Ausgabepuffer.";

	kprintf("kprintf-Tests:\n");
	for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
		test_number(values[i]);
	}
	kprintf("Maximal unsigned: [%u] [%8u] [%08u]\n", UINT_MAX, UINT_MAX, UINT_MAX);
	kprintf("Maximal hex: [%x] [%8x] [%08x]\n", UINT_MAX, UINT_MAX, UINT_MAX);
	kprintf("Zeichen: [%c] [%c] [%c]\n", 71, 'A', '%');
	kprintf("Strings: [%s] [%s]\n", "Allg", "");
	kprintf("Prozent: [%%]\n");
	kprintf("Pointer: [%p] [%p] [%p]\n", (void *)NULL, (void *)(uintptr_t)0x416c6c67,
		(void *)(uintptr_t)UINT_MAX);
	kprintf("Unbekannt: [%M]\n");
	kprintf("Langer String: %s\n", long_text);
	kprintf("Dies ist ein laengerer Formatstring. Er wird ohne Zwischenpuffer direkt "
		"ausgegeben. Auch mehrere aneinanderhaengende Textteile duerfen dabei "
		"beliebig lang sein.\n");
}
