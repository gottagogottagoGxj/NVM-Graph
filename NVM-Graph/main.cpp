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
#include<algorithm>
#include<fstream>
#include<time.h>
#include "Property_Graph.h"
#include "Hub_Label.h"
#include "Dijkstra.h"
#include"CHL_UNWeight.h"
#include"UNDirect_UNWeight_Graph.h"
#include"UNDirect_Weight_Graph.h"
#include"CHL_Weight.h"
using namespace std;

void Property_Graph_Test();
void Label_Test();
void UNDirect_UNWeight_Test();
void UNDirect_Weight_Test();
void LoadData(UNDirect_UNWeight_Graph& graph);

int main(){
    
    //Property_Graph_Test();
    //Label_Test();
    //UNDirect_UNWeight_Test();
    UNDirect_Weight_Test();
    return 0;
}


/// <#Description#>
void UNDirect_Weight_Test(){
    Arena Node(UNDirect_Weight_Graph::GetNodeSize(),1024*1024);
    Arena Name(0,1024),Index(0,1024*1024),Value(0,1024*1024);
    UNDirect_Weight_Graph Graph(&Node,&Name,&Index,&Value);
    const int WeightId=Graph.AddAttrNameE("EdgeWeight");
    Graph.AddEdge2(1, 2, WeightId, 2);
    Graph.AddEdge2(2, 3, WeightId, 3);
    Graph.AddEdge2(1, 3, WeightId, 8);
    Graph.AddEdge2(3, 4, WeightId, 5);
    Graph.AddEdge2(1, 4, WeightId, 5);
    Graph.AddEdge2(4, 5, WeightId, 4);
    Graph.AddEdge2(1, 5, WeightId, 7);
    Graph.AddEdge2(5, 6, WeightId, 2);
    Graph.AddEdge2(1, 6, WeightId, 1);
    /*auto iter=Graph.BegNI();
    while (!iter.IsEnd()) {
        cout<<iter.GetId()<<"Deg:"<<iter.GetDeg()<<endl;
        for(int i=0;i<iter.GetDeg();++i){
            int nid=iter.GetNbrNid(i);
            int value;
            Graph.GetAttrDatE(iter.GetId(), nid, WeightId, &value);
            cout<<"<"<<iter.GetId()<<","<<nid<<">:"<<value<<endl;
            int value1;
            Graph.GetAttrDatE(nid, iter.GetId(), WeightId, &value1);
            cout<<"<"<<nid<<","<<iter.GetId()<<">:"<<value1<<endl;
        }
        iter++;
    }*/
    CHL_Weight<UNDirect_Weight_Graph> chl(&Graph,WeightId);
    chl.ConstructIndexMin();
    /*for(int i=1;i<6;++i){
        for(int j=i+1;j<=6;++j){
            cout<<i<<"and"<<j<<"diatance is:"<<chl.Query(i, j)<<endl;
        }
    }*/
    Arena label(0,1024*1024);
    chl.SaveLabel(&label);
    for(int i=1;i<6;++i){
        for(int j=i+1;j<=6;++j){
            cout<<i<<"and"<<j<<"diatance is:"<<chl.QueryMin(i, j)<<endl;
        }
    }
}


void UNDirect_UNWeight_Test(){
    Arena arena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024*16);
    UNDirect_UNWeight_Graph Graph(&arena);
    //LoadData(Graph);
    Graph.AddEdge2(1, 2);
    Graph.AddEdge2(2, 3);
    Graph.AddEdge2(3, 4);
    Graph.AddEdge2(2, 4);
    Graph.AddEdge2(4, 5);
    Graph.AddEdge2(5, 6);
    Graph.AddEdge2(6, 7);
    Graph.AddEdge2(7, 8);
    Graph.AddEdge2(8, 9);
    Graph.AddEdge2(5, 8);
    
    CHL<UNDirect_UNWeight_Graph> chl(&Graph);
    clock_t start,finish;
    start=clock();
    chl.ConstructIndexBetweenMin();
    finish=clock();
    //cout<<"BCHL时间"<<finish-start<<endl;
    cout<<"BCHL标签"<<chl.GetLabelSize()<<endl;
    start=clock();
    for(int i=1;i<8;++i){
        for(int j=i+1;j<=8;++j){
            cout<<chl.QueryBetweenMin(i, j)<<" ";
        }
    }
    cout<<endl;
    finish=clock();
    cout<<"BCHL查询时间："<<finish-start<<endl;
    
    CHL<UNDirect_UNWeight_Graph> chll(&Graph);
    start=clock();
    chll.ConstructIndex();
    finish=clock();
    //cout<<"CHL时间"<<finish-start<<endl;
    cout<<"CHL标签"<<chll.GetLabelSize()<<endl;
    start=clock();
    for(int i=1;i<8;++i){
        for(int j=i+1;j<=8;++j){
            cout<<chll.Query(i, j)<<" ";
        }
    }
    cout<<endl;
    finish=clock();
    cout<<"CHL查询时间："<<finish-start<<endl;
    cout<<arena.EndPtr()-arena.BeginPtr()<<endl;
    cout<<Graph.GetNodeNum()<<" "<<Graph.GetEdgeNum()<<endl;
    
    
}
void LoadData(UNDirect_UNWeight_Graph& graph){
    ifstream datafile("/Users/jiangqingyuejinren/Desktop/data.txt");
    while(!datafile.eof()){
        int SrcNid,DstNid;
        datafile>>SrcNid>>DstNid;
        if(SrcNid==0 || DstNid==0) continue;
        graph.AddEdge2(SrcNid, DstNid);}
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

