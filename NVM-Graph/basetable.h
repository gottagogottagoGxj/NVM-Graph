//
//  basetable.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/20.
//

#ifndef basetable_h
#define basetable_h


#include<assert.h>
#include"InOut.h"

namespace CuckooHash {
//
class Pair{
public:
    uint32_t tag;
    int key;
    uint64_t location;
public:
    Pair():tag(),key(-1),location(){}
    Pair(const Pair& p):tag(p.tag),key(p.key),location(p.location){}
    Pair(const uint32_t& t,const int& k,const uint64_t& l):tag(t),key(k),location(l){}
    Pair(MOut& Out){
        Out.Load(tag);
        Out.Load(key);
        Out.Load(location);
    }
    void Save(MIn& In){
        In.Save(tag);
        In.Save(key);
        In.Save(location);
    }
    size_t GetSize()const{return sizeof(uint32_t)+sizeof(int)+sizeof(uint64_t);}
    
    bool operator ==(const Pair& p) const  {
        return (p.tag==tag) && (p.key==key)&&(p.location==location);
    }
    bool operator !=(const Pair& p) const{return !(*this==p);}
    Pair& operator=(const Pair& p){
        if(this!=&p){
            tag=p.tag;
            key=p.key;
            location=p.location;
        }
        return *this;
    }
    
};
class IterPair{
private:
    Pair* CurPairI;
    Pair* EndPairI;
public:
    IterPair():CurPairI(NULL),EndPairI(NULL){}
    IterPair(const IterPair& _IterPair)
    :CurPairI(_IterPair.CurPairI),EndPairI(_IterPair.EndPairI){}
    IterPair(Pair* c,Pair* e):CurPairI(c),EndPairI(e){}
    IterPair& operator=(const IterPair& _IterPair){
        CurPairI=_IterPair.CurPairI;
        EndPairI=_IterPair.EndPairI;
        return *this;
    }
    bool operator==(const IterPair& _IterPair)const{
        return CurPairI==_IterPair.CurPairI;
    }
    bool operator<(const IterPair& _IterPair) const{
        return CurPairI<_IterPair.CurPairI;
    }
    IterPair& operator++(int){
        CurPairI++;
        while(CurPairI<EndPairI && CurPairI->tag==0){CurPairI++;}
        return *this;
    }
    IterPair& operator--(int){
        CurPairI--;
        while(CurPairI->tag==0){CurPairI--;}
        return *this;
    }
    Pair& operator*() const{return *CurPairI;}
    Pair& operator()() const{return *CurPairI;}
    Pair* operator->() const{return CurPairI;}
    IterPair& Next(){operator++(1);return *this;}
    bool IsEmpty() const{return CurPairI==NULL;}
    bool IsEnd() const{return CurPairI==EndPairI;}
    uint32_t& GetTag()const{
        assert(CurPairI!=NULL);
        return CurPairI->tag;
    }
    int& GetKey()const{
        assert(CurPairI!=NULL);
        return CurPairI->key;
    }
    uint64_t& GetLocation()const{
        assert(CurPairI!=NULL);
        return CurPairI->location;
    }
};

class BaseTable{
private:
    size_t BucketNum;
    size_t SlotPerBucket;
    Pair* bucket;
public:
    BaseTable(const size_t num,const size_t slot):BucketNum(num),SlotPerBucket(slot){
        bucket=new Pair[BucketNum*SlotPerBucket];
    }
    BaseTable(const size_t num,const size_t slot,const Pair* src):BucketNum(num),SlotPerBucket(slot){
        bucket=new Pair[BucketNum*SlotPerBucket];
        memcpy(bucket, src, BucketNum*SlotPerBucket*sizeof(Pair));
    }
    ~BaseTable(){delete[] bucket;}
    const Pair* GetBucketPtr()const{return bucket;}
    
    size_t GetSlotSize() const{return sizeof(Pair);}
    size_t GetBucketSize() const {return GetSlotSize()*SlotPerBucket;}
    size_t GetSize() const{return GetBucketSize()*BucketNum;}
    uint32_t ReadTag(const size_t i,const size_t j) const{
        return bucket[i*SlotPerBucket+j].tag;
    }
    int Readkey(const size_t i,const size_t j) const{
        return bucket[i*SlotPerBucket+j].key;
    }
    uint64_t ReadLocation(const size_t i,const size_t j) const{
        return bucket[i*SlotPerBucket+j].location;
    }
    void ReadSlot(const  size_t i,const size_t j,Pair& p) const{
        p.tag=ReadTag(i, j);
        p.key=Readkey(i, j);
        p.location=ReadLocation(i, j);
    }
    void SetTag(const size_t i,const size_t j,const uint32_t & t){
        bucket[i*SlotPerBucket+j].tag=t;
    }
    void SetKey(const size_t i,const size_t j,const int& k){
        bucket[i*SlotPerBucket+j].key=k;
    }
    void SetLocation(const size_t i,const size_t j,const uint64_t & l){
        bucket[i*SlotPerBucket+j].location=l;
    }
    
    void ClearSlot(const size_t i,const size_t j){
        SetTag(i, j, 0);
        SetKey(i, j, 0);
        SetLocation(i, j, 0);
    }
    
    void WriteSlot(const  size_t i,const size_t j, const Pair& p){
        SetTag(i, j, p.tag);
        SetKey(i, j, p.key);
        SetLocation(i, j, p.location);
    }
    
    bool FindKeyInBucket(const size_t i1,const size_t i2, const int& key, uint64_t& location) const{
        int curkey=0;
        for(int j=0;j<SlotPerBucket;++j){
            curkey=Readkey(i1, j);
            if(curkey==key){
                location=ReadLocation(i1, j);
                return true;
            }
            if(i2!=i1){
                curkey=Readkey(i2, j);
                if(curkey==key){
                    location=ReadLocation(i2, j);
                    return true;
                }
            }
        }
        return false;
    }
    bool IsKeyInBucket(const size_t i1,const size_t i2, const int& key) const{
        int curkey=0;
        for(int j=0;j<SlotPerBucket;++j){
            curkey=Readkey(i1, j);
            if(curkey==key){
                return true;
            }
            if(i2!=i1){
                curkey=Readkey(i2, j);
                if(curkey==key){
                    return true;
                }
            }
        }
        return false;
    }
    
    bool DeleteSlotFromBucket(const size_t i, const int& key){
        for(int j=0;j<SlotPerBucket;++j){
            if(Readkey(i, j)==key){
                ClearSlot(i, j);
                return true;
            }
        }
        return false;
    }
    
    bool InsertSlotToBucket(const size_t i, const Pair& slot,const bool kicout,Pair& oldslot){
        for(int j=0;j<SlotPerBucket;++j){
            if(ReadTag(i, j)==0){
                WriteSlot(i, j, slot);
                return true;
            }
        }
        if(kicout){
            size_t r=rand()&(SlotPerBucket-1);
            ReadSlot(i, r, oldslot);
            WriteSlot(i, r, slot);
        }
        return false;
    }
    size_t NumSlotInBucket(const size_t i) const{
        size_t num=0;
        for(int j=0;j<SlotPerBucket;++j){
            if(ReadTag(i, j)==0) num++;
        }
        return num;
    }
    size_t NumValidSlot() const{
        size_t num=0;
        int i;
        for(i=0;i<BucketNum;++i) num+=NumSlotInBucket(i);
        return num;
    }
    size_t NumBucket()const{
        return BucketNum;
    }
    size_t NumSlot()const{
        return BucketNum*SlotPerBucket;
    }
    
    IterPair Begin() const{
        IterPair  iter(bucket,bucket+SlotPerBucket*BucketNum);
        if(iter.GetTag()==0) iter++;
        return iter;
    }
    IterPair End()const{
        return IterPair(bucket+SlotPerBucket*BucketNum,bucket+SlotPerBucket*BucketNum);
    }
    IterPair GetIter(Pair* p)const{
        return IterPair(p,bucket+SlotPerBucket*BucketNum);
    }
};
}
#endif /* basetable_h */
