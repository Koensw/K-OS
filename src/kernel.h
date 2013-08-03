#ifndef KOS_KERNEL_H
#define KOS_KERNEL_H

namespace kos{
	class Kernel{
	public:
		Kernel(Graphics &g, Memory &m): grph(g), mem(m) {}
		~Kernel() {}
		
		Graphics &getGraphics();
		Memory &getMemory();
	private:
		Memory &mem;
		Graphics &grph;
	};
}

#endif