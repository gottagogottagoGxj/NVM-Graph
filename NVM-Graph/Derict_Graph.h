//
//  Derict_Graph.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/30.
//

#ifndef Derict_Graph_h
#define Derict_Graph_h
#include<string>

#include"hashtable.h"
#include"Arena.h"
#include"InOut.h"
const size_t NodeDataLengthDef=32;
const size_t InOutEdgeNumDef=10;

class Derict_graph{
    class NvmNode{
    private:
        int Nid;
        char Data[NodeDataLengthDef];
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
            Out.Load(InNidV, InOutEidNumDef);
            Out.Load(OutNidV, InOutEidNumDef);
            Out.Load(Flag);
            Out.Load(NextNode);
        }
        NvmNode(const int nid,const int flag):Nid(nid),Data("\0"),InNidVNum(0),OutNidVNum(0),Flag(flag),NextNode(0){}
        
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
        
        int GetId()const{return Nid;}
        const char* GetData()const{return Data;}
        void SetData(const char* data){
            size_t length=strlen(data);
            if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
            memcpy(Data, data, length);
            Data[length]='\0';
        }
        bool GetFlag()const{return Flag;}
        void SetFlag(const bool flag){Flag=flag;}
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
            if(InNidVNum<InOutEidNumDef){
                InNidV[InNidVNum]=nid;
                InNidVNum++;
                return true;
            }
            return false;
        }
        bool AddOutNid(const int& nid){
            if(OutNidVNum<InOutEidNumDef){
                OutNidV[OutNidVNum]=nid;
                OutNidVNum++;
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
                    return true;
                }
            }
            return false;
        }
        bool DeleteOutNid(const int& nid){
            for(int i=0;i<OutNidVNum;++i){
                if(OutNidV[i]==nid){
                    for(int j=i;j<(OutNidVNum-1);++j){OutNidV[j]=OutNidV[j+1];}
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
            if(InNidVNum>=InOutEidNumDef) return false;
            if(asc){
                
            }
            
        }
        bool AddOutNidSort(const int& nid,const bool asc=true){
            if(OutNidVNum>=InOutEidNumDef) return false;
            
        }
        bool DeleteInNidSort(const int& nid,const bool asc=true){}
        bool DeleteOutNidSort(const int& nid,const bool asc=true){}
        
    };
};

#endif /* Derict_Graph_h */
