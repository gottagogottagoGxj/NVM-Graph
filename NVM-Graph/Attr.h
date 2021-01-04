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
class Attr{
private:
    HashTable<const char*,int> AttrNameToId;
    HashTable<int,size_t> AttrIdToName;
    Arena* NameTable;
    NVMSkipList<AttrIndexKey,Comparator<AttrIndexKey>> AttrIndex;
    Arena* AttrTable;
    int MxAttrId;
    
public:
    Attr(Arena* nametable,const Comparator<AttrIndexKey> compare, Arena* attrindex,Arena* attrtable):NameTable(nametable),AttrIndex(compare,attrindex),AttrTable(attrtable),AttrNameToId(32,4),AttrIdToName(32,4),MxAttrId(0){
        if(NameTable->BeginPtr()!=NameTable->EndPtr()){
            
        }
    }
    bool AddAttrDat(const int& Id,const char* AttrName,const char* Val);//若AttrName不存在返回false,若该属性已存在，返回false
    bool AddAttrDat(const int& Id,const int& AttrId,const char* Val);
    bool GetAttrDat(const int& Id,const char* AttrName,char* Val) const;
    bool GetAttrDat(const int& Id,const int& AttrId,char* Val)const;
    void DelAttrDat(const int& Id,const char* AttrName);
    void DelAttrDat(const int& Id,const int& AttrId);
    void DelAttrDat(const int& Id);
    int AddAttrName(const char* AttrName);
    int GetAttrId(const char* AttrName)const;
    bool GetAttrName(const int& AttrId,char* AttrName)const;
    
};

bool Attr::AddAttrDat(const int& Id,const char* AttrName,const char* Val){
    int AttrId;
    if(! AttrNameToId.Find(AttrName,strlen(AttrName),AttrId)) return false;
    return AddAttrDat(Id,AttrId,Val);
}
bool Attr::AddAttrDat(const int& Id,const int& AttrId,const char* Val){
    size_t location;
    if(!AttrIdToName.Find(AttrId,sizeof(int),location)) return false;
    AttrIndexKey indexkey(Id,AttrId);
    if(AttrIndex.Find(indexkey)) return false;
    int vallength=strlen(Val);
    int length=vallength+sizeof(int);
    char* curptr=AttrTable->AllocateBytes(length);
    MIn NVMIn(curptr,length);
    NVMIn.Save(vallength);
    NVMIn.Save(Val,vallength);
    indexkey.Location=curptr-AttrTable->BeginPtr();
    AttrIndex.Insert(indexkey);
    return true;
}
bool Attr::GetAttrDat(const int &Id, const char *AttrName, char *Val)const{
    int AttrId;
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return false;
    return GetAttrDat(Id, AttrId, Val);
}
bool Attr::GetAttrDat(const int &Id, const int &AttrId, char *Val)const{
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

int Attr::GetAttrId(const char *AttrName)const{
    int AttrId;
    if(!AttrNameToId.Find(AttrName, strlen(AttrName), AttrId)) return -1;
    return AttrId;
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

#endif /* Attr_h */
