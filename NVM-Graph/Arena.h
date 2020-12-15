//
//  Arena.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/17.
//

#ifndef Arena_h
#define Arena_h
#include<stdlib.h>

typedef unsigned int uint;

const int NodeDatalengthDef=32;//默认节点标签最大长度是31个字节，最后一子节存'\0'
const int EdgeDataLengthDef=32;//默认边标签最大长度
const int InOutEidNumDef=10;//默认边数
const size_t InOutEdgeNumDef=10;

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
    
};
Arena::Arena(const uint NodeSize): EveryNodeSize(NodeSize){
    begin_ptr=static_cast<char*>(malloc(1024));
    curAlloc_ptr=begin_ptr;
}
Arena::Arena(const uint NodeSize,const size_t MaxSize):EveryNodeSize(NodeSize){
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
    char * old_ptr=curAlloc_ptr;
    curAlloc_ptr+=bytes;
    return old_ptr;
}

inline void Arena::RecoverNode(){
    if(curAlloc_ptr>begin_ptr) curAlloc_ptr-=EveryNodeSize;
}

#endif /* Arena_h */
