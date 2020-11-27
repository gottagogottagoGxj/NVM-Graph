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
        NvmNode(const int nid):Nid(nid),NidVNum(0),PrevNode(0),NextNode(0),Flag(true),Data("\0"){}
        NvmNode(const int nid,const char* nodedata):Nid(nid),NidVNum(0),PrevNode(0),NextNode(0),Flag(true){
        size_t length=strlen(nodedata);
        memcpy(Data,nodedata,length);
        Data[length]='\0';
        }
        NvmNode(const int nid,const uint64_t prev):Nid(nid),NidVNum(0),Data("\0"),Flag(true),PrevNode(prev),NextNode(0){}
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
        bool GetFlag()const{return Flag;}
        void SetFlag(const bool& flag){Flag=flag;}
        const char* GetData()const{return Data;}
        void SetData(const char* data){
            size_t length=strlen(data);
            memcpy(Data, data, length);
            Data[length]='\0';
        }
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
        static void* operator new(size_t size,char* start){
            return (void*)start;
        }
        static void operator delete(void* p,char* start){}
        
        
        
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
        
        uint64_t GetPrevNodeAddre()const{return PrevNode;}
        void SetPrevNodeAddre(const uint64_t& location){PrevNode=location;}
        uint64_t GetNextNodeAddre()const{return NextNode;}
        void SetNextNodeAddre(const uint64_t& location){NextNode=location;}
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
            while((char*)CurNode<End && CurNode->GetPrevNodeAddre()!=0) CurNode++;
            return *this;
        }
        NvmNodeI& operator--(int){CurNode--;while(CurNode->GetPrevNodeAddre()!=0) CurNode--;return *this;}
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
    Arena* NodeTable;
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
    bool IsNodeNid(const int& nid)const{
        return NodeHash.IsIn(nid);
    }
    void DeleteNode(const uint64_t& location);
public:
    UNDerict_Graph(Arena* arena);
    
    static uint GetNodeSize(){return sizeof(NvmNode);}
    
    int AddNode(const int& nid=-1);
    int AddNode(const int& nid=-1,const char* data="");
    NvmNode* AddExistNode(const int& nid=0,const uint64_t& prev=0);
    size_t GetNodeNum()const{return NodeHash.GetValidSlotNum();}
    void DelNode(const int& nid);
    void DelNode(const NvmNode& node){DelNode(node.GetId());}
    int GetMxNid()const{return MxNid;}
   
   
    
    
    
    
    
};

#endif /* Header_h */
