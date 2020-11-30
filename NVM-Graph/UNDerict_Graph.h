//
//  Header.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/10/21.
//

#ifndef Header_h
#define Header_h

#include<string.h>


#include"hashtable.h"
#include"Arena.h"
#include"InOut.h"
const int NodeDatalengthDef=32;//默认标签最大长度是31个字节，最后一子节存'\0'
const int InOutEidNumDef=10;//默认边数

//位置都是相对于Arena的begin_ptr的偏移
class UNDerict_Graph{
public:
    class NvmNode{
    private:
        int Nid;//节点id
        char Data[NodeDatalengthDef];//标签
        int NidVNum;//当前节点边数
        int NidV[InOutEidNumDef];//每条边另一端节点id
        int Flag;//当前节点标识，-1代表无效（被删除），0代表中间节点，1代表头节点，因此插入节点默认为1，插入已存在节点默认为0
        uint64_t NextNode;//下一个节点location,默认值为0（无下一个节点）
    public:
        NvmNode(const int nid):Nid(nid),NidVNum(0),NextNode(0),Flag(1),Data("\0"){}
        NvmNode(const int nid,const char* nodedata):Nid(nid),NidVNum(0),NextNode(0),Flag(1){
            int length=strlen(nodedata);
            if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
            memcpy(Data,nodedata,length);
            Data[length]='\0';
        }
        NvmNode(const int nid,const int flag):Nid(nid),NidVNum(0),Data("\0"),Flag(flag),NextNode(0){}
        NvmNode(MOut& Out){
            Out.Load(Nid);
            Out.Load(Data,NodeDatalengthDef);
            Out.Load(NidVNum);
            Out.Load(NidV,InOutEidNumDef);
            Out.Load(Flag);
            Out.Load(NextNode);
        }
        void Load(MOut& Out){
            Out.Load(Nid);
            Out.Load(Data,NodeDatalengthDef);
            Out.Load(NidVNum);
            Out.Load(NidV,InOutEidNumDef);
            Out.Load(Flag);
            Out.Load(NextNode);
        }
        void Save(MIn& In)const{
            In.Save(Nid);
            In.Save(Data,NodeDatalengthDef);
            In.Save(NidVNum);
            In.Save(NidV,InOutEidNumDef);
            In.Save(Flag);
            In.Save(NextNode);
        }
        int GetId()const{return Nid;}
        int GetFlag()const{return Flag;}
        void SetFlag(const int& flag){Flag=flag;}
        const char* GetData()const{return Data;}
        void SetData(const char* data){
            size_t length=strlen(data);
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        uint64_t GetNextNodeAddre()const{return NextNode;}
        void SetNextNodeAddre(const uint64_t& location){NextNode=location;}
        
        int GetDeg() const{return NidVNum;}
        int GetInDeg()const{return NidVNum;}
        int GetOutDeg()const{return NidVNum;}
        int GetNbrNid(const int& NodeN)const{return NidV[NodeN];}
        int GetInNid(const int& NodeN)const{return GetNbrNid(NodeN);}
        int GetOutNid(const int& NodeN)const{return GetNbrNid(NodeN);}
        bool IsNbrNid(const int& nid) const{
            for(int i=0;i<NidVNum;++i){
                if(NidV[i]==nid) return true;
            }
            return false;
        }
        bool IsInNid(const int& nid)const{return IsNbrNid(nid);}
        bool IsOutNid(const int&nid)const{return IsNbrNid(nid);}
        void SortNidV(){
            int value;
            for(int i=1;i<NidVNum;++i){
                int j=0;
                while(j<i && NidV[j]<=NidV[i]) ++j;
                value=NidV[i];
                for(int k=i;k>j;k--) NidV[k]=NidV[k-1];
                NidV[j]=value;
            }
        }
        
        bool AddNbrNid(const int& nid){
            if(NidVNum<InOutEidNumDef){
                NidV[NidVNum]=nid;
                NidVNum++;
                return true;
            }
            return false;
        }
        bool AddInNid(const int& nid){return AddNbrNid(nid);}
        bool AddOutNid(const int&nid){return AddNbrNid(nid);}
        
        void DeleteNbrNidN(const int& NodeN){
            for(int i=NodeN;i<NidVNum;++i){NidV[i]=NidV[i+1];}
            NidVNum--;
        }
        void DeleteInNidN(const int& NodeN){DeleteNbrNidN(NodeN);}
        void DeleteOutNidN(const int& NodeN){DeleteNbrNidN(NodeN);}
        bool DeleteNbrNid(const int& nid){
            for(int i=0;i<NidVNum;++i){
                if(NidV[i]==nid){
                    for(int j=i;j<NidVNum;++j) NidV[j]=NidV[j+1];
                    NidVNum--;
                    return true;
                }
            }
            return false;
        }
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
    class NvmNodeI{
    private:
        NvmNode* Begin;
        NvmNode* CurNode;
        const char* End;
    public:
        NvmNodeI():Begin(NULL),CurNode(NULL),End(NULL){}
        NvmNodeI(NvmNode* begin,NvmNode* node,const char* end):Begin(begin),CurNode(node),End(end){}
        NvmNodeI(const NvmNodeI& nodeI):Begin(nodeI.Begin),CurNode(nodeI.CurNode),End(nodeI.End){}
        NvmNodeI& operator= (const NvmNodeI& nodeI){
            Begin=nodeI.Begin;
            CurNode=nodeI.CurNode;
            End=nodeI.End;
            return *this;
        }
        NvmNodeI& operator++(int){
            char* cur=(char*)CurNode;
            if(cur>=End) return *this;
            CurNode++;
            while((char*)CurNode<End && CurNode->GetFlag()<=0) CurNode++;
            return *this;
        }
        NvmNodeI& operator--(int){
            if(CurNode<=Begin) return *this;
            CurNode--;
            while(CurNode>Begin && CurNode->GetFlag()<=0) CurNode--;
            return *this;
        }
        bool operator<(const NvmNodeI& nodeI)const{return CurNode<nodeI.CurNode;}
        bool operator==(const NvmNodeI& nodeI)const{return CurNode==nodeI.CurNode;}
        NvmNode& operator*()const{return *CurNode;}
        NvmNode& operator()()const{return *CurNode;}
        NvmNode& operator->()const{return *CurNode;}
        
        int GetId()const{return CurNode->GetId();}
        const char* GetData()const{return CurNode->GetData();}
        int GetFlag()const{return CurNode->GetFlag();}
        int GetDeg()const{
            int Deg=0;
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                Deg+=temp->GetDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            Deg+=temp->GetDeg();
            return Deg;
        }
        int GetInDeg()const{
            int InDeg=0;
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                InDeg+=temp->GetInDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            InDeg+=temp->GetInDeg();
            return InDeg;
        }
        int GetOutDeg()const{
            int OutDeg=0;
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                OutDeg+=temp->GetOutDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            OutDeg+=temp->GetOutDeg();
            return OutDeg;
        }
        void SortNidV(){
            NvmNode* temp=CurNode;
            while(temp->GetNextNodeAddre()!=0){
                temp->SortNidV();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            temp->SortNidV();
        }
        //获取第n条边，n取值为1～Deg
        int GetNbrNid(const int& nodeN)const{
            int edgen=nodeN;
            NvmNode* temp=CurNode;
            while(edgen>temp->GetDeg() && temp->GetNextNodeAddre()!=0){
                edgen-=temp->GetDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            if(edgen>temp->GetDeg()) return -1;
            return temp->GetNbrNid(edgen-1);
        }
        int GetInNid(const int& nodeN)const{
            int edgen=nodeN;
            NvmNode* temp=CurNode;
            while(edgen>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
                edgen-=temp->GetInDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));;
            }
            if(edgen>temp->GetInDeg()) return -1;
            return temp->GetInNid(edgen-1);
        }
        int GetOutNid(const int& nodeN)const{
            int edgen=nodeN;
            NvmNode* temp=CurNode;
            while(edgen>temp->GetOutDeg() && temp->GetNextNodeAddre()!=0){
                edgen-=temp->GetOutDeg();
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            if(edgen>temp->GetOutDeg()) return -1;
            return temp->GetOutNid(edgen-1);
        }
        bool IsNbrNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while((!temp->IsNbrNid(nid)) && temp->GetNextNodeAddre()!=0){
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            return temp->IsNbrNid(nid);
        }
        bool IsInNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while((!temp->IsInNid(nid)) && temp->GetNextNodeAddre()!=0){
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            return temp->IsInNid(nid);
        }
        bool IsOutNid(const int& nid)const{
            NvmNode* temp=CurNode;
            while((!temp->IsOutNid(nid)) && temp->GetNextNodeAddre()!=0){
                temp=Begin+(size_t)(temp->GetNextNodeAddre()/sizeof(NvmNode));
            }
            return temp->IsOutNid(nid);
        }
        
        bool IsEmpty()const{return CurNode==NULL;}
        bool IsBegin()const{return CurNode==Begin;}
        bool IsEnd()const{return (char*)CurNode==End;}
    };
    class EdgeI{
    private:
        NvmNodeI CurNode;
        int CurEdge;
    public:
        EdgeI():CurNode(),CurEdge(0){}
        EdgeI(const NvmNodeI& node,const int& edge):CurNode(node),CurEdge(edge){}
        EdgeI(const EdgeI& edgei):CurNode(edgei.CurNode),CurEdge(edgei.CurEdge){}
        EdgeI& operator=(const EdgeI& edgei){
            if(this!=&edgei){
                CurNode=edgei.CurNode;
                CurEdge=edgei.CurEdge;
            }
            return *this;
        }
        bool operator==(const EdgeI& edgei) const{return CurNode==edgei.CurNode && CurEdge==edgei.CurEdge;}
        EdgeI& operator++(int){
            do{
                CurEdge++;
                if(CurEdge>CurNode.GetOutDeg()){
                    CurEdge=1;
                    CurNode++;
                    while(!CurNode.IsEnd() && CurNode.GetOutDeg()==0){CurNode++;}
                }
            }while(!CurNode.IsEnd() && GetSrcNid()<GetDstNid());
            return *this;
        }
        bool operator<(const EdgeI& edgei)const{
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
   
    NvmNode* HeadNode;
    int MxNid,EdgeNum;
    uint64_t FreeNodeLocation;
    int FreeNodeNum;
private:
    NvmNode* GetNodePtr(const uint64_t& location) const{
        size_t nodeN=location/NodeTable->NodeSize();
        return HeadNode+nodeN;
    }
    uint64_t GetNodeLocation(const NvmNode* node)const{
        return (char*)node-NodeTable->BeginPtr();
    }
    void DeleteNode(const uint64_t& location);
    NvmNode* AddExistNode(const int& nid=0,const int& flag=0);
    void AddEdgeToNode(const uint64_t &location, const int &edgenid);
    void DelEdgeOfNode(const uint64_t &location, const int &edgenid);
public:
    UNDerict_Graph(Arena* arena);
    UNDerict_Graph(const UNDerict_Graph& graph);
    ~UNDerict_Graph();
    UNDerict_Graph& operator=(const UNDerict_Graph& graph);
    
    static uint GetNodeSize(){return sizeof(NvmNode);}
    
    bool IsNodeNid(const int& nid)const{
        return NodeHash.IsIn(nid);
    }
    int AddNode(const int& nid=-1,const char* data="");
    size_t GetNodeNum()const{return NodeHash.GetValidSlotNum();}
    void DelNode(const int& nid);
    void DelNode(const NvmNode& node){DelNode(node.GetId());}
    int GetMxNid()const{return MxNid;}
    NvmNodeI HeadNI()const{return NvmNodeI(HeadNode,HeadNode,NodeTable->EndPtr());}
    NvmNodeI GetNI(const int& nid)const{
        NvmNode* curnode;
        uint64_t location;
        if(NodeHash.Find(nid, location)){
            curnode=GetNodePtr(location);
        }
        else curnode=NULL;
        return NvmNodeI(HeadNode, curnode, NodeTable->EndPtr());
    }
    NvmNodeI EndNI()const{
        NvmNode* curnode=GetNodePtr(NodeTable->BeginPtr()-NodeTable->EndPtr());
        return NvmNodeI(HeadNode, curnode, NodeTable->EndPtr());
    }
    
    bool IsEdge(const int& SrcNid,const int& DstNid)const{
        if(!IsNodeNid(SrcNid) || !IsNodeNid(DstNid)) return false;
        return GetNI(SrcNid).IsNbrNid(DstNid);
    }
    //插入边，返回-1代表节点不存在，返回0代表边已存在，返回1代表插入成功
    int AddEdge(const int& SrcNid,const int& DstNid);
    //插入边，返回0代表边已存在，返回1代表插入成功，若节点不存在，先新建节点
    int AddEdge2(const int& SrcNid,const int& DstNid);
    void DelEdge(const int& SrcNid,const int& DstNid);
    int GetEdgeNum()const{return EdgeNum;}
    EdgeI BegEI()const{
        NvmNodeI beginNI=HeadNI();
        beginNI++;
        EdgeI beginEI=EdgeI(beginNI,1);
        if(GetNodeNum()!=0 && !beginEI.IsEnd() && (beginNI.GetOutDeg()==0 || beginEI.GetSrcNid()>beginEI.GetDstNid())){beginEI++;}
        return  beginEI;
    }
    EdgeI GetEI(const NvmNodeI& nodeI,const int& edge)const{return EdgeI(nodeI,edge);}
};



UNDerict_Graph::UNDerict_Graph(Arena* arena): NodeTable(arena),NodeHash(32,4),MxNid(1),EdgeNum(0),FreeNodeLocation(0),FreeNodeNum(0){
    if(NodeTable->EndPtr()==NodeTable->BeginPtr()){
        HeadNode=AddExistNode();
    }
    else{
        HeadNode=new(NodeTable->HeadPtr()) NvmNode(0,0);
        NvmNode* temp=HeadNode+1;
        const char* end=NodeTable->EndPtr();
        const char* begin=NodeTable->BeginPtr();
        while((char*)temp<end){
            if(temp->GetFlag()==-1){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=(char*)temp-begin;
                FreeNodeNum++;
            }
            else {
                int nid=temp->GetId();
                if(temp->GetFlag()==1){
                    uint64_t location=(char*)temp-begin;
                    NodeHash.Add(nid,location);
                    MxNid=MxNid>(nid+1)?MxNid:(nid+1);
                }
                int deg=temp->GetDeg();
                for(int i=0;i<deg;++i){
                    if(temp->GetNbrNid(i)>=nid) EdgeNum++;
                }
            }
            temp++;
        }
    }
}
//有问题
UNDerict_Graph::UNDerict_Graph(const UNDerict_Graph& graph):NodeTable(graph.NodeTable),NodeHash(graph.NodeHash),MxNid(graph.MxNid),EdgeNum(graph.EdgeNum),FreeNodeLocation(graph.FreeNodeLocation),FreeNodeNum(graph.FreeNodeNum){}
UNDerict_Graph::~UNDerict_Graph(){}

int UNDerict_Graph::AddNode(const int& nid,const char* data){
    int newnid;
    if(nid==-1){newnid=MxNid;MxNid++;}
    else{
        if(IsNodeNid(nid)){return -1;}
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
        NvmNode* newnode=new(start) NvmNode(newnid,data);
        FreeNodeNum--;
        address=GetNodeLocation(newnode);
    }
    else{
        NvmNode * newnode=new(NodeTable) NvmNode(newnid,data);
        address=GetNodeLocation(newnode);
    }
    NodeHash.Add(newnid,address);
    return newnid;
}

UNDerict_Graph::NvmNode* UNDerict_Graph::AddExistNode(const int& nid,const int& flag){
    NvmNode* newnode;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        newnode=new(start) NvmNode(nid,flag);
        FreeNodeNum--;
    }
    else{
        newnode=new(NodeTable) NvmNode(nid,flag);
    }
    return newnode;
}

void UNDerict_Graph::DeleteNode(const uint64_t &location){
    NvmNode* node=GetNodePtr(location);
    node->SetNextNodeAddre(FreeNodeLocation);
    node->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void UNDerict_Graph::DelNode(const int &nid){
    uint64_t location;
    if(NodeHash.Find(nid,location)){
        NvmNode* node=GetNodePtr(location);
        while(node->GetNextNodeAddre()!=0){
            uint64_t temp=node->GetNextNodeAddre();
            DeleteNode(location);
            location=temp;
            node=GetNodePtr(location);
        }
        DeleteNode(location);
    }
}

void UNDerict_Graph::AddEdgeToNode(const uint64_t &location, const int &edgenid){
    uint64_t temp=location;
    NvmNode* curnode=GetNodePtr(temp);
    while(curnode->GetNextNodeAddre()!=0 && (curnode->GetDeg())>=InOutEidNumDef){
        temp=curnode->GetNextNodeAddre();
        curnode=GetNodePtr(temp);
    }
    if(curnode->GetDeg()<InOutEidNumDef){curnode->AddNbrNid(edgenid);}
    else{
        NvmNode* newnode=AddExistNode(curnode->GetId(),0);
        newnode->AddNbrNid(edgenid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

int UNDerict_Graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) {return -1;}
    if(IsEdge(SrcNid, DstNid)) {return 0;}
    AddEdgeToNode(location1, DstNid);
    AddEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}

int UNDerict_Graph::AddEdge2(const int &SrcNid, const int &DstNid){
    if(IsEdge(SrcNid, DstNid)) {return 0;}
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);}
    AddEdgeToNode(location1, DstNid);
    AddEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}

void UNDerict_Graph::DelEdgeOfNode(const uint64_t &location, const int &edgenid){
    uint64_t temp=location;
    NvmNode* curnode=GetNodePtr(temp);
    NvmNode* prevnode=curnode;
    if((!curnode->DeleteNbrNid(edgenid))&&curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        temp=curnode->GetNextNodeAddre();
        curnode=GetNodePtr(temp);
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(temp);
    }
}
void UNDerict_Graph::DelEdge(const int &SrcNid, const int &DstNid){
    if(!IsEdge(SrcNid, DstNid)) {return;}
    uint64_t location1,location2;
    NodeHash.Find(SrcNid, location1);
    NodeHash.Find(DstNid, location2);
    DelEdgeOfNode(location1, DstNid);
    DelEdgeOfNode(location2, SrcNid);
    EdgeNum--;
}




#endif /* Header_h */
