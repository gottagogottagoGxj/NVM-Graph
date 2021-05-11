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
#include<sstream>
#include"UNDirect_UNWeight_Graph.h"
#include"SubgroupMatch_Filter.h"
#include"SubgraphMatch_Query.h"
using namespace std;

void UNDirect_UNWeight_Test();
void split(const string& s, vector<string>& sv, const char flag = ' ');

void LoadSubgraphMatchHPRDData(UNDirect_UNWeight_Graph& graph,const char* Path);
void LoadSubgraphMatchYeastData(UNDirect_UNWeight_Graph& graph,const char* Path);

int main(){
    
    //Property_Graph_Test();
    //Label_Test();
    //UNDirect_UNWeight_Test();
    UNDirect_UNWeight_Test();
    return 0;
}


/// <#Description#>

void UNDirect_UNWeight_Test(){
    Arena dataarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024*16);
    Arena queryarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024);
    UNDirect_UNWeight_Graph DataGraph(&dataarena),QueryGraph(&queryarena);
    const char* querygraphpath="/Users/jiangqingyuejinren/Desktop/参考代码/子图匹配/SubgraphMatching-master/test/query_graph/query_dense_16_2.graph";
    const char* datagraphpath="/Users/jiangqingyuejinren/Desktop/NVM-Graph/NVM-Graph/HPRD.graph";
    LoadSubgraphMatchHPRDData(DataGraph,datagraphpath);
    LoadSubgraphMatchHPRDData(QueryGraph,querygraphpath);
  
    Arena matchtable(0,1024*1024*1024);
    
    SubgraphMatch_Graph DataMatchGraph(&DataGraph, &matchtable, &matchtable);
    
    SubgraphMatch_Graph QueryMatchGraph(&QueryGraph,&matchtable,&matchtable);
    
    QueryMatchGraph.BuildCoreTable();
    
    /*int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    SubgraphMatch_Filter::CFLFilter(DataMatchGraph, QueryMatchGraph, &matchtable, candidate, candidate_count, order, tree);
    Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[QueryGraph.GetNodeNum()];
    for(int i=0;i<QueryGraph.GetNodeNum();++i) edge_matrix[i]=new Edges*[QueryGraph.GetNodeNum()];
    for(int i=0;i<QueryGraph.GetNodeNum();++i){
        for(int j=0;j<QueryGraph.GetNodeNum();++j){
            edge_matrix[i][j]=NULL;
        }
    }
    Arena gxj(1,1024*1024*32);
    SubgraphMatch_GraphOperations::BuildTables(*DataMatchGraph.GetGraph(), *QueryMatchGraph.GetGraph(),candidate, candidate_count, edge_matrix,&gxj);
    
    int* query_order;
    int* query_pivot;
    
    SubgraphMatch_Query::GenerateCFLQueryPlan(DataMatchGraph, QueryMatchGraph, edge_matrix, query_order, query_pivot, tree, order, candidate_count);
    
    size_t embedding_count;
    size_t call_count=0;
    embedding_count=SubgraphMatch_Query::ExecuteQuery(DataGraph, QueryGraph, edge_matrix, candidate, candidate_count, query_order, query_pivot, 1000, call_count);
    cout<<call_count<<endl;
    cout<<embedding_count<<endl;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    delete[] query_pivot;
    
    if(edge_matrix!=NULL){
        for(int i=0;i<QueryGraph.GetNodeNum();++i){
            delete[] edge_matrix[i];
        }
        delete [] edge_matrix;
    }*/
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    SubgraphMatch_Filter::DPisoFilter(DataMatchGraph, QueryMatchGraph, &matchtable, candidate, candidate_count, order, tree);
    Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[QueryGraph.GetNodeNum()];
    for(int i=0;i<QueryGraph.GetNodeNum();++i) edge_matrix[i]=new Edges*[QueryGraph.GetNodeNum()];
    
    Arena gxj(1,1024*1024*32);
    SubgraphMatch_GraphOperations::BuildTables(*DataMatchGraph.GetGraph(), *QueryMatchGraph.GetGraph(),candidate, candidate_count, edge_matrix,&gxj);
    
    int* query_order;
    int* query_pivot;
    
    SubgraphMatch_Query::GenerateCFLQueryPlan(DataMatchGraph, QueryMatchGraph, edge_matrix, query_order, query_pivot, tree, order, candidate_count);
    
    size_t embedding_count;
    size_t call_count=0;
    embedding_count=SubgraphMatch_Query::ExecuteQuery(DataGraph, QueryGraph, edge_matrix, candidate, candidate_count, query_order, query_pivot, 1000, call_count);
    cout<<call_count<<endl;
    cout<<embedding_count<<endl;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    delete[] query_pivot;
    
    if(edge_matrix!=NULL){
        for(int i=0;i<QueryGraph.GetNodeNum();++i){
            delete[] edge_matrix[i];
        }
        delete [] edge_matrix;
    }
    
    
    
    
}

void LoadSubgraphMatchHPRDData(UNDirect_UNWeight_Graph& graph,const char* Path){
    ifstream datafile(Path);
    if(datafile.fail()){
        cout<<Path<<" cannot open"<<endl;
        return ;
    }
    char type;
    int Vertex_Count,Edge_Count;
    datafile>>type>>Vertex_Count>>Edge_Count;
    while(datafile>>type){
        if(type=='v'){
            int id,label,degree;
            datafile>>id>>label>>degree;
            graph.AddNode(id, &label, 1);
        }
        else if(type=='e'){
            int SrcNid,DstNid;
            datafile>>SrcNid>>DstNid;
            graph.AddEdge(SrcNid, DstNid);
        }
    }
    datafile.close();
}

void LoadSubgraphMatchYeastData(UNDirect_UNWeight_Graph& graph,const char* Path){
    ifstream datafile(Path);
    if(!datafile.is_open()){cout<<"datafail open fail"<<endl;return;}
    string buf;
    while(getline(datafile,buf)){
        vector<string> data;
        split(buf, data);
        if(data[0]=="t") continue;
        if(data[0]=="v"){
            int nid=stoi(data[1]);
            const int labelsize=data.size()-2;
            int label[labelsize];
            for(int i=0;i<labelsize;++i){
                label[i]=stoi(data[i+2]);
            }
            graph.AddNode(nid, label, labelsize);
        }
        else if(data[0]=="e"){
            int srcnid=stoi(data[1]);
            int dstnid=stoi(data[2]);
            graph.AddEdge(srcnid, dstnid);
        }
        
    }
}

void split(const string& s, vector<string>& sv, const char flag) {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(temp);
    }
    return;
}


