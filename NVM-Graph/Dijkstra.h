//
//  Dijkstra.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/8.
//

#ifndef Dijkstra_h
#define Dijkstra_h

#include <unordered_map>
#include<utility>
#include<queue>
#include<tuple>
#include<vector>
#include"Property_Graph.h"


template<class PGraph>
class Dijkstra{
public:
    const PGraph* Graph;
    int StartNid;
    // tuple<bool,int,int>: 是否访问、与StartNid距离、最短路径中前一个顶点id
    std::unordered_map<int,std::tuple<bool,int,int>> NidDistH;
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
public:
    Dijkstra(const PGraph* GraphPt):Graph(GraphPt){}
    int DoDijkstra(const int& SrcNid,const int& TargetNid,const int& AttrId);
    int GetShortestPathLength(const int& StartNid,const int& DstNid);
    bool GetShortestPath(const int& SrcNid,const int& DstNid,std::vector<int>& path);
    
private:
    void ClearQueue(std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>>& q){
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>>  empty;
        std::swap(empty, q);
    }
};
template<class PGraph>
int Dijkstra<PGraph>::DoDijkstra(const int& SrcNid,const int& TargetNid,const int& AttrId){
    NidDistH.clear();
    StartNid=SrcNid;
    std::tuple<bool,int,int> init(false,INT_MAX,-1);
    CuckooHash::IterPair NidIter=Graph->GetNidBegin();
    while(!NidIter.IsEnd()){
        NidDistH[NidIter.GetKey()]=init;
        NidIter++;
    }
    std::get<1>(NidDistH[StartNid])=0;
    ClearQueue(Queue);
    Queue.push(std::pair<int,int>(0,StartNid));
    while (!Queue.empty()) {
        std::pair<int,int> CurPair=Queue.top();
        Queue.pop();
        int nid=CurPair.second;
        if(nid==TargetNid) return CurPair.first;
        int dist=CurPair.first;
        if(std::get<0>(NidDistH[nid])) continue;
        std::get<0>(NidDistH[nid])=true;
        typename PGraph::NodeIter iter=Graph->GetNI(nid);
        for(int i=0;i<iter.GetOutDeg();++i){
            int DstNid=iter.GetOutNid(i);
            if(std::get<0>(NidDistH[DstNid])) continue;
            int value=INT_MAX;
            Graph->GetAttrDatE(nid,DstNid,AttrId,&value);
            if(dist+value<std::get<1>(NidDistH[DstNid])){
                int curdist=dist+value;
                std::get<1>(NidDistH[DstNid])=curdist;
                std::get<2>(NidDistH[DstNid])=nid;
                Queue.push(std::pair<int,int>(curdist,DstNid));
            }
        }
        
    }
    return -1;
    
}
template<class PGraph>
int Dijkstra<PGraph>::GetShortestPathLength(const int& SrcNid,const int& DstNid){
    int dist=std::get<1>(NidDistH[DstNid]);
    if(dist!=INT_MAX) return dist;
    else return -1;
}

template<class PGraph>
bool Dijkstra<PGraph>::GetShortestPath(const int &SrcNid, const int &DstNid, std::vector<int> &path){
    int curnid=DstNid;
    path.push_back(curnid);
    while (std::get<2>(NidDistH[curnid])!=-1) {
        curnid=std::get<2>(NidDistH[curnid]);
        path.push_back(curnid);
    }
    if(curnid!=SrcNid) return false;
    std::reverse(path.begin(), path.end());
    return true;
}

#endif /* Dijkstra_h */
