//
//  CHL_UNWeight.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/25.
//

#ifndef CHL_UNWeight_h
#define CHL_UNWeight_h

#include<iostream>
#include<vector>
#include<utility>
#include<queue>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>
#include"UNDirect_UNWeight_Graph.h"

const int InitMax=10;

class CHL{
public:
    const UNDirect_UNWeight_Graph* Graph;
    Arena* LabelTable;
    CuckooHash::HashTable NidToLabel;
    std::unordered_set<int> MinSet;
    

public:
    CHL(const UNDirect_UNWeight_Graph* graph,Arena* table);
    void ConstructIndex();
    void ConstructIndexBetweenMin();
    void ConstructIndexBetween();
    int Query(const int& SrcNid,const int& DstNid)const;
    int GetAllocLabelNum()const{
        int sum=0;
        auto iter=NidToLabel.Begin();
        while(!iter.IsEnd()){
            uint64_t location=iter.GetLocation();
            const char* LabelAddress=LabelTable->BeginPtr()+location+sizeof(int);
            MOut NVMout(LabelAddress,sizeof(int));
            int num=0;
            NVMout.Load(num);
            sum+=num;
            iter++;
        }
        return sum;
    }
    int GetLabelNum()const{
        int sum=0;
        auto iter=NidToLabel.Begin();
        while(!iter.IsEnd()){
            uint64_t location=iter.GetLocation();
            const char* LabelAddress=LabelTable->BeginPtr()+location+sizeof(int)*2;
            MOut NVMout(LabelAddress,sizeof(int));
            int num=0;
            NVMout.Load(num);
            sum+=num;
            iter++;
        }
        return sum;
    }
    
private:
    void AddHubLabel(const int& SrcNid,const int& DstNid,const int& Dist);
    int QueryFromTable(const int& SrcNid,const int& DstNid)const;
    void PrunedBFS(const int& SrcNid);
    void PrunedBFSMin(const int& SrcNid);
    void InitLabletable();
    
};

CHL::CHL(const UNDirect_UNWeight_Graph* graph,Arena* table):Graph(graph),LabelTable(table),NidToLabel(32, 4){
    InitLabletable();
}

void CHL::InitLabletable(){
    if(LabelTable->EndPtr()!=LabelTable->BeginPtr()){
        const char* curptr=LabelTable->BeginPtr();
        while(curptr<LabelTable->EndPtr()){
            MOut curnode(curptr,sizeof(int)*2);
            int Nid,MaxLabelNum;
            curnode.Load(Nid);
            curnode.Load(MaxLabelNum);
            NidToLabel.Add(Nid, curptr-LabelTable->BeginPtr());
            curptr+=sizeof(int)*(3+MaxLabelNum*2);
        }
    }
    else{
        auto iter=Graph->GetNidBegin();
        while(!iter.IsEnd()){
            if(MinSet.find(iter.GetKey())==MinSet.end()){
                int length=(sizeof(int))*(3+InitMax*2);
                char* curptr=LabelTable->AllocateBytes(length);
                MIn curnode(curptr,sizeof(int)*3);
                curnode.Save(iter.GetKey());
                curnode.Save(InitMax);
                curnode.Save(int(0));
                NidToLabel.Add(iter.GetKey(), curptr-LabelTable->BeginPtr());
            }
            iter++;
        }
    }
}

void CHL::AddHubLabel(const int &SrcNid, const int &DstNid, const int &Dist){
    uint64_t location;
    NidToLabel.Find(SrcNid, location);
    char* curptr=LabelTable->HeadPtr()+location;
    MOut curnode(curptr+sizeof(int),sizeof(int)*2);
    int CurMax,CurNum;
    curnode.Load(CurMax);
    curnode.Load(CurNum);
    int* LF=(int*)(curptr+sizeof(int)*3);
    int* LS=(int*)(curptr+sizeof(int)*(3+CurMax));
    int start=0;
    int end=CurNum;
    while(start<end){
        int mid=(start+end)>>1;
        if(DstNid<LF[mid]) end=mid;
        else start=mid+1;
    }
    if(CurNum+1<=CurMax){
        for(int i=CurNum;i>start;--i){
            LF[i]=LF[i-1];
            LS[i]=LS[i-1];
        }
        LF[start]=DstNid;
        LS[start]=Dist;
        CurNum+=1;
        MIn node(curptr+sizeof(int)*2,sizeof(int));
        node.Save(CurNum);
    }
    else{
        CurMax*=2;
        CurNum+=1;
        char* allocptr=LabelTable->AllocateBytes(sizeof(int)*(3+CurMax*2));
        MIn node(allocptr,sizeof(int)*3);
        node.Save(SrcNid);
        node.Save(CurMax);
        node.Save(CurNum);
        int* LFN=(int*)(allocptr+sizeof(int)*3);
        int* LSN=(int*)(allocptr+sizeof(int)*(3+CurMax));
        int i;
        for(i=0;i<start;++i){
            LFN[i]=LF[i];
            LSN[i]=LSN[i];
        }
        LFN[i]=DstNid;
        LSN[i]=Dist;
        for(;i<CurNum;++i){
            LFN[i]=LF[i-1];
            LSN[i]=LS[i-1];
        }
        NidToLabel.Delete(SrcNid);
        NidToLabel.Add(SrcNid, allocptr-LabelTable->BeginPtr());
    }
    
}

int CHL::QueryFromTable(const int &SrcNid, const int &DstNid)const{
    int i=0;int j=0;int Dist=INT_MAX;
    uint64_t SrcLocation,DstLocation;
    if(NidToLabel.Find(SrcNid, SrcLocation)&&NidToLabel.Find(DstNid, DstLocation)){
        int SrcNum,DstNum,SrcMax,DstMax;
        MOut srcnode(LabelTable->BeginPtr()+SrcLocation+sizeof(int),sizeof(int)*2);
        srcnode.Load(SrcMax);
        srcnode.Load(SrcNum);
        MOut dstnode(LabelTable->BeginPtr()+DstLocation+sizeof(int),sizeof(int)*2);
        dstnode.Load(DstMax);
        dstnode.Load(DstNum);
        const int* SrcF=(int*)(LabelTable->BeginPtr()+SrcLocation+sizeof(int)*3);
        const int* SrcS=SrcF+SrcMax;
        const int* DstF=(int*)(LabelTable->BeginPtr()+DstLocation+sizeof(int)*3);
        const int* DstS=DstF+DstMax;
        while(i<SrcNum && j<DstNum){
            if(SrcF[i]==DstF[j]){
                Dist=std::min(Dist,SrcS[i]+DstS[j]);
                i++;
                j++;
            }
            else if(SrcF[i]<DstF[j]) i++;
            else j++;
        }
        
    }
    return Dist;
}



void CHL::ConstructIndex(){
    std::vector<std::pair<int64_t, int>> Nid;
    auto Iter=Graph->BegNI();
    while (!Iter.IsEnd()) {
        int Deg=Iter.GetDeg();
        Nid.push_back(std::pair<int64_t, int>(Deg,Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    for(int i=0;i<Nid.size();++i){
        PrunedBFS(Nid[i].second);
    }
}


void CHL::PrunedBFS(const int& SrcNid){
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
        Queue.push(std::pair<int, int>(0,SrcNid));
        while (!Queue.empty()) {
            auto CurPair=Queue.top();Queue.pop();
            int Dist=QueryFromTable(SrcNid,CurPair.second);
            if(Dist>CurPair.first){
                AddHubLabel(CurPair.second,SrcNid,CurPair.first);
                auto iter=Graph->GetNI(CurPair.second);
                auto temp=iter;
                do{
                    temp=iter;
                    int Deg;
                    const int* Nbr=iter.GetCurNbr(Deg);
                    for(int i=0;i<Deg;++i){
                        int DstNid=Nbr[i];
                        Queue.push(std::pair<int, int>(CurPair.first+1,DstNid));
                    }
                    iter.ToNextNode();
                }while(!temp.IsNodeEnd());
                
            }
        }
}
int CHL::Query(const int &SrcNid, const int &DstNid)const{
     int Dist=INT_MAX;
     if(MinSet.find(SrcNid)==MinSet.end() && MinSet.find(DstNid)==MinSet.end()){
         Dist=QueryFromTable(SrcNid, DstNid);
     }
     else if(MinSet.find(SrcNid)!=MinSet.end()&& MinSet.find(DstNid)==MinSet.end()){
         auto srcnode=Graph->GetNI(SrcNid);
         auto tempnode=srcnode;
         do{
             tempnode=srcnode;
             int nbr_count;
             const int* nbr=srcnode.GetCurNbr(nbr_count);
             for(int i=0;i<nbr_count;++i){
                 const int src_nbr=nbr[i];
                 Dist=std::min(Dist,QueryFromTable(src_nbr, DstNid));
             }
             srcnode.ToNextNode();
         }while(!tempnode.IsNodeEnd());
         Dist++;
     }
     else if(MinSet.find(SrcNid)==MinSet.end() && MinSet.find(DstNid)!=MinSet.end()){
         auto dstnode=Graph->GetNI(DstNid);
         auto tempnode=dstnode;
         do{
             tempnode=dstnode;
             int nbr_count;
             const int* nbr=dstnode.GetCurNbr(nbr_count);
             for(int i=0;i<nbr_count;++i){
                 const int dst_nbr=nbr[i];
                 Dist=std::min(Dist,QueryFromTable(SrcNid, dst_nbr));
             }
             dstnode.ToNextNode();
         }while(!tempnode.IsNodeEnd());
         Dist++;
     }
     else{
         auto srcnode=Graph->GetNI(SrcNid);
         auto srctemp=srcnode;
         do{
             srctemp=srcnode;
             int src_nbr_count;
             const int* src_nbr=srcnode.GetCurNbr(src_nbr_count);
             for(int i=0;i<src_nbr_count;++i){
                 const int src_nbr_nid=src_nbr[i];
                 auto dstnode=Graph->GetNI(DstNid);
                 auto dsttemp=dstnode;
                 do{
                     dsttemp=dstnode;
                     int dst_nbr_count;
                     const int* dst_nbr=dstnode.GetCurNbr(dst_nbr_count);
                     for(int j=0;j<dst_nbr_count;++j){
                         const int dst_nbr_nid=dst_nbr[i];
                         Dist=std::min(Dist,QueryFromTable(src_nbr_nid, dst_nbr_nid));
                     }
                     
                     dstnode.ToNextNode();
                     
                 }while(!dsttemp.IsNodeEnd());
             }
             srcnode.ToNextNode();
         }while(!srctemp.IsNodeEnd());
         Dist+=2;
     }
     return Dist;
}

void CHL::ConstructIndexBetween(){
    
    std::vector<std::pair<int64_t, int>> Nid;
    auto Iter=Graph->BegNI();
    while (!Iter.IsEnd()) {
        int between=0;
        for(int i=0;i<Iter.GetDeg();++i){
            int SrcNid=Iter.GetNbrNid(i);
            for(int j=i+1;j<Iter.GetDeg();++j){
                int DstNid=Iter.GetNbrNid(j);
                    if(!Graph->IsEdge(SrcNid,DstNid))
                        between++;
            }
        }
        Nid.push_back(std::pair<int64_t, int>(between,Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    
    for(int i=0;i<Nid.size();++i){
        PrunedBFS(Nid[i].second);
    }
   
}


void CHL::ConstructIndexBetweenMin(){
    std::vector<std::pair<int64_t, int>> Nid;
    auto Iter=Graph->BegNI();
    while (!Iter.IsEnd()) {
        int between=0;
        for(int i=0;i<Iter.GetDeg();++i){
            int SrcNid=Iter.GetNbrNid(i);
            for(int j=i+1;j<Iter.GetDeg();++j){
                int DstNid=Iter.GetNbrNid(j);
                    if(!Graph->IsEdge(SrcNid,DstNid))
                        between++;
            }
        }
        if(between==0) MinSet.insert(Iter.GetId());
        else  Nid.push_back(std::pair<int64_t, int>(between,Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    for(int i=0;i<Nid.size();++i){
        PrunedBFSMin(Nid[i].second);
    }
}


void CHL::PrunedBFSMin(const int& SrcNid){
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
        Queue.push(std::pair<int, int>(0,SrcNid));
        while (!Queue.empty()) {
            auto CurPair=Queue.top();Queue.pop();
            int Dist=QueryFromTable(SrcNid, CurPair.second);
            if(Dist>CurPair.first){
                AddHubLabel(CurPair.second,SrcNid,CurPair.first);
                auto iter=Graph->GetNI(CurPair.second);
                auto temp=iter;
                do{
                    temp=iter;
                    int Deg;
                    const int* Nbr=iter.GetCurNbr(Deg);
                    for(int i=0;i<Deg;++i){
                        int DstNid=Nbr[i];
                        if(MinSet.find(DstNid)!=MinSet.end()) continue;
                        Queue.push(std::pair<int, int>(CurPair.first+1,DstNid));
                    }
                    iter.ToNextNode();
                }while(!temp.IsNodeEnd());
            }
        }
}



#endif /* CHL_UNWeight_h */
