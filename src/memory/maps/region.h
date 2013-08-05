#ifndef KOS_REGION_H
#define KOS_REGION_H

#include <stddef.h>

namespace kos{
	struct Region{
		unsigned int mag;
		/* flags
		 * 1 = start of page
		 * 2 = big region
		 */
		uint8_t flgs;
		Region *nxt;
		Region *prv;
		unsigned int sz;
		//starts here
	};
}

#endif