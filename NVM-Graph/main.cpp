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
#include"network.h"
#include"Property_Graph.h"
using namespace std;

void Property_Graph_Test();

int main(){
    
    Property_Graph_Test();
    return 0;
}

void Property_Graph_Test(){
    int NodeSize=Property_Graph::GetNodeSize();
    Arena NodeTable(NodeSize,NodeSize*10);
    Arena AttrNameNTable(0,1024),AttrIndexNTable(0,1024),AttrValueTable(0,1024);
    Arena AttrNameETable(0,1024),AttrIndexETable(0,1024);
    Property_Graph Graph(&NodeTable,&AttrNameNTable,&AttrIndexNTable,&AttrValueTable,&AttrNameETable,&AttrIndexETable,&AttrValueTable);
    for(int i=0;i<7;++i) Graph.AddNode();
    auto iter=Graph.BegNI();
    while(!iter.IsEnd()){
        cout<<iter.GetId()<<":"<<iter.GetData()<<endl;
        cout<<"InDeg:"<<iter.GetInDeg()<<","<<"OutDeg:"<<iter.GetOutDeg()<<endl;
        for(int i=0;i<iter.GetInDeg();++i) cout<<iter.GetInNid(i)<<" ";
        cout<<endl;
        for(int i=0;i<iter.GetOutDeg();++i) cout<<iter.GetOutNid(i)<<" ";
        cout<<endl;
        
    }
    
}
