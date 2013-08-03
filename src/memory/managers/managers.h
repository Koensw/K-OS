#ifndef KOS_MEMORYMANAGERS_H
#define KOS_MEMORYMANAGERS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory.h"
#include "../maps/memmap.h"
 
namespace kos{ 
	class IdentifyManager: public MemoryManager{
	public:
		IdentifyManager() {}
		
		void *virt(void *);
		void *phys(void *);
		bool map(void *a, void *b, unsigned int) {
			if(a == b) return true;
			else return false;
		}
		bool change(void *, unsigned int) {
			return false;
		}
		void clear(void *) {}
		
		/*void save(void *);
		void load(void *);*/
	};
}

#endif