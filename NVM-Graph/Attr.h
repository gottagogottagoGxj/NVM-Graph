//
//  Attr.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/29.
//

#ifndef Attr_h
#define Attr_h
#include"NVMSkiplist.h"
#include<string>
#include"hash.h"
#include"NVMSkiplist.h"
#include"InOut.h"

// 目前属性分配也是利用Arena，没有空闲空间管理
class Attr{
private:
    HashTable<const char*,int> AttrNameToId;
    HashTable<int,size_t> AttrIdToName;
    Arena* NameTable;
    NVMSkipList<AttrIndexKey,Comparator<AttrIndexKey>> AttrIndex;
    Arena* AttrTable;
    int MxAttrId;
    
public:
    Attr(Arena* nametable, Arena* attrindex,Arena* attrtable):NameTable(nametable),AttrIndex(Comparator<AttrIndexKey>(),attrindex),AttrTable(attrtable),AttrNameToId(32,4),AttrIdToName(32,4),MxAttrId(0){
        size_t maxlength=NameTable->EndPtr()-NameTable->BeginPtr();
        if(maxlength!=0){
            MOut NVMout(NameTable->BeginPtr(),maxlength);
            while (NVMout.Len()<NVMout.MaxLen()) {
                bool flag;
                int attrid,length;
                size_t offset=NVMout.Len();
                NVMout.Load(flag);
                NVMout.Load(attrid);
                NVMout.Load(length);
                const int namelength=length;
                char attrname[namelength];
                NVMout.Load(attrname, namelength);
                if(!flag) continue;
                AttrNameToId.Add(attrname, namelength, attrid);
                AttrIdToName.Add(attrid, sizeof(int), offset);
                MxAttrId=MxAttrId>(attrid+1)? MxAttrId:(attrid+1);
            }
        }
    }
    bool AddAttrDat(const int& Id,const char* AttrName,const void* Val,const uint& length);//若AttrName不存在返回false,若该属性已存在，返回false
    bool AddAttrDat(const int& Id,const int& AttrId,const void* Val,const uint& length);
    bool GetAttrDat(const int& Id,const char* AttrName,void* Val) const;
    bool GetAttrDat(const int& Id,const int& AttrId,void* Val)const;
    void UpdateAttrDat(const int& Id,const char* AttrName,const void* Val,const uint& length);
    void UpdateAttrDat(const int& Id,const int& AttrId,const void* Val,const uint& length);
    void DelAttrDat(const int& Id,const char* AttrName);
    void DelAttrDat(const int& Id,const int& AttrId);
    void DelAttrDat(const int& Id);
    int AddAttrName(const char* AttrName);
    bool GetAttrId(int& AttrId,const char* AttrName)const;
    bool GetAttrName(const int& AttrId,char* AttrName)const;
    
};

bool Attr::AddAttrDat(const int& Id,const char* AttrName,const void* Val,const uint& length){
    int AttrId;
    if(! AttrNameToId.Find(AttrName,strlen(AttrName),AttrId)) return false;
    return AddAttrDat(Id,AttrId,Val,length);
}
bool Attr::AddAttrDat(const int& Id,const int& AttrId,const void* Val, const uint& length){
    size_t location;
    if(!AttrIdToName.Find(AttrId,sizeof(int),location)) return false;
    AttrIndexKey indexkey(Id,AttrId);
    if(AttrIndex.Find(indexkey)) return false;
    int Length=length+sizeof(int);
    char* curptr=AttrTable->AllocateBytes(Length);
    MIn NVMIn(curptr,Length);
    NVMIn.Save(length);
    NVMIn.Save(Val,length);
    indexkey.Location=curptr-AttrTable->BeginPtr();
    AttrIndex.Insert(indexkey);
    return true;
}
bool Attr::GetAttrDat(const int &Id, const char *AttrName, void *Val)const{
    int AttrId;
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return false;
    return GetAttrDat(Id, AttrId, Val);
}
bool Attr::GetAttrDat(const int &Id, const int &AttrId, void *Val)const{
    size_t location;
    if(!AttrIdToName.Find(AttrId, sizeof(int), location)) return false;
    AttrIndexKey indexkey(Id,AttrId);
    if(!AttrIndex.Find(indexkey)) return false;
    MOut NVMout(AttrTable->HeadPtr()+indexkey.Location,AttrTable->EndPtr()-AttrTable->BeginPtr()-indexkey.Location);
    int length;
    NVMout.Load(length);
    NVMout.Load(Val, length);
    return true;
}
void Attr::UpdateAttrDat(const int &Id, const char *AttrName, const void *Val, const uint &length){
    int AttrId;
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return;
    UpdateAttrDat(Id, AttrId, Val, length);
}
void Attr::UpdateAttrDat(const int &Id, const int &AttrId, const void *Val, const uint &length){
    int Length=length+sizeof(int);
    char* curptr=AttrTable->AllocateBytes(Length);
    MIn NVMIn(curptr,Length);
    NVMIn.Save(length);
    NVMIn.Save(Val, length);
    AttrIndex.Update(AttrIndexKey(Id, AttrId, curptr-AttrTable->BeginPtr()));
}
void Attr::DelAttrDat(const int &Id, const char *AttrName){
    int AttrId;
    if(AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)){
        DelAttrDat(Id, AttrId);
    }
}
void Attr::DelAttrDat(const int &Id, const int &AttrId){
    AttrIndexKey indexkey(Id,AttrId);
    AttrIndex.Delete(indexkey);
}
void Attr::DelAttrDat(const int &Id){
    AttrIndexKey MinKey(Id,0),MaxKey(Id,MxAttrId);
    AttrIndex.Delete(MinKey, MaxKey);
}

int Attr::AddAttrName(const char *AttrName){
    int AttrId=MxAttrId;
    MxAttrId++;
    int namelength=strlen(AttrName);
    int length=namelength+sizeof(bool)+sizeof(int)*2;
    char* curptr=NameTable->AllocateBytes(length);
    MIn NVMin(curptr,length);
    NVMin.Save(true);
    NVMin.Save(AttrId);
    NVMin.Save(namelength);
    NVMin.Save(AttrName, namelength);
    size_t offset=curptr-NameTable->BeginPtr();
    AttrNameToId.Add(AttrName, namelength, AttrId);
    AttrIdToName.Add(AttrId, sizeof(int), offset);
    return AttrId;
}

bool Attr::GetAttrId(int& AttrId,const char *AttrName)const{
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return false;
    return true;
}
bool Attr::GetAttrName(const int &AttrId, char *AttrName)const{
    size_t offset;
    if(!AttrIdToName.Find(AttrId, sizeof(int), offset)) return false;
    const char* curptr=NameTable->HeadPtr()+offset;
    size_t MaxLength=NameTable->EndPtr()-NameTable->BeginPtr()-offset;
    MOut NVMout(curptr,MaxLength);
    bool flag;
    int attrid;
    int namelength;
    NVMout.Load(flag);
    NVMout.Load(attrid);
    if(flag && attrid==AttrId){
        NVMout.Load(namelength);
        NVMout.Load(AttrName, namelength);
        return true;
    }
    return false;
}
class AttrPair{
private:
    HashTable<const char*,int> AttrNameToId;
    HashTable<int,size_t> AttrIdToName;
    Arena* NameTable;
    NVMSkipList<AttrEdgeIndexKey,ComparatorEdge<AttrEdgeIndexKey>> AttrIndex;
    Arena* AttrTable;
    int MxAttrId;
    
public:
    AttrPair(Arena* nametable, Arena* attrindex,Arena* attrtable):NameTable(nametable),AttrIndex(ComparatorEdge<AttrEdgeIndexKey>(),attrindex),AttrTable(attrtable),AttrNameToId(32,4),AttrIdToName(32,4),MxAttrId(0){
        size_t maxlength=NameTable->EndPtr()-NameTable->BeginPtr();
        if(maxlength!=0){
            MOut NVMout(NameTable->BeginPtr(),maxlength);
            while (NVMout.Len()<NVMout.MaxLen()) {
                bool flag;
                int attrid,length;
                size_t offset=NVMout.Len();
                NVMout.Load(flag);
                NVMout.Load(attrid);
                NVMout.Load(length);
                const int namelength=length;
                char attrname[namelength];
                NVMout.Load(attrname, namelength);
                if(!flag) continue;
                AttrNameToId.Add(attrname, namelength, attrid);
                AttrIdToName.Add(attrid, sizeof(int), offset);
                MxAttrId=MxAttrId>(attrid+1)? MxAttrId:(attrid+1);
            }
        }
    }
    bool AddAttrDat(const int& SrcNid,const int& DstNid,const char* AttrName,const void* Val,const uint& length,const bool & Direct=true);//若AttrName不存在返回false,若该属性已存在，返回false
    bool AddAttrDat(const int& SrcNid,const int& DstNid,const int& AttrId,const void* Val,const uint& length,const bool & Direct=true);
    bool GetAttrDat(const int& SrcNid,const int& DstNid,const char* AttrName,void* Val) const;
    bool GetAttrDat(const int& SrcNid,const int& DstNid,const int& AttrId,void* Val)const;
    void UpdateAttrDat(const int& SrcNid,const int& DstNid,const char* AttrName,const void* Val,const uint& length,const bool& Direct=true);
    void UpdateAttrDat(const int& SrcNid,const int& DstNid,const int& AttrId,const void* Val,const uint& length,const bool& Direct=true);
    void DelAttrDat(const int& SrcNid,const int& DstNid,const char* AttrName,const bool& Direct=true);
    void DelAttrDat(const int& SrcNid,const int& DstNid,const int& AttrId,const bool& Direct=true);
    void DelAttrDat(const int& SrcNid,const int& DstNid,const bool& Direct=true);
    int AddAttrName(const char* AttrName);
    bool GetAttrId(int& AttrId,const char* AttrName)const;
    bool GetAttrName(const int& AttrId,char* AttrName)const;
    
};

bool AttrPair::AddAttrDat(const int& SrcNid,const int& DstNid,const char* AttrName,const void* Val,const uint& length,const bool& Direct){
    int AttrId;
    if(! AttrNameToId.Find(AttrName,strlen(AttrName),AttrId)) return false;
    return AddAttrDat(SrcNid,DstNid,AttrId,Val,length,Direct);
}
bool AttrPair::AddAttrDat(const int& SrcNid,const int& DstNid,const int& AttrId,const void* Val,const uint& length,const bool& Direct){
    size_t location;
    if(!AttrIdToName.Find(AttrId,sizeof(int),location)) return false;
    AttrEdgeIndexKey indexkey(SrcNid,DstNid,AttrId);
    if(AttrIndex.Find(indexkey)) return false;
    int alllength=length+sizeof(int);
    char* curptr=AttrTable->AllocateBytes(alllength);
    MIn NVMIn(curptr,alllength);
    NVMIn.Save(length);
    NVMIn.Save(Val,length);
    indexkey.Location=curptr-AttrTable->BeginPtr();
    AttrIndex.Insert(indexkey);
    if(!Direct){
        AttrIndex.Insert(AttrEdgeIndexKey(DstNid, SrcNid, AttrId, indexkey.Location));
    }
    return true;
}
bool AttrPair::GetAttrDat(const int &SrcNid,const int& DstNid, const char *AttrName, void *Val)const{
    int AttrId;
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return false;
    return GetAttrDat(SrcNid,DstNid, AttrId, Val);
}
bool AttrPair::GetAttrDat(const int &SrcNid, const int& DstNid, const int &AttrId, void *Val)const{
    size_t location;
    if(!AttrIdToName.Find(AttrId, sizeof(int), location)) return false;
    AttrEdgeIndexKey indexkey(SrcNid,DstNid,AttrId);
    if(!AttrIndex.Find(indexkey)) return false;
    MOut NVMout(AttrTable->HeadPtr()+indexkey.Location,AttrTable->EndPtr()-AttrTable->BeginPtr()-indexkey.Location);
    int length;
    NVMout.Load(length);
    NVMout.Load(Val, length);
    return true;
}
void AttrPair::UpdateAttrDat(const int &SrcNid, const int &DstNid, const char *AttrName, const void *Val, const uint &length,const bool& Direct){
    int AttrId;
    if(AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)){
        UpdateAttrDat(SrcNid, DstNid, AttrId, Val, length, Direct);
    }
}
void AttrPair::UpdateAttrDat(const int &SrcNid, const int &DstNid, const int &AttrId, const void *Val, const uint &length, const bool& Direct){
    size_t location;
    if(AttrIdToName.Find(AttrId, sizeof(int), location)){
        int Length=length+sizeof(int);
        char* curptr=AttrTable->AllocateBytes(Length);
        MIn NVMIn(curptr,Length);
        NVMIn.Save(length);
        NVMIn.Save(Val, length);
        AttrIndex.Update(AttrEdgeIndexKey(SrcNid, DstNid, AttrId, curptr-AttrTable->BeginPtr()));
        if(!Direct){
            AttrIndex.Update(AttrEdgeIndexKey(DstNid, SrcNid, AttrId, curptr-AttrTable->BeginPtr()));
        }
    }
}
void AttrPair::DelAttrDat(const int& SrcNid,const int& DstNid, const char *AttrName,const bool& Direct){
    int AttrId;
    if(AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)){
        DelAttrDat(SrcNid,DstNid, AttrId,Direct);
    }
}
void AttrPair::DelAttrDat(const int &SrcNid,const int& DstNid, const int &AttrId,const bool& Direct){
    AttrEdgeIndexKey indexkey(SrcNid,DstNid,AttrId);
    AttrIndex.Delete(indexkey);
    if(!Direct) AttrIndex.Delete(AttrEdgeIndexKey(DstNid, SrcNid, AttrId));
}
void AttrPair::DelAttrDat(const int &SrcNid,const int& DstNid,const bool& Direct){
    AttrEdgeIndexKey MinKey(SrcNid,DstNid,0),MaxKey(SrcNid,DstNid,MxAttrId);
    AttrIndex.Delete(MinKey, MaxKey);
    if(!Direct) AttrIndex.Delete(AttrEdgeIndexKey(DstNid, SrcNid, 0), AttrEdgeIndexKey(DstNid, SrcNid, MxAttrId));
}

int AttrPair::AddAttrName(const char *AttrName){
    int AttrId=MxAttrId;
    MxAttrId++;
    int namelength=strlen(AttrName);
    int length=namelength+sizeof(bool)+sizeof(int)*2;
    char* curptr=NameTable->AllocateBytes(length);
    MIn NVMin(curptr,length);
    NVMin.Save(true);
    NVMin.Save(AttrId);
    NVMin.Save(namelength);
    NVMin.Save(AttrName, namelength);
    size_t offset=curptr-NameTable->BeginPtr();
    AttrNameToId.Add(AttrName, namelength, AttrId);
    AttrIdToName.Add(AttrId, sizeof(int), offset);
    return AttrId;
}

bool AttrPair::GetAttrId(int& AttrId,const char *AttrName)const{
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return false;
    return true;
}
bool AttrPair::GetAttrName(const int &AttrId, char *AttrName)const{
    size_t offset;
    if(!AttrIdToName.Find(AttrId, sizeof(int), offset)) return false;
    const char* curptr=NameTable->HeadPtr()+offset;
    size_t MaxLength=NameTable->EndPtr()-NameTable->BeginPtr()-offset;
    MOut NVMout(curptr,MaxLength);
    bool flag;
    int attrid;
    int namelength;
    NVMout.Load(flag);
    NVMout.Load(attrid);
    if(flag && attrid==AttrId){
        NVMout.Load(namelength);
        NVMout.Load(AttrName, namelength);
        return true;
    }
    return false;
}

#endif /* Attr_h */
