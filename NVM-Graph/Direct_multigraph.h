//
//  Direct_multigraph.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/14.
//

#ifndef Direct_multigraph_h
#define Direct_multigraph_h

#include"Arena.h"
#include"hashtable.h"
#include<string>

class Direct_multigraph{
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
        const Direct_multigraph* Graph;
    public:
        NodeIter():Begin(NULL),End(NULL),CurNode(NULL),Graph(NULL){}
        NodeIter(const char* begin,const char* end,Node* cur,const Direct_multigraph* graph):Begin(begin),End(end),CurNode(cur),Graph(graph){}
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
        const Direct_multigraph* Graph;
    public:
        EdgeIter():Begin(NULL),End(NULL),CurEdge(NULL),Graph(NULL){}
        EdgeIter(const char* begin,const char* end,Edge* edge,const Direct_multigraph* graph):Begin(begin),End(end),CurEdge(edge),Graph(graph){}
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
    Direct_multigraph(Arena* node,Arena* edge);
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
    bool IsNode(const int& Nid){uint64_t location; return NodeHash.Find(Nid,location);}
    
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
};




inline Direct_multigraph::Node::Node(const int nid,const char* data):Nid(nid),InEidVNum(0),OutEidVNum(0),Flag(1),NextNode(0){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline Direct_multigraph::Node::Node(MOut& Out){
    Out.Load(Nid);
    Out.Load(Data, NodeDatalengthDef);
    Out.Load(InEidVNum);
    Out.Load(OutEidVNum);
    Out.Load(InEidV, InOutEidNumDef);
    Out.Load(OutEidV, InOutEidNumDef);
    Out.Load(Flag);
    Out.Load(NextNode);
}
void Direct_multigraph::Node::Save(MIn &In){
    In.Save(Nid);
    In.Save(Data, NodeDatalengthDef);
    In.Save(InEidVNum);
    In.Save(OutEidVNum);
    In.Save(InEidV, InOutEidNumDef);
    In.Save(OutEidV, InOutEidNumDef);
    In.Save(Flag);
    In.Save(NextNode);
}
inline void Direct_multigraph::Node::SetData(const char *data){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline bool Direct_multigraph::Node::IsInEid(const int &eid)const{
    for(int i=0;i<InEidVNum;++i){if(InEidV[i]==eid) return true;}
    return false;
}
inline bool Direct_multigraph::Node::IsOutEid(const int& eid)const{
    for(int i=0;i<OutEidVNum;++i){if(OutEidV[i]==eid) return true;}
    return false;
}
inline bool Direct_multigraph::Node::AddInEid(const int& Eid){
    if(InEidVNum<InOutEidNumDef){
        InEidV[InEidVNum]=Eid;
        InEidVNum++;
        return true;
    }
    return false;
}
inline bool Direct_multigraph::Node::AddOutEid(const int &Eid){
    if(OutEidVNum<InOutEidNumDef){
        OutEidV[OutEidVNum]=Eid;
        OutEidVNum++;
        return true;
    }
    return false;
}
inline bool Direct_multigraph::Node::AddInEid(const int *Eid, const int &EidN){
    if(InEidVNum+EidN<=InOutEidNumDef){
        memcpy(InEidV, Eid, sizeof(int)*EidN);
        InEidVNum+=EidN;
        return true;
    }
    return false;
}
inline bool Direct_multigraph::Node::AddOutEid(const int *Eid, const int &EidN){
    if(OutEidVNum+EidN<=InOutEidNumDef){
        memcpy(OutEidV, Eid, sizeof(int)*EidN);
        OutEidVNum+=EidN;
        return true;
    }
    return false;
}
inline void Direct_multigraph::Node::DeleteInEidN(const int &EidN){
    for(int i=EidN;i<(InEidVNum-1);++i){InEidV[i]=InEidV[i+1];}
    InEidVNum--;
}
inline void Direct_multigraph::Node::DeleteOutEidN(const int &EidN){
    for(int i=EidN;i<(OutEidVNum-1);++i){OutEidV[i]=OutEidV[i+1];}
    OutEidVNum--;
}
inline bool Direct_multigraph::Node::DeleteInEid(const int& Eid){
    for(int i=0;i<InEidVNum;++i){
        if(InEidV[i]==Eid){
            for(int j=i;j<(InEidVNum-1);++j){InEidV[j]=InEidV[j+1];}
            InEidVNum--;
            return true;
        }
    }
    return false;
}
inline bool Direct_multigraph::Node::DeleteOutEid(const int& Eid){
    for(int i=0;i<OutEidVNum;++i){
        if(OutEidV[i]==Eid){
            for(int j=i;j<(OutEidVNum-1);++j){OutEidV[j]=OutEidV[j+1];}
            OutEidVNum--;
            return true;
        }
    }
    return false;
}
bool Direct_multigraph::Node::AddInEidSort(const int &Eid,const bool& asc){
    if(InEidVNum<InOutEidNumDef){
        int index=SearchBinRight(InEidV, 0, InEidVNum, Eid, Comparison(asc));
        for(int i=InEidVNum;i>index;--i) InEidV[i]=InEidV[i-1];
        InEidV[index]=Eid;
        InEidVNum++;
        return true;
    }
    return false;
}
bool Direct_multigraph::Node::AddOutEidSort(const int &Eid,const bool& asc){
    if(OutEidVNum<InOutEidNumDef){
        int index=SearchBinRight(OutEidV, 0, OutEidVNum, Eid, Comparison(asc));
        for(int i=OutEidVNum;i>index;--i) OutEidV[i]=OutEidV[i-1];
        OutEidV[index]=Eid;
        OutEidVNum++;
        return true;
    }
    return false;
}
bool Direct_multigraph::Node::DeleteInEidSort(const int &Eid,const bool& asc){
    int index=SearchBin(InEidV, 0, InEidVNum, Eid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<(InEidVNum-1);++i) InEidV[i]=InEidV[i+1];
        InEidVNum--;
        return true;
    }
    return false;
}
bool Direct_multigraph::Node::DeleteOutEidSort(const int &Eid,const bool& asc){
    int index=SearchBin(OutEidV, 0, OutEidVNum, Eid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<(OutEidVNum-1);++i) OutEidV[i]=OutEidV[i+1];
        OutEidVNum--;
        return true;
    }
    return false;
}

Direct_multigraph::Direct_multigraph(Arena* node,Arena* edge):Nodetable(node),EdgeTable(edge),NodeHash(32,4),EdgeHash(32,4),MxNid(1),MxEid(1),NodeNum(0),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0),FreeEdgeNum(0),FreeEdgeLocation(0){
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

int Direct_multigraph::NodeIter::GetInDeg()const{
    int InDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        InDeg+=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    InDeg+=temp->GetInDeg();
    return InDeg;
}
int Direct_multigraph::NodeIter::GetOutDeg()const{
    int OutDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        OutDeg+=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    OutDeg+=temp->GetOutDeg();
    return OutDeg;
}
bool Direct_multigraph::NodeIter::IsNbrEid(const int& eid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrEid(eid);
}
bool Direct_multigraph::NodeIter::IsInEid(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInEid(eid);
}
bool Direct_multigraph::NodeIter::IsOutEid(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutEid(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutEid(eid);
}
bool Direct_multigraph::NodeIter::IsNbrEidSort(const int &eid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrEid(eid);
}
bool Direct_multigraph::NodeIter::IsInEidSort(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInEid(eid);
}
bool Direct_multigraph::NodeIter::IsOutEidSort(const int &eid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutEidSort(eid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutEid(eid);
}
void Direct_multigraph::NodeIter::SortEidV(){
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        temp->SortInEidV();
        temp->SortOutEidV();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    temp->SortInEidV();
    temp->SortOutEidV();
}
int Direct_multigraph::NodeIter::GetInEid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while(n>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
        n-=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetInDeg()) return -1;
    return temp->GetInEid(n-1);
}
int Direct_multigraph::NodeIter::GetOutEid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetOutDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetOutDeg()) return -1;
    return temp->GetOutEid(n-1);
}
int Direct_multigraph::NodeIter::GetNbrEid(const int &edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetDeg()) return -1;
    return temp->GetNbrEid(n-1);
}
int Direct_multigraph::NodeIter::GetInNid(const int &edgeN)const{
    return Graph->GetEI(GetInEid(edgeN)).GetSrcNid();
}
int Direct_multigraph::NodeIter::GetOutNid(const int &edgeN)const{
    return Graph->GetEI(edgeN).GetDstNid();
}
int Direct_multigraph::NodeIter::GetNbrNid(const int &edgeN)const{
    int eid=GetNbrEid(edgeN);
    if(edgeN<=GetInDeg()){return Graph->GetEI(eid).GetSrcNid();}
    else return Graph->GetEI(eid).GetDstNid();
}
int Direct_multigraph::AddNode(int Nid,const char* data){
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
Direct_multigraph::Node* Direct_multigraph::AddExistNode(const int &nid, const int &flag){
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
void Direct_multigraph::DelNodeOfLocation(const uint64_t &location){
    Node* DelNode=GetNodePtr(location);
    DelNode->SetNextNodeAddre(FreeNodeLocation);
    DelNode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Direct_multigraph::DelNode(const int &Nid){
    uint64_t location;
    if(!NodeHash.Find(Nid, location)) return;
    Node* temp=GetNodePtr(location);
    do{
        for(int i=0;i<temp->GetInDeg();++i){
            int Eid=temp->GetInEid(i);
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
    NodeHash.Delete(Nid);
    NodeNum--;
}
void Direct_multigraph::DelInEidOfNode(const uint64_t &location, const int &Eid){
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
void Direct_multigraph::DelOutEidOfNode(const uint64_t &location, const int &Eid){
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
Direct_multigraph::Edge* Direct_multigraph::DelEidOfTable(const int &Eid){
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

void Direct_multigraph::DelEdge(const int &Eid){
    Edge* CurEdge=DelEidOfTable(Eid);
    if(CurEdge==NULL) return;
    int SrcNid=CurEdge->GetSrcNid();
    int DstNid=CurEdge->GetDstNid();
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || NodeHash.Find(DstNid, location2)) return;
    DelOutEidOfNode(location1, Eid);
    DelInEidOfNode(location2, Eid);
}
void Direct_multigraph::DelEdge(const int &SrcNid, const int &DstNid){
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
    }
}

bool Direct_multigraph::IsEdge(const int &SrcNid, const int &DstNid, int &Eid)const{
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
void Direct_multigraph::AddInEidOfNode(const uint64_t &location, const int &Eid){
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
void Direct_multigraph::AddOutEidOfNode(const uint64_t &location, const int &Eid){
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
void Direct_multigraph::AddEdgeToTable(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
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
int Direct_multigraph::AddEdge(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
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
int Direct_multigraph::AddEdge2(const int &SrcNid, const int &DstNid,const char* Data,int Eid){
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

inline const char* Direct_multigraph::GetNData(const int &Nid)const{
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        return GetNodePtr(location)->GetData();
    }
    return NULL;
}
inline const char* Direct_multigraph::GetEData(const int &Eid)const{
    uint64_t location;
    if(EdgeHash.Find(Eid, location)){
        return GetEdgePtr(location)->GetData();
    }
    return NULL;
}

void Direct_multigraph::SetNData(const int &Nid,const char* Data){
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        GetNodePtr(location)->SetData(Data);
    }
}
void Direct_multigraph::SetEData(const int &Eid, const char *Data){
    uint64_t location;
    if(EdgeHash.Find(Eid, location)){
        GetEdgePtr(location)->SetData(Data);
    }
}




#endif /* Direct_multigraph_h */
