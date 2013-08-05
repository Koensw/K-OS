#ifndef KOS_MEMORYMANAGERS_H
#define KOS_MEMORYMANAGERS_H

#include <stddef.h>
#include <stdint.h>

#include "../memory.h"
#include "../maps/memmap.h"
 
namespace kos{ 
	class IdentityManager: public MemoryManager{
	public:
		IdentityManager() {}
		
		void *virt(void *);
		void *phys(void *);
		bool map(void *a, void *b, unsigned int) {
			if(a == b) return true;
			else return false;
		}
		bool change(void *, unsigned int) {
			return false;
		}
		
		void *search(unsigned int sz, void *ptr);
		
		void clear(void *) {}
		
		/*void save(void *);
		void load(void *);*/
	};
}

#endif