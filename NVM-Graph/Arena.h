//
//  Arena.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/17.
//

#ifndef Arena_h
#define Arena_h
#include<stdlib.h>

class Arena{
public:
    Arena(const uint NodeSize);
    Arena(const uint NodeSize,const size_t MaxSize);
    ~Arena();
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
Arena::Arena(const uint NodeSize,const size_t MaxSize){
    begin_ptr=static_cast<char*>(malloc(MaxSize));
    curAlloc_ptr=begin_ptr;
}
Arena::~Arena(){
    free(begin_ptr);
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
