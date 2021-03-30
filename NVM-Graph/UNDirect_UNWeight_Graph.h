//
//  UNDirect_UNWeight_Graph.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/30.
//

#ifndef UNDirect_UNWeight_Graph_h
#define UNDirect_UNWeight_Graph_h

#include<string.h>

#include"hashtable.h"
#include"Arena.h"
#include"InOut.h"
#include"Attr.h"


//位置都是相对于Arena的begin_ptr的偏移
class UNDirect_UNWeight_Graph{
public:
    class Node{
    private:
        int Nid;//节点id
        char Data[NodeDatalengthDef];//标签
        int NidVNum;//当前节点边数
        int NidV[InOutEidNumDef];//每条边另一端节点id
        int Flag;//当前节点标识，-1代表无效（被删除），0代表中间节点，1代表头节点，因此插入节点默认为1，插入已存在节点默认为0
        uint64_t NextNode;//下一个节点location,默认值为0（无下一个节点）
    public:
        Node(const int nid):Nid(nid),NidVNum(0),NextNode(0),Flag(1),Data("\0"){}
        Node(const int nid,const char* nodedata);
        Node(const int nid,const int flag):Nid(nid),NidVNum(0),Data("\0"),Flag(flag),NextNode(0){}
        
        int GetId()const{return Nid;}
        int GetFlag()const{return Flag;}
        void SetFlag(const int& flag){Flag=flag;}
        const char* GetData()const{return Data;}
        void SetData(const char* data);
        uint64_t GetNextNodeAddre()const{return NextNode;}
        void SetNextNodeAddre(const uint64_t& location){NextNode=location;}
        
        int GetDeg() const{return NidVNum;}
        int GetInDeg()const{return NidVNum;}
        int GetOutDeg()const{return NidVNum;}
        int GetNbrNid(const int& NodeN)const{return NidV[NodeN];}
        int GetInNid(const int& NodeN)const{return GetNbrNid(NodeN);}
        int GetOutNid(const int& NodeN)const{return GetNbrNid(NodeN);}
        bool IsNbrNid(const int& nid) const;
        bool IsInNid(const int& nid)const{return IsNbrNid(nid);}
        bool IsOutNid(const int&nid)const{return IsNbrNid(nid);}
        bool AddNbrNid(const int& nid);
        bool AddInNid(const int& nid){return AddNbrNid(nid);}
        bool AddOutNid(const int&nid){return AddNbrNid(nid);}
        void DeleteNbrNidN(const int& NodeN);
        void DeleteInNidN(const int& NodeN){DeleteNbrNidN(NodeN);}
        void DeleteOutNidN(const int& NodeN){DeleteNbrNidN(NodeN);}
        bool DeleteNbrNid(const int& nid);
        bool DeleteInNid(const int& nid){return DeleteNbrNid(nid);}
        bool DeleteOutNid(const int& nid){return DeleteNbrNid(nid);}
        
        
        static void* operator new(size_t size_t,Arena* table){
            char * ptr=table->AllocateNode();
            return (void*)ptr;
        }
        static void operator delete(void* p, Arena* table){
            table->RecoverNode();
        }
        static void* operator new(size_t size,char* start){
            return (void*)start;
        }
        static void operator delete(void* p,char* start){}
    };
    class NodeIter{
    private:
        const char* Begin;
        const char* End;
        Node* CurNode;
    public:
        NodeIter():Begin(NULL),CurNode(NULL),End(NULL){}
        NodeIter(const char* begin,const char* end,Node* node):Begin(begin),CurNode(node),End(end){}
        NodeIter(const NodeIter& nodeI):Begin(nodeI.Begin),CurNode(nodeI.CurNode),End(nodeI.End){}
        NodeIter& operator= (const NodeIter& nodeI){
            Begin=nodeI.Begin;
            CurNode=nodeI.CurNode;
            End=nodeI.End;
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
            while((char*)CurNode>Begin && CurNode->GetFlag()<=0) CurNode--;
            return *this;
        }
        bool operator<(const NodeIter& nodeI)const{return CurNode<nodeI.CurNode;}
        bool operator==(const NodeIter& nodeI)const{return CurNode==nodeI.CurNode;}
        Node& operator*()const{return *CurNode;}
        Node& operator()()const{return *CurNode;}
        Node& operator->()const{return *CurNode;}
        
        int GetId()const{return CurNode->GetId();}
        const char* GetData()const{return CurNode->GetData();}
        int GetFlag()const{return CurNode->GetFlag();}
        int GetDeg()const;
        int GetInDeg()const;
        int GetOutDeg()const;
        //获取第n条边，n取值为0～Deg-1
        int GetNbrNid(const int& nodeN)const;
        int GetInNid(const int& nodeN)const;
        int GetOutNid(const int& nodeN)const;
        bool IsNbrNid(const int& nid)const;
        bool IsInNid(const int& nid)const;
        bool IsOutNid(const int& nid)const;
        bool IsEmpty()const{return CurNode==NULL;}
        bool IsBegin()const{return (char*)CurNode==Begin;}
        bool IsEnd()const{return (char*)CurNode==End;}
    };
    class EdgeIter{
    private:
        NodeIter CurNode;
        int CurEdge;
    public:
        EdgeIter():CurNode(),CurEdge(0){}
        EdgeIter(const NodeIter& node,const int& edge):CurNode(node),CurEdge(edge){}
        EdgeIter(const EdgeIter& edgeIter):CurNode(edgeIter.CurNode),CurEdge(edgeIter.CurEdge){}
        EdgeIter& operator=(const EdgeIter& edgei){
            if(this!=&edgei){
                CurNode=edgei.CurNode;
                CurEdge=edgei.CurEdge;
            }
            return *this;
        }
        bool operator==(const EdgeIter& edgei) const{return CurNode==edgei.CurNode && CurEdge==edgei.CurEdge;}
        EdgeIter& operator++(int){
            do{
                CurEdge++;
                if(CurEdge>CurNode.GetOutDeg()){
                    CurEdge=1;
                    CurNode++;
                    while(!CurNode.IsEnd() && CurNode.GetOutDeg()==0){CurNode++;}
                }
            }while(!CurNode.IsEnd() && GetSrcNid()>GetDstNid());
            return *this;
        }
        bool operator<(const EdgeIter& edgei)const{
            return CurNode<edgei.CurNode||(CurNode==edgei.CurNode && CurEdge<edgei.CurEdge);
        }
        int GetSrcNid()const{return CurNode.GetId();}
        int GetDstNid()const{return CurNode.GetOutNid(CurEdge);}
        bool IsEmpty()const{return CurNode.IsEmpty();}
        bool IsEnd()const{return CurNode.IsEnd();}
    };
private:
    Arena* NodeTable;
    CuckooHash::HashTable NodeHash;
    
    int MxNid;
    int NodeNum,EdgeNum;
    uint64_t FreeNodeLocation;
    int FreeNodeNum;
    
    
    
    
private:
    Node* GetNodePtr(const uint64_t& location) const{
        return (Node*)(NodeTable->BeginPtr()+location);
    }
    uint64_t GetNodeLocation(const Node* node)const{
        return (char*)node-NodeTable->BeginPtr();
    }
    void DeleteNode(const uint64_t& location);
    Node* AddExistNode(const int& nid=0,const int& flag=0);
    void AddEdgeToNode(const uint64_t &location, const int &edgenid);
    void DelEdgeOfNode(const uint64_t &location, const int &edgenid);
public:
    static uint GetNodeSize(){return sizeof(Node);}
public:
    UNDirect_UNWeight_Graph(Arena* arena);
    bool IsNode(const int& nid)const{return NodeHash.IsIn(nid);}
    size_t GetNodeNum()const{return NodeNum;}
    int GetEdgeNum()const{return EdgeNum;}
    int GetMxNid()const{return MxNid;}
    
    int AddNode(const int& nid=-1,const char* data="");
    void DelNode(const int& nid);
    void DelNode(const Node& node){DelNode(node.GetId());}
    
    NodeIter BegNI()const{
        Node* curnode=(Node*)NodeTable->BeginPtr();
        NodeIter iter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
        if(iter.GetFlag()<=0) iter++;
        return iter;
    }
    NodeIter GetNI(const int& nid)const{
        Node* curnode;
        uint64_t location;
        if(NodeHash.Find(nid, location)){
            curnode=GetNodePtr(location);
        }
        else curnode=NULL;
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    NodeIter EndNI()const{
        Node* curnode=(Node*)NodeTable->EndPtr();
        return NodeIter(NodeTable->BeginPtr(),NodeTable->EndPtr(),curnode);
    }
    
    bool IsEdge(const int& SrcNid,const int& DstNid)const{
        if(!IsNode(SrcNid) || !IsNode(DstNid)) return false;
        return GetNI(SrcNid).IsNbrNid(DstNid);
    }
    //插入边，返回-1代表节点不存在，返回0代表边已存在，返回1代表插入成功
    int AddEdge(const int& SrcNid,const int& DstNid);
    //插入边，返回0代表边已存在，返回1代表插入成功，若节点不存在，先新建节点
    int AddEdge2(const int& SrcNid,const int& DstNid);
    void DelEdge(const int& SrcNid,const int& DstNid);
    
    EdgeIter BegEI()const{
        NodeIter beginNI=BegNI();
        EdgeIter beginEI=EdgeIter(beginNI,1);
        if(GetNodeNum()!=0 && !beginEI.IsEnd() && (beginNI.GetOutDeg()==0 || beginEI.GetSrcNid()>beginEI.GetDstNid())){beginEI++;}
        return  beginEI;
    }
    EdgeIter GetEI(const NodeIter& nodeI,const int& edge)const{return EdgeIter(nodeI,edge);}
    EdgeIter EndEI()const{return EdgeIter(EndNI(),1);}
    
    size_t GetHashSize()const{return NodeHash.GetSize();}
    CuckooHash::IterPair GetNidBegin()const{return NodeHash.Begin();}
    CuckooHash::IterPair GetNidEnd()const{return NodeHash.End();}
    
};
inline UNDirect_UNWeight_Graph::Node::Node(const int nid,const char* nodedata):Nid(nid),NidVNum(0),NextNode(0),Flag(1){
    size_t length=strlen(nodedata);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data,nodedata,length);
    Data[length]='\0';
}
inline void UNDirect_UNWeight_Graph::Node::SetData(const char *data){
    size_t length=strlen(data);
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline bool UNDirect_UNWeight_Graph::Node::IsNbrNid(const int &nid)const{
    for(int i=0;i<NidVNum;++i){
        if(NidV[i]==nid) return true;
    }
    return false;
}
inline bool UNDirect_UNWeight_Graph::Node::AddNbrNid(const int& nid){
    if(NidVNum<InOutEidNumDef){
        NidV[NidVNum]=nid;
        NidVNum++;
        return true;
    }
    return false;
}
inline void UNDirect_UNWeight_Graph::Node::DeleteNbrNidN(const int& NodeN){
    for(int i=NodeN;i<NidVNum;++i){NidV[i]=NidV[i+1];}
    NidVNum--;
}
inline bool UNDirect_UNWeight_Graph::Node::DeleteNbrNid(const int& nid){
    for(int i=0;i<NidVNum;++i){
        if(NidV[i]==nid){
            for(int j=i;j<NidVNum;++j) NidV[j]=NidV[j+1];
            NidVNum--;
            return true;
        }
    }
    return false;
}

inline int UNDirect_UNWeight_Graph::NodeIter::GetDeg()const{
    int Deg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        Deg+=temp->GetDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    Deg+=temp->GetDeg();
    return Deg;
}
inline int UNDirect_UNWeight_Graph::NodeIter::GetInDeg()const{
    int InDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        InDeg+=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    InDeg+=temp->GetInDeg();
    return InDeg;
}
inline int UNDirect_UNWeight_Graph::NodeIter::GetOutDeg()const{
    int OutDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        OutDeg+=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    OutDeg+=temp->GetOutDeg();
    return OutDeg;
}
inline int UNDirect_UNWeight_Graph::NodeIter::GetNbrNid(const int &nodeN)const{
    int n=nodeN+1;
    Node* temp=CurNode;
    while (n>temp->GetDeg()&&temp->GetNextNodeAddre()!=0) {
        n-=temp->GetDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetDeg()) return -1;
    return temp->GetNbrNid(n-1);
}
inline int UNDirect_UNWeight_Graph::NodeIter::GetInNid(const int &nodeN)const{
    int n=nodeN+1;
    Node* temp=CurNode;
    while (n>temp->GetInDeg()&&temp->GetNextNodeAddre()!=0) {
        n-=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetInDeg()) return -1;
    return temp->GetInNid(n-1);
}
inline int UNDirect_UNWeight_Graph::NodeIter::GetOutNid(const int &nodeN)const{
    int n=nodeN+1;
    Node* temp=CurNode;
    while (n>temp->GetOutDeg()&&temp->GetNextNodeAddre()!=0) {
        n-=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetOutDeg()) return -1;
    return temp->GetOutNid(n-1);
}
inline bool UNDirect_UNWeight_Graph::NodeIter::IsNbrNid(const int &nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrNid(nid);
}
inline bool UNDirect_UNWeight_Graph::NodeIter::IsInNid(const int &nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsInNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNid(nid);
}
inline bool UNDirect_UNWeight_Graph::NodeIter::IsOutNid(const int &nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsOutNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutNid(nid);
}
UNDirect_UNWeight_Graph::UNDirect_UNWeight_Graph(Arena* arena):NodeTable(arena),NodeHash(32, 4),MxNid(1),NodeNum(0),EdgeNum(0),FreeNodeLocation(0),FreeNodeNum(0){
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
                    uint64_t location=GetNodeLocation(temp);
                    NodeHash.Add(temp->GetId(), location);
                    NodeNum++;
                    MxNid=MxNid>(temp->GetId()+1)?MxNid:(temp->GetId()+1);
                }
                EdgeNum+=temp->GetDeg();
            }
            temp++;
        }
        EdgeNum>>=1;
    }
   
}

int UNDirect_UNWeight_Graph::AddNode(const int& nid,const char* data){
    int newnid;
    if(nid==-1){newnid=MxNid;MxNid++;}
    else{
        if(IsNode(nid)){return -1;}
        else{
            newnid=nid;
            MxNid=MxNid>(nid+1)?MxNid:(nid+1);
        }
    }
    uint64_t address;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        Node* newnode=new(start) Node(newnid,data);
        FreeNodeNum--;
        address=GetNodeLocation(newnode);
    }
    else{
        Node * newnode=new(NodeTable) Node(newnid,data);
        address=GetNodeLocation(newnode);
    }
    NodeHash.Add(newnid,address);
    NodeNum++;
    return newnid;
}
void UNDirect_UNWeight_Graph::DeleteNode(const uint64_t &location){
    Node* node=GetNodePtr(location);
    node->SetNextNodeAddre(FreeNodeLocation);
    node->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void UNDirect_UNWeight_Graph::DelEdgeOfNode(const uint64_t &location, const int &edgenid){
    uint64_t temp=location;
    Node* curnode=GetNodePtr(temp);
    Node* prevnode=curnode;
    while((!curnode->DeleteNbrNid(edgenid))&&curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        temp=curnode->GetNextNodeAddre();
        curnode=GetNodePtr(temp);
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(temp);
    }
}
void UNDirect_UNWeight_Graph::DelNode(const int &nid){
    uint64_t location;
    if(! NodeHash.Find(nid,location)) return;
    Node* node=GetNodePtr(location);
    do{
        for(int i=0;i<node->GetDeg();++i){
            int SrcNid=node->GetNbrNid(i);
            uint64_t SrcLocation;
            if(NodeHash.Find(SrcNid, SrcLocation)){
                DelEdgeOfNode(SrcLocation, nid);
                EdgeNum--;
            }
        }
        uint64_t temp=node->GetNextNodeAddre();
        DeleteNode(location);
        location=temp;
        node=GetNodePtr(location);
    }while(location!=0);
    NodeHash.Delete(nid);
    NodeNum--;
}
UNDirect_UNWeight_Graph::Node* UNDirect_UNWeight_Graph::AddExistNode(const int& nid,const int& flag){
    Node* newnode;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        newnode=new(start) Node(nid,flag);
        FreeNodeNum--;
    }
    else{
        newnode=new(NodeTable) Node(nid,flag);
    }
    return newnode;
}
void UNDirect_UNWeight_Graph::AddEdgeToNode(const uint64_t &location, const int &edgenid){
    uint64_t temp=location;
    Node* curnode=GetNodePtr(temp);
    while(curnode->GetNextNodeAddre()!=0 && (curnode->GetDeg())>=InOutEidNumDef){
        temp=curnode->GetNextNodeAddre();
        curnode=GetNodePtr(temp);
    }
    if(curnode->GetDeg()<InOutEidNumDef){curnode->AddNbrNid(edgenid);}
    else{
        Node* newnode=AddExistNode(curnode->GetId(),0);
        newnode->AddNbrNid(edgenid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}
int UNDirect_UNWeight_Graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) {return -1;}
    if(IsEdge(SrcNid, DstNid)) {return 0;}
    AddEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid){
        AddEdgeToNode(location2, SrcNid);
    }
    EdgeNum++;
    return 1;
}
int UNDirect_UNWeight_Graph::AddEdge2(const int &SrcNid, const int &DstNid){
    if(IsEdge(SrcNid, DstNid)) {return 0;}
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    AddEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid){
        AddEdgeToNode(location2, SrcNid);
    }
    EdgeNum++;
    return 1;
}
void UNDirect_UNWeight_Graph::DelEdge(const int &SrcNid, const int &DstNid){
    if(!IsEdge(SrcNid, DstNid)) {return;}
    uint64_t location1,location2;
    NodeHash.Find(SrcNid, location1);
    NodeHash.Find(DstNid, location2);
    DelEdgeOfNode(location1, DstNid);
    if(SrcNid!=DstNid){
        DelEdgeOfNode(location2, SrcNid);
    }
    EdgeNum--;
}


#endif /* UNDirect_UNWeight_Graph_h */
