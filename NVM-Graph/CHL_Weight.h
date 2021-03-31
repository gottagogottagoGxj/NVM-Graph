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

template<class PGraph>
class CHL_Weight{
public:
    PGraph* Graph;
    std::vector<std::pair<std::vector<int>,std::vector<int>>> Label;
    std::unordered_set<int> LocalMinSet;
    const int AttrId;
    
public:
    CHL_Weight(PGraph* graph,const int& attrId):Graph(graph),AttrId(attrId),Label(std::vector<std::pair<std::vector<int>,std::vector<int>>>(Graph->GetMxNid())){}
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
private:
    void AddHubLabel(const int& SrcNid,const int& DstNid,const int& Dist);
    void PrunedDijkstra(const int& SrcNid);
    void PrunedDijkstraMin(const int& SrcNid);
    
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
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
    Queue.push(std::pair<int, int>(0,SrcNid));
    while (!Queue.empty()) {
        auto CurPair=Queue.top();Queue.pop();
        int Dist=Query(SrcNid,CurPair.second);
        if(Dist>CurPair.first){
            AddHubLabel(CurPair.second,SrcNid,CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(CurPair.second);
            for(int i=0;i<iter.GetDeg();++i){
                int DstNid=iter.GetNbrNid(i);
                int value;
                Graph->GetAttrDatE(CurPair.second,DstNid,AttrId,&value);
                Queue.push(std::pair<int, int>(CurPair.first+value,DstNid));
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
    }
}

template<class PGraph>
void CHL_Weight<PGraph>::PrunedDijkstraMin(const int &SrcNid){
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
    Queue.push(std::pair<int, int>(0,SrcNid));
    while (!Queue.empty()) {
        auto CurPair=Queue.top();Queue.pop();
        int Dist=Query(SrcNid,CurPair.second);
        if(Dist>CurPair.first){
            AddHubLabel(CurPair.second,SrcNid,CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(CurPair.second);
            for(int i=0;i<iter.GetDeg();++i){
                int DstNid=iter.GetNbrNid(i);
                if(LocalMinSet.find(DstNid)!=LocalMinSet.end()) continue;
                int value;
                Graph->GetAttrDatE(CurPair.second,DstNid,AttrId,&value);
                Queue.push(std::pair<int, int>(CurPair.first+value,DstNid));
            }
        }
    }
}



template<class PGraph>
int CHL_Weight<PGraph>::QueryMin(const int &SrcNid, const int &DstNid)const{
    int Dist=INT_MAX;
     if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)==LocalMinSet.end()){
         int i=0;int j=0;
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
     }
     else if(LocalMinSet.find(SrcNid)!=LocalMinSet.end()&& LocalMinSet.find(DstNid)==LocalMinSet.end()){
         std::vector<int> LR=Label[DstNid].first;
         int DstR=LR.size();
         typename PGraph::NodeIter SrcNode=Graph->GetNI(SrcNid);
         for(int k=0;k<SrcNode.GetDeg();++k){
             int NbrNid=SrcNode.GetNbrNid(k);
             int value;
             Graph->GetAttrDatE(SrcNid,NbrNid,AttrId,&value);
             int i=0;int j=0;
             std::vector<int> LF=Label[NbrNid].first;
             int SrcF=LF.size();
             while(i<SrcF && j<DstR){
                 if(LF[i]==LR[j]){
                     Dist=std::min(Dist, Label[NbrNid].second[i]+Label[DstNid].second[j]+value);
                     i++;
                     j++;
                 }
                 else if(LF[i]<LR[j]) i++;
                 else j++;
             }
         }
     }
     else if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)!=LocalMinSet.end()){
         std::vector<int> LF=Label[SrcNid].first;
         int SrcF=LF.size();
         typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
         for(int k=0;k<DstNode.GetDeg();++k){
             int NbrNid=DstNode.GetNbrNid(k);
             int value;
             Graph->GetAttrDatE(NbrNid,DstNid,AttrId,&value);
             int i=0;int j=0;
             std::vector<int> LR=Label[NbrNid].first;
             int DstR=LR.size();
             while(i<SrcF && j<DstR){
                 if(LF[i]==LR[j]){
                     Dist=std::min(Dist, Label[SrcNid].second[i]+Label[NbrNid].second[j]+value);
                     i++;
                     j++;
                 }
                 else if(LF[i]<LR[j]) i++;
                 else j++;
             }
         }
     }
     else{
         typename PGraph::NodeIter SrcNode=Graph->GetNI(SrcNid);
         typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
         for(int k=0;k<SrcNode.GetDeg();++k){
             int SrcNbrNid=SrcNode.GetNbrNid(k);
             int value1;
             Graph->GetAttrDatE(SrcNid,SrcNbrNid,AttrId,&value1);
             std::vector<int> LF=Label[SrcNbrNid].first;
             int SrcF=LF.size();
             for(int l=0;l<DstNode.GetDeg();++l){
                 int DstNbrNid=DstNode.GetNbrNid(l);
                 int value2;
                 Graph->GetAttrDatE(DstNbrNid,DstNid,AttrId,&value2);
                 std::vector<int> LR=Label[DstNbrNid].first;
                 int DstR=LR.size();
                 int i=0;int j=0;
                 while(i<SrcF && j<DstR){
                     if(LF[i]==LR[j]){
                         Dist=std::min(Dist, Label[SrcNbrNid].second[i]+Label[DstNbrNid].second[j]+value1+value2);
                         i++;
                         j++;
                     }
                     else if(LF[i]<LR[j]) i++;
                     else j++;
                 }
             }
         }
     }
     return Dist;
 }


#endif /* CHL_Weight_h */
