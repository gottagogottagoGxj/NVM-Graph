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
    size_t alloc=nodesize*20;
    Arena arena(nodesize,alloc);
    UNDerict_Graph graph(&arena);
    for(int i=1;i<20;++i){
        graph.AddNode(i);
    }
    UNDerict_Graph::NvmNodeI iter=graph.HeadNI();
    iter++;
    while(!iter.IsEnd()){cout<<iter.GetId()<<endl;iter++;}
    
    
    
    
    return 0;
}
