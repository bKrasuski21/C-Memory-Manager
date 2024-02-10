#include <cstddef>
#include <iostream>
#include <string>
#include <cmath>
#include <array>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>
#include <functional>
#include <fcntl.h>
#include <unistd.h>
#include "allocator.h"
//#include "MemoryManager.cpp"
//#pragma once;
using namespace std;

class MemoryManager{
private:
    int wordSize = 0;
    int memSizeInBytes = 0;
    int wordCount = 0;
    char * array;
    bool initialized = false;
    std::function<int(int, void *)> defaultAllocator;
    vector<std::pair<uint16_t , uint16_t> > holeyHolder;
    vector<std::pair<int*, int> > partition;


public:
    MemoryManager(unsigned wordSize, function<int(int, void *)> allocator);
    ~MemoryManager();
    void initialize(size_t sizeInWords);
    void shutdown();
    void *allocate(size_t sizeInBytes);
    void free(void *address);
    void *getList();
    void setAllocator(function<int(int, void*)> allocator);
    bool destructGetList(void *pointer);
    //oid holeDeleter(int mem, int size);
    void editArray(int loc, int size);
    void printArray();
    void holeCounter();
    void printHoles();
    unsigned getWordSize(){return wordSize;};
    void * getMemoryStart();
    unsigned getMemoryLimit();
    void *getBitmap();
    int dumpMemoryMap(char* filename);

};