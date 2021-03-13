//
//  network.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/1/5.
//

#ifndef network_h
#define network_h
#include"Attr.h"
#include"hashtable.h"
class Net{
public:
    class Node{
    private:
        int Nid;
        char Data[NodeDatalengthDef];
        int InEidVNum;
        int OutEidVNum;
        int InEidV[InOutEidNumDef];
        int OutEidV[InOutEidNumDef];
        int Flag;//标示当前节点状态，-1代表无效，1代表头节点，0代表中间节点
        uint64_t NextNode;
    public:
        Node(const int nid):Nid(nid),Data("\0"),InEidVNum(0),OutEidVNum(0),Flag(1),NextNode(0){}
        Node(const int nid,const int flag):Nid(nid),Data("\0"),InEidVNum(0),OutEidVNum(0),Flag(flag),NextNode(0){}
        Node(const int nid,const char* data);
        Node(MOut& Out);
        void Save(MIn& In);
        
        static void* operator new(size_t size,Arena* table){return (void*)(table->AllocateNode());}
        static void operator delete(void* ptr,Arena* table){table->RecoverNode();}
        static void* operator new(size_t size,char* start){return (void*)start;}
        static void operator delete(void* ptr,char* start){}
    
        int GetNid()const{return Nid;}
        const char* GetData()const{return Data;}
        void SetData(const char* data);//Data长度要小于默认最大长度
        int GetFlag()const{return Flag;}
        void SetFlag(const int& flag){Flag=flag;}
        uint64_t GetNextNodeAddre()const{return NextNode;}
        void SetNextNodeAddre(const uint64_t& location){NextNode=location;}
        int GetDeg()const{return GetInDeg()+GetOutDeg();}
        int GetInDeg()const{return InEidVNum;}
        int GetOutDeg()const{return OutEidVNum;}
        int GetNbrEid(const int& EidN)const{return EidN<GetOutDeg()?GetOutEid(EidN):GetInEid(EidN-GetOutDeg());}
        int GetInEid(const int& EidN)const{return InEidV[EidN];}//获取边的信息，EidN取值[0,InEidVNum)
        int GetOutEid(const int& EidN)const{return OutEidV[EidN];}
        bool IsInEid(const int& eid)const;
        bool IsOutEid(const int& eid)const;
        bool IsNbrEid(const int& eid)const{return IsInEid(eid)||IsOutEid(eid);}
        bool AddInEid(const int& Eid);
        bool AddOutEid(const int& Eid);
        bool AddInEid(const int* Eid,const int& EidN);//批量插入边，插入int Eid[EidN]
        bool AddOutEid(const int* Eid,const int& EidN);
        bool AddNbrEid(const int& Eid){return AddInEid(Eid)&&AddOutEid(Eid);}
        void DeleteInEidN(const int & EidN);//删除第EidN条入边,EidN取值为[0，InEidVNum-1]
        void DeleteOutEidN(const int& EidN);
        bool DeleteInEid(const int& Eid);
        bool DeleteOutEid(const int& Eid);
        bool DeleteNbrEid(const int& Eid){return DeleteInEid(Eid)&&DeleteOutEid(Eid);}
        
        //排序，默认从小到大排序,插入排序
        void SortInEidV(const bool& asc=true){QuickSortCmp(InEidV, 0, InEidVNum, Comparison(asc));}
        void SortOutEidV(const bool& asc=true){QuickSortCmp(OutEidV, 0, OutEidVNum, Comparison(asc));}
        //二分查找
        bool IsInEidSort(const int& Eid,const bool& asc=true)const{return SearchBin(InEidV, 0, InEidVNum, Eid, Comparison(asc))>=0;}
        bool IsOutEidSort(const int& Eid,const bool& asc=true)const{return SearchBin(OutEidV, 0, OutEidVNum, Eid, Comparison(asc))>=0;}
        bool IsNbrEidSort(const int& Eid,const bool& asc=true)const{return IsInEidSort(Eid,asc)||IsOutEidSort(Eid,asc);}
        bool AddInEidSort(const int& Eid,const bool& asc=true);
        bool AddOutEidSort(const int& Eid,const bool& asc=true);
        bool DeleteInEidSort(const int& Eid,const bool& asc=true);
        bool DeleteOutEidSort(const int& Eid,const bool& asc=true);
    };
    class Edge{
    private:
        int Eid,SrcNid,DstNid;
        char Data[EdgeDataLengthDef];
    public:
        Edge(const int eid,const int src,const int dst):Eid(eid),SrcNid(src),DstNid(dst),Data("\0"){}
        Edge(const int eid,const int src,const int dst,const char* data):Eid(eid),SrcNid(src),DstNid(dst){
            size_t length=strlen(data);
            if(length>=EdgeDataLengthDef) length=EdgeDataLengthDef-1;
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        static void* operator new(size_t size,Arena* table){return (void*)(table->AllocateNode());}
        static void operator delete(void* ptr,Arena*table){table->RecoverNode();}
        static void* operator new(size_t size,char* ptr){return (void*)ptr;}
        static void operator delete(void* ptr,char* p){}
        
        int GetEid()const{return Eid;}
        void SetEid(const int& eid){Eid=eid;}//利用Eid标示是否有效，无效Eid=-1,无效边串成链表，利用Data存储下一个Edge的location
        int GetSrcNid()const{return SrcNid;}
        int GetDstNid()const{return DstNid;}
        const char* GetData()const{return Data;}
        void SetData(const char* data){
            size_t length=strlen(data);
            if(length>=EdgeDataLengthDef) length=EdgeDataLengthDef-1;
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        uint64_t GetNextEdge()const{
            uint64_t location;
            memcpy(&location, Data, sizeof(uint64_t));
            return location;
        }
        void SetNextEdge(const uint64_t& location){memcpy(Data, &location, sizeof(uint64_t));}
    };
    class NodeIter{
    private:
        const char* Begin;
        const char* End;
        Node* CurNode;
        const Net* Graph;
    public:
        NodeIter():Begin(NULL),End(NULL),CurNode(NULL),Graph(NULL){}
        NodeIter(const char* begin,const char* end,Node* cur,const Net* graph):Begin(begin),End(end),CurNode(cur),Graph(graph){}
        NodeIter(const NodeIter& nodeI):Begin(nodeI.Begin),End(nodeI.End),CurNode(nodeI.CurNode),Graph(nodeI.Graph){}
        NodeIter& operator=(const NodeIter& nodeI){Begin=nodeI.Begin;End=nodeI.End;CurNode=nodeI.CurNode;Graph=nodeI.Graph;return *this;}
        NodeIter& operator++(int){
            if(!IsEnd()){CurNode++;while((char*)CurNode<End && CurNode->GetFlag()<=0) CurNode++;}
            return *this;
        }
        NodeIter& operator--(int){
            if(!IsBegin()){CurNode--;while ((char*)CurNode>Begin && CurNode->GetFlag()<=0) CurNode--;}
            return *this;
        }
        bool operator<(const NodeIter& nodeI)const {return CurNode<nodeI.CurNode;}
        bool operator==(const NodeIter& nodeI)const {return CurNode==nodeI.CurNode;}
        Node& operator*()const{return *CurNode;}
        const char* operator()()const{return CurNode->GetData();}
        Node& operator->()const{return *CurNode;}
        bool IsEmpty()const{return CurNode==NULL;}
        bool IsBegin()const{return (char*)CurNode==Begin;}
        bool IsEnd()const{return (char*)CurNode==End;}
        
        int GetId()const{return CurNode->GetNid();}
        const char* GetData()const{return CurNode->GetData();}
        bool GetFlag()const{return CurNode->GetFlag();}
        int GetInDeg()const;
        int GetOutDeg()const;
        bool IsNbrEid(const int& eid)const;
        bool IsInEid(const int& eid)const;
        bool IsOutEid(const int& eid)const;
        bool IsNbrEidSort(const int& eid)const;
        bool IsInEidSort(const int& eid)const;
        bool IsOutEidSort(const int& eid)const;
        int GetInEid(const int edgeN)const;//获取边的信息(Eid)，edgeN取值范围是[1,GetInDeg()],即第edgeN条边的Eid
        int GetOutEid(const int edgeN)const;
        const char* GetInEData(const int edgeN)const{return Graph->GetEData(GetInEid(edgeN));}
        const char* GetOutEData(const int edgeN)const{return Graph->GetEData(GetOutEid(edgeN));}
        int GetNbrEid(const int& edgeN)const;//当前实现很迷
        int GetInNid(const int& edgeN)const;//edgeN取值范围是[1,GetInDeg()],即求第edgeN条边的Nid
        int GetOutNid(const int& edgeN)const;
        const char* GetInNData(const int edgeN)const{return Graph->GetNData(GetInNid(edgeN));}
        const char* GetOutNData(const int edgeN)const{return Graph->GetNData(GetOutNid(edgeN));}
        int GetNbrNid(const int&edgeN)const;
        void SortEidV();
        
    };
    class EdgeIter{
    private:
        const char* Begin,*End;
        Edge* CurEdge;
        const Net* Graph;
    public:
        EdgeIter():Begin(NULL),End(NULL),CurEdge(NULL),Graph(NULL){}
        EdgeIter(const char* begin,const char* end,Edge* edge,const Net* graph):Begin(begin),End(end),CurEdge(edge),Graph(graph){}
        EdgeIter(const EdgeIter& edgeI):Begin(edgeI.Begin),End(edgeI.End),CurEdge(edgeI.CurEdge),Graph(edgeI.Graph){}
        EdgeIter& operator=(const EdgeIter& edgeI){Begin=edgeI.Begin; End=edgeI.End;CurEdge=edgeI.CurEdge;Graph=edgeI.Graph;return *this; }
        EdgeIter& operator++(int){
            if(!IsEnd()){CurEdge++;while((char*)CurEdge<End && CurEdge->GetEid()==-1) CurEdge++;}
            return *this;
        }
        EdgeIter& operator--(int){
            if(!IsBegin()){CurEdge--;while((char*)CurEdge>Begin && CurEdge->GetEid()==-1) CurEdge--;}
            return *this;
        }
        Edge& operator*()const{return *CurEdge;}
        const char* operator()()const{return CurEdge->GetData();}
        Edge& operator->()const{return *CurEdge;}
        bool operator<(const EdgeIter& edgeI)const{return CurEdge<edgeI.CurEdge;}
        bool operator==(const EdgeIter& edgeI)const{return CurEdge==edgeI.CurEdge;}
        bool IsEnd()const{return (char*)CurEdge==End;}
        bool IsBegin()const{return (char*)CurEdge==Begin;}
        bool IsEmpty()const{return CurEdge==NULL;}
        int GetEid()const{return CurEdge->GetEid();}
        const char* GetData()const{return CurEdge->GetData();}
        int GetSrcNid()const{return CurEdge->GetSrcNid();}
        const char* GetSrcNData()const{return Graph->GetNData(GetSrcNid());}
        int GetDstNid()const{return CurEdge->GetDstNid();}
        const char* GetDstNData()const{return Graph->GetNData(GetDstNid());}
    };
public:
    static size_t GetEdgeSize(){return sizeof(Edge);}
    static size_t GetNodeSize(){return sizeof(Node);}
private:
    Arena* Nodetable,*EdgeTable;
    CuckooHash::HashTable NodeHash,EdgeHash;
    
    int MxNid,MxEid,NodeNum,EdgeNum;
    int FreeNodeNum,FreeEdgeNum;
    uint64_t FreeNodeLocation,FreeEdgeLocation;
    Attr AttrN,AttrE;
private:
    Node* GetNodePtr(const uint64_t& location)const{return (Node*)(Nodetable->BeginPtr()+location);}
    uint64_t GetNodeLocation(const Node* node)const{return (char*)node-Nodetable->BeginPtr();}
    Edge* GetEdgePtr(const uint64_t& location)const{return (Edge*)(EdgeTable->BeginPtr()+location);}
    uint64_t GetEdgeLocation(const Edge* edge)const{return (char*)edge-EdgeTable->BeginPtr();}
    void DelInEidOfNode(const uint64_t& location,const int& Eid);//删除顶点的入边Eid,遍历对应的链表删除
    void DelOutEidOfNode(const uint64_t& location,const int& Eid);
    Edge* DelEidOfTable(const int& Eid);
    void DelNodeOfLocation(const uint64_t& location);//根据位置删除，删除顶点表中一个节点
    void AddInEidOfNode(const uint64_t& location,const int& Eid);
    void AddOutEidOfNode(const uint64_t& location,const int& Eid);
    Node* AddExistNode(const int& nid,const int& flag);
    void AddEdgeToTable(const int &SrcNid, const int &DstNid,const char* Data,int Eid);
public:
    Net(Arena* node,Arena* edge,Arena* namen,Arena* indexn,Arena* atrren,Arena* namee,Arena* indexe,Arena* attre);
    NodeIter BegNI()const{
        NodeIter iter=NodeIter(Nodetable->BeginPtr(),Nodetable->EndPtr(),(Node*)(Nodetable->HeadPtr()),this);
        if(iter.GetFlag()<=0) iter++;
        return iter;
    }
    NodeIter EndNI()const{return NodeIter(Nodetable->BeginPtr(),Nodetable->EndPtr(),(Node*)(Nodetable->EndPtr()),this);}
    NodeIter GetNI(const int& nid)const{
        Node* CurNode=NULL;
        uint64_t location;
        if(NodeHash.Find(nid, location)){CurNode=GetNodePtr(location);}
        return NodeIter(Nodetable->BeginPtr(),Nodetable->EndPtr(),CurNode,this);
    }
    EdgeIter BegEI()const{
        EdgeIter iter=EdgeIter(EdgeTable->BeginPtr(),EdgeTable->EndPtr(),(Edge*)(EdgeTable->HeadPtr()),this);
        if(iter.GetEid()==-1) iter++;
        return iter;
    }
    EdgeIter EndEI()const{return EdgeIter(EdgeTable->BeginPtr(),EdgeTable->EndPtr(),(Edge*)(EdgeTable->EndPtr()),this);}
    EdgeIter GetEI(const int& eid)const{
        Edge* CurEdge=NULL;
        uint64_t location;
        if(EdgeHash.Find(eid, location)){CurEdge=GetEdgePtr(location);}
        return EdgeIter(EdgeTable->BeginPtr(), EdgeTable->EndPtr(), CurEdge,this);
    }
    int GetNodes()const{return NodeNum;}
    int GetMxNid()const{return MxNid;}
    int AddNode(int Nid=-1,const char* data="\0");
    int AddNode(const NodeIter& NodeI){return AddNode(NodeI.GetId(),NodeI.GetData());}
    void DelNode(const int& Nid);
    void DelNode(const NodeIter& NodeI){DelNode(NodeI.GetId());}
    bool IsNode(const int& Nid)const{uint64_t location; return NodeHash.Find(Nid,location);}
    
    int GetEdges()const{return EdgeNum;}
    int GetMxEid()const{return MxEid;}
    int AddEdge(const int& SrcNid,const int& DstNid,const char* Data="\0",int Eid=-1);
    int AddEdge2(const int& SrcNid,const int& DstNid,const char* Data="\0",int Eid=-1);
    int AddEdge(const EdgeIter& iter){return AddEdge(iter.GetSrcNid(), iter.GetDstNid(),iter.GetData(),iter.GetEid());}
    void DelEdge(const int& Eid);
    void DelEdge(const int& SrcNid,const int& DstNid);
    void DelEdge(const EdgeIter& iter){DelEdge(iter.GetEid());}
    bool IsEdge(const int& SrcNid,const int& DstNid,int& Eid)const;
    bool IsEdge(const int& Eid)const{uint64_t location;return EdgeHash.Find(Eid, location);}
    
    const char* GetNData(const int& Nid)const;
    void SetNData(const int& Nid,const char* Data);
    const char* GetEData(const int& Eid)const;
    void SetEData(const int& Eid,const char* Data);
    
    bool AddAttrDatN(const int& Nid,const int& AttrId,const void* Val,const uint& length);
    bool AddAttrDatN(const int& Nid,const char* AttrName,const void* Val,const uint& length);
    bool GetAttrDatN(const int& Nid,const int& AttrId,void* Val)const;
    bool GetAttrDatN(const int& Nid,const char* AttrName,void* Val)const;
    void DelAttrDatN(const int& Nid,const int& AttrId);
    void DelAttrDatN(const int& Nid,const char* AttrName);
    void DelAttrDatN(const int& Nid);
    int AddAttrNameN(const char* AttrName);
    bool GetAttrIdN(int& AttrId,const char* AttrName)const;
    bool GetAttrNameN(const int& AttrId,char* AttrName)const;
    
    
    bool AddAttrDatE(const int& Eid,const int& AttrId,const void* Val,const uint& length);
    bool AddAttrDatE(const int& Eid,const char* AttrName,const void* Val,const uint& length);
    bool GetAttrDatE(const int& Eid,const int& AttrId,void* Val)const;
    bool GetAttrDatE(const int& Eid,const char* AttrName,void* Val)const;
    void DelAttrDatE(const int& Eid,const int& AttrId);
    void DelAttrDatE(const int& Eid,const char* AttrName);
    void DelAttrDatE(const int& Eid);
    int AddAttrNameE(const char* AttrName);
    bool GetAttrIdE(int& AttrId,const char* AttrName)const;
    bool GetAttrNameE(const int& AttrId,char* AttrName)const;
    
};




inline Net::Node::Node(const int nid,const char* data):Nid(nid),InEidVNum(0),OutEidVNum(0),Flag(1),NextNode(0){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline Net::Node::Node(MOut& Out){
    Out.Load(Nid);
    Out.Load(Data, NodeDatalengthDef);
    Out.Load(InEidVNum);
    Out.Load(OutEidVNum);
    Out.Load(InEidV, InOutEidNumDef);
    Out.Load(OutEidV, InOutEidNumDef);
    Out.Load(Flag);
    Out.Load(NextNode);
}
void Net::Node::Save(MIn &In){
    In.Save(Nid);
    In.Save(Data, NodeDatalengthDef);
    In.Save(InEidVNum);
    In.Save(OutEidVNum);
    In.Save(InEidV, InOutEidNumDef);
    In.Save(OutEidV, InOutEidNumDef);
    In.Save(Flag);
    In.Save(NextNode);
}
inline void Net::Node::SetData(const char *data){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline bool Net::Node::IsInEid(const int &eid)const{
    for(int i=0;i<InEidVNum;++i){if(InEidV[i]==eid) return true;}
    return false;
}
inline bool Net::Node::IsOutEid(const int& eid)const{
    for(int i=0;i<OutEidVNum;++i){if(OutEidV[i]==eid) return true;}
    return false;
}
inline bool Net::Node::AddInEid(const int& Eid){
    if(InEidVNum<InOutEidNumDef){
        InEidV[InEidVNum]=Eid;
        InEidVNum++;
        return true;
    }
    return false;
}
inline bool Net::Node::AddOutEid(const int &Eid){
    if(OutEidVNum<InOutEidNumDef){
        OutEidV[OutEidVNum]=Eid;
        OutEidVNum++;
        return true;
    }
    return false;
}
inline bool Net::Node::AddInEid(const int *Eid, const int &EidN){
    if(InEidVNum+EidN<=InOutEidNumDef){
        memcpy(InEidV, Eid, sizeof(int)*EidN);
        InEidVNum+=EidN;
        return true;
    }
    return false;
}
inline bool Net::Node::AddOutEid(const int *Eid, const int &EidN){
    if(OutEidVNum+EidN<=InOutEidNumDef){
        memcpy(OutEidV, Eid, sizeof(int)*EidN);
        OutEidVNum+=EidN;
        return true;
    }
    return false;
}
inline void Net::Node::DeleteInEidN(const int &EidN){
    for(int i=EidN;i<(InEidVNum-1);++i){InEidV[i]=InEidV[i+1];}
    InEidVNum--;
}
inline void Net::Node::DeleteOutEidN(const int &EidN){
    for(int i=EidN;i<(OutEidVNum-1);++i){OutEidV[i]=OutEidV[i+1];}
    OutEidVNum--;
}
inline bool Net::Node::DeleteInEid(const int& Eid){
    for(int i=0;i<InEidVNum;++i){
        if(InEidV[i]==Eid){
            for(int j=i;j<(InEidVNum-1);++j){InEidV[j]=InEidV[j+1];}
            InEidVNum--;
            return true;
        }
    }
    return false;
}
inline bool Net::Node::DeleteOutEid(const int& Eid){
    for(int i=0;i<OutEidVNum;++i){
        if(OutEidV[i]==Eid){
            for(int j=i;j<(OutEidVNum-1);++j){OutEidV[j]=OutEidV[j+1];}
            OutEidVNum--;
            return true;
        }
    }
    return false;
}

bool Net::Node::AddInEidSort(const int &Eid,const bool& asc){
    if(InEidVNum<InOutEidNumDef){
        int index=SearchBinRight(InEidV, 0, InEidVNum, Eid, Comparison(asc));
        for(int i=InEidVNum;i>index;--i) InEidV[i]=InEidV[i-1];
        InEidV[index]=Eid;
        InEidVNum++;
        return true;
    }
    return false;
}
bool Net::Node::AddOutEidSort(const int &Eid,const bool& asc){
    if(OutEidVNum<InOutEidNumDef){
        int index=SearchBinRight(OutEidV, 0, OutEidVNum, Eid, Comparison(asc));
        for(int i=OutEidVNum;i>index;--i) OutEidV[i]=OutEidV[i-1];
        OutEidV[index]=Eid;
        OutEidVNum++;
        return true;
    }
    return false;
}
bool Net::Node::DeleteInEidSort(const int &Eid,const bool& asc){
    int index=SearchBin(InEidV, 0, InEidVNum, Eid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<(InEidVNum-1);++i) InEidV[i]=InEidV[i+1];
        InEidVNum--;
        return true;
    }
    return false;
}
bool Net::Node::DeleteOutEidSort(const int &Eid,const bool& asc){
    int index=SearchBin(OutEidV, 0, OutEidVNum, Eid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<(OutEidVNum-1);++i) OutEidV[i]=OutEidV[i+1];
        OutEidVNum--;
        return true;
    }
    return false;
}

Net::Net(Arena* node,Arena* edge,Arena* namen,Arena* indexn,Arena* attrn,Arena* namee,Arena* indexe,Arena* attre):Nodetable(node),EdgeTable(edge),NodeHash(32,4),EdgeHash(32,4),MxNid(1),MxEid(1),NodeNum(0),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0),FreeEdgeNum(0),FreeEdgeLocation(0),AttrN(namen,indexn,attrn),AttrE(namee,indexe,attre){
    if(Nodetable->EndPtr()!=Nodetable->BeginPtr()){
        Node* temp=(Node*)Nodetable->HeadPtr();
        for(;(char*)temp<Nodetable->EndPtr();++temp){
            if(temp->GetFlag()<0){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=GetNodeLocation(temp);
                FreeNodeNum++;
            }
            else if(temp->GetFlag()>0){
                NodeNum++;
                int CurNid=temp->GetNid();
                MxNid=MxNid>(CurNid+1)?MxNid:(CurNid+1);
                NodeHash.Add(CurNid, GetNodeLocation(temp));
            }
        }
    }
    if(EdgeTable->EndPtr()!=EdgeTable->BeginPtr()){
        Edge* temp=(Edge*)(EdgeTable->HeadPtr());
        for(;(char*)temp<EdgeTable->EndPtr();++temp){
            if(temp->GetEid()==-1){
                temp->SetNextEdge(FreeEdgeLocation);
                FreeEdgeLocation=GetEdgeLocation(temp);
                FreeEdgeNum++;
            }
            else{
                EdgeNum++;
                int CurEid=temp->GetEid();
                MxEid=MxEid>(CurEid+1)?MxEid:(CurEid+1);
                EdgeHash.Add(CurEid, GetEdgeLocation(temp));
            }
        }
    }
}

int Net::NodeIter::GetInDeg()const{
    int InDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        InDeg+=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    InDeg+=temp->GetInDeg();
    return InDeg;
}
int Net::NodeIter::GetOutDeg()const{
    int OutDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        OutDeg+=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    OutDeg+=temp->GetOutDeg();
    return OutDeg;
}
bool Net::NodeIter::IsNbrEid(const int& eid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrEid(eid);
}
bool Net::NodeIter::IsInEid(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInEid(eid);
}
bool Net::NodeIter::IsOutEid(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutEid(eid);
}
bool Net::NodeIter::IsNbrEidSort(const int &eid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrEid(eid);
}
bool Net::NodeIter::IsInEidSort(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInEid(eid);
}
bool Net::NodeIter::IsOutEidSort(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutEid(eid);
}
void Net::NodeIter::SortEidV(){
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        temp->SortInEidV();
        temp->SortOutEidV();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    temp->SortInEidV();
    temp->SortOutEidV();
}
int Net::NodeIter::GetInEid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while(n>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
        n-=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetInDeg()) return -1;
    return temp->GetInEid(n-1);
}
int Net::NodeIter::GetOutEid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetOutDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetOutDeg()) return -1;
    return temp->GetOutEid(n-1);
}
int Net::NodeIter::GetNbrEid(const int &edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetDeg()) return -1;
    return temp->GetNbrEid(n-1);
}
int Net::NodeIter::GetInNid(const int &edgeN)const{
    return Graph->GetEI(GetInEid(edgeN)).GetSrcNid();
}
int Net::NodeIter::GetOutNid(const int &edgeN)const{
    return Graph->GetEI(edgeN).GetDstNid();
}
int Net::NodeIter::GetNbrNid(const int &edgeN)const{
    int eid=GetNbrEid(edgeN);
    if(edgeN<=GetInDeg()){return Graph->GetEI(eid).GetSrcNid();}
    else return Graph->GetEI(eid).GetDstNid();
}
int Net::AddNode(int Nid,const char* data){
    if(Nid==-1){Nid=MxNid;MxNid++;}
    else{
        if(IsNode(Nid)) return -1;
        else MxNid=MxNid>(Nid+1)?MxNid:(Nid+1);
    }
    uint64_t location;
    Node * NewNode;
    if(FreeNodeNum>0){
        location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        FreeNodeNum--;
        NewNode=new(Nodetable->HeadPtr()+location) Node(Nid,data);
    }
    else{
        NewNode=new(Nodetable)Node(Nid,data);
        location=GetNodeLocation(NewNode);
    }
    NodeHash.Add(Nid, location);
    NodeNum++;
    return  Nid;
}
Net::Node* Net::AddExistNode(const int &nid, const int &flag){
    Node* node;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(location)->GetNextNodeAddre();
        FreeNodeNum--;
        node=new(Nodetable->HeadPtr()+location) Node(nid,flag);
    }
    else{
        node=new(Nodetable) Node(nid,flag);
    }
    return node;
}
void Net::DelNodeOfLocation(const uint64_t &location){
    Node* DelNode=GetNodePtr(location);
    DelNode->SetNextNodeAddre(FreeNodeLocation);
    DelNode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Net::DelNode(const int &Nid){
    uint64_t location;
    if(!NodeHash.Find(Nid, location)) return;
    Node* temp=GetNodePtr(location);
    do{
        for(int i=0;i<temp->GetInDeg();++i){
            int Eid=temp->GetInEid(i);
            Edge* CurEdge=DelEidOfTable(Eid);
            if(CurEdge!=NULL){
                uint64_t location2;
               if( NodeHash.Find(CurEdge->GetSrcNid(), location2))
                   DelOutEidOfNode(location2, Eid);
            }
        }
            for(int i=0;i<temp->GetOutDeg();++i){
                int Eid=temp->GetOutEid(i);
                Edge* CurEdge=DelEidOfTable(Eid);
                if(CurEdge!=NULL){
                    uint64_t location2;
                   if( NodeHash.Find(CurEdge->GetDstNid(), location2))
                       DelInEidOfNode(location2, Eid);
                }
        }
        uint64_t old=location;
        location=temp->GetNextNodeAddre();
        temp=GetNodePtr(location);
        DelNodeOfLocation(old);
    } while (temp->GetNextNodeAddre()!=0);
    AttrN.DelAttrDat(Nid);
    NodeHash.Delete(Nid);
    NodeNum--;
}
void Net::DelInEidOfNode(const uint64_t &location, const int &Eid){
    Node* CurNode=GetNodePtr(location);
    Node* PrevNode=CurNode;
    while(!CurNode->DeleteInEid(Eid) && CurNode->GetNextNodeAddre()!=0){
        PrevNode=CurNode;
        CurNode=GetNodePtr(CurNode->GetNextNodeAddre());
    }
    if(CurNode->GetFlag()==0 && CurNode->GetInDeg()==0 && CurNode->GetOutDeg()==0){
        PrevNode->SetNextNodeAddre(CurNode->GetNextNodeAddre());
        DelNodeOfLocation(GetNodeLocation(CurNode));
    }
}
void Net::DelOutEidOfNode(const uint64_t &location, const int &Eid){
    Node* CurNode=GetNodePtr(location);
    Node* PrevNode=CurNode;
    while (!CurNode->DeleteOutEid(Eid) && CurNode->GetNextNodeAddre()!=0) {
        PrevNode=CurNode;
        CurNode=GetNodePtr(CurNode->GetNextNodeAddre());
    }
    if(CurNode->GetFlag()==0 && CurNode->GetInDeg()==0 && CurNode->GetOutDeg()==0){
        PrevNode->SetNextNodeAddre(CurNode->GetNextNodeAddre());
        DelNodeOfLocation(GetNodeLocation(CurNode));
    }
}
Net::Edge* Net::DelEidOfTable(const int &Eid){
    uint64_t location;
    if(EdgeHash.Find(Eid, location)){
        Edge* DelEdge=GetEdgePtr(location);
        DelEdge->SetNextEdge(FreeEdgeLocation);
        DelEdge->SetEid(-1);
        FreeEdgeLocation=location;
        FreeEdgeNum--;
        EdgeNum--;
        EdgeHash.Delete(Eid);
        return DelEdge;
    }
    return NULL;
}

void Net::DelEdge(const int &Eid){
    Edge* CurEdge=DelEidOfTable(Eid);
    if(CurEdge==NULL) return;
    int SrcNid=CurEdge->GetSrcNid();
    int DstNid=CurEdge->GetDstNid();
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || NodeHash.Find(DstNid, location2)) return;
    DelOutEidOfNode(location1, Eid);
    DelInEidOfNode(location2, Eid);
    AttrE.DelAttrDat(Eid);
}
void Net::DelEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return;
    Node* SrcNode=GetNodePtr(location1);
    int Eid=-1;
    do{
        int i;
        for(i=0;i<SrcNode->GetOutDeg();++i){
            int eid=SrcNode->GetOutEid(i);
            uint64_t location;
            if(EdgeHash.Find(eid, location)){
                Edge* CurEdge=GetEdgePtr(location);
                if(CurEdge->GetDstNid()==DstNid){
                    Eid=eid;
                    break;
                }
            }
        }
        if(Eid!=-1){
            SrcNode->DeleteOutEidN(i);
            break;
        }
        SrcNode=GetNodePtr(SrcNode->GetNextNodeAddre());
    }while (SrcNode->GetNextNodeAddre()!=0);
    
    if(Eid!=-1){
        DelInEidOfNode(location2, Eid);
        DelEidOfTable(Eid);
        AttrE.DelAttrDat(Eid);
    }
}

bool Net::IsEdge(const int &SrcNid, const int &DstNid, int &Eid)const{
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return false;
    Node* SrcNode=GetNodePtr(location1);
     do{
        for(int i=0;i<SrcNode->GetOutDeg();++i){
            int eid=SrcNode->GetOutEid(i);
            uint64_t location;
            if(EdgeHash.Find(eid, location)){
                Edge* CurEdge=GetEdgePtr(location);
                if(CurEdge->GetDstNid()==DstNid){
                    Eid=eid;
                    return true;
                }
            }
        }
        SrcNode=GetNodePtr(SrcNode->GetNextNodeAddre());
     }while (SrcNode->GetNextNodeAddre()!=0);
    Eid=-1;
    return false;
}
void Net::AddInEidOfNode(const uint64_t &location, const int &Eid){
    Node* temp=GetNodePtr(location);
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->AddInEid(Eid)) return;
        temp=GetNodePtr(temp->GetNextNodeAddre());
    }
    if(temp->AddInEid(Eid)) return;
    Node* NewNode=AddExistNode(temp->GetNid(), 0);
    NewNode->AddInEid(Eid);
    temp->SetNextNodeAddre(GetNodeLocation(NewNode));
}
void Net::AddOutEidOfNode(const uint64_t &location, const int &Eid){
    Node* temp=GetNodePtr(location);
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->AddOutEid(Eid)) return;
        temp=GetNodePtr(temp->GetNextNodeAddre());
    }
    if(temp->AddOutEid(Eid)) return;
    Node* NewNode=AddExistNode(temp->GetNid(), 0);
    NewNode->AddOutEid(Eid);
    temp->SetNextNodeAddre(GetNodeLocation(NewNode));
}
void Net::AddEdgeToTable(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
    Edge* NewEdge;
    if(FreeEdgeNum>0){
        char* edge=EdgeTable->HeadPtr()+FreeEdgeLocation;
        FreeEdgeLocation=GetEdgePtr(FreeEdgeLocation)->GetNextEdge();
        FreeEdgeNum--;
        NewEdge=new(edge) Edge(Eid,SrcNid,DstNid,Data);
    }
    else NewEdge=new(EdgeTable) Edge(Eid,SrcNid,DstNid,Data);
    EdgeHash.Add(Eid, GetEdgeLocation(NewEdge));
}
int Net::AddEdge(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
    if(Eid==-1){
        Eid=MxEid;
        MxEid++;
    }
    else{
        if(IsEdge(Eid)) return -1;
        MxEid=MxEid>(Eid+1)?MxEid:(Eid+1);
    }
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)||!NodeHash.Find(DstNid, location2)) return -1;
    AddOutEidOfNode(location1, Eid);
    AddInEidOfNode(location2, Eid);
    AddEdgeToTable(SrcNid, DstNid, Data, Eid);
    EdgeNum++;
    return Eid;
}
int Net::AddEdge2(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
    if(Eid==-1){
        Eid=MxEid;
        MxEid++;
    }
    else{
        if(IsEdge(Eid)) return -1;
        MxEid=MxEid>(Eid+1)?MxEid:(Eid+1);
    }
    AddEdgeToTable(SrcNid, DstNid,Data,Eid);
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){
        AddNode(SrcNid);
        NodeHash.Find(SrcNid, location1);
    }
    if(!NodeHash.Find(DstNid, location2)){
        AddNode(DstNid);
        NodeHash.Find(DstNid, location2);
    }
    AddOutEidOfNode(location1, Eid);
    AddInEidOfNode(location2, Eid);
    EdgeNum++;
    return Eid;
}

inline const char* Net::GetNData(const int &Nid)const{
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        return GetNodePtr(location)->GetData();
    }
    return NULL;
}
inline const char* Net::GetEData(const int &Eid)const{
    uint64_t location;
    if(EdgeHash.Find(Eid, location)){
        return GetEdgePtr(location)->GetData();
    }
    return NULL;
}

void Net::SetNData(const int &Nid,const char* Data){
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        GetNodePtr(location)->SetData(Data);
    }
}
void Net::SetEData(const int &Eid, const char *Data){
    uint64_t location;
    if(EdgeHash.Find(Eid, location)){
        GetEdgePtr(location)->SetData(Data);
    }
}

bool Net::AddAttrDatN(const int& Nid,const int& AttrId,const void* Val,const uint& length){
    if(!IsNode(Nid)) return false;
   return AttrN.AddAttrDat(Nid,AttrId,Val,length);
}
bool Net::AddAttrDatN(const int& Nid,const char* AttrName,const void* Val,const uint& length){
    if(!IsNode(Nid)) return false;
    return AttrN.AddAttrDat(Nid,AttrName,Val,length);
}
bool Net::GetAttrDatN(const int& Nid,const int& AttrId,void* Val)const{
    if(!IsNode(Nid)) return false;
    return AttrN.GetAttrDat(Nid,AttrId,Val);
}
bool Net::GetAttrDatN(const int& Nid,const char* AttrName,void* Val)const{
    if(!IsNode(Nid)) return false;
    return AttrN.GetAttrDat(Nid,AttrName,Val);
}
void Net::DelAttrDatN(const int& Nid,const int& AttrId){
    if(!IsNode(Nid)) return;
    AttrN.DelAttrDat(Nid,AttrId);
}
void Net::DelAttrDatN(const int& Nid,const char* AttrName){
    if(!IsNode(Nid)) return;
    AttrN.DelAttrDat(Nid,AttrName);
}
void Net::DelAttrDatN(const int& Nid){
    if(!IsNode(Nid)) return;
    AttrN.DelAttrDat(Nid);
}
int Net::AddAttrNameN(const char* AttrName){
    return AttrN.AddAttrName(AttrName);
}
bool Net::GetAttrIdN(int& AttrId,const char* AttrName)const{
    return AttrN.GetAttrId(AttrId,AttrName);
}
bool Net::GetAttrNameN(const int& AttrId,char* AttrName)const{
    return AttrN.GetAttrName(AttrId,AttrName);
}

bool Net::AddAttrDatE(const int& Eid,const int& AttrId,const void* Val,const uint& length){
    if(!IsEdge(Eid)) return false;
    return AttrE.AddAttrDat(Eid,AttrId,Val ,length);
}
bool Net::AddAttrDatE(const int& Eid,const char* AttrName,const void* Val,const uint& length){
    if(!IsEdge(Eid)) return false;
    return AttrE.AddAttrDat(Eid,AttrName,Val, length);
}
bool Net::GetAttrDatE(const int& Eid,const int& AttrId,void* Val)const{
    if(!IsEdge(Eid)) return false;
    return AttrE.GetAttrDat(Eid,AttrId,Val);
}
bool Net::GetAttrDatE(const int& Eid,const char* AttrName,void* Val)const{
    if(!IsEdge(Eid)) return false;
    return AttrE.GetAttrDat(Eid,AttrName,Val);
}
void Net::DelAttrDatE(const int& Eid,const int& AttrId){
    if(!IsEdge(Eid)) return;
    AttrE.DelAttrDat(Eid,AttrId);
}
void Net::DelAttrDatE(const int& Eid,const char* AttrName){
    if(!IsEdge(Eid)) return;
    AttrE.DelAttrDat(Eid,AttrName);
}
void Net::DelAttrDatE(const int& Eid){
    if(!IsEdge(Eid)) return;
    AttrE.DelAttrDat(Eid);
}
int Net::AddAttrNameE(const char* AttrName){
    return AttrE.AddAttrName(AttrName);
}
bool Net::GetAttrIdE(int& AttrId,const char* AttrName)const{
    return AttrE.GetAttrId(AttrId,AttrName);
}
bool Net::GetAttrNameE(const int& AttrId,char* AttrName)const{
    return AttrE.GetAttrName(AttrId,AttrName);
}


#endif /* network_h */
