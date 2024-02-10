#include "MemoryManager.h"
//#include "allocator.cpp"

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator){
    this->wordSize = wordSize;
    setAllocator(allocator);

};
MemoryManager::~MemoryManager(){
    shutdown();
};
void * MemoryManager::getMemoryStart() {
    char *addy = &array[0];
    return static_cast<void*>(addy);
};
void MemoryManager::initialize(size_t sizeInWords){
    if(sizeInWords > 65536){
        return;
    }
    //delete[] this->array;
    if(this->initialized){
        shutdown();
    }
    this->wordCount = sizeInWords;
    this->initialized = true;
    this->memSizeInBytes = this->wordSize * sizeInWords;
    //cout << "memsize" << memSizeInBytes << endl;
    array = new char [this->memSizeInBytes];


    //cout << sizeof(array) << " = size of contigous array" << endl;
    for(int i =0; i < memSizeInBytes; i++){
        array[i] = '\0';
    }
    //cout << "ref" << &array[0] << endl;
};
void MemoryManager::shutdown(){
    //cout << "shut" << endl;
    /*for(int i =0; i < memSizeInBytes; i++){
        array[i] = '\0';
    }*/
    holeyHolder.clear();
    partition.clear();
    //destructGetList(getList());
    delete[] this->array;
    initialized = false;
    array = nullptr;
    //wordSize = 0;

};
void *MemoryManager::allocate(size_t sizeInBytes) {
    if(holeyHolder.size() == 0 && array[0] != '\0'){
        //cout << " array full" << endl;
        return nullptr;}
    //cout << "happened " << endl;
    int allocationSize = sizeInBytes / this->wordSize;
    int modulusSize = sizeInBytes % this->wordSize;
    int allocationWordcount;
    if(modulusSize != 0){
        allocationWordcount = allocationSize + 1;
    }else {
        allocationWordcount = allocationSize;
    }

    void *temp = getList();
    //cout << "212 " << endl;

    int locationInMem = defaultAllocator(allocationWordcount, temp);
        if(locationInMem == -1 ){
            // << "size to big" << endl;
            return nullptr;}
    destructGetList(temp);
    //cout << "loca " << locationInMem << endl;

    editArray(locationInMem, allocationWordcount);
    //cout << "WA" << endl;
    holeCounter();
    //cout << "Aw" << endl;
    partition.emplace_back((int*)&array[locationInMem*wordSize], allocationWordcount * wordSize);
    char *addy = &array[locationInMem*wordSize];
    //cout << "for" << endl;
    return static_cast<void*>(addy);
};
/*void MemoryManager::holeDeleter(int mem, int size) {

    for(int i =0; i < holeyHolder.size(); i++){
        if(holeyHolder[i].first == mem){
            cout << " happ[ened" << endl;
            int newMemSpot = mem + size;
            cout << newMemSpot << endl;
            if(newMemSpot != holeyHolder[i].second){
                holeyHolder[i].first = newMemSpot;

            }else {
                holeyHolder.erase(holeyHolder.begin() + i);
            }
            break;
        }
    }

}*/
void MemoryManager::holeCounter() {
    for(int i =0; i < holeyHolder.size(); i++){
        holeyHolder.clear();
    }
    int count = 0;
    int start = 0;
    int finish = 0;
    int holeCount = 0;
    //cout << array[0] << endl;
    //cout << holeyHolder.size() << "size " << endl;
    //cout << memSizeInBytes/wordSize <<  " mem / word " << endl;
    for(int i =0 ; i< memSizeInBytes; i++){
        //cout << i << memSizeInBytes-1 << endl;
        if(array[i] == '\0' && count == 0){
            start = i;
            count++;
            //cout << " #1count" << count << " start" << start << " finish" << finish << array[i] << i << endl;
        }else if(array[i] == '\0' && i != memSizeInBytes-1){
            //cout << " #2count" << count << " start" << start << " finish" << finish << "i " << i << endl;
            count++;
        }else if(array[i] == 'F' && count != 0 || i == memSizeInBytes-1 && start != 0 || i == memSizeInBytes-1 && array[i] == '\0'){
            finish = i + 1;
            count++;
            //cout << " #3count" << count << " start" << start << " finish" << finish << endl;
            int tempStart = (start/wordSize);
            int tempCount = (count/wordSize);
            uint16_t finalStart = tempStart;
            uint16_t finalCount = tempCount;
            holeyHolder.emplace_back(finalStart, finalCount);
            count = 0;
            start = 0;
            finish = 0;
        }
    }


}
void MemoryManager::free(void *address){
    int * ptr = (int*)address;
    //cout << *ptr << "frEE" << endl;
    int partitionNum = 0;
    int arrayNum = 0;
    for(int i = 0; i < partition.size(); i++){
        if(address == partition[i].first){
            //cout << "apple" << i << partition[i].second << endl;
            partitionNum = i;
        }
    }
    for(int i = 0; i < memSizeInBytes; i++){
        if(address == &array[i]){
            arrayNum = i;
            //cout << "orange" <<  i <<endl;
        }
    }
    for(int i = arrayNum; i < arrayNum + partition[partitionNum].second; i++){
        array[i] = '\0';
    }
    partition.erase(partition.begin() + partitionNum);
    //delete ptr;
    holeCounter();

};
void MemoryManager::editArray(int loc, int size) {
    //cout << " loc: " << loc << "size " << size << " w" << wordSize << endl;
    int locVal = loc * wordSize;
    int sizeVal = size * wordSize;
    //cout << " locVal : " << locVal << "size " << locVal + sizeVal << endl;
    for(int i = locVal; i < locVal+sizeVal; i++){
        //cout << "i:" <<i << " ";
        array[i] = 'F';
    }
    //cout << "262143: " << array[0] << array[262143] <<  array[262144] << endl;
};
void MemoryManager::printHoles() {
    //cout << endl;
    //cout << "PrintHoles: number of holes: " << holeyHolder.size() << " : " << endl;
    for(int i =0; i < holeyHolder.size(); i++){
        //cout << " "<< holeyHolder[i].first << " "<< holeyHolder[i].second << ", " << endl;
    }
}
void *MemoryManager::getList(){

    int numOfHoles = holeyHolder.size();
    if(numOfHoles == 0 ){
        return nullptr;
    }
    //cout << holeyHolder.size() << " size ";
    int sizeOfListArray = (numOfHoles * 2) + 1;
    uint16_t * listArray;
    listArray = new uint16_t [sizeOfListArray];
    listArray[0] = numOfHoles;
    int count = 0;
    bool temp = true;
    for(int i = 1; i < sizeOfListArray; i++){
        count = (i-1)/2;
        if(temp){
            listArray[i] = (uint16_t)holeyHolder[count].first;
            temp = false;
        }else {
            listArray[i] = (uint16_t)holeyHolder[count].second;
            temp = true;
        }
    }
    //cout << " get list: ";
    for(int i =0; i < sizeOfListArray; i++){
        //cout <<  listArray[i] << " ";
    }

    return listArray;
};

void MemoryManager::setAllocator(std::function<int(int, void*)> allocator){
    defaultAllocator = allocator;
};
bool MemoryManager::destructGetList(void * pointer) {
    delete[] pointer;
};
void MemoryManager::printArray(){
    //cout << "print array: ";
    for(int i =0; i < memSizeInBytes; i++){
        //cout <<  array[i] <<" ";
    }
}
unsigned MemoryManager::getMemoryLimit() {
    return memSizeInBytes;
}
void *MemoryManager::getBitmap() {
    //cout << " word count " << wordCount << endl;
    int totalWords = wordCount;
    int bitmapSize = totalWords / 8;
    if(totalWords % 8 > 0){
        bitmapSize++;
    }
    //cout << bitmapSize * wordSize << endl;
    //cout << wordSize << endl;
    int totalSize = (bitmapSize * wordSize);
    int map[bitmapSize * 8];
    //cout << bitmapSize * wordSize  <<  totalSize << endl;
    for(int i =0; i < totalWords; i++){
        map[i] = 1;
    }
    for(int i = totalWords; i < bitmapSize*8; i++){
        map[i] = 0;
    }
    //cout << " hap " << endl;
    for ( auto& hole : holeyHolder) {
        //cout << hole.first << " " << hole.second << endl;
        for (int i = hole.first; i < hole.first + hole.second; ++i) {
            map[i] = 0;
        }
    }
    for(int i =0; i < bitmapSize*8; i++){
        //cout << map[i] << " ";
    }
    //cout << " endl "<< endl;
    int length = bitmapSize * 8;
    for(int i = 0; i < length / 2; ++i) {
        swap(map[i], map[length - i - 1]);
    }
    for(int i =0; i < bitmapSize*8; i++){
       // cout << map[i] << " ";
    }
    //cout << " hap 2" << endl;

    uint8_t * bitmap= new uint8_t[bitmapSize + 2];
    uint8_t decimalNumber = 0;
    int count = 7;
    int temp = bitmapSize +1;
    int stop = totalSize - 16;
    //cout << bitmapSize << endl;
    for(int i = 0; i < bitmapSize; i++){
        for(int k = 8* i; k < 8*i + 8; k++ ){
            //cout << map[k] << " ";
            if (map[k] == 1) {
                decimalNumber += (uint8_t)(pow(2, count -1));
                }
            count--;
            }
        count = 8;
        //cout << temp << endl;
        bitmap[temp] = (uint8_t)decimalNumber;
        temp--;
        //cout << " dec " << decimalNumber;
        decimalNumber = 0;
    }
    short tempx = memSizeInBytes/wordSize;
    //cout << "bit map size" << bitmapSize << endl;
    //bitmap[0] = (uint8_t)bitmapSize;
    //bitmap[1] = (uint8_t)0;
    bitmap[0] = static_cast<uint8_t>(bitmapSize & 0xFF);
    bitmap[1] = static_cast<uint8_t>((bitmapSize >> 8) & 0xFF);

    for(int i =0; i < bitmapSize + 2; i++){
        //cout << std::hex <<(int)bitmap[i] << endl;
    }
   return reinterpret_cast<void *>(&bitmap[0]);

}

int MemoryManager::dumpMemoryMap(char *filename) {
    int fileDescriptor = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fileDescriptor == -1) {
        return fileDescriptor;
    }
    for (int i = 0; i < holeyHolder.size(); i++) {
        ostringstream ss;
        if (i + 1 != holeyHolder.size()) {
            ss << "[" << holeyHolder[i].first << ", " << holeyHolder[i].second << "] - ";
        } else {
            ss << "[" << holeyHolder[i].first << ", " << holeyHolder[i].second << "]";
        }
        string holeStr = ss.str();

        if (write(fileDescriptor, holeStr.c_str(), holeStr.size()) == -1) {
            close(fileDescriptor);
            return fileDescriptor;
        }

    }
    close(fileDescriptor);

    return fileDescriptor;
}