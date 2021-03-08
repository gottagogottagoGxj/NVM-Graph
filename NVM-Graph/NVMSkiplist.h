//
//  NVMSkiplist.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/23.
//

#ifndef NVMSkiplist_h
#define NVMSkiplist_h
#include<assert.h>
#include"Arena.h"


class Random {
 private:
  uint32_t seed_;
 public:
  explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) { }
  uint32_t Next() {
    static const uint32_t M = 2147483647L;   // 2^31-1
    static const uint64_t A = 16807;
    uint64_t product = seed_ * A;
    seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
    if (seed_ > M) {
      seed_ -= M;
    }
    return seed_;
  }
  uint32_t Uniform(int n) { return Next() % n; }
  bool OneIn(int n) { return (Next() % n) == 0; }
  uint32_t Skewed(int max_log) {
    return Uniform(1 << Uniform(max_log + 1));
  }
};

class AttrIndexKey{
public:
    int Nid,AttrId;//Nid/Eid
    size_t Location;
    AttrIndexKey(){}
    AttrIndexKey(const int nid,const int attrid,const size_t location):Nid(nid),AttrId(attrid),Location(location){}
    AttrIndexKey(const int nid,const int attrid):Nid(nid),AttrId(attrid),Location(0){}
    AttrIndexKey& operator=(const AttrIndexKey& key){
        Nid=key.Nid;
        AttrId=key.AttrId;
        Location=key.Location;
        return *this;
    }
};

template<class Key>
class Comparator{
private:
    bool Asc;
public:
    Comparator(const bool& asc=true):Asc(asc){}//Asc默认为真，降序排序，key1>key2返回1，key1<key2返回-1,相等返回0
    int operator()(const Key& key1,const Key& key2)const{
        if(Asc){
            if(key1.Nid>key2.Nid) return 1;
            if(key1.Nid<key2.Nid) return -1;
            if(key1.AttrId>key2.AttrId) return 1;
            if(key1.AttrId<key2.AttrId) return -1;
        }
        else{
            if(key1.Nid>key2.Nid) return -1;
            if(key1.Nid<key2.Nid) return 1;
            if(key1.AttrId>key2.AttrId) return -1;
            if(key1.AttrId<key2.AttrId) return 1;
        }
        return 0;
    }
};

class AttrEdgeIndexKey{
public:
    int SrcNid,DstNid,AttrId;
    size_t Location;
    AttrEdgeIndexKey(){}
    AttrEdgeIndexKey(const int& SrcNode,const int& DstNode,const int& Attrid,const size_t& location):SrcNid(SrcNode),DstNid(DstNode),AttrId(Attrid),Location(location){}
    AttrEdgeIndexKey(const int& SrcNode,const int& DstNode,const int& Attrid):SrcNid(SrcNode),DstNid(DstNode),AttrId(Attrid),Location(0){}
    AttrEdgeIndexKey& operator=(const AttrEdgeIndexKey& key){
        SrcNid=key.SrcNid;
        DstNid=key.DstNid;
        AttrId=key.AttrId;
        Location=key.Location;
        return *this;
    }
};
template<class Key>
class ComparatorEdge{
private:
    bool Asc;
public:
    ComparatorEdge(const bool& asc=true):Asc(asc){}
    int operator()(const Key& key1,const Key& key2)const{
        if(Asc){
            if(key1.SrcNid>key2.SrcNid) return 1;
            if(key1.SrcNid<key2.SrcNid) return -1;
            if(key1.DstNid>key2.DstNid) return 1;
            if(key1.DstNid<key2.DstNid) return -1;
            if(key1.AttrId>key2.AttrId) return 1;
            if(key1.AttrId<key2.AttrId) return -1;
        }
        else{
            if(key1.SrcNid>key2.SrcNid) return -1;
            if(key1.SrcNid<key2.SrcNid) return 1;
            if(key1.DstNid>key2.DstNid) return -1;
            if(key1.DstNid<key2.DstNid) return 1;
            if(key1.AttrId>key2.AttrId) return -1;
            if(key1.AttrId<key2.AttrId) return 1;
        }
        return 0;
    }
};


template<typename Key,class Comparator>
class NVMSkipList{
private:
    struct Node;
public:
    explicit NVMSkipList(Comparator cmpare,Arena* arena);
    void Insert(const Key& key);
    bool Contains(const Key& key) const;
    bool Find(Key& key)const;
    void Delete(const Key& key);
    void Delete(const Key& MinKey,const Key& MaxKey);//删除跳表中[MinKey,MaxKey]
private:
    enum {kMaxHeight=12};
    Arena* const IndexTable;
    Node*  Head;
    Comparator Compare;
    Random Rnd;
    
private:
    Node* NewNode(const Key& key,int height);
    Node* GetNodePtr(const size_t offset)const {return (Node*)(IndexTable->BeginPtr()+offset);}
    size_t GetNodeOffset(const Node* node)const {return (char*)node-IndexTable->BeginPtr();}
    int RandomHeight();
    bool KeyIsAfterNode(const Key& key,Node* n)const;
    Node* FindGreaterOrEqual(const Key& key,Node** prev)const;
    Node* FindLessThan(const Key& key)const;
    Node* FindLast()const;
public:
    class Iterator{
    private:
        const NVMSkipList* skiplist;
        Node* node;
    public:
        explicit Iterator(const NVMSkipList* list):skiplist(list),node(NULL){}
        bool Valid()const{return skiplist->GetNodeOffset(node)!=0;}
        const Key& GetKey()const{assert(Valid());return node->key;}
        void Next(){assert(Valid());node=skiplist->GetNodePtr(node->Next(0));}
        void Prev(){
            assert(Valid());
            node=skiplist->FindLessThan(node->key);
            if(node==skiplist->Head) node=NULL;
        }
        void Seek(const Key& target){node=skiplist->FindGreaterOrEqual(target, NULL);}
        void SeekToFirst(){node=skiplist->GetNodePtr(skiplist->Head->Next(0));}
        void SeekToLast(){node=skiplist->FindLast();if(node==skiplist->Head) node=NULL;}
    };
};

template<typename Key,class Comparator>
struct NVMSkipList<Key,Comparator>::Node{
    const Key key;
    size_t next[1];
    
    explicit Node(const Key& k):key(k){}
    static void* operator new(size_t size,char* ptr){
        return (void*)ptr;
    }
    size_t Next(int n){
        assert(n>=0);
        return next[n];
    }
    void SetNext(int n,size_t x){
        assert(n>=0);
        next[n]=x;
    }
};

template<typename Key,class Comparator>
typename NVMSkipList<Key,Comparator>::Node*
NVMSkipList<Key,Comparator>::NewNode(const Key& key,int height){
    char* CurPtr=IndexTable->AllocateBytes(sizeof(Node)+sizeof(size_t)*(height-1));
    return new(CurPtr)Node(key);
}
template<typename Key,class Comparator>
NVMSkipList<Key,Comparator>::NVMSkipList(Comparator cmp,Arena* arena):Compare(cmp),IndexTable(arena),Rnd(0xdeadbeef){
    if(IndexTable->EndPtr()==IndexTable->BeginPtr()){
        Head=NewNode(Key(), kMaxHeight);
        for(int i=0;i<kMaxHeight;++i){
            Head->SetNext(i,0);
        }
    }
    else{
        Head=(Node*)IndexTable->HeadPtr();
    }
}
template<typename Key,class Comparator>
int NVMSkipList<Key,Comparator>::RandomHeight(){
    static const unsigned int kBranching = 4;
    int height = 1;
    while (height < kMaxHeight && ((Rnd.Next() % kBranching) == 0)) {
      height++;
    }
    assert(height > 0);
    assert(height <= kMaxHeight);
    return height;
}
template<typename Key,class Comparator>
bool NVMSkipList<Key,Comparator>::KeyIsAfterNode(const Key& key,Node* n)const{
    return (n!=NULL)&&(Compare(key,n->key)>0);
}
template<typename Key,class Comparator>
typename NVMSkipList<Key,Comparator>::Node* NVMSkipList<Key,Comparator>::FindGreaterOrEqual(const Key& key,Node** prev)const{
    Node* x=Head;
    int level=kMaxHeight-1;
    while(true){
        size_t NextOffset=x->Next(level);
        if(NextOffset==0 || Compare(GetNodePtr(NextOffset)->key,key)>=0){
            if(prev!=NULL) prev[level]=x;
            if(level==0){if(NextOffset!=0) return GetNodePtr(NextOffset); return NULL;}
            else level--;
        }
        else x=GetNodePtr(NextOffset);
    }
}
template<typename Key,class Comparator>
typename NVMSkipList<Key,Comparator>::Node*
NVMSkipList<Key,Comparator>::FindLessThan(const Key& key)const{
    if(Compare(GetNodePtr(Head->Next(0))->key,key)>=0) return NULL;
    Node* x=Head;
    int level=kMaxHeight-1;
    while(true){
        size_t NextOffset=x->Next(level);
        if(NextOffset==0 || Compare(GetNodePtr(NextOffset)->key,key)>=0){
            if(level==0) return x;
            else level--;
        }
        else x=GetNodePtr(NextOffset);
    }
}
template<typename Key,class Comparator>
typename NVMSkipList<Key,Comparator>::Node* NVMSkipList<Key,Comparator>::FindLast()const{
    Node* x=Head;
    int level=kMaxHeight-1;
    while(true){
        size_t NextOffset=x->Next(level);
        if(NextOffset==0){
            if(level==0) return x;
            else level--;
        }
        else x=GetNodePtr(NextOffset);
    }
}
template<typename Key,class Comparator>
void NVMSkipList<Key,Comparator>::Insert(const Key& key){
    Node* prev[kMaxHeight];
    Node* x=FindGreaterOrEqual(key,prev);
    assert(x==NULL || Compare(x->key,key)!=0);
    int height=RandomHeight();
    x=NewNode(key,height);
    for(int i=0;i<height;++i){
        x->SetNext(i,prev[i]->Next(i));
        prev[i]->SetNext(i,GetNodeOffset(x));
    }
}
template<typename Key,class Comparator>
bool NVMSkipList<Key,Comparator>::Contains(const Key& key)const{
    Node* x=FindGreaterOrEqual(key,NULL);
    if(x!=NULL && Compare(x->key,key)==0) return true;
    return false;
}
template<typename Key,class Comparator>
bool NVMSkipList<Key,Comparator>::Find(Key& key)const{
    Node* x=FindGreaterOrEqual(key,NULL);
    if(x!=NULL && Compare(x->key,key)==0){
        key=x->key;
        return true;
    }
    return false;
}
template<typename Key,class Comparator>
void NVMSkipList<Key,Comparator>::Delete(const Key &key){
    Node* prev[kMaxHeight];
    Node* x=FindGreaterOrEqual(key, prev);
    if(Compare(x->key,key)==0){
        for(int i=0;i<kMaxHeight;++i){
            if(prev[i]->Next(i)!=GetNodeOffset(x)) break;
            prev[i]->SetNext(i,x->Next(i));
        }
    }
}
template<typename Key,class Comparator>
void NVMSkipList<Key,Comparator>::Delete(const Key &MinKey, const Key &MaxKey){
    Node* prev[kMaxHeight];
    FindGreaterOrEqual(MinKey, prev);
    for(int i=0;i<kMaxHeight;++i){
        if(prev[i]->Next(i)==0 || Compare(GetNodePtr(prev[i]->Next(i))->key,MaxKey)>0) break;
        size_t tempoffset=prev[i]->Next(i);
        Node* temp=GetNodePtr(tempoffset);
        while(tempoffset!=0 && Compare(temp->key,MaxKey)<=0){
            tempoffset=prev[i]->Next(i);
            temp=GetNodePtr(tempoffset);
        }
        prev[i]->SetNext(i,GetNodeOffset(temp));
    }
}

#endif /* NVMSkiplist_h */
