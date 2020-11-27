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
    Arena arena(UNDerict_Graph::GetNodeSize());
    UNDerict_Graph graph(&arena);
    graph.AddNode(1);
    graph.AddNode(2, "abcd");
    graph.AddNode(3, "1234");
    
    return 0;
}
