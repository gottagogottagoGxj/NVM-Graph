//
//  CHL_Weight.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/31.
//

#ifndef CHL_Weight_h
#define CHL_Weight_h

#include<vector>
#include<utility>
#include<queue>
#include<algorithm>
#include<unordered_map>
#include<map>
#include<unordered_set>
#include"hash.h"
#include "Arena.h"
#include "InOut.h"

template<class PGraph>
class CHL_Weight{
public:
    PGraph* Graph;
    std::vector<std::pair<std::vector<int>,std::vector<int>>> Label;
    std::unordered_set<int> LocalMinSet;
    const int AttrId;
    HashTable<int, size_t> NidToLabel;
    Arena* LabelTable;
    
    
public:
    CHL_Weight(PGraph* graph,const int& attrId):Graph(graph),AttrId(attrId),Label(std::vector<std::pair<std::vector<int>,std::vector<int>>>(Graph->GetMxNid())),NidToLabel(32,4){}
    void ConstructIndex();
    void ConstructIndexMin();
    int Query(const int& SrcNid,const int& DstNid)const;
    int QueryMin(const int& SrcNid,const int& DstNid)const;
    int GetLabelSize()const{
        int size=0;
        for(int i=0;i<Label.size();++i){
            size+=Label[i].first.size();
        }
        return size;
    }
    
    void LoadLabel(const Arena* arena);
    void SaveLabel(Arena* arena);
    int QueryNVM(const int& SrcNid,const int& DstNid)const;
    int QueryMinNVM(const int& SrcNid,const int& DstNid)const;
    
private:
    void AddHubLabel(const int& SrcNid,const int& DstNid,const int& Dist);
    void PrunedDijkstra(const int& SrcNid);
    void PrunedDijkstraMin(const int& SrcNid);
    void ContractNode(const int& Nid);
    void Dijkstra(const int& Nid,std::unordered_map<int, int>& Dist,const int& MaxValue, const int& Skip)const;
    
};

template<class PGraph>
void CHL_Weight<PGraph>::ConstructIndex(){
    std::vector<std::pair<int,int>> DegToNid;
    auto iter=Graph->BegNI();
    while(!iter.IsEnd()){
        DegToNid.push_back(std::pair<int,int>(iter.GetDeg(),iter.GetId()));
        iter++;
    }
    std::sort(DegToNid.rbegin(),DegToNid.rend());
    for(int i=0;i<DegToNid.size();++i){
        PrunedDijkstra(DegToNid[i].second);
    }
}

template<class PGraph>
void CHL_Weight<PGraph>::AddHubLabel(const int &SrcNid, const int &DstNid, const int &Dist){
    std::vector<int> LF=Label[SrcNid].first;
    int start=0;
    int end=LF.size();
    while(start<end){
        int mid=(start+end)>>1;
        if(DstNid<LF[mid]) end=mid;
        else start=mid+1;
    }
    Label[SrcNid].first.insert(Label[SrcNid].first.begin()+start,DstNid);
    Label[SrcNid].second.insert(Label[SrcNid].second.begin()+start,Dist);
}

template<class PGraph>
void CHL_Weight<PGraph>::PrunedDijkstra(const int& SrcNid){
    std::unordered_map<int, int> NidToDist;
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
    Queue.push(std::pair<int, int>(0,SrcNid));
    NidToDist[SrcNid]=0;
    while (!Queue.empty()) {
        auto CurPair=Queue.top();Queue.pop();
        if(NidToDist[CurPair.second]<CurPair.first) continue;
        int Dist=Query(SrcNid,CurPair.second);
        if(Dist>CurPair.first){
            AddHubLabel(CurPair.second,SrcNid,CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(CurPair.second);
            for(int i=0;i<iter.GetDeg();++i){
                int DstNid=iter.GetNbrNid(i);
                int value;
                Graph->GetAttrDatE(CurPair.second,DstNid,AttrId,&value);
                if(NidToDist.find(DstNid)==NidToDist.end() || NidToDist[DstNid]>CurPair.first+value){
                    NidToDist[DstNid]=CurPair.first+value;
                    Queue.push(std::pair<int, int>(CurPair.first+value,DstNid));
                }
                
            }
        }
    }

}

template<class PGraph>
int CHL_Weight<PGraph>::Query(const int &SrcNid, const int &DstNid)const{
    int i=0;int j=0;int Dist=INT_MAX;
    std::vector<int> LF=Label[SrcNid].first;
    std::vector<int> LR=Label[DstNid].first;
    int SrcF=LF.size();
    int DstR=LR.size();
    while(i<SrcF && j<DstR){
        if(LF[i]==LR[j]){
            Dist=std::min(Dist, Label[SrcNid].second[i]+Label[DstNid].second[j]);
            i++;
            j++;
        }
        else if(LF[i]<LR[j]) i++;
        else j++;
    }
    return Dist;
}

template<class PGraph>
void CHL_Weight<PGraph>::ConstructIndexMin(){
    std::vector<std::pair<int,int>> DegToNid;
    auto iter=Graph->BegNI();
    while(!iter.IsEnd()){
        DegToNid.push_back(std::pair<int,int>(iter.GetDeg(),iter.GetId()));
        iter++;
    }
    std::sort(DegToNid.rbegin(),DegToNid.rend());
    std::map<int,int> NidToIndex;
    for(int i=0;i<DegToNid.size();++i){
        NidToIndex[DegToNid[i].second]=i;
    }
    iter=Graph->BegNI();
    while(!iter.IsEnd()){
        bool flag=true;
        for(int i=0;i<iter.GetDeg();++i){
            if(NidToIndex[iter.GetId()]<NidToIndex[iter.GetNbrNid(i)]){
                flag=false;
                break;
            }
        }
        if(flag){
            LocalMinSet.insert(iter.GetId());
            ContractNode(iter.GetId());
            DegToNid.erase(DegToNid.begin()+NidToIndex[iter.GetId()]);
        }
        iter++;
    }
    for(int i=0;i<DegToNid.size();++i){
        PrunedDijkstraMin(DegToNid[i].second);
    }
}

template<class PGraph>
void CHL_Weight<PGraph>::ContractNode(const int &Nid){
    int MaxValue=0;
    typename PGraph::NodeIter CurNode=Graph->GetNI(Nid);
    for(int i=0;i<CurNode.GetDeg();++i){
        int value;
        Graph->GetAttrDatE(Nid,CurNode.GetNbrNid(i),AttrId,&value);
        MaxValue=std::max(value,MaxValue);
    }
    MaxValue<<=1;
    for(int i=0;i<CurNode.GetDeg()-1;++i){
        int InCost;
        int InNid=CurNode.GetNbrNid(i);
        Graph->GetAttrDatE(Nid,InNid,AttrId,&InCost);
        std::unordered_map<int, int> NidToDist;
        Dijkstra(InNid, NidToDist, MaxValue, 5);
        for(int j=i+1;j<CurNode.GetDeg();++j){
            int OutCost;
            int OutNid=CurNode.GetNbrNid(j);
            Graph->GetAttrDatE(Nid,OutNid,AttrId,&OutCost);
            if(NidToDist.find(OutNid)==NidToDist.end() || NidToDist[OutNid]>InCost+OutCost){
                Graph->AddEdge3(InNid,OutNid,AttrId,InCost+OutCost);
            }
        }

    }
}

template<class PGraph>
void CHL_Weight<PGraph>::Dijkstra(const int &Nid, std::unordered_map<int, int> &Dist, const int &MaxValue, const int &Skip)const{
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
    Queue.push(std::make_pair(0, Nid));
    Dist[Nid]=0;
    int CurSkip=0;
    while(!Queue.empty()){
        auto CurPair=Queue.top();Queue.pop();
        if(CurSkip>Skip || CurPair.first>MaxValue) return;
        if(Dist[CurPair.second]<CurPair.first) continue;
        typename PGraph::NodeIter CurNode=Graph->GetNI(CurPair.second);
        for(int i=0;i<CurNode.GetDeg();++i){
            int NbrNid=CurNode.GetNbrNid(i);
            if(LocalMinSet.find(NbrNid)!=LocalMinSet.end()) continue;
            int weight;
            Graph->GetAttrDatE(CurPair.second,NbrNid,AttrId,&weight);
            if(Dist.find(NbrNid)==Dist.end() || Dist[NbrNid]>weight+CurPair.first){
                Dist[NbrNid]=weight+CurPair.first;
                Queue.push(std::make_pair(weight+CurPair.first, NbrNid));
            }
            
        }
        CurSkip++;
    }
}

template<class PGraph>
void CHL_Weight<PGraph>::PrunedDijkstraMin(const int &SrcNid){
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
    Queue.push(std::pair<int, int>(0,SrcNid));
    std::unordered_map<int, int> NidToDist;
    NidToDist[SrcNid]=0;
    while (!Queue.empty()) {
        auto CurPair=Queue.top();Queue.pop();
        if(NidToDist[CurPair.second]<CurPair.first) continue;
        int Dist=Query(SrcNid,CurPair.second);
        if(Dist>CurPair.first){
            AddHubLabel(CurPair.second,SrcNid,CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(CurPair.second);
            for(int i=0;i<iter.GetDeg();++i){
                int DstNid=iter.GetNbrNid(i);
                if(LocalMinSet.find(DstNid)!=LocalMinSet.end()) continue;
                int value;
                Graph->GetAttrDatE(CurPair.second,DstNid,AttrId,&value);
                if(NidToDist.find(DstNid)==NidToDist.end() || NidToDist[DstNid]>CurPair.first+value){
                    NidToDist[DstNid]=CurPair.first+value;
                    Queue.push(std::pair<int, int>(CurPair.first+value,DstNid));
                } 
            }
        }
    }
}



template<class PGraph>
int CHL_Weight<PGraph>::QueryMin(const int &SrcNid, const int &DstNid)const{
    int Dist=INT_MAX;
     if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)==LocalMinSet.end()){
         Dist=Query(SrcNid, DstNid);
     }
     else if(LocalMinSet.find(SrcNid)!=LocalMinSet.end()&& LocalMinSet.find(DstNid)==LocalMinSet.end()){
         typename PGraph::NodeIter SrcNode=Graph->GetNI(SrcNid);
         for(int k=0;k<SrcNode.GetDeg();++k){
             int NbrNid=SrcNode.GetNbrNid(k);
             int value;
             Graph->GetAttrDatE(SrcNid,NbrNid,AttrId,&value);
             Dist=std::min(Dist, value+Query(NbrNid, DstNid));
         }
     }
     else if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)!=LocalMinSet.end()){
         typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
         for(int k=0;k<DstNode.GetDeg();++k){
             int NbrNid=DstNode.GetNbrNid(k);
             int value;
             Graph->GetAttrDatE(NbrNid,DstNid,AttrId,&value);
             Dist=std::min(Dist, value+Query(SrcNid, NbrNid));
         }
     }
     else{
         typename PGraph::NodeIter SrcNode=Graph->GetNI(SrcNid);
         typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
         for(int k=0;k<SrcNode.GetDeg();++k){
             int SrcNbrNid=SrcNode.GetNbrNid(k);
             int value1;
             Graph->GetAttrDatE(SrcNid,SrcNbrNid,AttrId,&value1);
             for(int l=0;l<DstNode.GetDeg();++l){
                 int DstNbrNid=DstNode.GetNbrNid(l);
                 int value2;
                 Graph->GetAttrDatE(DstNbrNid,DstNid,AttrId,&value2);
                 Dist=std::min(Dist, value1+value2+Query(SrcNbrNid, DstNbrNid));
             }
         }
     }
     return Dist;
 }

template<class PGraph>
void CHL_Weight<PGraph>::SaveLabel(Arena *arena){
    LabelTable=arena;
    for(int i=0;i<Label.size();++i){
        if(Label[i].first.size()==0) continue;
        int length=Label[i].first.size();
        int Length=(length<<3)+(sizeof(int)<<1);
        char* CurPtr=LabelTable->AllocateBytes(Length);
        MIn NVMIn(CurPtr,Length);
        NVMIn.Save(i);
        NVMIn.Save(length);
        for(int j=0;j<length;++j) NVMIn.Save(Label[i].first[j]);
        for(int j=0;j<length;++j) NVMIn.Save(Label[i].second[j]);
        NidToLabel.Add(i, sizeof(int), CurPtr-LabelTable->BeginPtr());
    }
}
template<class PGraph>
int CHL_Weight<PGraph>::QueryNVM(const int &SrcNid, const int &DstNid)const{
    size_t SrcLocation,DstLocation;
    NidToLabel.Find(SrcNid, sizeof(int), SrcLocation);
    NidToLabel.Find(DstNid, sizeof(int), DstLocation);
    int SrcF,DstR;
    for(int i=0;i<4;++i){
        ((char*)&SrcF)[i]=(LabelTable->BeginPtr()+SrcLocation+sizeof(int))[i];
        ((char*)&DstR)[i]=(LabelTable->BeginPtr()+DstLocation+sizeof(int))[i];
    }
    MOut LF(LabelTable->BeginPtr()+SrcLocation+sizeof(int)*2,SrcF<<2);
    MOut LR(LabelTable->BeginPtr()+DstLocation+sizeof(int)*2,DstR<<2);
    int i=0,j=0;
    int Dist=INT_MAX;
    int LFK,LRK;
    LF.Load(LFK);
    LR.Load(LRK);
    while(i<SrcF && j<DstR){
        if(LFK==LRK){
            MOut Value1(LabelTable->BeginPtr()+SrcLocation+4*2+SrcF*4+i*4,4);
            MOut Value2(LabelTable->BeginPtr()+DstLocation+4*2+DstR*4+j*4,4);
            int LFV,LRV;
            Value1.Load(LFV);
            Value2.Load(LRV);
            Dist=std::min(Dist, LFV+LRV);
            i++;
            LF.Load(LFK);
            j++;
            LR.Load(LRK);
        }
        else if(LFK<LRK){
            LF.Load(LFK);
            i++;
        }
        else{
            LR.Load(LRK);
            j++;
        }
    }
    return Dist;
    
}
template<class PGraph>
int CHL_Weight<PGraph>::QueryMinNVM(const int &SrcNid, const int &DstNid)const{
    int Dist=INT_MAX;
    if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)==LocalMinSet.end()){
        Dist=QueryNVM(SrcNid, DstNid);
    }
    else if(LocalMinSet.find(SrcNid)!=LocalMinSet.end() && LocalMinSet.find(DstNid)==LocalMinSet.end()){
        auto SrcNode=Graph->GetNI(SrcNid);
        for(int i=0;i<SrcNode.GetDeg();++i){
            int SrcNbrNid=SrcNode.GetNbrNid(i);
            int value;
            Graph->GetAttrDatE(SrcNid,SrcNbrNid,AttrId,&value);
            Dist=std::min(Dist,value+QueryNVM(SrcNbrNid, DstNid));
        }
    }
    else if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)!=LocalMinSet.end()){
        auto DstNode=Graph->GetNI(DstNid);
        for(int i=0;i<DstNode.GetDeg();++i){
            int DstNbrNid=DstNode.GetNbrNid(i);
            int value;
            Graph->GetAttrDatE(DstNid,DstNbrNid,AttrId,&value);
            Dist=std::min(Dist,value+QueryNVM(SrcNid, DstNbrNid));
        }
    }
    else{
        auto SrcNode=Graph->GetNI(SrcNid), DstNode=Graph->GetNI(DstNid);
        for(int i=0;i<SrcNode.GetDeg();++i){
            int SrcNbrNid=SrcNode.GetNbrNid(i);
            int value1;
            Graph->GetAttrDatE(SrcNid,SrcNbrNid,AttrId,&value1);
            for(int j=0;j<DstNode.GetDeg();++j){
                int DstNbrNid=DstNode.GetNbrNid(j);
                int value2;
                Graph->GetAttrDatE(DstNid,DstNbrNid,AttrId,&value2);
                Dist=std::min(Dist, value1+value2+QueryNVM(SrcNbrNid, DstNbrNid));
            }
        }
    }
    return Dist;
}

#endif /* CHL_Weight_h */
