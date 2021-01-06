//
//  hashtable.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/20.
//

#ifndef hashtable_h
#define hashtable_h

#include"MurmurHash.h"
#include"basetable.h"

namespace CuckooHash {

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
