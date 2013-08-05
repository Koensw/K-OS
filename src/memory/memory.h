#ifndef KOS_MEMORY_H
#define KOS_MEMORY_H

#include "maps/region.h"
#include "../boot.h"
#include "../graphics/graphics.h"

#define PAGE_SIZE 4096

namespace kos{
	class MemoryDriver{
	public:
		virtual void *alloc() = 0;
		virtual void dealloc(void *) = 0;
		virtual unsigned int size() = 0;
	};
	 
	class MemoryManager{
	public:
		virtual void *virt(void *) = 0;
		virtual void *phys(void *) = 0;
		virtual bool map(void *, void*, unsigned int flags) = 0;
		virtual bool change(void *, unsigned int flags) = 0;
		virtual void *search(unsigned int, void*) = 0;
		virtual void clear(void *) = 0;
		
		/*
		virtual void save(void *) = 0;
		virtual void load(void *) = 0;
		*/
	};
	
	class Memory{
	public:
		Memory(MemoryDriver*, MemoryManager*, Graphics &);
		~Memory() {};
		
		void *alloc(unsigned int bt);
		void dealloc(void *);
		
		//debugging
		void print();
	private:
		MemoryDriver *drv;
		MemoryManager *mng;
		
		Region *st;
		Graphics &g;
	};
}

#endif