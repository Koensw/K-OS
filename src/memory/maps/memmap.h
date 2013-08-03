#ifndef KOS_MEMMAP_H
#define KOS_MEMMAP_H

#include <stddef.h>

namespace kos{
	struct MemoryMap{
		unsigned int *next;
		unsigned int base_low, base_high;
		unsigned int length_low, length_high;
		unsigned int type;
	};
}

#endif