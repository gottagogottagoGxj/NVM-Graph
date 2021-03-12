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

#include"Dijkstra.h"
#include "Hub_Label.h"
using namespace std;

void Property_Graph_Test();
void Label_Test();

int main(){
    
    //Property_Graph_Test();
    Label_Test();
    return 0;
}

void Property_Graph_Test(){
    int NodeSize=Property_Graph::GetNodeSize();
    Arena NodeTable(NodeSize,NodeSize*10);
    Arena AttrNameNTable(0,1024),AttrIndexNTable(0,1024),AttrValueTable(0,1024);
    Arena AttrNameETable(0,1024),AttrIndexETable(0,1024);
    Property_Graph Graph(&NodeTable,&AttrNameNTable,&AttrIndexNTable,&AttrValueTable,&AttrNameETable,&AttrIndexETable,&AttrValueTable);
    
    
    
    for(int i=0;i<7;++i) Graph.AddNode();
    
    
    Graph.AddEdge(1, 2);
    Graph.AddEdge(1, 4);
    Graph.AddEdge(2, 4);
    Graph.AddEdge(2, 5);
    Graph.AddEdge(3, 1);
    Graph.AddEdge(3, 6);
    Graph.AddEdge(4, 3);
    Graph.AddEdge(4, 5);
    Graph.AddEdge(4, 6);
    Graph.AddEdge(4, 7);
    Graph.AddEdge(5, 7);
    Graph.AddEdge(7, 6);
   
    
    Graph.SetNData(1, "v1");
    Graph.SetNData(2, "v2");
    Graph.SetNData(3, "v3");
    Graph.SetNData(4, "v4");
    Graph.SetNData(5, "v5");
    Graph.SetNData(6, "v6");
    Graph.SetNData(7, "v7");
    
    int AttrNId=Graph.AddAttrNameN("地名");
    int AttrEId=Graph.AddAttrNameE("距离");
    Graph.AddAttrDatN(1, AttrNId, "A",1);
    Graph.AddAttrDatN(2, AttrNId, "B", 1);
    Graph.AddAttrDatN(3, AttrNId, "C", 1);
    Graph.AddAttrDatN(4, AttrNId, "D", 1);
    Graph.AddAttrDatN(5, AttrNId, "E", 1);
    Graph.AddAttrDatN(6, AttrNId, "F", 1);
    Graph.AddAttrDatN(7, AttrNId, "G", 1);
    
    
    int a=2;
    Graph.AddAttrDatE(1, 2, AttrEId, &a,sizeof(int));
    a=1;
    Graph.AddAttrDatE(1, 4, AttrEId, &a,sizeof(int));
    a=3;
    Graph.AddAttrDatE(2, 4, AttrEId, &a,sizeof(int));
    a=10;
    Graph.AddAttrDatE(2, 5, AttrEId, &a,sizeof(int));
    a=4;
    Graph.AddAttrDatE(3, 1, AttrEId, &a,sizeof(int));
    a=5;
    Graph.AddAttrDatE(3, 6, AttrEId, &a,sizeof(int));
    a=2;
    Graph.AddAttrDatE(4, 3, AttrEId, &a,sizeof(int));
    a=2;
    Graph.AddAttrDatE(4, 5, AttrEId, &a,sizeof(int));
    a=8;
    Graph.AddAttrDatE(4, 6, AttrEId, &a,sizeof(int));
    a=4;
    Graph.AddAttrDatE(4, 7, AttrEId, &a,sizeof(int));
    a=6;
    Graph.AddAttrDatE(5, 7, AttrEId, &a,sizeof(int));
    a=1;
    Graph.AddAttrDatE(7, 6, AttrEId, &a,sizeof(int));
    auto iter=Graph.BegNI();
    while(!iter.IsEnd()){
        cout<<iter.GetId()<<":"<<iter.GetData()<<endl;
        cout<<"InDeg:"<<iter.GetInDeg()<<","<<"OutDeg:"<<iter.GetOutDeg()<<endl;
        for(int i=0;i<iter.GetInDeg();++i) {
            int SrcNid=iter.GetInNid(i);
            int b=0;
            Graph.GetAttrDatE(SrcNid, iter.GetId(), AttrEId, &b);
            cout<<SrcNid<<"->"<<iter.GetId()<<":"<<b<<endl;
            
        }
        cout<<endl;
        //for(int i=1;i<=iter.GetOutDeg();++i) cout<<iter.GetOutNid(i)<<" ";
        cout<<endl;
        iter++;
    }
    
    
    
    Dijkstra<Property_Graph> Dij(&Graph);
    Dij.DoDijkstra(1,-1,AttrEId);
    vector<int> path;
    Dij.GetShortestPath(1,3,path);
    
    
    
    
    
    
    
    
}


void Label_Test(){
    int NodeSize=Property_Graph::GetNodeSize();
    Arena NodeTable(NodeSize,NodeSize*10);
    Arena AttrNameNTable(0,1024),AttrIndexNTable(0,1024),AttrValueTable(0,1024);
    Arena AttrNameETable(0,1024),AttrIndexETable(0,1024);
    Property_Graph Graph(&NodeTable,&AttrNameNTable,&AttrIndexNTable,&AttrValueTable,&AttrNameETable,&AttrIndexETable,&AttrValueTable);
    
    
    
    for(int i=0;i<7;++i) Graph.AddNode();
    
    
    Graph.AddEdge(1, 2);
    Graph.AddEdge(1, 4);
    Graph.AddEdge(2, 4);
    Graph.AddEdge(2, 5);
    Graph.AddEdge(3, 1);
    Graph.AddEdge(3, 6);
    Graph.AddEdge(4, 3);
    Graph.AddEdge(4, 5);
    Graph.AddEdge(4, 6);
    Graph.AddEdge(4, 7);
    Graph.AddEdge(5, 7);
    Graph.AddEdge(7, 6);
    
    int AttrNId=Graph.AddAttrNameN("地名");
    int AttrEId=Graph.AddAttrNameE("距离");
    Graph.AddAttrDatN(1, AttrNId, "A",1);
    Graph.AddAttrDatN(2, AttrNId, "B", 1);
    Graph.AddAttrDatN(3, AttrNId, "C", 1);
    Graph.AddAttrDatN(4, AttrNId, "D", 1);
    Graph.AddAttrDatN(5, AttrNId, "E", 1);
    Graph.AddAttrDatN(6, AttrNId, "F", 1);
    Graph.AddAttrDatN(7, AttrNId, "G", 1);
    
    
    int a=2;
    Graph.AddAttrDatE(1, 2, AttrEId, &a,sizeof(int));
    a=1;
    Graph.AddAttrDatE(1, 4, AttrEId, &a,sizeof(int));
    a=3;
    Graph.AddAttrDatE(2, 4, AttrEId, &a,sizeof(int));
    a=10;
    Graph.AddAttrDatE(2, 5, AttrEId, &a,sizeof(int));
    a=4;
    Graph.AddAttrDatE(3, 1, AttrEId, &a,sizeof(int));
    a=5;
    Graph.AddAttrDatE(3, 6, AttrEId, &a,sizeof(int));
    a=2;
    Graph.AddAttrDatE(4, 3, AttrEId, &a,sizeof(int));
    a=2;
    Graph.AddAttrDatE(4, 5, AttrEId, &a,sizeof(int));
    a=8;
    Graph.AddAttrDatE(4, 6, AttrEId, &a,sizeof(int));
    a=4;
    Graph.AddAttrDatE(4, 7, AttrEId, &a,sizeof(int));
    a=6;
    Graph.AddAttrDatE(5, 7, AttrEId, &a,sizeof(int));
    a=1;
    Graph.AddAttrDatE(7, 6, AttrEId, &a,sizeof(int));
    
    HubLabel<Property_Graph> label(&Graph,AttrEId);
    label.ConstructIndex();
    cout<<label.Query(1, 3);
    
    
}
