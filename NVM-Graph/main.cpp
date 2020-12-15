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
#include "Direct_graph.h"
#include"Direct_multigraph.h"
using namespace std;



int main(){
    Direct_multigraph::Node node(1,"1234");
    node.AddInEidSort(3);
    node.AddInEidSort(2);
    node.AddInEidSort(5);
    node.AddInEidSort(1);
    for(int i=0;i<node.GetInDeg();++i){cout<<node.GetInEid(i)<<" ";}
    cout<<endl;
    node.DeleteInEidSort(3);
    for(int i=0;i<node.GetInDeg();++i){cout<<node.GetInEid(i)<<" ";}
    cout<<endl;
    
    
    return 0;
}
