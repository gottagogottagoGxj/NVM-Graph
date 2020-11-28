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
#include "UNDerict_Graph.cpp"
using namespace std;


int main(){
    Arena arena(UNDerict_Graph::GetNodeSize());
    UNDerict_Graph graph(&arena);
    graph.AddNode(1);
    graph.AddNode(2, "abcdefdghijklmnopqrstuvwxyzabcdefghijklmnopqresidhfgdbsjs");
    graph.AddNode(3, "1234");
    cout<<graph.GetNodeNum()<<endl;
    //graph.DelNode(1);
    UNDerict_Graph::NvmNodeI iter=graph.HeadNI();
    iter++;
    while(!iter.IsEnd()){
        cout<<iter.GetId()<<endl;
        iter++;
    }
    graph.DelNode(1);
    graph.AddNode(4,"gxj");
    iter=graph.HeadNI();
    iter++;
    while(!iter.IsEnd()){
        cout<<iter.GetId()<<endl;
        iter++;
    }
   
    graph.AddExistNode(2);
    graph.AddNode(5);
    iter=graph.HeadNI();
    iter++;
    while(!iter.IsEnd()){
        cout<<iter.GetData()<<endl;
        iter++;
    }
    
    return 0;
}
