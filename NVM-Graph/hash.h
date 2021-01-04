//
//  hash.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/30.
//

#ifndef hash_h
#define hash_h
#include <stdint.h>
#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif

typedef unsigned int uint;
template<class Key,class Dat>
class HashKeyDat{
public:
    uint64_t HashCd;
    Dat Data;
public:
    HashKeyDat():Data(),HashCd(0){}
    HashKeyDat(const uint64_t hashCd,const Dat& data):HashCd(hashCd),Data(data){}
    HashKeyDat(const HashKeyDat& hashKeyDat):HashCd(hashKeyDat.HashCd),Data(hashKeyDat.Data){}
    HashKeyDat& operator=(const HashKeyDat& hashKeyDat){
        HashCd=hashKeyDat.HashCd;
        Data=hashKeyDat.Data;
        return *this;
    }
    bool operator==(const HashKeyDat& hashKeyDat)const{
        return HashCd==hashKeyDat.HashCd;
    }
};
template<class Key,class Dat>
class HashKeyDatI{
public:
    typedef HashKeyDat<Key,Dat> HKeyDat;
private:
    HKeyDat* KeyDatI;
    HKeyDat* EndI;
public:
    HashKeyDatI():KeyDatI(NULL),EndI(NULL){}
    HashKeyDatI(HKeyDat* curI,HKeyDat* endI):KeyDatI(curI),EndI(endI){}
    HashKeyDatI(const HashKeyDatI& hashKeyDatI):KeyDatI(hashKeyDatI.KeyDatI),EndI(hashKeyDatI.EndI){}
    HashKeyDatI& operator=(const HashKeyDatI& hashKeyDatI){
        KeyDatI=hashKeyDatI.KeyDatI;
        EndI=hashKeyDatI.KeyDatI;
        return *this;
    }
    bool operator==(const HashKeyDatI& hashKeyDatI) const{return KeyDatI==hashKeyDatI.KeyDatI;}
    bool operator<(const HashKeyDatI& hashKeyDatI) const{return KeyDatI<hashKeyDatI.KeyDatI;}
    HashKeyDatI& operator++(int){
        KeyDatI++;
        while(KeyDatI<EndI&&KeyDatI->HashCd==0) KeyDatI++;
        return *this;
    }
    HashKeyDatI& operator--(int){
        KeyDatI--;
        while(KeyDatI==0) KeyDatI--;
        return *this;
    }
    bool IsEmpty()const{return KeyDatI==NULL;}
    bool IsEnd()const {return KeyDatI==EndI;}
    Dat& GetDat()const{return KeyDatI->Data;}
    uint64_t GetTag()const{return KeyDatI->HashCd;}
};

uint64_t MurmurHash64(const void* key, size_t len, unsigned int seed=17) {

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

template<class Key,class Dat>
class BaseTable{
public:
    typedef HashKeyDat<Key,Dat> HkeyDat;
    typedef HashKeyDatI<Key, Dat> HKeyDatI;
private:
    size_t BucketNum;
    size_t SlotNumPerBucket;
    HkeyDat* bucket;
public:
    BaseTable(const size_t num,const size_t slot):BucketNum(num),SlotNumPerBucket(slot){
        bucket=new HkeyDat[BucketNum*SlotNumPerBucket];
    }
    BaseTable(const size_t num,const size_t slot,const HkeyDat* src):BucketNum(num),SlotNumPerBucket(slot){
        bucket=new HkeyDat[BucketNum*SlotNumPerBucket];
        memcpy(bucket, src, BucketNum*SlotNumPerBucket*sizeof(HkeyDat));
    }
    ~BaseTable(){delete[] bucket;}
    const HkeyDat* GetBucketPtr()const{return bucket;}
    
    size_t GetSlotSize() const{return sizeof(HkeyDat);}
    size_t GetBucketSize() const {return GetSlotSize()*SlotNumPerBucket;}
    size_t GetSize() const{return GetBucketSize()*BucketNum;}
    uint64_t ReadTag(const size_t i,const size_t j) const{return bucket[i*SlotNumPerBucket+j].HashCd;}
    
    Dat ReadDat(const size_t i,const size_t j)const{return bucket[i*SlotNumPerBucket+j].Data;}
    void ReadSlot(const  size_t i,const size_t j,HkeyDat& p) const{
        p=bucket[i*SlotNumPerBucket+j];
    }
    void SetTag(const size_t i,const size_t j,const uint64_t & t){bucket[i*SlotNumPerBucket+j].HashCd=t;}
    void SetDat(const size_t i,const size_t j,const Dat* data){ bucket[i*SlotNumPerBucket+j].Data=data;}
    void ClearSlot(const size_t i,const size_t j){
        bucket[i*SlotNumPerBucket+j]=HkeyDat(0,Dat());
    }
    void WriteSlot(const  size_t i,const size_t j, const HkeyDat& p){
        bucket[i*SlotNumPerBucket+j]=p;
    }
    
    bool FindKeyInBucket(const size_t i1,const size_t i2, const uint64_t& tag, Dat& data) const{
        uint64_t curtag=0;
        for(int j=0;j<SlotNumPerBucket;++j){
            curtag=ReadTag(i1, j);
            if(curtag!=0 && curtag==tag){
                data=ReadDat(i1, j);
                return true;
            }
            if(i2!=i1){
                curtag=ReadTag(i2, j);
                if(curtag!=0 && curtag==tag){
                    data=ReadDat(i2, j);
                    return true;
                }
            }
        }
        return false;
    }
    bool IsKeyInBucket(const size_t i1,const size_t i2, const uint64_t& tag) const{
        int curtag=0;
        for(int j=0;j<SlotNumPerBucket;++j){
            curtag=ReadTag(i1, j);
            if(curtag!=0 && curtag==tag){
                return true;
            }
            if(i2!=i1){
                curtag=ReadTag(i2, j);
                if(curtag!=0 && curtag==tag){
                    return true;
                }
            }
        }
        return false;
    }
    
    bool DeleteSlotFromBucket(const size_t i, const uint64_t& tag){
        for(int j=0;j<SlotNumPerBucket;++j){
            if(ReadTag(i,j)==tag){
                ClearSlot(i, j);
                return true;
            }
        }
        return false;
    }
    
    bool InsertSlotToBucket(const size_t i, const HkeyDat& slot,const bool kicout,HkeyDat& oldslot){
        for(int j=0;j<SlotNumPerBucket;++j){
            if(ReadTag(i, j)==0){
                WriteSlot(i, j, slot);
                return true;
            }
        }
        if(kicout){
            size_t r=rand()&(SlotNumPerBucket-1);
            ReadSlot(i, r, oldslot);
            WriteSlot(i, r, slot);
        }
        return false;
    }
    size_t NumValidSlotInBucket(const size_t i) const{
        size_t num=0;
        for(int j=0;j<SlotNumPerBucket;++j){
            if(ReadTag(i, j)!=0) num++;
        }
        return num;
    }
    size_t NumValidSlot() const{
        size_t num=0;
        int i;
        for(i=0;i<BucketNum;++i) num+=NumValidSlotInBucket(i);
        return num;
    }
    size_t NumBucket()const{
        return BucketNum;
    }
    size_t NumSlot()const{
        return BucketNum*SlotNumPerBucket;
    }
    
    HKeyDatI Begin() const{
        HKeyDatI iter=HKeyDatI(bucket,bucket+SlotNumPerBucket*BucketNum);
        if(iter.GetTag()==0) iter++;
        return iter;
    }
    HKeyDatI End()const{
        return HKeyDatI(bucket+SlotNumPerBucket*BucketNum,bucket+SlotNumPerBucket*BucketNum);
    }
    HKeyDatI GetIter(HkeyDat* p)const{
        return HKeyDatI(p,bucket+SlotNumPerBucket*BucketNum);
    }
};
template<class Key,class Dat>
class HashTable{
public:
    typedef HashKeyDat<Key,Dat> HkeyDat;
    typedef HashKeyDatI<Key, Dat> HKeyDatI;
    typedef BaseTable<Key, Dat> Table;
private:
    int MaxCuckooKickCount=500;
    Table* table;
    size_t BucketNum;
    size_t slot_per_bucket;
    size_t ValidSlotNum;
    
    struct VictimItem{
        HkeyDat TempSlot;
        bool IsUse;
    };
    VictimItem victim;
    
    uint32_t IndexHash(const uint32_t prevhash) const{
        return prevhash &(table->NumBucket()-1);
    }
    uint32_t IndexHash(const uint32_t prevhash,const size_t bucketnum)const{
        return prevhash &(bucketnum-1);
    }
    void GenerateIndexTagHash(const Key& key,const size_t& length, size_t & index, uint64_t& tag) const{
        tag=MurmurHash64(&key,length);
        index=IndexHash(static_cast<uint32_t>(tag>>32));
        tag+=(tag==0);
    }
    size_t AltIndex(const size_t index,const uint64_t tag) const{
        return IndexHash((uint32_t)(index^((uint32_t)tag*0x5bd1e995)));
    }
    size_t AltIndex(const size_t index,const uint64_t tag,const size_t bn) const{
        return IndexHash((uint32_t)(index^((uint32_t)tag*0x5bd1e995)),bn);
    }
    bool AddImpl(const size_t index,const uint64_t tag,const Dat& data);
public:
    HashTable(const size_t& bn,const size_t& sn):BucketNum(bn),slot_per_bucket(sn),ValidSlotNum(0),victim(){
        table=new Table(BucketNum,slot_per_bucket);
    }
    HashTable(const HashTable& hashtable):BucketNum(hashtable.BucketNum),slot_per_bucket(hashtable.slot_per_bucket),ValidSlotNum(hashtable.ValidSlotNum),victim(hashtable.victim){
        table=new Table(BucketNum,slot_per_bucket,hashtable.table->GetBucketPtr());
    }
    ~HashTable(){delete table;}
    
    HashTable& operator=(const HashTable& hashtable){
        BucketNum=hashtable.BucketNum;
        slot_per_bucket=hashtable.slot_per_bucket;
        ValidSlotNum=hashtable.ValidSlotNum;
        victim=hashtable.victim;
        table=new Table(BucketNum,slot_per_bucket,hashtable.table->GetBucketPtr());
        return *this;
    }
    
    bool Add(const Key& key,const size_t& length,const Dat& data);
    bool Find(const Key& key, const size_t& length, Dat& data) const;
    bool IsIn(const Key& key, const size_t& length)const;
    bool Delete(const Key& key,const size_t& length);
    bool ReSize(const size_t& size=0);
    
    HKeyDatI Begin()const{return table->Begin();}
    HKeyDatI End()const{return table->End();}
    HKeyDatI GetIter(HkeyDat* p)const{return table->GetIter(p);}
    
    double LoadFactor()const{return 1.0*ValidSlotNum/(slot_per_bucket*BucketNum);}
    size_t GetBucketNum()const{return BucketNum;}
    size_t GetSlotNumPerBucket()const{return slot_per_bucket;}
    size_t GetValidSlotNum()const{return ValidSlotNum;}
    size_t GetSize()const{return table->GetSize();}
    
};
template<class Key,class Dat>
bool HashTable<Key,Dat>::ReSize(const size_t& size){
    size_t NewBucketNum;
    if(size==0) NewBucketNum=BucketNum*2;
    else NewBucketNum=size;
    Table* newtable=new Table(NewBucketNum,slot_per_bucket);
    HKeyDatI iter=table->Begin();
    HkeyDat oldslot;
    size_t curindex;
    while(!iter.IsEnd()){
        curindex=IndexHash((uint32_t)(iter.GetTag()>>32),NewBucketNum);
        HkeyDat curslot(iter.GetTag(),iter.GetDat());
        bool Flag=true;
        for (uint count=0; count<MaxCuckooKickCount; ++count) {
            bool kickout=count>0;
            if(newtable->InsertSlotToBucket(curindex,curslot , kickout, oldslot)){
                Flag=false;
                break;
            }
            if(kickout) curslot=oldslot;
            curindex=AltIndex(curindex, curslot.HashCd, NewBucketNum);
        }
        if(Flag){
            delete newtable;
            return false;
        }
        iter++;
    }
    if(victim.IsUse){
        HkeyDat curslot(victim.TempSlot);
        size_t curindex=IndexHash((uint32_t)(victim.TempSlot.HashCd>>32), NewBucketNum);
        bool Flag=true;
        for (uint count=0; count<MaxCuckooKickCount; ++count) {
            bool kickout=count>0;
            if(newtable->InsertSlotToBucket(curindex,curslot, kickout, oldslot)){
                Flag=false;
                break;
            }
            if(kickout) curslot=oldslot;
            curindex=AltIndex(curindex, curslot.HashCd, NewBucketNum);
        }
        if(Flag){
            delete newtable;
            return false;
        }
        victim.IsUse=false;
    }
    delete table;
    table=newtable;
    BucketNum=NewBucketNum;
    return true;
    }
template<class Key,class Dat>
bool HashTable<Key,Dat>::Add(const Key &key, const size_t& length, const Dat &data){
    size_t index;
    uint64_t tag;
    GenerateIndexTagHash(key, length, index, tag);
    return AddImpl(index, tag, data);
}
template<class Key,class Dat>
bool HashTable<Key,Dat>::AddImpl(const size_t index,const uint64_t tag,const Dat& data){
    HkeyDat curslot(tag,data);
    HkeyDat oldslot;
    size_t curindex=index;
    for(int count=0;count<MaxCuckooKickCount;++count){
        bool kicout=count>0;
        if(table->InsertSlotToBucket(curindex, curslot, kicout, oldslot)){
            ValidSlotNum++;
            return true;
        }
        if(kicout) curslot=oldslot;
        curindex=AltIndex(curindex, curslot.HashCd);
    }
    victim.TempSlot=oldslot;
    victim.IsUse=true;
    ValidSlotNum++;
    return false;
}
template<class Key,class Dat>
bool HashTable<Key,Dat>::Find(const Key& key,const size_t& length,Dat& data)const{
    size_t i1,i2;
    uint64_t tag;
    GenerateIndexTagHash(key,length, i1, tag);
    i2=AltIndex(i1, tag);
    if(victim.IsUse && victim.TempSlot.HashCd==tag){
        data=victim.TempSlot.Data;
        return true;
    }
    return table->FindKeyInBucket(i1, i2, tag, data);
}
template<class Key,class Dat>
bool HashTable<Key,Dat>::IsIn(const Key &key,const size_t& length)const{
    size_t i1,i2;
    uint64_t tag;
    GenerateIndexTagHash(key, length,i1, tag);
    i2=AltIndex(i1, tag);
    if(victim.TempSlot.HashCd==tag) return true;
    return table->IsKeyInBucket(i1, i2, tag);
}
template<class Key,class Dat>
bool HashTable<Key,Dat>::Delete(const Key &key,const size_t& length){
    size_t i1,i2;
    uint32_t tag;
    GenerateIndexTagHash(key, length, i1, tag);
    i2=AltIndex(i1, tag);
    if(victim.IsUse && victim.TempSlot.HashCd==tag){
        victim.IsUse=false;
        ValidSlotNum--;
        return true;
    }
    if(table->DeleteSlotFromBucket(i1, key)||table->DeleteSlotFromBucket(i2, key)){
        ValidSlotNum--;
        return true;
    }
    return false;
}
#endif /* hash_h */
