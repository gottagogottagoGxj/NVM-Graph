//
//  Direct_graph.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/30.
//

#ifndef Direct_graph_h
#define Direct_graph_h
#include<string>

#include"hashtable.h"
#include"Arena.h"
#include"InOut.h"


class Direct_graph{
public:
    class NvmNode{
    private:
        int Nid;
        char Data[NodeDatalengthDef];
        int InNidVNum;
        int OutNidVNum;
        int InNidV[InOutEdgeNumDef];
        int OutNidV[InOutEdgeNumDef];
        int Flag;
        uint64_t NextNode;
    public:
        NvmNode(const int nid):Nid(nid),Data("\0"),InNidVNum(0),OutNidVNum(0),Flag(1),NextNode(0){}
        NvmNode(const int nid,const char* data):Nid(nid),InNidVNum(0),OutNidVNum(0),Flag(1),NextNode(0){
            size_t length=strlen(data);
            if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        NvmNode(MOut& Out){
            Out.Load(Nid);
            Out.Load(Data, NodeDatalengthDef);
            Out.Load(InNidVNum);
            Out.Load(OutNidVNum);
            Out.Load(InNidV, InOutEdgeNumDef);
            Out.Load(OutNidV, InOutEdgeNumDef);
            Out.Load(Flag);
            Out.Load(NextNode);
        }
        NvmNode(const int nid,const int flag):Nid(nid),Data("\0"),InNidVNum(0),OutNidVNum(0),Flag(flag),NextNode(0){}
        void Save(MIn& In){
            In.Save(Nid);
            In.Save(Data, NodeDatalengthDef);
            In.Save(InNidVNum);
            In.Save(OutNidVNum);
            In.Save(InNidV, InOutEdgeNumDef);
            In.Save(OutNidV, InOutEdgeNumDef);
            In.Save(Flag);
            In.Save(NextNode);
        }
        
        static void* operator new(size_t size,Arena* table){
            return (void*)(table->AllocateNode());
        }
        static void operator delete(void* ptr,Arena* table){
            table->RecoverNode();
        }
        static void* operator new(size_t size,char* start){
            return (void*)start;
        }
        static void operator delete(void* ptr,char* start){
            
        }
        
        size_t GetMaxDeg()const{return InOutEdgeNumDef;}
        
        int GetId()const{return Nid;}
        const char* GetData()const{return Data;}
        void SetData(const char* data){
            size_t length=strlen(data);
            if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        int GetFlag()const{return Flag;}
        void SetFlag(const int& flag){Flag=flag;}
        uint64_t GetNextNodeAddre()const{return NextNode;}
        void SetNextNodeAddre(const uint64_t& location){NextNode=location;}
        int GetDeg()const{return GetInDeg()+GetOutDeg();}
        int GetInDeg()const{return InNidVNum;}
        int GetOutDeg()const{return OutNidVNum;}
        int GetNbrNid(const int& NodeN)const{return NodeN<GetOutDeg()?GetOutNid(NodeN):GetInNid(NodeN-GetOutDeg());}
        int GetInNid(const int& NodeN)const{return InNidV[NodeN];}
        int GetOutNid(const int& NodeN)const{return OutNidV[NodeN];}
        bool IsInNid(const int& nid)const{
            for(int i=0;i<InNidVNum;++i){
                if(InNidV[i]==nid) return true;
            }
            return false;
        }
        bool IsOutNid(const int& nid)const{
            for(int i=0;i<OutNidVNum;++i){
                if(OutNidV[i]==nid) return true;
            }
            return false;
        }
        bool IsNbrNid(const int& nid)const{return IsInNid(nid)||IsOutNid(nid);}
        
        bool AddInNid(const int& nid){
            if(InNidVNum<InOutEdgeNumDef){
                InNidV[InNidVNum]=nid;
                InNidVNum++;
                return true;
            }
            return false;
        }
        bool AddOutNid(const int& nid){
            if(OutNidVNum<InOutEdgeNumDef){
                OutNidV[OutNidVNum]=nid;
                OutNidVNum++;
                return true;
            }
            return false;
        }
        bool AddInNid(const int* nid,const int& nidN){
            if(InNidVNum+nidN<=InOutEdgeNumDef){
                memcpy(InNidV, nid, sizeof(int)*nidN);
                InNidVNum+=nidN;
                return true;
            }
            return false;
        }
        bool AddOutNid(const int* nid,const int& nidN){
            if(OutNidVNum+nidN<=InOutEdgeNumDef){
                memcpy(OutNidV, nid, sizeof(int)*nidN);
                OutNidVNum+=nidN;
                return true;
            }
            return false;
        }
        bool AddNbrNid(const int& nid){return AddInNid(nid)&&AddOutNid(nid);}
        void DeleteInNidN(const int & NodeN){
            for(int i=NodeN;i<(InNidVNum-1);++i) {InNidV[i]=InNidV[i+1];}
            InNidVNum--;
        }
        void DeleteOutNidN(const int& NodeN){
            for(int i=NodeN;i<(OutNidVNum-1);++i) {OutNidV[i]=OutNidV[i+1];}
            OutNidVNum--;
        }
        bool DeleteInNid(const int& nid){
            for(int i=0;i<InNidVNum;++i){
                if(InNidV[i]==nid){
                    for(int j=i;j<(InNidVNum-1);++j){InNidV[j]=InNidV[j+1];}
                    InNidVNum--;
                    return true;
                }
            }
            return false;
        }
        bool DeleteOutNid(const int& nid){
            for(int i=0;i<OutNidVNum;++i){
                if(OutNidV[i]==nid){
                    for(int j=i;j<(OutNidVNum-1);++j){OutNidV[j]=OutNidV[j+1];}
                    OutNidVNum--;
                    return true;
                }
            }
            return false;
        }
        bool DeleteNbrNid(const int& nid){return DeleteInNid(nid)&&DeleteOutNid(nid);}
        
        //排序，默认从小到大排序,插入排序
        void SortInNidV(const bool& asc=true){
            if(asc){
                for(int i=1;i<InNidVNum;++i){
                    int value=InNidV[i];
                    int j=0;
                    while(j<i && value>=InNidV[j]) j++;
                    for(int k=i;k>j;--k) InNidV[k]=InNidV[k-1];
                    InNidV[j]=value;
                }
            }
            else{
                for(int i=1;i<InNidVNum;++i){
                    int value=InNidV[i];
                    int j=0;
                    while(j<i && value<=InNidV[j]) j++;
                    for(int k=i;k>j;--k) InNidV[k]=InNidV[k-1];
                    InNidV[j]=value;
                }
            }
        }
        void SortOutNidV(const bool& asc=true){
            if(asc){
                for(int i=1;i<OutNidVNum;++i){
                    int value=OutNidV[i];
                    int j=0;
                    while(j<i && value>=OutNidV[j]) ++j;
                    for(int k=i;k>j;--k) OutNidV[k]=OutNidV[k-1];
                    OutNidV[j]=value;
                }
            }
            else{
                for(int i=1;i<OutNidVNum;++i){
                    int value=OutNidV[i];
                    int j=0;
                    while(j<i && value<=OutNidV[j]) ++j;
                    for(int k=i;k>j;--k) OutNidV[k]=OutNidV[k-1];
                    OutNidV[j]=value;
                }
            }
        }
        //二分查找
        bool IsInNidSort(const int& nid,const bool& asc=true)const{
            if(asc){
                int left=0,right=InNidVNum-1;
                while(left<=right){
                    int mid=(left+right)>>1;
                    if(InNidV[mid]==nid) {return true;}
                    else if(InNidV[mid]>nid){right=mid-1;}
                    else{left=mid+1;}
                }
                return false;
            }
            else{
                int left=0,right=InNidVNum-1;
                while(left<=right){
                    int mid=(left+right)>>1;
                    if(InNidV[mid]==nid) {return true;}
                    else if(InNidV[mid]>nid){left=mid+1;}
                    else{right=mid-1;}
                }
                return false;
            }
        }
        bool IsOutNidSort(const int& nid,const bool& asc=true)const{
            if(asc){
                int left=0,right=OutNidVNum-1;
                while(left<=right){
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]==nid){return true;}
                    else if(OutNidV[mid]>nid){right=mid-1;}
                    else{left=mid+1;}
                }
                return false;
            }
            else{
                int left=0,right=OutNidVNum-1;
                while(left<=right){
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]==nid){return true;}
                    else if(OutNidV[mid]>nid){left=mid+1;}
                    else{right=mid-1;}
                }
                return false;
            }
        }
        bool AddInNidSort(const int& nid,const bool asc=true){
            if(InNidVNum>=InOutEdgeNumDef) return false;
            int left=0,right=InNidVNum;
            if(asc){
                while(left<right){
                    int mid=(left+right)>>1;
                    if(InNidV[mid]<=nid){left=mid+1;}
                    else {right=mid;}
                }
            }
            else{
                while(left<right){
                    int mid=(left+right)>>1;
                    if(InNidV[mid]>=nid){left=mid+1;}
                    else {right=mid;}
                }
            }
            for(int i=InNidVNum;i>left;--i){InNidV[i]=InNidV[i-1];}
            InNidV[left]=nid;
            InNidVNum++;
            return true;
        }
        bool AddOutNidSort(const int& nid,const bool asc=true){
            if(OutNidVNum>=InOutEdgeNumDef) return false;
            int left=0,right=OutNidVNum;
            if(asc){
                while (left<right) {
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]<=nid){left=mid+1;}
                    else {right=mid;}
                }
            }
            else{
                while(left<right){
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]>=mid){left=mid+1;}
                    else{right=mid;}
                }
            }
            for(int i=OutNidVNum;i>left;--i){OutNidV[i]=OutNidV[i-1];}
            OutNidV[left]=nid;
            OutNidVNum++;
            return true;
        }
        bool DeleteInNidSort(const int& nid,const bool asc=true){
            int left=0,right=InNidVNum;
            if(asc){
                while (left<right) {
                    int mid=(left+right)>>1;
                    if(InNidV[mid]>=nid){right=mid;}
                    else{left=mid+1;}
                }
            }
            else{
                while(left<right){
                    int mid=(left+right)>>1;
                    if(InNidV[mid]<=nid){right=mid;}
                    else{left=mid+1;}
                }
            }
            if(InNidV[left]!=nid) return false;
            for(int i=left;i<(InNidVNum-1);++i){InNidV[i]=InNidV[i+1];}
            InNidVNum--;
            return true;
        }
        bool DeleteOutNidSort(const int& nid,const bool asc=true){
            int left=0,right=OutNidVNum;
            if(asc){
                while(left<right){
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]>=nid){right=mid;}
                    else{left=mid+1;}
                }
            }
            else{
                while(left<right){
                    int mid=(left+right)>>1;
                    if(OutNidV[mid]<=nid){right=mid;}
                    else{left=mid+1;}
                }
            }
            if(OutNidV[left]!=nid) return false;
            for(int i=left;i<(OutNidVNum-1);++i){OutNidV[i]=OutNidV[i+1];}
            OutNidVNum--;
            return true;
        }
        
    };
    class NodeIter{
    private:
        const char* Begin;
        const char* End;
        NvmNode* CurNode;
    public:
        NodeIter():Begin(NULL),End(NULL),CurNode(NULL){}
        NodeIter(const char* begin,const char* end,NvmNode* cur):Begin(begin),End(end),CurNode(cur){}
        NodeIter(const NodeIter& nodeI):Begin(nodeI.Begin),End(nodeI.End),CurNode(nodeI.CurNode){}
        NodeIter& operator=(const NodeIter& nodeI){
            Begin=nodeI.Begin;
            End=nodeI.End;
            CurNode=nodeI.CurNode;
            return *this;
        }
        NodeIter& operator++(int){
            if((char*)CurNode>=End) return *this;
            CurNode++;
            while((char*)CurNode<End && CurNode->GetFlag()<=0) CurNode++;
            return *this;
        }
        NodeIter& operator--(int){
            if((char*)CurNode<=Begin) return *this;
            CurNode--;
            while ((char*)CurNode>Begin && CurNode->GetFlag()<=0) CurNode--;
            return *this;
        }
        bool operator<(const NodeIter& nodeI)const {return CurNode<nodeI.CurNode;}
        bool operator==(const NodeIter& nodeI)const {return CurNode==nodeI.CurNode;}
        NvmNode& operator*()const{return *CurNode;}
        NvmNode& operator()()const{return *CurNode;}
        NvmNode& operator->()const{return *CurNode;}
        bool IsEmpty()const{return CurNode==NULL;}
        bool IsBegin()const{return (char*)CurNode==Begin;}
        bool IsEnd()const{return (char*)CurNode==End;}
        
        int GetId()const{return CurNode->GetId();}
        const char* GetData()const{return CurNode->GetData();}
        bool GetFlag()const{return CurNode->GetFlag();}
        int GetInDeg()const{
            int InDeg=0;
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                InDeg+=temp->GetInDeg();
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            InDeg+=temp->GetNextNodeAddre();
            return InDeg;
        }
        int GetOutDeg()const{
            int OutDeg=0;
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                OutDeg+=temp->GetOutDeg();
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            OutDeg+=temp->GetNextNodeAddre();
            return OutDeg;
        }
        bool IsNbrNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                if(temp->IsNbrNid(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsNbrNid(nid);
        }
        bool IsInNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                if(temp->IsInNid(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsInNid(nid);
        }
        bool IsOutNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                if(temp->IsOutNid(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsOutNid(nid);
        }
        int GetInNid(const int edgeN)const{
            int n=edgeN;
            NvmNode* temp=CurNode;
            while(n>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
                n-=temp->GetInDeg();
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            if(n>temp->GetInDeg()) return -1;
            return temp->GetInNid(n-1);
        }
        int GetOutNid(const int edgeN)const{
            int n=edgeN;
            NvmNode* temp=CurNode;
            while (n>temp->GetOutDeg()&& temp->GetNextNodeAddre()!=0) {
                n-=temp->GetOutDeg();
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            if(n>temp->GetOutDeg()) return -1;
            return temp->GetOutNid(n-1);
        }
        
        void SortNidV(){
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                temp->SortInNidV();
                temp->SortOutNidV();
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            temp->SortInNidV();
            temp->SortOutNidV();
        }
        bool IsNbrNidSort(const int& nid)const{
            NvmNode* temp=CurNode;
            while (temp->GetNextNodeAddre()!=0) {
                if(temp->IsInNidSort(nid)||temp->IsOutNidSort(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsInNidSort(nid)||temp->IsOutNidSort(nid);
        }
        bool IsInNidSort(const int& nid)const{
            NvmNode* temp=CurNode;
            while (temp->GetNextNodeAddre()!=0) {
                if(temp->IsInNidSort(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsInNidSort(nid);
        }
        bool IsOutNidSort(const int& nid)const{
            NvmNode* temp=CurNode;
            while (temp->GetNextNodeAddre()!=0) {
                if(temp->IsOutNidSort(nid)) return true;
                temp=(NvmNode*)(Begin+temp->GetNextNodeAddre());
            }
            return temp->IsOutNidSort(nid);
        }
    };
    class EdgeIter{
    private:
        NodeIter CurNode;
        int CurEdge;
    public:
        EdgeIter():CurNode(),CurEdge(0){}
        EdgeIter(const NodeIter& nodeI,const int& edge):CurNode(nodeI),CurEdge(edge){}
        EdgeIter(const EdgeIter& edgeI):CurNode(edgeI.CurNode),CurEdge(edgeI.CurEdge){}
        EdgeIter& operator=(const EdgeIter& edgeI){
            CurNode=edgeI.CurNode;
            CurEdge=edgeI.CurEdge;
            return *this;
        }
        EdgeIter& operator++(int){
            CurEdge++;
            if(!CurNode.IsEnd() && CurEdge>CurNode.GetOutDeg()) {
                CurEdge=1;
                CurNode++;
                while(!CurNode.IsEnd() && CurNode.GetOutDeg()==0) CurNode++;
            }
            return *this;
        }
        bool operator<(const EdgeIter& edgeI)const{return CurNode<edgeI.CurNode || (CurNode==edgeI.CurNode && CurEdge<edgeI.CurEdge);}
        bool IsEmpty()const{return CurNode.IsEmpty()||(CurEdge>CurNode.GetOutDeg());}
        bool IsEnd()const{return CurNode.IsEnd();}
        int GetSrcNid()const{return CurNode.GetId();}
        int GetDstNid()const{return CurNode.GetOutNid(CurEdge);}
    };
public:
    static uint GetNodeSize(){return sizeof(NvmNode);}
private:
    NvmNode* GetNodePtr(const uint64_t& location) const{
        size_t nodeN=location/NodeTable->NodeSize();
        return (NvmNode*)(NodeTable->BeginPtr())+nodeN;
    }
    uint64_t GetNodeLocation(const NvmNode* node)const{
        return (char*)node-NodeTable->BeginPtr();
    }
    void DeleteNode(const uint64_t& location);
    NvmNode* AddExistNode(const int& nid,const int& flag=0);
    void AddInEdgeToNode(const uint64_t& location,const int& nid);
    void AddOutEdgeToNode(const uint64_t& location,const int& nid);
    void DelInEdgeOfNode(const uint64_t& location,const int& nid);
    void DelOutEdgeOfNode(const uint64_t& location,const int& nid);
private:
    Arena* NodeTable;
    CuckooHash::HashTable NodeHash;
    
    int MxNid,EdgeNum;
    int FreeNodeNum;
    uint64_t FreeNodeLocation;
    
    
public:
    Direct_graph(Arena* arena);
    NodeIter BegNI()const{
        NvmNode* curnode=(NvmNode*)NodeTable->BeginPtr();
        NodeIter iter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
        if(iter.GetFlag()<=0) iter++;
        return iter;
    }
    NodeIter GetNI(const int& nid)const{
        NvmNode* curnode;
        uint64_t location;
        if(NodeHash.Find(nid, location)){curnode=GetNodePtr(location);}
        else{curnode=NULL;}
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    NodeIter EndNI()const{
        NvmNode* curnode=(NvmNode*)NodeTable->EndPtr();
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    size_t GetNodeNum()const{return NodeHash.GetValidSlotNum();}
    bool NidIsExist(const int& nid)const{
        uint64_t location;
        return NodeHash.Find(nid, location);
    }
    int AddNode(const int nid=-1,const char* data="");
    int AddNode(const NodeIter& nodeI){return AddNode(nodeI.GetId(),nodeI.GetData());}
    void DelNode(const int nid);
    void DelNoe(const NodeIter& nodeI){DelNode(nodeI.GetId());}
    EdgeIter BegEI()const{
        NodeIter beginNI=BegNI();
        EdgeIter beginEI(beginNI,1);
        if(!beginNI.IsEnd() && beginEI.IsEmpty()) beginEI++;
        return beginEI;
    }
    EdgeIter GetEI(const NodeIter& nodeI,const int& edgeN)const {return EdgeIter(nodeI,edgeN);}
    EdgeIter GetEI(const int& nid,const int& edgeN)const{return EdgeIter(GetNI(nid), edgeN);}
    EdgeIter EndEI()const{return EdgeIter(EndNI(),1);}
    int GetEdgeNum()const{return EdgeNum;}
    bool EdgeIsExist(const int& SrcNid,const int& DstNid)const{
        if(!NidIsExist(SrcNid) || !NidIsExist(DstNid)) return false;
        return GetNI(SrcNid).IsOutNid(DstNid);
    }
    //若起始节点或终止节点不存在，返回-1，若边已存在返回0，否则返回1，注意当SrcNid=DstNid,只插入一次
    int AddEdge(const int& SrcNid,const int& DstNid);
    //若边已存在返回0，若起始节点或终止节点不存在时，先插入节点，然后插入边
    int AddEdge2(const int&SrcNid,const int& DstNid);
    //不检查边是否存在，若节点不存在，先新建节点，然后插入边
    void AddEdgeUnCheck(const int&SrcNid,const int& DstNid);
    int AddEdge(const EdgeIter& edgeI){return AddEdge(edgeI.GetSrcNid(), edgeI.GetDstNid());}
    void DelEdge(const int& SrcNid,const int& DstNid);
    void DelEdge(const EdgeIter& edgeI){DelEdge(edgeI.GetSrcNid(), edgeI.GetDstNid());}
};




//
//  Direct_graph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/1.
//

//#include"Direct_graph.h"

Direct_graph::Direct_graph(Arena* arena):NodeTable(arena),NodeHash(32, 4),MxNid(1),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0){
    if(NodeTable->EndPtr()!=NodeTable->BeginPtr()){
        NvmNode* temp=(NvmNode*)(NodeTable->BeginPtr());
        const char* end=NodeTable->EndPtr();
        while((char*)temp<end){
            if(temp->GetFlag()==-1){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=GetNodeLocation(temp);
                FreeNodeNum++;
            }
            else{
                if(temp->GetFlag()==1){
                    uint64_t location=GetNodeLocation(temp);
                    NodeHash.Add(temp->GetId(), location);
                    MxNid=MxNid>(temp->GetId()+1)?MxNid:(temp->GetId()+1);
                }
                EdgeNum+=temp->GetOutDeg();
            }
            temp++;
        }
    }
    
}

int Direct_graph::AddNode(const int nid,const char* data){
    int curnid;
    uint64_t curlocation;
    if(nid==-1){curnid=MxNid;MxNid++;}
    else{
        if(NidIsExist(nid)) return -1;
        if(nid==MxNid) MxNid++;
        curnid=nid;
    }
    if(FreeNodeNum>0){
        curlocation=FreeNodeLocation;
        NvmNode* old=GetNodePtr(FreeNodeLocation);
        FreeNodeLocation=old->GetNextNodeAddre();
        FreeNodeNum--;
        char* node=NodeTable->HeadPtr()+curlocation;
        new(node) NvmNode(curnid,data);
    }
    else{
        curlocation=NodeTable->EndPtr()-NodeTable->BeginPtr();
        new(NodeTable) NvmNode(curnid,data);
    }
    NodeHash.Add(curnid, curlocation);
    return curnid;
}

void Direct_graph::DeleteNode(const uint64_t &location){
    NvmNode* curnode=GetNodePtr(location);
    curnode->SetNextNodeAddre(FreeNodeLocation);
    curnode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Direct_graph::DelNode(const int nid){
    uint64_t location;
    if(NodeHash.Find(nid, location)){
        NvmNode* temp=GetNodePtr(location);
        while(temp->GetNextNodeAddre()!=0){
            uint64_t old=location;
            location=temp->GetNextNodeAddre();
            temp=GetNodePtr(location);
            DeleteNode(old);
        }
        DeleteNode(location);
    }
}

Direct_graph::NvmNode* Direct_graph::AddExistNode(const int &nid,const int& flag){
    NvmNode* newnode;
    if(FreeNodeNum>0){
        char* curptr=NodeTable->HeadPtr()+FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        FreeNodeNum--;
        newnode=new(curptr)NvmNode(nid,flag);
    }
    else{
        newnode=new(NodeTable)NvmNode(nid,flag);
    }
    return newnode;
}

void Direct_graph::AddInEdgeToNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0){
        if(curnode->AddInNid(nid)){
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddInNid(nid))
    {
        NvmNode* newnode=AddExistNode(curnode->GetId());
        newnode->AddInNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

void Direct_graph::AddOutEdgeToNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0)
    {
        if(curnode->AddOutNid(nid))
        {
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddOutNid(nid))
    {
        NvmNode* newnode=AddExistNode(curnode->GetId());
        newnode->AddOutNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

int Direct_graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return -1;
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid) AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}
int Direct_graph::AddEdge2(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid) AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}

void Direct_graph::AddEdgeUnCheck(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
}

void Direct_graph::DelInEdgeOfNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    NvmNode* prevnode=curnode;
    while(!curnode->DeleteInNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Direct_graph::DelOutEdgeOfNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    NvmNode* prevnode=curnode;
    while(!curnode->DeleteOutNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Direct_graph::DelEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) return;
    DelOutEdgeOfNode(location1, DstNid);
    DelInEdgeOfNode(location2, SrcNid);
    EdgeNum--;
}


#endif /* Direct_graph_h */
