//
//  Dijkstra.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/8.
//

/*#ifndef Dijkstra_h
#define Dijkstra_h

#inlcude<unordered_map>
#include<utility>
#include<queue>
#include"Property_Graph.h"
#include
template<class PGraph>
class Dijkstra{
public:
    PGraph* Graph;
    int StartNid;
    int AttrId_DistanceId;
    std::unordered_map<int,int> NidDistH;
    std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>> Queue;
public:
    Dijkstra(const PGraph* GraphPt):Graph(GraphPt){Graph->GetAttrIdE(AttrId_DistanceId,"Distance");}
    int DoDijKstra(const int& StartNode,const int& TargetNid=-1);
    
private:
    void ClearQueue(std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>>& q){
        std::priority_queue<std::pair<int, int>,std::vector<std::pair<int, int>>,std::greater<std::pair<int, int>>>  empty;
        swap(empty, q);
    }
}
template<class PGraph>
int Dijkstra<PGraph>::DoDijkstra(const int& StartNode,const int& TargetNid){
    StarttNid=StartNode;
    CuckooHash::IterPair NidIter=Graph->GetNidBegin();
    while(NidIter!=Graph->GetNidEnd()){
        NidDistH[NidIter.GetKey()]=INT_MAX;
    }
    NidDistH[StartNid]=0;
    ClearQueue(Queue);
    Queue.push(std::pair<int,int>(0,StartNid));
    while (!Queue.empty()) {
        std::pair<int,int> CurPair=Queue.top();
        Queue.pop();
        int nid=CurPair.second;
        
    }
    
}
#endif /* Dijkstra_h */
