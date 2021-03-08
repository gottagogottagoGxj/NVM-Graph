//
//  net.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/7.
//

#ifndef Property_Graph_h
#define Property_Graph_h
#include"Attr.h"
#include"hashtable.h"


//属性图，图是有向图，顶点表，没有边表，顶点有属性，边有属性（起始顶点、终止顶点标示一条边）
class Property_Graph{
    class Node{
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
        Node(const int nid):Nid(nid),Data("\0"),InNidVNum(0),OutNidVNum(0),Flag(1),NextNode(0){}
        Node(const int nid,const char* data);
        Node(MOut& Out);
        Node(const int nid,const int flag):Nid(nid),Data("\0"),InNidVNum(0),OutNidVNum(0),Flag(flag),NextNode(0){}
        void Save(MIn& In);
        
        static void* operator new(size_t size,Arena* table){return (void*)(table->AllocateNode());}
        static void operator delete(void* ptr,Arena* table){table->RecoverNode();}
        static void* operator new(size_t size,char* start){return (void*)start;}
        static void operator delete(void* ptr,char* start){}
        
        size_t GetMaxDeg()const{return InOutEdgeNumDef;}
        
        int GetId()const{return Nid;}
        const char* GetData()const{return Data;}
        void SetData(const char* data);
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
        bool IsInNid(const int& nid)const;
        bool IsOutNid(const int& nid)const;
        bool IsNbrNid(const int& nid)const{return IsInNid(nid)||IsOutNid(nid);}
        
        bool AddInNid(const int& nid);
        bool AddOutNid(const int& nid);
        bool AddInNid(const int* nid,const int& nidN);
        bool AddOutNid(const int* nid,const int& nidN);
        bool AddNbrNid(const int& nid){return AddInNid(nid)&&AddOutNid(nid);}
        void DeleteInNidN(const int & NodeN);
        void DeleteOutNidN(const int& NodeN);
        bool DeleteInNid(const int& nid);
        bool DeleteOutNid(const int& nid);
        bool DeleteNbrNid(const int& nid){return DeleteInNid(nid)&&DeleteOutNid(nid);}
        
        //排序，默认从小到大排序,快速排序
        void SortInNidV(const bool& asc=true){QuickSortCmp(InNidV, 0, InNidVNum, Comparison(asc));}
        void SortOutNidV(const bool& asc=true){QuickSortCmp(OutNidV, 0, OutNidVNum, Comparison(asc));}
        //二分查找
        bool IsInNidSort(const int& nid,const bool& asc=true)const{return SearchBin(InNidV, 0, InNidVNum, nid, Comparison(asc));}
        bool IsOutNidSort(const int& nid,const bool& asc=true)const{return SearchBin(OutNidV, 0, OutNidVNum, nid, Comparison(asc));}
        bool IsNbrNidSort(const int& nid,const bool& asc=true)const{return IsInNidSort(nid,asc)||IsOutNidSort(nid,asc);}
        bool AddInNidSort(const int& nid,const bool asc=true);
        bool AddOutNidSort(const int& nid,const bool asc=true);
        bool DeleteInNidSort(const int& nid,const bool asc=true);
        bool DeleteOutNidSort(const int& nid,const bool asc=true);
           
    };
    class NodeIter{
    private:
        const char* Begin;
        const char* End;
        Node* CurNode;
    public:
        NodeIter():Begin(NULL),End(NULL),CurNode(NULL){}
        NodeIter(const char* begin,const char* end,Node* cur):Begin(begin),End(end),CurNode(cur){}
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
        bool operator!=(const NodeIter& nodeI)const{return CurNode!=nodeI.CurNode;}
        Node& operator*()const{return *CurNode;}
        Node& operator()()const{return *CurNode;}
        Node& operator->()const{return *CurNode;}
        bool IsEmpty()const{return CurNode==NULL;}
        bool IsBegin()const{return (char*)CurNode==Begin;}
        bool IsEnd()const{return (char*)CurNode==End;}
        
        int GetId()const{return CurNode->GetId();}
        const char* GetData()const{return CurNode->GetData();}
        bool GetFlag()const{return CurNode->GetFlag();}
        int GetInDeg()const;
        int GetOutDeg()const;
        bool IsNbrNid(const int& nid)const;
        bool IsInNid(const int& nid)const;
        bool IsOutNid(const int& nid)const;
        int GetInNid(const int edgeN)const;
        int GetOutNid(const int edgeN)const;
        bool IsNbrNidSort(const int& nid)const;
        bool IsInNidSort(const int& nid)const;
        bool IsOutNidSort(const int& nid)const;
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
    static uint GetNodeSize(){return sizeof(Node);}
private:
    Node* GetNodePtr(const uint64_t& location) const{
        size_t nodeN=location/NodeTable->NodeSize();
        return (Node*)(NodeTable->BeginPtr())+nodeN;
    }
    uint64_t GetNodeLocation(const Node* node)const{
        return (char*)node-NodeTable->BeginPtr();
    }
    void DeleteNode(const uint64_t& location);
    Node* AddExistNode(const int& nid,const int& flag=0);
    void AddInEdgeToNode(const uint64_t& location,const int& nid);
    void AddOutEdgeToNode(const uint64_t& location,const int& nid);
    void DelInEdgeOfNode(const uint64_t& location,const int& nid);
    void DelOutEdgeOfNode(const uint64_t& location,const int& nid);
private:
    Arena* NodeTable;
    CuckooHash::HashTable NodeHash;
    
    int MxNid,NodeNum,EdgeNum;
    int FreeNodeNum;
    uint64_t FreeNodeLocation;
    Attr AttrN;
    AttrPair AttrE;
    
    
    
public:
    Property_Graph(Arena* arena,Arena* AttrNameN,Arena* AttrIndexN,Arena* AttrValueN,Arena* AttrNameE,Arena* AttrIndexE,Arena* AttrValueE);
    NodeIter BegNI()const{
        Node* curnode=(Node*)NodeTable->BeginPtr();
        NodeIter iter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
        if(iter.GetFlag()<=0) iter++;
        return iter;
    }
    NodeIter GetNI(const int& nid)const{
        Node* curnode;
        uint64_t location;
        if(NodeHash.Find(nid, location)){curnode=GetNodePtr(location);}
        else{curnode=NULL;}
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    NodeIter EndNI()const{
        Node* curnode=(Node*)NodeTable->EndPtr();
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    size_t GetNodeNum()const{return NodeNum;}
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
    int AddEdge(const EdgeIter& edgeI){return AddEdge(edgeI.GetSrcNid(), edgeI.GetDstNid());}
    void DelEdge(const int& SrcNid,const int& DstNid);
    void DelEdge(const EdgeIter& edgeI){DelEdge(edgeI.GetSrcNid(), edgeI.GetDstNid());}
    
    const char* GetNData(const int& Nid)const;
    void SetNData(const int& Nid,const char* Data);
    
    bool AddAttrDatN(const int& Nid,const int& AttrId,const char* Val);
    bool AddAttrDatN(const int& Nid,const char* AttrName,const char* Val);
    bool GetAttrDatN(const int& Nid,const int& AttrId,char* Val)const;
    bool GetAttrDatN(const int& Nid,const char* AttrName,char* Val)const;
    void DelAttrDatN(const int& Nid,const int& AttrId);
    void DelAttrDatN(const int& Nid,const char* AttrName);
    void DelAttrDatN(const int& Nid);
    int  AddAttrNameN(const char* AttrName){return AttrN.AddAttrName(AttrName);}
    bool GetAttrIdN(int& AttrId,const char* AttrName)const{return AttrN.GetAttrId(AttrId, AttrName);}
    bool GetAttrNameN(const int& AttrId,char* AttrName)const{return AttrN.GetAttrName(AttrId, AttrName);}
    
    bool AddAttrDatE(const int& SrcNid,const int& DstNid,const int& AttrId,const char* Val);
    bool AddAttrDatE(const int& SrcNid,const int& DstNid,const char* AttrName,const char* Val);
    bool GetAttrDatE(const int& SrcNid,const int& DstNid,const int& AttrId,char* Val)const;
    bool GetAttrDatE(const int& SrcNid,const int& DstNid,const char* AttrName,char* Val)const;
    void DelAttrDatE(const int& SrcNid,const int& DstNid,const int& AttrId);
    void DelAttrDatE(const int& SrcNid,const int& DstNid,const char* AttrName);
    void DelAttrDatE(const int& SrcNid,const int& DstNid);
    int  AddAttrNameE(const char* AttrName){return AttrE.AddAttrName(AttrName);}
    bool GetAttrIdE(int& AttrId,const char* AttrName)const{return AttrE.GetAttrId(AttrId, AttrName);}
    bool GetAttrNameE(const int& AttrId,char* AttrName)const{return AttrE.GetAttrName(AttrId, AttrName);}
    
    //遍历NodeHash,可以快速得到图中全部顶点
    CuckooHash::IterPair GetNidBegin()const{return NodeHash.Begin();}
    CuckooHash::IterPair GetNidEnd()const{return NodeHash.End();}
};



inline Property_Graph::Node::Node(const int nid,const char* data):Nid(nid),InNidVNum(0),OutNidVNum(0),Flag(1),NextNode(0){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline Property_Graph::Node::Node(MOut& Out){
    Out.Load(Nid);
    Out.Load(Data, NodeDatalengthDef);
    Out.Load(InNidVNum);
    Out.Load(OutNidVNum);
    Out.Load(InNidV, InOutEdgeNumDef);
    Out.Load(OutNidV, InOutEdgeNumDef);
    Out.Load(Flag);
    Out.Load(NextNode);
}
inline void Property_Graph::Node::Save(MIn& In){
    In.Save(Nid);
    In.Save(Data, NodeDatalengthDef);
    In.Save(InNidVNum);
    In.Save(OutNidVNum);
    In.Save(InNidV, InOutEdgeNumDef);
    In.Save(OutNidV, InOutEdgeNumDef);
    In.Save(Flag);
    In.Save(NextNode);
}
inline void Property_Graph::Node::SetData(const char* data){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline bool Property_Graph::Node::IsInNid(const int& nid)const{
    for(int i=0;i<InNidVNum;++i){
        if(InNidV[i]==nid) return true;
    }
    return false;
}
inline bool Property_Graph::Node::IsOutNid(const int& nid)const{
    for(int i=0;i<OutNidVNum;++i){
        if(OutNidV[i]==nid) return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddInNid(const int& nid){
    if(InNidVNum<InOutEdgeNumDef){
        InNidV[InNidVNum]=nid;
        InNidVNum++;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddOutNid(const int& nid){
    if(OutNidVNum<InOutEdgeNumDef){
        OutNidV[OutNidVNum]=nid;
        OutNidVNum++;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddInNid(const int* nid,const int& nidN){
    if(InNidVNum+nidN<=InOutEdgeNumDef){
        memcpy(InNidV, nid, sizeof(int)*nidN);
        InNidVNum+=nidN;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddOutNid(const int* nid,const int& nidN){
    if(OutNidVNum+nidN<=InOutEdgeNumDef){
        memcpy(OutNidV, nid, sizeof(int)*nidN);
        OutNidVNum+=nidN;
        return true;
    }
    return false;
}
inline void Property_Graph::Node::DeleteInNidN(const int & NodeN){
    for(int i=NodeN;i<(InNidVNum-1);++i) {InNidV[i]=InNidV[i+1];}
    InNidVNum--;
}
inline void Property_Graph::Node::DeleteOutNidN(const int& NodeN){
    for(int i=NodeN;i<(OutNidVNum-1);++i) {OutNidV[i]=OutNidV[i+1];}
    OutNidVNum--;
}
inline bool Property_Graph::Node::DeleteInNid(const int& nid){
    for(int i=0;i<InNidVNum;++i){
        if(InNidV[i]==nid){
            for(int j=i;j<(InNidVNum-1);++j){InNidV[j]=InNidV[j+1];}
            InNidVNum--;
            return true;
        }
    }
    return false;
}
inline bool Property_Graph::Node::DeleteOutNid(const int& nid){
    for(int i=0;i<OutNidVNum;++i){
        if(OutNidV[i]==nid){
            for(int j=i;j<(OutNidVNum-1);++j){OutNidV[j]=OutNidV[j+1];}
            OutNidVNum--;
            return true;
        }
    }
    return false;
}
bool Property_Graph::Node::AddInNidSort(const int& nid,const bool asc){
    if(InNidVNum<InOutEdgeNumDef){
        int index=SearchBinRight(InNidV, 0, InNidVNum, nid, Comparison(asc));
        for(int i=InNidVNum;i>index;--i) InNidV[i]=InNidV[i-1];
        InNidV[index]=nid;
        InNidVNum++;
        return true;
    }
    return false;
}
bool Property_Graph::Node::AddOutNidSort(const int& nid,const bool asc){
    if(OutNidVNum<InOutEdgeNumDef){
        int index=SearchBinRight(OutNidV, 0, OutNidVNum, nid, Comparison(asc));
        for(int i=OutNidVNum;i>index;--i) OutNidV[i]=OutNidV[i-1];
        OutNidV[index]=nid;
        OutNidVNum++;
        return true;
    }
    return false;
}
bool Property_Graph::Node::DeleteInNidSort(const int& nid,const bool asc){
    int index=SearchBin(InNidV, 0, InNidVNum, nid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<InNidVNum;++i) InNidV[i]=InNidV[i+1];
        InNidVNum--;
        return  true;
    }
    return false;
}
bool Property_Graph::Node::DeleteOutNidSort(const int& nid,const bool asc){
    int index=SearchBin(OutNidV, 0, OutNidVNum, nid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<OutNidVNum;++i) OutNidV[i]=OutNidV[i+1];
        OutNidVNum--;
        return true;
    }
    return false;
}
int Property_Graph::NodeIter::GetInDeg()const{
    int InDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        InDeg+=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    InDeg+=temp->GetNextNodeAddre();
    return InDeg;
}
int Property_Graph::NodeIter::GetOutDeg()const{
    int OutDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        OutDeg+=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    OutDeg+=temp->GetNextNodeAddre();
    return OutDeg;
}
bool Property_Graph::NodeIter::IsNbrNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrNid(nid);
}
bool Property_Graph::NodeIter::IsInNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsInNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNid(nid);
}
bool Property_Graph::NodeIter::IsOutNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsOutNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutNid(nid);
}
int Property_Graph::NodeIter::GetInNid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while(n>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
        n-=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetInDeg()) return -1;
    return temp->GetInNid(n-1);
}
int Property_Graph::NodeIter::GetOutNid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetOutDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetOutDeg()) return -1;
    return temp->GetOutNid(n-1);
}
bool Property_Graph::NodeIter::IsNbrNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInNidSort(nid)||temp->IsOutNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNidSort(nid)||temp->IsOutNidSort(nid);
}
bool Property_Graph::NodeIter::IsInNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNidSort(nid);
}
bool Property_Graph::NodeIter::IsOutNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutNidSort(nid);
}

Property_Graph::Property_Graph(Arena* arena,Arena* AttrNameN,Arena* AttrIndexN,Arena* AttrValueN,Arena* AttrNameE,Arena* AttrIndexE,Arena* AttrValueE):NodeTable(arena),NodeHash(32, 4),MxNid(1),NodeNum(0),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0),AttrN(AttrNameN, AttrIndexN, AttrValueN),AttrE(AttrNameE, AttrIndexE, AttrValueE){
    if(NodeTable->EndPtr()!=NodeTable->BeginPtr()){
        Node* temp=(Node*)(NodeTable->BeginPtr());
        const char* end=NodeTable->EndPtr();
        while((char*)temp<end){
            if(temp->GetFlag()==-1){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=GetNodeLocation(temp);
                FreeNodeNum++;
            }
            else{
                if(temp->GetFlag()==1){
                    NodeNum++;
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

int Property_Graph::AddNode(const int nid,const char* data){
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
        Node* old=GetNodePtr(FreeNodeLocation);
        FreeNodeLocation=old->GetNextNodeAddre();
        FreeNodeNum--;
        char* node=NodeTable->HeadPtr()+curlocation;
        new(node) Node(curnid,data);
    }
    else{
        curlocation=NodeTable->EndPtr()-NodeTable->BeginPtr();
        new(NodeTable) Node(curnid,data);
    }
    NodeHash.Add(curnid, curlocation);
    NodeNum++;
    return curnid;
}
Property_Graph::Node* Property_Graph::AddExistNode(const int &nid,const int& flag){
    Node* newnode;
    if(FreeNodeNum>0){
        char* curptr=NodeTable->HeadPtr()+FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        FreeNodeNum--;
        newnode=new(curptr)Node(nid,flag);
    }
    else{
        newnode=new(NodeTable)Node(nid,flag);
    }
    return newnode;
}

void Property_Graph::DeleteNode(const uint64_t &location){
    Node* curnode=GetNodePtr(location);
    curnode->SetNextNodeAddre(FreeNodeLocation);
    curnode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Property_Graph::DelNode(const int nid){
    uint64_t location;
    if(!NodeHash.Find(nid, location)) return;
    Node* temp=GetNodePtr(location);
    do{
        for(int i=0;i<temp->GetInDeg();++i){
            int SrcNid=temp->GetInNid(i);
            uint64_t SrcNodeLocation;
            if(NodeHash.Find(SrcNid, SrcNodeLocation)){
                DelOutEdgeOfNode(SrcNodeLocation, nid);
                AttrE.DelAttrDat(SrcNid, nid);
            }
        }
        for(int i=0;i<temp->GetOutDeg();++i){
            int DstNid=temp->GetOutNid(i);
            uint64_t DstNodeLocation;
            if(NodeHash.Find(DstNid, DstNodeLocation)){
                DelInEdgeOfNode(DstNodeLocation, nid);
                AttrE.DelAttrDat(nid, DstNid);
            }
        }
        uint64_t old=location;
        location=temp->GetNextNodeAddre();
        temp=GetNodePtr(location);
        DeleteNode(old);
    }while(temp->GetNextNodeAddre()!=0);
    AttrN.DelAttrDat(nid);
    NodeHash.Delete(nid);
    NodeNum--;
}



void Property_Graph::AddInEdgeToNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0){
        if(curnode->AddInNid(nid)){
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddInNid(nid))
    {
        Node* newnode=AddExistNode(curnode->GetId());
        newnode->AddInNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

void Property_Graph::AddOutEdgeToNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
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
        Node* newnode=AddExistNode(curnode->GetId());
        newnode->AddOutNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

int Property_Graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return -1;
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}
int Property_Graph::AddEdge2(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}



void Property_Graph::DelInEdgeOfNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    Node* prevnode=curnode;
    while(!curnode->DeleteInNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Property_Graph::DelOutEdgeOfNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    Node* prevnode=curnode;
    while(!curnode->DeleteOutNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Property_Graph::DelEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) return;
    DelOutEdgeOfNode(location1, DstNid);
    DelInEdgeOfNode(location2, SrcNid);
    AttrE.DelAttrDat(SrcNid, DstNid);
    EdgeNum--;
}

inline const char* Property_Graph::GetNData(const int &Nid) const{
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        return GetNodePtr(location)->GetData();
    }
    return NULL;
}
void Property_Graph::SetNData(const int &Nid, const char *Data){
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        GetNodePtr(location)->SetData(Data);
    }
}
bool Property_Graph::AddAttrDatN(const int &Nid, const int &AttrId, const char *Val){
    if(!NidIsExist(Nid)) return false;
    return AttrN.AddAttrDat(Nid, AttrId, Val);
}
bool Property_Graph::AddAttrDatN(const int &Nid, const char *AttrName, const char *Val){
    if(NidIsExist(Nid)) return false;
    return AttrN.AddAttrDat(Nid, AttrName, Val);
}
bool Property_Graph::GetAttrDatN(const int &Nid, const int &AttrId, char *Val)const{
    if(NidIsExist(Nid)) return false;
    return AttrN.GetAttrDat(Nid, AttrId, Val);
}
bool Property_Graph::GetAttrDatN(const int& Nid,const char* AttrName,char* Val)const{
    if(!NidIsExist(Nid)) return false;
    return AttrN.GetAttrDat(Nid, AttrName, Val);
}
void Property_Graph::DelAttrDatN(const int &Nid, const int &AttrId){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid, AttrId);}
}
void Property_Graph::DelAttrDatN(const int &Nid, const char *AttrName){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid, AttrName);}
}
void Property_Graph::DelAttrDatN(const int &Nid){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid);}
}

bool Property_Graph::AddAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId, const char *Val){
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.AddAttrDat(SrcNid, DstNid, AttrId, Val);
}
bool Property_Graph::AddAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName, const char *Val){
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.AddAttrDat(SrcNid, DstNid, AttrName, Val);
}
bool Property_Graph::GetAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId, char *Val)const{
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.GetAttrDat(SrcNid, DstNid, AttrId, Val);
}
bool Property_Graph::GetAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName, char *Val)const{
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.GetAttrDat(SrcNid, DstNid, AttrName, Val);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid, AttrId);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid, AttrName);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid);
}


#endif /* net_h */
