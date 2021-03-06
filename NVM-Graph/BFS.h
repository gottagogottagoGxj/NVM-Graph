//
//  BFS.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/6.
//

#ifndef BFS_h
#define BFS_h

#include<queue>
#include<vector>
#include"hash.h"
// 针对无权重、无多重边（没有边表存在的）的图求最短路径，利用广度优先搜索算法
template<PGraph> int GetShortPath(const PGraph* Graph,const int& SrcNid,const int& DstNid,const bool& IsDir=false);
template<PGraph> int GetShortPath(const PGraph* Graph,const int& SrcNid,HashTable<int,int>& NidDistH,const bool& IsDir=false,const int& MxDist=INT_MAX);


template<class PGraph>
class BreathFS{
public:
    const PGraph* Graph;//指针
    std::queue<int> Queue;
    int StartNid;
    const HashTable<int,int>* NidDistH;//指针，从类外传进来
public:
    BreathFS(const PGraph* GraphPt,const HashTable<int,int>* NodeDistH):Graph(GraphPt),NidDistH(NodeDistH){}
    int DoBfs(const int& StartNode,const bool& FollowIn,const bool& FollowOut,const int& TargetNid=-1,const int& MxDist=INT_MAX);
    int GetHops(const int& SrcNid,const int& DstNid)const;
};
template<class PGraph>
int BreathFs<PGraph>::DoBfs(const int& StartNode,const bool& FollowIn,const bool& FollowOut,const int& TargetNid=-1,const int& MxDist=INT_MAX){
    StartNid=StartNode;
    NidDistH->Clear();
    NidDistH.Add(StartNid,sizeof(int),0);
    Queue.push(StartNid);
    int v,MaxDist=0;
    int TempDist;
    while(!Queue.empty()){
        const int Nid=Queue.front();Queue.pop();
        NidDistH.Find(Nid,sizeof(int),TempDist);
        if(TempDist==MxDist){break;}
        PGraph::NodeIter NodeI=Graph->GetNI(Nid);
        if(FollowOut){
            for(v=0;v<NodeI.GetOutdeg();++v){
                const int DstNid=NodeI.GetOutNid(v);
                if(!NidDistH->IsIn(DstNid,sizeof(int))){
                    NidDistH->Add(DstNid,TempDist+1);
                    if(DstNid==TargetNid){return TempDist+1;}
                    Queue.push(DstNid);
                }
            }
        }
        if(FollowIn){
            for(v=0;v<NodeI.GetInDeg();++v){
                const int DstNid=NodeI.GetInNid(v);
                if(!NidDistH->IsIn(DstNid,sizeof(int))){
                    NidDistH->Add(DstNid,TempDist+1);
                    if(DstNid==TargetNid){return TempDist;}
                    Queue.push(DstNid);
                }
            }
        }
        
    }
    return TempDist;
}

tempalate<PGraph>
int BreathFS<PGraph> GetHops(const int& SrcNid,const int& DstNid)const{
    int Dist;
    if(SrcNid!=StartNid) return -1;
    if(!NidDistH->Find(DstNid,sizeof(int),Dist)) return -1;
    return Dist;
}

template<PGraph> int GetShortPath(const PGraph* Graph,const int& SrcNid,hashTable<int,int>& NidDistH,const bool& IsDir=false,const int& MxDist=INT_MAX){
    Breath<PGraph> BFS(Graph,&NidDistNid);
    return BFS.DoBfs(SrcNid,true,!IsDir,-1,MxDist);
}
template<PGraph> int GetShortPath(const PGraph* Graph,const int& SrcNid,const int& DstNid,const bool& IsDir){
    HashTable<int,int> NidDistH(128,4);
    BreathFS<PGraph> BFS(Graph,&NidDistH);
    BFS.BoBfs(SrcNid,true,!IsDir,DstNid);
    return BFS.GetHops(SrcNid,DstNid);
}


#endif /* BFS_h */
