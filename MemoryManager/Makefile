rule: allocator.h allocator.cpp MemoryManager.cpp  MemoryManager.h
	g++ -c MemoryManager.cpp -o MemoryManager.o
	g++ -c allocator.cpp -o allocator.o
	ar rcs libMemoryManager.a MemoryManager.o allocator.o
	ar -t libMemoryManager.a





