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

#endif /* Arena_h */
