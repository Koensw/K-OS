#ifndef KOS_MEMORYDRIVERS_H
#define KOS_MEMORYDRIVERS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory.h"
#include "../maps/memmap.h"

#include "../../graphics/graphics.h"
 
namespace kos{ 
	class StackDriver: public MemoryDriver{
	public:
		StackDriver(MemoryMap*);
		
		void *alloc();
		void dealloc(void *);
	private:
		void push(unsigned int *);
		unsigned int *pop();
		
		unsigned int *bot;
		unsigned int *cur;
		unsigned int *top;
	};
}

#endif