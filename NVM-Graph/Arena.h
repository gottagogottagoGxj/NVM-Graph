//
//  Arena.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/17.
//

#ifndef Arena_h
#define Arena_h
#include<cstring>
#include<stdlib.h>

typedef unsigned int uint;

const int NodeDatalengthDef=1;//默认节点标签最大长度是31个字节，最后一子节存'\0'
const int EdgeDataLengthDef=32;//默认边标签最大长度
const int InOutEidNumDef=10;//默认边数
const size_t InOutEdgeNumDef=10;//默认边数

class Arena{
public:
    Arena(const uint NodeSize);
    Arena(const uint NodeSize,const size_t MaxSize);
    Arena(const Arena& arena);
    ~Arena();
    Arena& operator=(const Arena& arena);
    
    char* AllocateBytes(size_t bytes);
    char* AllocateNode();
    void RecoverNode();
    const char* BeginPtr() const {return begin_ptr;}
    char* HeadPtr()const{return begin_ptr;}
    const char* EndPtr()const {return curAlloc_ptr;}
    const uint NodeSize()const{return EveryNodeSize; }
    size_t GetCurAllocateSize()const{return curAlloc_ptr-begin_ptr;}
    size_t GetCurAllocateNodeNum()const {return (curAlloc_ptr-begin_ptr)/(1LL*EveryNodeSize);}
    
private:
    char* begin_ptr;
    char* curAlloc_ptr;
    uint EveryNodeSize;
    size_t MaxSize;
    
};
Arena::Arena(const uint NodeSize): EveryNodeSize(NodeSize){
    begin_ptr=static_cast<char*>(malloc(1024));
    curAlloc_ptr=begin_ptr;
}
Arena::Arena(const uint NodeSize,const size_t maxSize):EveryNodeSize(NodeSize),MaxSize(maxSize){
    begin_ptr=static_cast<char*>(malloc(MaxSize));
    curAlloc_ptr=begin_ptr;
}
Arena::Arena(const Arena& arena):EveryNodeSize(arena.EveryNodeSize){
    begin_ptr=static_cast<char*>(malloc(1024));
    size_t length=arena.curAlloc_ptr-arena.begin_ptr;
    curAlloc_ptr=begin_ptr+length;
    memcpy(begin_ptr, arena.begin_ptr,length+1);
}
Arena::~Arena(){
    free(begin_ptr);
}

Arena& Arena:: operator=(const Arena& arena){
    EveryNodeSize=arena.EveryNodeSize;
    begin_ptr=static_cast<char*>(malloc(1024));
    size_t length=arena.curAlloc_ptr-arena.begin_ptr;
    curAlloc_ptr=begin_ptr+length;
    memcpy(begin_ptr, arena.begin_ptr,length+1);
    return *this;
}

inline char* Arena::AllocateNode(){
    char * old_ptr=curAlloc_ptr;
    curAlloc_ptr+=EveryNodeSize;
    return old_ptr;
}
inline char* Arena::AllocateBytes(size_t bytes){
    assert((curAlloc_ptr-begin_ptr+bytes)<MaxSize);
    char * old_ptr=curAlloc_ptr;
    curAlloc_ptr+=bytes;
    return old_ptr;
}

inline void Arena::RecoverNode(){
    if(curAlloc_ptr>begin_ptr) curAlloc_ptr-=EveryNodeSize;
}

class Comparison{
private:
    bool IsAsc;
public:
    Comparison(const bool& Asc=true):IsAsc(Asc){}
    bool operator()(const int& val1,const int& val2)const{
        if(IsAsc){return val1<val2;}
        else{return val2<val1;}
    }
};

void InsertSortCmp(int Arr[],int Start,int End,const Comparison& Cmp);
int ParitionCmp(int Arr[],int Start,int End,const Comparison& Cmp);
void QuickSortCmp(int Arr[],int Start,int End,const Comparison& Cmp);

int SearchBin(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp);
int SearchBinRight(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp);

//数组排序、二分查找(正序、逆序）
void InsertSortCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    for(int i=Start+1;i<End;++i){
        int value=Arr[i];
        int j=i;
        while(j>Start && Cmp(value,Arr[j-1])){Arr[j]=Arr[j-1];j--;}
        Arr[j]=value;
    }
}
int ParitionCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    int index=End-1;
    int key=Arr[index];
    End--;
    int temp1;
    while (Start<End) {
        while(Start<End &&Cmp(Arr[Start],key)) Start++;
        End--;
        while(Start<End && Cmp(key,Arr[End])) End--;
        if(Start<End){
            temp1=Arr[Start];
            Arr[Start]=Arr[End];
            Arr[End]=temp1;
            Start++;
        }
    }
    Arr[index]=Arr[Start];
    Arr[Start]=key;
    return Start;
}
void QuickSortCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    if(Start+1<End){
        if(End-Start<20){InsertSortCmp(Arr, Start,End, Cmp);}
        else{
            int splite=ParitionCmp(Arr, Start, End, Cmp);
            QuickSortCmp(Arr, Start, splite, Cmp);
            QuickSortCmp(Arr, splite+1, End, Cmp);
        }
    }
}

int SearchBin(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp){
    End--;
    while(Start<=End){
        int mid=(Start+End)>>1;
        if(Arr[mid]==key) return mid;
        if(Cmp(key,Arr[mid])) End=mid-1;
        else Start=mid+1;
    }
    return -1;
}
int SearchBinRight(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp){
    while (Start<End) {
        int mid=(Start+End)>>1;
        if((Cmp(key,Arr[mid]))){End=mid;}
        else Start=mid+1;
    }
    return Start;
}
/*
 // NVMArena NVM中空间分配
 #include<libpmemobj.h>
 #ifndef Arena_h
 #define Arena_h



 class Arena {
 public:
     Arena() :begin_ptr(nullptr), curAlloc_ptr(nullptr), EveryNodeSize(0) {}
     Arena(const uint NodeSize);
     Arena(const uint NodeSize, const size_t MaxSize);
     Arena(const Arena& arena);
     virtual ~Arena();
     Arena& operator=(const Arena& arena);

     virtual char* AllocateBytes(size_t bytes);
     virtual char* AllocateNode();
     virtual void RecoverNode();
     virtual const char* BeginPtr() const { return begin_ptr; }
     virtual char* HeadPtr()const { return begin_ptr; }
     virtual const char* EndPtr()const { return curAlloc_ptr; }
     virtual const uint NodeSize()const { return EveryNodeSize; }
     virtual size_t GetCurAllocateSize()const { return curAlloc_ptr - begin_ptr; }
     virtual size_t GetCurAllocateNodeNum()const { return (curAlloc_ptr - begin_ptr) / (1LL * EveryNodeSize); }
     virtual void Recover(){curAlloc_ptr=begin_ptr;}

 private:
     char* begin_ptr;
     char* curAlloc_ptr;
     uint EveryNodeSize;

 };
 Arena::Arena(const uint NodeSize) : EveryNodeSize(NodeSize) {
     begin_ptr = static_cast<char*>(malloc(1024));
     curAlloc_ptr = begin_ptr;
 }
 Arena::Arena(const uint NodeSize, const size_t MaxSize) : EveryNodeSize(NodeSize) {
     begin_ptr = static_cast<char*>(malloc(MaxSize));
     curAlloc_ptr = begin_ptr;
 }
 Arena::Arena(const Arena& arena) : EveryNodeSize(arena.EveryNodeSize) {
     begin_ptr = static_cast<char*>(malloc(1024));
     size_t length = arena.curAlloc_ptr - arena.begin_ptr;
     curAlloc_ptr = begin_ptr + length;
     memcpy(begin_ptr, arena.begin_ptr, length + 1);
 }
 Arena::~Arena() {
     free(begin_ptr);
 }

 Arena& Arena:: operator=(const Arena& arena) {
     EveryNodeSize = arena.EveryNodeSize;
     begin_ptr = static_cast<char*>(malloc(1024));
     size_t length = arena.curAlloc_ptr - arena.begin_ptr;
     curAlloc_ptr = begin_ptr + length;
     memcpy(begin_ptr, arena.begin_ptr, length + 1);
     return *this;
 }

 inline char* Arena::AllocateNode() {
     char* old_ptr = curAlloc_ptr;
     curAlloc_ptr += EveryNodeSize;
     return old_ptr;
 }
 inline char* Arena::AllocateBytes(size_t bytes) {
     char* old_ptr = curAlloc_ptr;
     curAlloc_ptr += bytes;
     return old_ptr;
 }

 inline void Arena::RecoverNode() {
     if (curAlloc_ptr > begin_ptr) curAlloc_ptr -= EveryNodeSize;
 }

 POBJ_LAYOUT_BEGIN(store);
 POBJ_LAYOUT_ROOT(store, struct table);
 POBJ_LAYOUT_TOID(store, char);
 POBJ_LAYOUT_END(store);

 struct table {
     TOID(char) Begin;
     size_t CurSize;
     size_t MaxSize;
 };

 class NvmArena :public Arena {
 private:
     char* begin_ptr;
     char* curAlloc_ptr;
     uint EveryNodeSize;

     PMEMobjpool* pool;
     TOID(struct table) root;
 public:
     NvmArena(const uint NodeSize, const char* path, const size_t Max = 1024) :EveryNodeSize(NodeSize) {
         pool = pmemobj_create(path, POBJ_LAYOUT_NAME(store), PMEMOBJ_MIN_POOL *32 , 0666);
         if (pool == NULL) {
             pool = pmemobj_open(path, POBJ_LAYOUT_NAME(store));
             if (pool == NULL) { perror("pool is NUll"); }
             root = POBJ_ROOT(pool, struct table);
             begin_ptr = (char*)pmemobj_direct(D_RW(root)->Begin.oid);
             curAlloc_ptr = begin_ptr + D_RO(root)->CurSize;
         }
         else {
             root = POBJ_ROOT(pool, struct table);
             D_RW(root)->CurSize = 0;
             D_RW(root)->MaxSize = Max * EveryNodeSize;
             TOID(char) begin = OID_NULL;
             POBJ_ALLOC(pool, &begin, char, Max * EveryNodeSize, NULL, NULL);
             D_RW(root)->Begin = begin;
             begin_ptr = (char*)pmemobj_direct(D_RW(root)->Begin.oid);
             curAlloc_ptr = begin_ptr;
         }
     }
     virtual ~NvmArena() {
         if (pool != NULL) {
             pmemobj_close(pool);
         }
     }
     void Resize(const size_t MaxSize);
     virtual char* AllocateBytes(size_t bytes);
     virtual char* AllocateNode();
     virtual void RecoverNode();
     virtual const char* BeginPtr() const { return begin_ptr; }
     virtual char* HeadPtr()const { return begin_ptr; }
     virtual const char* EndPtr()const { return curAlloc_ptr; }
     virtual const uint NodeSize()const { return EveryNodeSize; }
     virtual size_t GetCurAllocateSize()const { return D_RO(root)->CurSize; }
     virtual size_t GetCurAllocateNodeNum()const { return D_RO(root)->CurSize / (1LL * EveryNodeSize); }
     virtual void Recover();

 };

 inline void NvmArena::Resize(const size_t MaxSize) {
     TOID(char) begin = OID_NULL;
     POBJ_ALLOC(pool, &begin, char, MaxSize, NULL, NULL);
     pmemobj_memcpy_persist(pool, pmemobj_direct(begin.oid), pmemobj_direct(D_RO(root)->Begin.oid), D_RO(root)->CurSize);
     TOID(char) old = D_RW(root)->Begin;
     D_RW(root)->Begin = begin;
     D_RW(root)->MaxSize = MaxSize;
     begin_ptr = (char*)pmemobj_direct(begin.oid);
     curAlloc_ptr = begin_ptr + D_RO(root)->CurSize;
     POBJ_FREE(&old);
 }

 inline char* NvmArena::AllocateBytes(size_t bytes) {
     if (D_RO(root)->CurSize + bytes > D_RO(root)->MaxSize) {
         Resize(D_RO(root)->MaxSize * 2);
     }
     char* old_ptr = curAlloc_ptr;
     curAlloc_ptr += bytes;
     D_RW(root)->CurSize = D_RO(root)->CurSize + bytes;
     //pmemobj_persist(pool, old_ptr, bytes);
     return old_ptr;
 }
 inline char* NvmArena::AllocateNode() {
     if (D_RO(root)->CurSize + EveryNodeSize > D_RO(root)->MaxSize) { Resize(D_RO(root)->MaxSize * 2); }
     char* old_ptr = curAlloc_ptr;
     D_RW(root)->CurSize = D_RO(root)->CurSize + EveryNodeSize;
     curAlloc_ptr += EveryNodeSize;
     pmemobj_persist(pool, old_ptr, EveryNodeSize);
     return old_ptr;
 }
 inline void NvmArena::RecoverNode() {
     if (curAlloc_ptr > begin_ptr) curAlloc_ptr -= EveryNodeSize;
     D_RW(root)->CurSize = D_RO(root)->CurSize - EveryNodeSize;
 }
 inline void NvmArena::Recover(){
     curAlloc_ptr=begin_ptr;
     D_RW(root)->CurSize =0;
 }

 #endif
 */



#endif /* Arena_h */
