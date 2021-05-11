//
//  hashtable.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/20.
//
#include<assert.h>
#include"InOut.h"

#ifndef hashtable_h
#define hashtable_h

namespace CuckooHash {

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
#include <stdint.h>
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif
uint64_t MurmurHash64(const void* key, int len, unsigned int seed=17) {

    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^(len * m);

    const uint64_t *data = (const uint64_t *) key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *) data;

    switch (len & 7) {
        case 7:
            h ^= ((uint64_t) data2[6]) << 48;
            FALLTHROUGH_INTENDED;
        case 6:
            h ^= ((uint64_t) data2[5]) << 40;
            FALLTHROUGH_INTENDED;
        case 5:
            h ^= ((uint64_t) data2[4]) << 32;
            FALLTHROUGH_INTENDED;
        case 4:
            h ^= ((uint64_t) data2[3]) << 24;
            FALLTHROUGH_INTENDED;
        case 3:
            h ^= ((uint64_t) data2[2]) << 16;
            FALLTHROUGH_INTENDED;
        case 2:
            h ^= ((uint64_t) data2[1]) << 8;
            FALLTHROUGH_INTENDED;
        case 1:
            h ^= ((uint64_t) data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}


class HashTable{
private:
    size_t MaxCuckooKickCount=500;
    BaseTable* table;
    size_t BucketNum;
    size_t slot_per_bucket;
    size_t ValidSlotNum;
    
    struct VictimItem{
        Pair TempSlot;
        bool IsUse;
    };
    VictimItem victim;
    
    uint32_t IndexHash(const uint32_t prevhash) const{
        return prevhash &(table->NumBucket()-1);
    }
    uint32_t TagHash(const uint32_t lasthash) const{
        uint32_t tag=lasthash;
        tag+=(tag==0);
        return tag;
    }
    void GenerateIndexTagHash(const int& key,size_t & index, uint32_t& tag) const{
        const uint64_t hashvalue=MurmurHash64(&key,sizeof(int));
        index=IndexHash(static_cast<uint32_t>(hashvalue>>32));
        tag=TagHash((uint32_t)hashvalue);
    }
    size_t AltIndex(const size_t index,const uint32_t tag) const{
        return IndexHash((uint32_t)(index^(tag*0x5bd1e995)));
    }
    bool AddImpl(const int& key,const size_t index,const uint32_t tag,const uint64_t& location);
    
    
    
    //用于扩容重新计算index,根据NewBucketNum
    uint32_t IndexHash(const uint32_t hash,const size_t bucketnum)const{
        return hash &(bucketnum-1);
    }
    void GenerateIndexTagHash(const int& key,const size_t bn,size_t & index, uint32_t& tag) const{
        const uint64_t hashvalue=MurmurHash64(&key,sizeof(int));
        index=IndexHash(static_cast<uint32_t>(hashvalue>>32),bn);
        tag=TagHash((uint32_t)hashvalue);
    }
    size_t AltIndex(const size_t index,const uint32_t tag,const size_t bn) const{
        return IndexHash((uint32_t)(index^(tag*0x5bd1e995)),bn);
    }
    
public:
    HashTable(const size_t& bn,const size_t& sn):BucketNum(bn),slot_per_bucket(sn),ValidSlotNum(0),victim(){
        table=new BaseTable(BucketNum,slot_per_bucket);
    }
    HashTable(const HashTable& hashtable):BucketNum(hashtable.BucketNum),slot_per_bucket(hashtable.slot_per_bucket),ValidSlotNum(hashtable.ValidSlotNum),victim(hashtable.victim){
        table=new BaseTable(BucketNum,slot_per_bucket,hashtable.table->GetBucketPtr());
    }
    ~HashTable(){delete table;}
    
    HashTable& operator=(const HashTable& hashtable){
        BucketNum=hashtable.BucketNum;
        slot_per_bucket=hashtable.slot_per_bucket;
        ValidSlotNum=hashtable.ValidSlotNum;
        victim=hashtable.victim;
        table=new BaseTable(BucketNum,slot_per_bucket,hashtable.table->GetBucketPtr());
        return *this;
    }
    
    bool Add(const int& key,const uint64_t& location);
    bool Find(const int& key,uint64_t& loaction) const;
    bool IsIn(const int& key)const;
    bool Delete(const int& key);
    bool ReSize(const size_t& NewBucketNum);
    
    IterPair Begin()const{return table->Begin();}
    IterPair End()const{return table->End();}
    IterPair GetIter(Pair* p)const{
        return table->GetIter(p);
    }
    
    double LoadFactor()const{return 1.0*ValidSlotNum/(slot_per_bucket*BucketNum);}
    size_t GetBucketNum()const{return BucketNum;}
    size_t GetSlotNumPerBucket()const{return slot_per_bucket;}
    size_t GetValidSlotNum()const{return ValidSlotNum;}
    size_t GetSize()const{return table->GetSize();}
    
};
bool HashTable::ReSize(const size_t &NewBucketNum){
    BaseTable* newtable=new BaseTable(NewBucketNum,slot_per_bucket);
    IterPair iter=table->Begin();
    Pair oldpair;
    size_t curindex;
    uint32_t tag;
    while(!iter.IsEnd()){
        GenerateIndexTagHash(iter.GetKey(), NewBucketNum, curindex, tag);
        Pair CurPair(tag,iter.GetKey(),iter.GetLocation());
        bool flag=true;
        for(uint count=0;count<MaxCuckooKickCount;++count){
            bool kickout=count>0;
            if(newtable->InsertSlotToBucket(curindex, CurPair, kickout, oldpair)){
                flag=false;
                break;
            }
            if(kickout) CurPair=oldpair;
            curindex=AltIndex(curindex, CurPair.tag, NewBucketNum);
        }
        if(flag) {
            delete newtable;
            return false;
        }
        iter++;
    }
    if(victim.IsUse){
        GenerateIndexTagHash(victim.TempSlot.key, NewBucketNum, curindex, tag);
        Pair CurPair(tag,victim.TempSlot.key,victim.TempSlot.location);
        bool flag=true;
        for(uint count=0;count<MaxCuckooKickCount;++count){
            bool kickout=count>0;
            if(newtable->InsertSlotToBucket(curindex, CurPair, kickout, oldpair)){
                flag=false;
                break;
            }
            if(kickout) CurPair=oldpair;
            curindex=AltIndex(curindex, CurPair.tag, NewBucketNum);
        }
        if(flag){
            delete newtable;
            return false;}
        victim.IsUse=false;
    }
    delete table;
    table=newtable;
    BucketNum=NewBucketNum;
    return true;
}

bool HashTable::Add(const int& key,const uint64_t& location){
    size_t index;
    uint32_t tag;
    GenerateIndexTagHash(key, index, tag);
    if(AddImpl(key,index, tag, location)) return true;
    size_t newbucketnum=BucketNum*2;
    while(!ReSize(newbucketnum)) newbucketnum*=2;
    return true;
}
bool HashTable::AddImpl(const int& key,const size_t index,const uint32_t tag,const uint64_t& location){
    Pair curslot(tag,key,location);
    Pair oldslot;
    size_t curindex=index;
    for(uint count=0;count<MaxCuckooKickCount;++count){
        bool kicout=count>0;
        if(table->InsertSlotToBucket(curindex, curslot, kicout, oldslot)){
            ValidSlotNum++;
            return true;
        }
        if(kicout) curslot=oldslot;
        curindex=AltIndex(curindex, curslot.tag);
    }
    victim.TempSlot=oldslot;
    victim.IsUse=true;
    ValidSlotNum++;
    return false;
}

bool HashTable::Find(const int& key,uint64_t& location)const{
    if(victim.IsUse && victim.TempSlot.key==key){
        location=victim.TempSlot.location;
        return true;
    }
    size_t i1,i2;
    uint32_t tag;
    GenerateIndexTagHash(key, i1, tag);
    i2=AltIndex(i1, tag);
    return table->FindKeyInBucket(i1, i2, key, location);
}
bool HashTable::IsIn(const int &key)const{
    if(victim.IsUse && victim.TempSlot.key==key){
        return true;
    }
    size_t i1,i2;
    uint32_t tag;
    GenerateIndexTagHash(key, i1, tag);
    i2=AltIndex(i1, tag);
    return table->IsKeyInBucket(i1, i2, key);
}


bool HashTable::Delete(const int &key){
    if(victim.IsUse && victim.TempSlot.key==key){
        victim.IsUse=false;
        ValidSlotNum--;
        return true;
    }
    size_t i1,i2;
    uint32_t tag;
    GenerateIndexTagHash(key, i1, tag);
    i2=AltIndex(i1, tag);
    if(table->DeleteSlotFromBucket(i1, key)||table->DeleteSlotFromBucket(i2, key)){
        ValidSlotNum--;
        return true;
    }
    return false;
}

}

#endif /* hashtable_h */
