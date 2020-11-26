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
const int NodeDatalengthDef=32;
const int InOutEidNumDef=10;
class UNDerict_Graph{
public:
    class NvmNode{
    private:
        int Nid;
        char Data[NodeDatalengthDef];
        int NidVNum;
        int NidV[InOutEidNumDef];
        bool Flag;
        uint64_t PrevNode;
        uint64_t NextNode;
    public:
        NvmNode(const int nid):Nid(nid),NidVNum(0),PrevNode(0),NextNode(0),Flag(true){}
        NvmNode(const int nid,const char* nodedata)
    :Nid(nid),NidVNum(0),PrevNode(0),NextNode(0),Flag(true){
        size_t length=strlen(nodedata);
        memcpy(Data,nodedata,length);
        Data[length]='\0';
        }
        NvmNode(MOut& Out){
            Out.Load(Nid);
            Out.Load(Data,NodeDatalengthDef);
            Out.Load(NidVNum);
            Out.Load(NidV,InOutEidNumDef);
            Out.Load(Flag);
            Out.Load(PrevNode);
            Out.Load(NextNode);
        }
        void Load(MOut& Out){
            Out.Load(Nid);
            Out.Load(Data,NodeDatalengthDef);
            Out.Load(NidVNum);
            Out.Load(NidV,InOutEidNumDef);
            Out.Load(Flag);
            Out.Load(PrevNode);
            Out.Load(NextNode);
        }
        void Save(MIn& In)const{
            In.Save(Nid);
            In.Save(Data,NodeDatalengthDef);
            In.Save(NidVNum);
            In.Save(NidV,InOutEidNumDef);
            In.Save(Flag);
            In.Save(PrevNode);
            In.Save(NextNode);
        }
        int GetId()const{return Nid;}
        const char* GetData()const{return Data;}
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
        
        static void* operator new(size_t size_t,Arena* table){
            char * ptr=table->AllocateNode();
            return (void*)ptr;
        }
        static void operator delete(void* p, Arena* table){
            table->RecoverNode();
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
    };
    class NvmNodeI{
    private:
        NvmNode* CurNode;
        const char* End;
    public:
        NvmNodeI():CurNode(NULL),End(NULL){}
        NvmNodeI(NvmNode* node,char* end):CurNode(node),End(end){}
        NvmNodeI(const NvmNodeI& nodeI):CurNode(nodeI.CurNode),End(nodeI.End){}
        NvmNodeI& operator= (const NvmNodeI& nodeI){
            CurNode=nodeI.CurNode;
            End=nodeI.End;
            return *this;
        }
        NvmNodeI& operator++(int){
            char* cur=(char*)CurNode;
            if(cur>=End) return *this;
            CurNode++;
            return *this;
        }
        NvmNodeI& operator--(int){CurNode--;return *this;}
        bool operator<(const NvmNodeI& nodeI)const{return CurNode<nodeI.CurNode;}
        bool operator==(const NvmNodeI& nodeI)const{return CurNode==nodeI.CurNode;}
        
        int GetId()const{return CurNode->GetId();}
        const char* GetData()const{return CurNode->GetData();}
        int GetDeg()const{return CurNode->GetDeg();}
        int GetInDeg()const{return CurNode->GetInDeg();}
        int GetOutDeg()const{return CurNode->GetOutDeg();}
        void SortNidV(){CurNode->SortNidV();}
        int GetNbrNid(const int& nodeN)const{return CurNode->GetNbrNid(nodeN);}
        int GetInNid(const int& nodeN)const{return CurNode->GetInNid(nodeN);}
        int GetOutNid(const int& nodeN)const{return CurNode->GetOutNid(nodeN);}
        bool IsNbrNid(const int& nid)const{return CurNode->IsNbrNid(nid);}
        bool IsInNid(const int& nid)const{return CurNode->IsInNid(nid);}
        bool IsOutNid(const int& nid)const{return CurNode->IsOutNid(nid);}
        
    };
private:
    CuckooHash::HashTable NodeHash;
    Arena NodeTable;
    NvmNode* HeadNode;
public:
    UNDerict_Graph():NodeTable(sizeof(NvmNode)),NodeHash(32,4){
        HeadNode=AddNode(0);
    }
    NvmNode* AddNode(const int& nid){
        NvmNode* node=new(&NodeTable) NvmNode(nid);
        uint64_t location=(char*)node-NodeTable.BeginPtr();
        if(!NodeHash.Add(nid, location)){
            int BucketNum=32;
            BucketNum*=2;
            while(!NodeHash.ReSize(BucketNum)) BucketNum*=2;
        }
        return node;
    }
    NvmNode* AddNode(const int& nid,const char* data){
        NvmNode* node=new(&NodeTable) NvmNode(nid,data);
        uint64_t location=(char*)node-NodeTable.BeginPtr();
        if(!NodeHash.Add(nid, location)){
            int BucketNum=32;
            BucketNum*=2;
            while(!NodeHash.ReSize(BucketNum)) BucketNum*=2;
        }
        return node;
    }
   
   
    size_t GetCurNodeNum()const{
        return NodeTable.GetCurAllocateNodeNum();
    }
    
    int GetNode(const int& nid)const{
        uint64_t location;
        if(NodeHash.Find(nid, location)){
            return (HeadNode+(location/NodeTable.NodeSize()))->GetId();
        }
        return -1;
    }
    const char* GetNodeData(const int& nid) const{
        uint64_t location;
        if(NodeHash.Find(nid, location)){
            return (HeadNode+(location/NodeTable.NodeSize()))->GetData();
        }
        return "\0";
    }
    
    
};

#endif /* Header_h */
