#ifndef PSR_H
#define PSR_H

#define PSR_USR	      0x10
#define PSR_IRQ	      0x12
#define PSR_SVC	      0x13
#define PSR_ABT	      0x17
#define PSR_UND	      0x1b
#define PSR_SYS	      0x1f
#define PSR_MODE_MASK 0x1f

#define PSR_N (1u << 31)
#define PSR_Z (1u << 30)
#define PSR_C (1u << 29)
#define PSR_V (1u << 28)
#define PSR_E (1u << 9)
#define PSR_I (1u << 7)
#define PSR_F (1u << 6)
#define PSR_T (1u << 5)

#endif
