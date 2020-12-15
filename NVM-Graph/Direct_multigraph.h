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

class Comparison{
private:
    bool IsAsc;
public:
    Comparison(const bool& Asc=true):IsAsc(Asc){}
    bool operator()(const int& val1,const int& val2)const{
        if(IsAsc){return val1<val2;}
        else{return val2<val1;}
    }
};

void InsertSortCmp(int Arr[],int Start,int End,const Comparison& Cmp);
int PariitionCmp(int Arr[],int Start,int End,const Comparison& Cmp);
void QuickSortCmp(int Arr[],int Start,int End,const Comparison& Cmp);

int SearchBin(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp);
int SearchBinRight(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp);

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
        int Flag;
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
        int GetInEid(const int& EidN)const{return InEidV[EidN];}
        int GetOutEid(const int& EidN)const{return OutEidV[EidN];}
        bool IsInEid(const int& eid)const;
        bool IsOutEid(const int& eid)const;
        bool IsNbrNid(const int& eid)const{return IsInEid(eid)||IsOutEid(eid);}
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
public:
    Direct_multigraph(Arena* node,Arena* edge);
    
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
//数组排序、二分查找(正序、逆序）
void InsertSortCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    for(int i=Start+1;i<End;++i){
        int value=Arr[i];
        int j=i;
        while(j>Start && Cmp(value,Arr[j-1])){Arr[j]=Arr[j-1];j--;}
        Arr[j]=value;
    }
}
int PariitionCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    int index=End-1;
    int key=Arr[index];
    End--;
    int temp1;
    while (Start<End) {
        while(Start<End &&Cmp(Arr[Start],key)) Start++;
        End--;
        while(Start<End && Cmp(key,Arr[End])) End--;
        if(Start<End){
            temp1=Arr[Start];
            Arr[Start]=Arr[End];
            Arr[End]=temp1;
            Start++;
        }
    }
    Arr[index]=Arr[Start];
    Arr[Start]=key;
    return Start;
}
void QuickSortCmp(int Arr[],int Start,int End,const Comparison& Cmp){
    if(Start+1<End){
        if(End-Start<20){InsertSortCmp(Arr, Start,End, Cmp);}
        else{
            int splite=PariitionCmp(Arr, Start, End, Cmp);
            QuickSortCmp(Arr, Start, splite, Cmp);
            QuickSortCmp(Arr, splite+1, End, Cmp);
        }
    }
}

int SearchBin(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp){
    End--;
    while(Start<=End){
        int mid=(Start+End)>>1;
        if(Arr[mid]==key) return mid;
        if(Cmp(key,Arr[mid])) End=mid-1;
        else Start=mid+1;
    }
    return -1;
}
int SearchBinRight(const int Arr[],int Start,int End,const int& key,const Comparison& Cmp){
    while (Start<End) {
        int mid=(Start+End)>>1;
        if((Cmp(key,Arr[mid]))){End=mid;}
        else Start=mid+1;
    }
    return Start;
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


#endif /* Direct_multigraph_h */
