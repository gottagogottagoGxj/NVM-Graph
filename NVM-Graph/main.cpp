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
#include"UNDrict_Graph.h"
using namespace std;


int main(){
    UNDerict_Graph graph;
    graph.AddNode(1);
    graph.AddNode(2, "abcd");
    graph.AddNode(3, "1234");
    cout<<graph.GetNode(1)<<endl;
    cout<<graph.GetNodeData(2)<<endl;
    cout<<graph.GetNodeData(3)<<endl;
    return 0;
}
