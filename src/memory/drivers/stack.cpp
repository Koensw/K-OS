#include "drivers.h"
#include "../memory.h"
#include "../../boot.h"

using namespace kos;

StackDriver::StackDriver(MemoryMap *mmap){	
	unsigned int sz = 0;
	MemoryMap *ptr = mmap;
	
	//find stack size
	MemoryMap *table = 0;
	unsigned int tb_sz = 0;
	while(ptr){ 
		sz += ptr->length_low;

		if(ptr->length_low > tb_sz){
			tb_sz = ptr->length_low;
			table = ptr;
		}
		ptr = reinterpret_cast<MemoryMap*>(ptr->next);
	}
	
	//set stack pointers
	unsigned int pages = sz/(PAGE_SIZE+4)+5; //4 byte buffer
	bot = reinterpret_cast<unsigned int*>(table->base_low);
	
	pages = min(pages, table->length_low*sizeof(unsigned int*));
	table->length_low = table->length_low - pages*sizeof(unsigned int*);
	table->base_low = table->base_low + pages*sizeof(unsigned int*);
	
	top = reinterpret_cast<unsigned int*>(table->base_low);

	cur = bot;
	
	//push all addresses on the stack
	int cnt = 0;
	ptr = mmap;
	while(ptr){ 
		unsigned int pt = (ptr->base_low+ptr->length_low-PAGE_SIZE) & 0xFFFFF000;
		
		for(;pt >= ptr->base_low; pt-=PAGE_SIZE){
			++cnt;
			unsigned int *tmp = reinterpret_cast<unsigned int*>(pt);
			*tmp = 0;
			push(tmp);
		}
		ptr = reinterpret_cast<MemoryMap*>(ptr->next);
	}
}
		
void *StackDriver::alloc(){
	return static_cast<void*>(pop());
}

void StackDriver::dealloc(void *pt){
	push(static_cast<unsigned int*>(pt));
}

void StackDriver::push(unsigned int *pt){
	if(cur == top) return;
	*cur = reinterpret_cast<unsigned int>(pt);
	++cur;
}

unsigned int* StackDriver::pop(){
	if(cur == bot) return 0;
	--cur;
	unsigned int* ret = reinterpret_cast<unsigned int*>(*cur);
	return ret;
}