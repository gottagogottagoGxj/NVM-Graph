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
    bool GetAttrDat(const int& Nid,const char* AttrName,char* Val)const;
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
    bool DelAttrDatE(const int& SrcNid,const int& DstNid,const int& AttrId);
    bool DelAttrDatE(const int& SrcNid,const int& DstNid,const char* AttrName);
    bool DelAttrDatE(const int& SrcNid,const int& DstNid);
    int  AddAttrNameE(const char* AttrName){return AttrE.AddAttrName(AttrName);}
    bool GetAttrIdE(int& AttrId,const char* AttrName)const{return AttrE.GetAttrId(AttrId, AttrName);}
    bool GetAttrNameE(const int& AttrId,char* AttrName)const{return AttrE.GetAttrName(AttrId, AttrName);}
    
    
};

#endif /* net_h */
