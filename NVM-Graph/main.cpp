//
//  main.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/10/21.
//

#include <iostream>
#include<string>
#include<vector>
#include<set>
#include<queue>
#include<bitset>
#include "UNDerict_Graph.h"
using namespace std;


int main(){
    size_t nodesize=UNDerict_Graph::GetNodeSize();
    size_t alloc=nodesize*50;
    Arena arena(nodesize,alloc);
    UNDerict_Graph graph(&arena);
    for(int i=1;i<20;++i){
        graph.AddNode(i);
    }
    for(int i=1;i<20;++i){
        for(int j=1;j<20;++j){
            graph.AddEdge(i, j);
        }
    }
    UNDerict_Graph::EdgeI eiter=graph.BegEI();
    while(!eiter.IsEnd()){cout<<'('<<eiter.GetSrcNid()<<','<<eiter.GetDstNid()<<')'<<" "; eiter++;}
    cout<<endl;
    for(int i=1;i<20;++i) {graph.DelEdge(1, i);graph.DelEdge(2, i);}
    eiter=graph.BegEI();
    while(!eiter.IsEnd()){cout<<'('<<eiter.GetSrcNid()<<','<<eiter.GetDstNid()<<')'<<" "; eiter++;}
    cout<<endl;
    
    
    return 0;
}
