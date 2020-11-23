//
//  Arena.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/17.
//

#ifndef Arena_h
#define Arena_h
#include<stdlib.h>
#include<string>
#include"InOut.h"


class Arena{
public:
    Arena(int NodeSize);
    ~Arena();
    char* AllocateBytes(size_t bytes);
    char* AllocateNode();
    const char* BeginPtr() const {return begin_ptr;}
    const int NodeSize()const{return EveryNodeSize; }
    
private:
    char* begin_ptr;
    char* curAlloc_ptr;
    int EveryNodeSize;
    
};

const int NodedatalengthDef=32;
const int INOutEidNumDef=10;
class Node{
public:
    Node(){}
    Node(MOut& Out);
    Node(const int nid, const char* nodedata):
    NextNode(-1),PrevNode(-1),InEidNum(0),OutEidNum(0),Nid(nid),Flag(true){
        DataLength=(int)strlen(nodedata);
        memcpy(NodeData,nodedata,DataLength+1);
    }
    Node(const int nid,std::string &s):
    NextNode(-1),PrevNode(-1),InEidNum(0),OutEidNum(0),Nid(nid),Flag(true){
        DataLength=(int)s.size();
        memcpy(NodeData,s.c_str(),s.size()+1);
    }
    bool Save(MIn& In) const;
    bool Load(MOut& Out);
    
    void AddInEdge(int eid){
        InEid[InEidNum]=eid;
        InEidNum++;
    }
    void AddOutEdge(int eid){
        OutEid[OutEidNum]=eid;
        OutEidNum++;
    }
    int GetInEidNum() const{return InEidNum;}
    int GetOutEidNum() const{return OutEidNum;}
    int GetDataLength() const{return DataLength;}
    const char* GetNodeData() const {return NodeData;}
    int GetNodeId() const{ return Nid;}
    int GetInEid(const int EidN) const {return InEid[EidN];}
    int GetOutEid(const int EidN) const {return OutEid[EidN];}
    // 判断Eid是否在InEid，若在返回位置，否在返回-1
    int IsInEid(const int Eid) const{
        if(InEidNum==0) return -1;
        for(int i=0;i<InEidNum;++i){
            if(InEid[i]==Eid) return i;
        }
        return -1;
    }
    int IsOutEid(const int Eid) const{
        if(OutEidNum==0) return -1;
        for(int i=0;i<OutEidNum;++i){
            if(OutEid[i]==Eid) return i;
        }
        return -1;
    }
    void DelInEid(const int eid){
        int location=IsInEid(eid);
        if(location<0) return;
        for(int i=location;i<InEidNum-1;++i) InEid[i]=InEid[i+1];
        InEidNum--;
    }
    void DelOutEid(const int eid){
        int location=IsOutEid(eid);
        if(location<0) return;
        for(int i=location;i<OutEidNum-1;++i) OutEid[i]=OutEid[i+1];
        OutEidNum--;
    }
    
    void SortInEId(){
        for(int i=1;i<InEidNum;++i){
            int j=0;
            int temp=InEid[i];
            while(j<i && temp>=InEid[j]) ++j;
            int k;
            for(k=i;k>j;--k) InEid[k]=InEid[k-1];
            InEid[k]=temp;
        }
    }
    int AddInEdgeSort(const int eid){
        int i=0;
        while(i<InEidNum && eid>=InEid[i]) ++i;
        int j;
        for(j=InEidNum;j>i;--j) InEid[j]=InEid[j-1];
        InEid[i]=eid;
        InEidNum++;
        return i;
    }
    int GetInEidSort(const int EidN){
        return InEid[EidN];
    }
    int IsInEidSort(const int Eid){
        if(InEidNum==0) return -1;
        int left=0,right=InEidNum;
        while(left<right){
            int mid=(left+right)>>1;
            if(InEid[mid]==Eid) return mid;
            if(InEid[mid]>Eid) right=mid-1;
            else left=mid+1;
        }
        return InEid[left]==Eid?left:-1;
    }
    void DelInEidSort(const int eid){
        int location=IsInEidSort(eid);
        if(location<0) return;
        for(int i=location;i<InEidNum-1;++i) InEid[i]=InEid[i+1];
        InEidNum--;
    }
    void SortOutEId(){
        for(int i=1;i<OutEidNum;++i){
            int j=0;
            int temp=OutEid[i];
            while(j<i && temp>=OutEid[j]) ++j;
            int k;
            for(k=i;k>j;--k) OutEid[k]=OutEid[k-1];
            OutEid[k]=temp;
        }
    }
    int AddOutEdgeSort(const int eid){
        int i=0;
        while(i<OutEidNum && eid>=OutEid[i]) ++i;
        int j;
        for(j=OutEidNum;j>i;--j) OutEid[j]=OutEid[j-1];
        OutEid[i]=eid;
        OutEidNum++;
        return i;
    }
    int GetOutEidSort(const int EidN){
        return OutEid[EidN];
    }
    int IsOutEidSort(const int Eid){
        if(OutEidNum==0) return -1;
        int left=0,right=OutEidNum;
        while(left<right){
            int mid=(left+right)>>1;
            if(OutEid[mid]==Eid) return mid;
            if(OutEid[mid]>Eid) right=mid-1;
            else left=mid+1;
        }
        return OutEid[left]==Eid?left:-1;
    }
    void DelOutEidSort(const int eid){
        int location=IsOutEidSort(eid);
        if(location<0) return;
        for(int i=location;i<OutEidNum-1;++i) OutEid[i]=OutEid[i+1];
        OutEidNum--;
    }
    
    void AddInEdgeCheck(const int eid){
        if(IsInEidSort(eid)<0){
            AddInEdgeSort(eid);
        }
    }
    
    void AddOutEdgeCheck(const int eid){
        if(IsOutEidSort(eid)<0){
            AddOutEdgeSort(eid);
        }
    }
    
    
    
private:
    int Nid;
    int DataLength;
    char NodeData[NodedatalengthDef];
    int InEidNum;
    int InEid[INOutEidNumDef];
    int OutEidNum;
    int OutEid[INOutEidNumDef];
    bool Flag;
    int NextNode;
    int PrevNode;
};

Node::Node(MOut& Out){
    Out.Load(Nid);
    Out.Load(DataLength);
    Out.Load(NodeData, NodedatalengthDef);
    Out.Load(InEidNum);
    Out.Load(InEid, INOutEidNumDef);
    Out.Load(OutEidNum);
    Out.Load(OutEid, INOutEidNumDef);
    Out.Load(Flag);
    Out.Load(NextNode);
    Out.Load(PrevNode);
}

bool Node::Load(MOut &Out){
    return  Out.Load(Nid)&&
    Out.Load(DataLength)&&
    Out.Load(NodeData, NodedatalengthDef)&&
    Out.Load(InEidNum)&&
    Out.Load(InEid, INOutEidNumDef)&&
    Out.Load(OutEidNum)&&
    Out.Load(OutEid, INOutEidNumDef)&&
    Out.Load(Flag)&&
    Out.Load(NextNode)&&
    Out.Load(PrevNode);
}
bool Node::Save(MIn &In) const{
    return In.Save(Nid)&&
    In.Save(DataLength)&&
    In.Save(NodeData, NodedatalengthDef)&&
    In.Save(InEidNum)&&
    In.Save(InEid, INOutEidNumDef)&&
    In.Save(OutEidNum)&&
    In.Save(OutEid, INOutEidNumDef)&&
    In.Save(Flag)&&
    In.Save(NextNode)&&
    In.Save(PrevNode);
}


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


#endif /* Arena_h */
