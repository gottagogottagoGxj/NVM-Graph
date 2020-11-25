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
    Arena(int NodeSize);
    ~Arena();
    char* AllocateBytes(size_t bytes);
    char* AllocateNode();
    void RecoverNode();
    const char* BeginPtr() const {return begin_ptr;}
    const int NodeSize()const{return EveryNodeSize; }
    size_t GetCurAllocateSize()const{return curAlloc_ptr-begin_ptr;}
    size_t GetCurAllocateNodeNum()const {return (curAlloc_ptr-begin_ptr)/(1LL*EveryNodeSize);}
    
private:
    char* begin_ptr;
    char* curAlloc_ptr;
    int EveryNodeSize;
    
};
Arena::Arena(int NodeSize): EveryNodeSize(NodeSize){
    begin_ptr=static_cast<char*>(malloc(1024));
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
