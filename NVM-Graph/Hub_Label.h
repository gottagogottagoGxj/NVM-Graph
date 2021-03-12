//
//  Hub_Label.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/12.
//

#ifndef Hub_Label_h
#define Hub_Label_h
#include<vector>
#include<utility>
#include<queue>
#include<algorithm>
#include<unordered_map>
template<class PGraph>
class HubLabel{
public:
    const PGraph* Graph;
    std::vector<std::pair<std::vector<int>,std::vector<int>>> ForwardLabel;
    std::vector<std::pair<std::vector<int>,std::vector<int>>> ReverseLabel;
private:
    int AttrId;//距离属性编号
    
public:
    HubLabel(const PGraph* graph,const int& attrId):Graph(graph),ForwardLabel(std::vector<std::pair<std::vector<int>,std::vector<int>>>(Graph->GetMxNid())),ReverseLabel(std::vector<std::pair<std::vector<int>,std::vector<int>>>(Graph->GetMxNid())),AttrId(attrId){}
    void ConstructIndex();
    int Query(const int& SrcNid,const int& DstNid)const;
    
private:
    void AddHubLabel(const int& SrcNid,const bool& Forward,const bool& Reverse,const int& DstNid,const int& Dist);
    void PrunedDijkstra(const int& SrcNid,const bool& Forward,const bool& Reverse);
    
};

template<class PGraph>
void HubLabel<PGraph>::ConstructIndex(){
    std::vector<std::tuple<int,int,int>> Nid;
    typename PGraph::NodeIter Iter=Graph->BegNI();
    while (!Iter.IsEnd()) {
        Nid.push_back(std::tuple<int,int,int>(Iter.GetOutDeg(),Iter.GetInDeg(),Iter.GetId()));
        Iter++;
    }
    std::sort(Nid.rbegin(), Nid.rend());
    for(auto iter=Nid.begin();iter!=Nid.end();++iter){
        PrunedDijkstra(std::get<2>(*iter), true, true);
    }
}
template<class PGraph>
void HubLabel<PGraph>::PrunedDijkstra(const int& SrcNid,const bool& Forward,const bool& Reverse){
    if(Forward){
        std::unordered_map<int, std::pair<bool, int>> NidDisH;
        CuckooHash::IterPair NidIter=Graph->GetNidBegin();
        while(!NidIter.IsEnd()){
            NidDisH[NidIter.GetKey()]=std::pair<bool, int>(false,INT_MAX);
            NidIter++;
        }
        NidDisH[SrcNid].second=0;
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
        Queue.push(std::pair<int, int>(0,SrcNid));
        while (!Queue.empty()) {
            auto CurPair=Queue.top();Queue.pop();
            int nid=CurPair.second;
            if(NidDisH[nid].first) continue;
            NidDisH[nid].first=true;
            AddHubLabel(nid, false, true, SrcNid, CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(nid);
            for(int i=0;i<iter.GetOutDeg();++i){
                int DstNid=iter.GetOutNid(i);
                if(NidDisH[DstNid].first) continue;
                int value=INT_MAX;
                Graph->GetAttrDatE(nid,DstNid,AttrId,&value);
                int dist=CurPair.first+value;
                if(dist<NidDisH[DstNid].second && dist<Query(SrcNid, DstNid)){
                    NidDisH[DstNid].second=dist;
                    Queue.push(std::pair<int, int>(dist,DstNid));
                }
                
            }
            
        }
    }
    if(Reverse){
        std::unordered_map<int, std::pair<bool, int>> NidDisH;
        CuckooHash::IterPair NidIter=Graph->GetNidBegin();
        while(!NidIter.IsEnd()){
            NidDisH[NidIter.GetKey()]=std::pair<bool, int>(false,INT_MAX);
            NidIter++;
        }
        NidDisH[SrcNid].second=0;
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
        Queue.push(std::pair<int, int>(0,SrcNid));
        while (!Queue.empty()) {
            auto CurPair=Queue.top();Queue.pop();
            int nid=CurPair.second;
            if(NidDisH[nid].first) continue;
            NidDisH[nid].first=true;
            AddHubLabel(nid, true, false, SrcNid, CurPair.first);
            typename PGraph::NodeIter iter=Graph->GetNI(nid);
            for(int i=0;i<iter.GetInDeg();++i){
                int DstNid=iter.GetInNid(i);
                if(NidDisH[DstNid].first) continue;
                int value=INT_MAX;
                Graph->GetAttrDatE(DstNid,nid,AttrId,&value);
                int dist=CurPair.first+value;
                if(dist<NidDisH[DstNid].second && dist<Query(DstNid, SrcNid)){
                    NidDisH[DstNid].second=dist;
                    Queue.push(std::pair<int, int>(dist,DstNid));
                }
                
            }
            
        }
        
    }
}
template<class PGraph>
int HubLabel<PGraph>::Query(const int &SrcNid, const int &DstNid)const{
    int i=0;int j=0;int Dist=INT_MAX;
    std::vector<int> LF=ForwardLabel[SrcNid].first;
    std::vector<int> LR=ReverseLabel[DstNid].first;
    int SrcF=LF.size();
    int DstR=LR.size();
    while(i<SrcF && j<DstR){
        if(LF[i]==LR[j]){
            Dist=std::min(Dist, ForwardLabel[SrcNid].second[i]+ReverseLabel[DstNid].second[j]);
            i++;
            j++;
        }
        else if(LF[i]<LR[j]) i++;
        else j++;
    }
    return Dist;
}
template<class PGraph>
void HubLabel<PGraph>::AddHubLabel(const int &SrcNid, const bool &Forward, const bool& Reverse,const int &DstNid, const int &Dist){
    if(Forward){
        std::vector<int> LF=ForwardLabel[SrcNid].first;
        int start=0;
        int end=LF.size();
        while(start<end){
            int mid=(start+end)>>1;
            if(DstNid<LF[mid]) end=mid;
            else start=mid+1;
        }
        ForwardLabel[SrcNid].first.insert(ForwardLabel[SrcNid].first.begin()+start,DstNid);
        ForwardLabel[SrcNid].second.insert(ForwardLabel[SrcNid].second.begin()+start,Dist);
    }
    if(Reverse){
        std::vector<int> LR=ReverseLabel[SrcNid].first;
        int start=0;
        int end=LR.size();
        while (start<end) {
            int mid=(start+end)>>1;
            if(DstNid<LR[mid]) end=mid;
            else start=mid+1;
        }
        ReverseLabel[SrcNid].first.insert(ReverseLabel[SrcNid].first.begin()+start,DstNid);
        ReverseLabel[SrcNid].second.insert(ReverseLabel[SrcNid].second.begin()+start,Dist);
    }
}


#endif /* Hub_Label_h */
