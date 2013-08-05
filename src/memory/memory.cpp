#include "drivers/drivers.h"
#include "managers/managers.h"
#include "memory.h"
#include "../boot.h"

using namespace kos;

#define USER_START 0x40000000
#define KERNEL_START 0xD0000000

Memory::Memory(MemoryDriver *d, MemoryManager *m, Graphics &gr): drv(d), mng(m), st(0), g(gr){
	void *tmp = d->alloc();
	st = reinterpret_cast<Region*>(mng->search(1, tmp)); //convert to virtual
	mng->map(tmp, st, 0);
	
	st->flgs = 0;
	st->mag = 0xF12EE;
	st->prv = st->nxt = 0;
	st->sz = PAGE_SIZE-sizeof(Region);
}

void *Memory::alloc(unsigned int bt){
	if(bt >= PAGE_SIZE-sizeof(Region)) { //large pages
		unsigned int pgs = 2+(bt-(PAGE_SIZE-sizeof(Region)))/PAGE_SIZE;

		if(drv->size() < pgs){
			//TODO: do page replacement
			return 0;
		}
		
		void *ptr = mng->search(pgs, 0);
		void *sv = ptr;
		
		if(ptr == 0) return 0; //FIXME: no virtual address space, we have a problem...
		
		void *tmp = drv->alloc();
		Region *rptr = reinterpret_cast<Region*>(ptr);
		mng->map(tmp, rptr, 0);
			
		rptr->flgs = (1<<1);
		rptr->mag = 0x4A6E;
		rptr->prv = rptr->nxt = 0;
		rptr->sz = bt;
		for(unsigned int i=1; i<pgs; ++i){
			tmp = drv->alloc();
			mng->map(tmp, ptr, 0);
			
			ptr = reinterpret_cast<void*>(reinterpret_cast<unsigned int>(ptr) + PAGE_SIZE);
		}
		
		/* DEBUGGING */
		st->prv = rptr;
		rptr->nxt = st;
		st = rptr;
		/* DEBUGGING END */
		
		unsigned int ret = reinterpret_cast<unsigned int>(sv)+sizeof(Region);
		return reinterpret_cast<void*>(ret);
	}
	
	Region *ptr = st;
	Region *save = 0;
	while(ptr){
		if(ptr->mag == 0xF12EE) {
			if(bt <= ptr->sz && ptr->sz-bt < sizeof(Region)+4){ //give back full page
				ptr->mag = 0x4A6E;
				
				unsigned int tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region);
				return reinterpret_cast<void*>(tmp);
			}else if(bt < ptr->sz){ //split page
				ptr->mag = 0x4A6E;
				unsigned int tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region)+bt;
				Region *n = reinterpret_cast<Region*>(tmp);
				n->mag = 0xF12EE;
				n->flgs = 1;
				
				n->nxt = ptr->nxt;
				n->prv = ptr;
				n->sz = ptr->sz-bt-sizeof(Region);
			
				ptr->sz = bt;
				if(ptr->nxt) ptr->nxt->prv = n;
				ptr->nxt = n;
				
				tmp = reinterpret_cast<unsigned int>(ptr)+sizeof(Region);
				return reinterpret_cast<void*>(tmp);
			}
		}
		save = ptr;
		ptr = ptr->nxt;
		
		if(ptr == 0){
			//open new page
			void *tmp = drv->alloc();
			ptr = reinterpret_cast<Region*>(mng->search(1, tmp)); //convert to virtual
			mng->map(tmp, ptr, 0);
			
			if(ptr == 0) {
				//TODO: do page replacement!
				return 0;
			}
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
			if(ptr->prv && (ptr->flgs & 1) && ptr->prv->mag == 0xF12EE){
				ptr->mag = 0xDEAD;
				ptr->prv->sz += ptr->sz+ptr->nxt->sz + 2*sizeof(Region);
				ptr->prv->nxt = ptr->nxt->nxt;
				if(ptr->nxt) ptr->nxt->prv = ptr->prv;
			}else{
				ptr->mag = 0xF12EE;
				ptr->sz += ptr->nxt->sz + sizeof(Region);
				ptr->nxt = ptr->nxt->nxt;
				if(ptr->nxt->nxt) ptr->nxt->prv = ptr;
			}
		}else if(ptr->prv && (ptr->prv->flgs & 1) && ptr->prv->mag == 0xF12EE){
			ptr->mag = 0xDEAD;
			ptr->prv->sz += ptr->sz + sizeof(Region);
			ptr->prv->nxt = ptr->nxt;
			if(ptr->nxt) ptr->nxt->prv = ptr;
		}else{
			ptr->mag = 0xF12EE;
		}
		
		//delete unassigned pages (if large page)
		if(ptr->flgs & (1<<1)){
			unsigned int end_page = (reinterpret_cast<unsigned int>(ptr) + sizeof(Region) + ptr->sz - 1) & 0xFFFFF000;
			unsigned int st_page = reinterpret_cast<unsigned int>(ptr) & 0xFFFFF000;
			while(end_page != st_page){
				mng->clear(reinterpret_cast<void*>(end_page));
				drv->dealloc(mng->phys(reinterpret_cast<void*>(end_page)));
				end_page -= PAGE_SIZE;
			}
			ptr->sz = min(PAGE_SIZE-sizeof(Region), ptr->sz);
		}
		
		//delete assigned page
		if((ptr->nxt || ptr->prv) && (ptr->flgs & 1) == 0 && ptr->sz == PAGE_SIZE-sizeof(Region)){
			ptr->mag = 0xDEAD;
			if(ptr->prv) ptr->prv->nxt = ptr->nxt;
			if(ptr->nxt) ptr->nxt->prv = ptr->prv;
			
			if(ptr->prv == 0) st = ptr->nxt;
			mng->clear(ptr);
			drv->dealloc(mng->phys(ptr));
		}
	}
}

void Memory::print(){
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
		g.write(" - DATA - ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr)+sizeof(Region)), 16);
		g.write(" - ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr)+sizeof(Region)+ptr->sz)-1, 16);
		g.write(" +++ ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr->prv)), 16);
		g.write(" +++ ");
		g.write(static_cast<unsigned int>(reinterpret_cast<unsigned int>(ptr->nxt)), 16);
		g.write("\n");
		
		ptr = ptr->nxt;
	}
	g.write("--MEMORY--\n");
}