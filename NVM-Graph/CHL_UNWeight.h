//
//  CHL_UNWeight.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/25.
//

#ifndef CHL_UNWeight_h
#define CHL_UNWeight_h

#include<iostream>
#include<time.h>
#include<vector>
#include<utility>
#include<queue>
#include<algorithm>
#include<unordered_map>
#include<unordered_set>
template<class PGraph>
class CHL{
public:
    const PGraph* Graph;
    std::vector<std::pair<std::vector<int>,std::vector<int>>> Label;
    std::unordered_set<int> LocalMinSet;
    

public:
    CHL(const PGraph* graph):Graph(graph),Label(std::vector<std::pair<std::vector<int>,std::vector<int>>>(Graph->GetMxNid())){}
    void ConstructIndex();
    void ConstructIndexBetweenMin();
    void ConstructIndexBetween();
    int Query(const int& SrcNid,const int& DstNid)const;
    int QueryBetweenMin(const int& SrcNid,const int& DstNid)const;
    int GetLabelSize(){
        int size=0;
        for(int i=0;i<Label.size();++i){
            size+=Label[i].first.size();
        }
        return size;
    }
    
private:
    void AddHubLabel(const int& SrcNid,const int& DstNid,const int& Dist);
    void PrunedBFS(const int& SrcNid);
    void PrunedBFSMin(const int& SrcNid);
    
};

template<class PGraph>
void CHL<PGraph>::ConstructIndex(){
    clock_t start,end;
    start=clock();
    std::vector<std::pair<int64_t, int>> Nid;
    typename PGraph::NodeIter Iter=Graph->BegNI();
    while (!Iter.IsEnd()) {
        int Deg=Iter.GetDeg();
        Nid.push_back(std::pair<int64_t, int>(Deg,Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    end=clock();
    std::cout<<"CHL排序时间："<<end-start<<std::endl;
    start=clock();
    for(int i=0;i<Nid.size();++i){
        PrunedBFS(Nid[i].second);
    }
    end=clock();
    std::cout<<"CHL构建标签时间："<<end-start<<std::endl;
}

template<class PGraph>
void CHL<PGraph>::PrunedBFS(const int& SrcNid){
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
                    Queue.push(std::pair<int, int>(CurPair.first+1,DstNid));
                }
            }
        }
}
template<class PGraph>
int CHL<PGraph>::Query(const int &SrcNid, const int &DstNid)const{
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
void CHL<PGraph>::ConstructIndexBetween(){
    clock_t start,end;
    start=clock();
    std::vector<std::pair<int64_t, int>> Nid;
    typename PGraph::NodeIter Iter=Graph->BegNI();
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
    end=clock();
    std::cout<<"BCHL排序时间："<<end-start<<std::endl;
    start=clock();
    for(int i=0;i<Nid.size();++i){
        PrunedBFS(Nid[i].second);
    }
    end=clock();
    std::cout<<"BCHL构建标签时间："<<end-start<<std::endl;
}


template<class PGraph>
void CHL<PGraph>::ConstructIndexBetweenMin(){
    clock_t start,end;
    start=clock();
    std::vector<std::pair<int64_t, int>> Nid;
    typename PGraph::NodeIter Iter=Graph->BegNI();
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
        if(between==0) LocalMinSet.insert(Iter.GetId());
        else  Nid.push_back(std::pair<int64_t, int>(between,Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    end=clock();
    std::cout<<"BCHL排序时间："<<end-start<<std::endl;
    start=clock();
    for(int i=0;i<Nid.size();++i){
        PrunedBFSMin(Nid[i].second);
    }
    end=clock();
    std::cout<<"BCHL构建标签时间："<<end-start<<std::endl;
}


template<class PGraph>
void CHL<PGraph>::PrunedBFSMin(const int& SrcNid){
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
                    Queue.push(std::pair<int, int>(CurPair.first+1,DstNid));
                }
            }
        }
}
template<class PGraph>
int CHL<PGraph>::QueryBetweenMin(const int &SrcNid, const int &DstNid)const{
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
            int i=0;int j=0;
            std::vector<int> LF=Label[NbrNid].first;
            int SrcF=LF.size();
            while(i<SrcF && j<DstR){
                if(LF[i]==LR[j]){
                    Dist=std::min(Dist, Label[NbrNid].second[i]+Label[DstNid].second[j]);
                    i++;
                    j++;
                }
                else if(LF[i]<LR[j]) i++;
                else j++;
            }
        }
        Dist++;
    }
    else if(LocalMinSet.find(SrcNid)==LocalMinSet.end() && LocalMinSet.find(DstNid)!=LocalMinSet.end()){
        std::vector<int> LF=Label[SrcNid].first;
        int SrcF=LF.size();
        typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
        for(int k=0;k<DstNode.GetDeg();++k){
            int NbrNid=DstNode.GetNbrNid(k);
            int i=0;int j=0;
            std::vector<int> LR=Label[NbrNid].first;
            int DstR=LR.size();
            while(i<SrcF && j<DstR){
                if(LF[i]==LR[j]){
                    Dist=std::min(Dist, Label[SrcNid].second[i]+Label[NbrNid].second[j]);
                    i++;
                    j++;
                }
                else if(LF[i]<LR[j]) i++;
                else j++;
            }
        }
        Dist++;
    }
    else{
        typename PGraph::NodeIter SrcNode=Graph->GetNI(SrcNid);
        typename PGraph::NodeIter DstNode=Graph->GetNI(DstNid);
        for(int k=0;k<SrcNode.GetDeg();++k){
            int SrcNbrNid=SrcNode.GetNbrNid(k);
            std::vector<int> LF=Label[SrcNbrNid].first;
            int SrcF=LF.size();
            for(int l=0;l<DstNode.GetDeg();++l){
                int DstNbrNid=DstNode.GetNbrNid(l);
                std::vector<int> LR=Label[DstNbrNid].first;
                int DstR=LR.size();
                int i=0;int j=0;
                while(i<SrcF && j<DstR){
                    if(LF[i]==LR[j]){
                        Dist=std::min(Dist, Label[SrcNbrNid].second[i]+Label[DstNbrNid].second[j]);
                        i++;
                        j++;
                    }
                    else if(LF[i]<LR[j]) i++;
                    else j++;
                }
            }
        }
        Dist+=2;
    }
    return Dist;
}


template<class PGraph>
void CHL<PGraph>::AddHubLabel(const int &SrcNid, const int &DstNid, const int &Dist){
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




#endif /* CHL_UNWeight_h */
