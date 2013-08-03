#include "drivers/drivers.h"
#include "managers/managers.h"
#include "memory.h"
#include "../boot.h"

using namespace kos;

#define USER_START 0x40000000
#define KERNEL_START 0xD0000000

Memory::Memory(MemoryDriver *d, MemoryManager *m): drv(d), mng(m), st(0){
	st = reinterpret_cast<Region*>(d->alloc());
	st->flgs = 0;
	st->mag = 0xF12EE;
	st->prv = st->nxt = 0;
	st->sz = PAGE_SIZE-sizeof(Region);
}

void *Memory::alloc(unsigned int bt){
	if(bt >= PAGE_SIZE-sizeof(Region)) return 0;
		
	Region *ptr = st;
	Region *save = 0;
	while(ptr){
		if(ptr->mag == 0xF12EE) {
			if(bt <= ptr->sz && ptr->sz-bt < sizeof(Region)+4){
				ptr->mag = 0x4A6E;
				
				//if(ptr->prv) ptr->prv->nxt = ptr->nxt;
				//if(ptr->nxt) ptr->nxt->prv = ptr->prv;
				
				unsigned int tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region);
				return reinterpret_cast<void*>(tmp);
			}else if(bt < ptr->sz){
				ptr->mag = 0x4A6E;
				unsigned int tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region)+bt;
				Region *n = reinterpret_cast<Region*>(tmp);
				n->mag = 0xF12EE;
				n->flgs = 1;
				
				n->nxt = ptr->nxt;
				n->sz = ptr->sz-bt-sizeof(Region);
				
				//if(ptr->prv) ptr->prv->nxt = ptr->nxt;
				ptr->sz = bt;
				ptr->nxt = n;
				
				tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region);
				return reinterpret_cast<void*>(tmp);
			}	
		}
		save = ptr;
		ptr = ptr->nxt;
		
		if(ptr == 0){
			//open new page
			ptr = reinterpret_cast<Region*>(drv->alloc());
			ptr->mag = 0xF12EE;
			ptr->flgs = 0;
			ptr->nxt = 0;
			save->nxt = ptr;
			ptr->prv = save;
			ptr->sz = PAGE_SIZE-sizeof(Region);
		}
	}
	
	return 0;
}

void Memory::dealloc(void *vptr){
	if(vptr == 0) return;
	
	unsigned int pt = reinterpret_cast<unsigned int>(vptr);
	pt-=sizeof(Region);
	Region *ptr = reinterpret_cast<Region*>(pt);
	if(ptr->mag == 0x4A6E){
		if(ptr->nxt && (ptr->nxt->flgs & 1) && ptr->nxt->mag == 0xF12EE){
			if(ptr->prv && (ptr->prv->flgs & 1) && ptr->prv->mag == 0xF12EE){
				ptr->mag = 0xDEAD;
				ptr->prv->sz += ptr->sz+ptr->nxt->sz + 2*sizeof(Region);
				ptr->prv->nxt = ptr->nxt->nxt;
				if(ptr->nxt->nxt) ptr->nxt->nxt->prv = ptr->prv;
			}else{
				ptr->mag = 0xF12EE;
				ptr->sz += ptr->nxt->sz + sizeof(Region);
				ptr->nxt = ptr->nxt->nxt;
				if(ptr->nxt->nxt) ptr->nxt->nxt->prv = ptr;
			}
		}else if(ptr->prv && (ptr->prv->flgs & 1) && ptr->prv->mag == 0xF12EE){
			ptr->mag = 0xDEAD;
			ptr->prv->sz += ptr->sz + sizeof(Region);
			ptr->prv->nxt = ptr->nxt;
			if(ptr->nxt) ptr->nxt->prv = ptr;
		}else{
			ptr->mag = 0xF12EE;
		}
		
		if((ptr->nxt || ptr->prv) && (ptr->flgs & 1) == 0){
			ptr->mag = 0xDEAD;
			ptr->prv->nxt = ptr->nxt;
			ptr->nxt->prv = ptr->prv;
			if(ptr->prv == 0) st = ptr->nxt;
			drv->dealloc(ptr);
		}
	}
}

void Memory::print(Graphics &g){
	g.write("--MEMORY--\n");
	Region *ptr = st;
	while(ptr){
		if(ptr->mag == 0xF12EE){
			g.write("- FREE - ");
		}else if(ptr->mag == 0x4A6E){
			g.write("- USED - ");
		}else{
			g.write("- ?CORRUPT? - ");
		}
		
		g.write("HEADER - ");
		g.write(reinterpret_cast<unsigned int>(ptr), 16);
		g.write(" - ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr)+sizeof(Region)-1), 16);
		//g.write("\n");
		g.write("- DATA - ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr)+sizeof(Region)), 16);
		g.write(" - ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr)+sizeof(Region)+ptr->sz)-1, 16);
		g.write("\n");
		
		ptr = ptr->nxt;
	}
	g.write("--MEMORY--\n");
}