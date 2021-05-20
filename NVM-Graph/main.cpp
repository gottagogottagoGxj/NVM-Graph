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
#include<chrono>
#include"UNDirect_UNWeight_Graph.h"
#include"SubgroupMatch_Filter.h"
#include"SubgraphMatch_Query.h"
#include"CHL_UNWeight.h"
using namespace std;

void UNDirect_UNWeight_Test();
void split(const string& s, vector<string>& sv, const char flag = ' ');

void LoadSubgraphMatchHPRDData(UNDirect_UNWeight_Graph& graph,const char* Path);
void LoadSubgraphMatchYeastData(UNDirect_UNWeight_Graph& graph,const char* Path);
void CFLDPSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result);

void CFLSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result);
void DPisoSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result);
void CRDPSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result);


int main(){
    
    Arena dataarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024);
    UNDirect_UNWeight_Graph Graph(&dataarena);
  
    Arena Label(0,1024*1024*16);
    
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
    
    CHL chl(&Graph,&Label);
    chl.ConstructIndex();
    cout<<chl.Query(1, 8)<<endl;
    
   
    return 0;
}


/// <#Description#>

void UNDirect_UNWeight_Test(){
    Arena dataarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024*16);
    UNDirect_UNWeight_Graph DataGraph(&dataarena);
    const char* datagraphpath="/Users/jiangqingyuejinren/Desktop/NVM-Graph/NVM-Graph/HPRD.graph";
    LoadSubgraphMatchHPRDData(DataGraph,datagraphpath);
    Arena datamatchtable(1,1024*1024*32);//存数据图的LDF NLF
    SubgraphMatch_Graph DataMatchGraph(&DataGraph, &datamatchtable, &datamatchtable);
    
    std::string path_prefix="/Users/jiangqingyuejinren/Desktop/参考代码/子图匹配/SubgraphMatching-master/test/query_graph/query_dense_16_";
    ofstream Result("/Users/jiangqingyuejinren/Desktop/NVM-Graph/NVM-Graph/Result.txt",ios::app);
    if(Result.fail()){cout<<"Result.txt cannot open"<<endl;}
    const int querygraphnum=40;
    double CFLResult[querygraphnum][9];
    double DPisoResult[querygraphnum][9];
    double CFLDPResult[querygraphnum][9];
    for(int i=1;i<querygraphnum;i++){
        std::ostringstream path_postfix;
        path_postfix<<i;
        std::string querygraphpath=path_prefix+path_postfix.str()+".graph";
        Arena queryarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*128);
        UNDirect_UNWeight_Graph QueryGraph(&queryarena);
        LoadSubgraphMatchHPRDData(QueryGraph, querygraphpath.c_str());
        Arena querymatchtable(1,1024*1024);
        SubgraphMatch_Graph QueryMatchGraph(&QueryGraph,&querymatchtable,&querymatchtable);
        auto start=std::chrono::high_resolution_clock::now();
        CFLSubgraphMatch(DataMatchGraph, QueryMatchGraph, CFLResult[i]);
        auto end=std::chrono::high_resolution_clock::now();
        CFLResult[i][8]=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        start=std::chrono::high_resolution_clock::now();
        DPisoSubgraphMatch(DataMatchGraph, QueryMatchGraph, DPisoResult[i]);
        end=std::chrono::high_resolution_clock::now();
        DPisoResult[i][8]=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        start=std::chrono::high_resolution_clock::now();
        CRDPSubgraphMatch(DataMatchGraph, QueryMatchGraph, CFLDPResult[i]);
        end=std::chrono::high_resolution_clock::now();
        CFLDPResult[i][8]=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
        
    }
    
    for(int i=1;i<querygraphnum;++i){
        Result<<"CFL :";
        for(int j=0;j<9;++j){
            Result<<CFLResult[i][j]<<" ";
        }
        Result<<endl;
        Result<<"DP  :";
        for(int j=0;j<9;++j){
            Result<<DPisoResult[i][j]<<" ";
        }
        Result<<endl;
        Result<<"CRDP:";
        for(int j=0;j<9;++j){
            Result<<CFLDPResult[i][j]<<" ";
        }
        Result<<endl;
    }
    
   
    
}

void CFLSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result){
    query_graph.BuildCoreTable();
    Arena matchtable(1,1024*1024*512);// 存匹配过程中的candidate Edges
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    
    auto start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Filter::CFLFilter(data_graph, query_graph, &matchtable, candidate, candidate_count, order, tree);
    auto end=std::chrono::high_resolution_clock::now();
    double filter_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[0]=filter_time_ns;
    int candidate_sum=0;
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) candidate_sum+=candidate_count[i];
    result[6]=candidate_sum;
    
    Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[query_graph.GetGraph()->GetNodeNum()];
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) edge_matrix[i]=new Edges*[query_graph.GetGraph()->GetNodeNum()];
    
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_GraphOperations::BuildTables(*data_graph.GetGraph(), *query_graph.GetGraph(),candidate, candidate_count, edge_matrix,&matchtable);
    end=std::chrono::high_resolution_clock::now();
    double bulidtable_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[1]=bulidtable_time_ns;
    
    
    int* query_order;
    int* query_pivot;
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Query::GenerateCFLQueryPlan(data_graph, query_graph, edge_matrix, query_order, query_pivot, tree, order, candidate_count);
    end=std::chrono::high_resolution_clock::now();
    double queryplan_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    
    result[2]=queryplan_time_ns;
    
    size_t embedding_count;
    size_t call_count=0;
    start=std::chrono::high_resolution_clock::now();
    embedding_count=SubgraphMatch_Query::ExploreGraph(*data_graph.GetGraph(), *query_graph.GetGraph(), edge_matrix, candidate, candidate_count, query_order, query_pivot, 1000, call_count);
    end=std::chrono::high_resolution_clock::now();
    double query_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[3]=query_time_ns;
    result[4]=call_count;
    result[5]=matchtable.GetCurAllocateSize();
    result[7]=queryplan_time_ns+query_time_ns;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    delete[] query_pivot;
    
    if(edge_matrix!=NULL){
        for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i){
            delete[] edge_matrix[i];
        }
        delete [] edge_matrix;
    }
}
void DPisoSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result){
    Arena matchtable(1,1024*1024*512);// 存匹配过程中的candidate Edges
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    
    auto start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Filter::DPisoFilter(data_graph, query_graph, &matchtable, candidate, candidate_count, order, tree);
    auto end=std::chrono::high_resolution_clock::now();
    double filter_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[0]=filter_time_ns;
    int candidate_sum=0;
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) candidate_sum+=candidate_count[i];
    result[6]=candidate_sum;
    
    Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[query_graph.GetGraph()->GetNodeNum()];
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) edge_matrix[i]=new Edges*[query_graph.GetGraph()->GetNodeNum()];
    
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_GraphOperations::BuildTables(*data_graph.GetGraph(), *query_graph.GetGraph(),candidate, candidate_count, edge_matrix,&matchtable);
    end=std::chrono::high_resolution_clock::now();
    double bulidtable_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[1]=bulidtable_time_ns;
    
    
    int* query_order;
    int* query_pivot;
    int** weight;
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Query::GenerateDSPisoQueryPlan(*query_graph.GetGraph(), edge_matrix, query_order, query_pivot, tree, order, candidate_count, weight);
    end=std::chrono::high_resolution_clock::now();
    double queryplan_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    
    result[2]=queryplan_time_ns;
    
    size_t embedding_count;
    size_t call_count=0;
    start=std::chrono::high_resolution_clock::now();
    embedding_count=SubgraphMatch_Query::ExploreDPisoStyle(*data_graph.GetGraph(), *query_graph.GetGraph(), tree, edge_matrix, candidate, candidate_count, weight, query_order, 1000, call_count);
    end=std::chrono::high_resolution_clock::now();
    double query_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[3]=query_time_ns;
    result[4]=call_count;
    result[5]=embedding_count;
    result[7]=queryplan_time_ns+query_time_ns;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    delete[] query_pivot;
    
    if(edge_matrix!=NULL){
        for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i){
            delete[] edge_matrix[i];
        }
        delete [] edge_matrix;
    }
    
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i){
        delete[] weight[i];
    }
    delete[] weight;
}

void CFLDPSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result){
    Arena matchtable(1,1024*1024*512);// 存匹配过程中的candidate Edges
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    
    auto start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Filter::CFLFilter(data_graph, query_graph, &matchtable, candidate, candidate_count, order, tree);
    auto end=std::chrono::high_resolution_clock::now();
    double filter_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[0]=filter_time_ns;
    int candidate_sum=0;
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) candidate_sum+=candidate_count[i];
    result[6]=candidate_sum;
    
    Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[query_graph.GetGraph()->GetNodeNum()];
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) edge_matrix[i]=new Edges*[query_graph.GetGraph()->GetNodeNum()];
    
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_GraphOperations::BuildTables(*data_graph.GetGraph(), *query_graph.GetGraph(),candidate, candidate_count, edge_matrix,&matchtable);
    end=std::chrono::high_resolution_clock::now();
    double bulidtable_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[1]=bulidtable_time_ns;
    
    
    int* query_order;
    int* query_pivot;
    int** weight;
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Query::GenerateCFLDPQueryPlan(*query_graph.GetGraph(), edge_matrix, query_order, query_pivot, tree, order, candidate_count, weight);
    end=std::chrono::high_resolution_clock::now();
    double queryplan_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    
    result[2]=queryplan_time_ns;
    
    size_t embedding_count;
    size_t call_count=0;
    start=std::chrono::high_resolution_clock::now();
    embedding_count=SubgraphMatch_Query::ExploreDPisoStyle(*data_graph.GetGraph(), *query_graph.GetGraph(), tree, edge_matrix, candidate, candidate_count, weight, query_order, 1000, call_count);
    end=std::chrono::high_resolution_clock::now();
    double query_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[3]=query_time_ns;
    result[4]=call_count;
    result[5]=matchtable.GetCurAllocateSize();
    result[7]=queryplan_time_ns+query_time_ns;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    delete[] query_pivot;
    
    if(edge_matrix!=NULL){
        for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i){
            delete[] edge_matrix[i];
        }
        delete [] edge_matrix;
    }
    
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i){
        delete[] weight[i];
    }
    delete[] weight;
}

void CRDPSubgraphMatch(const SubgraphMatch_Graph& data_graph,SubgraphMatch_Graph& query_graph,double* result){
    Arena matchtable(1,1024*1024*512);// 存匹配过程中的candidate Edges
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    
    auto start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Filter::CFLFilter(data_graph, query_graph, &matchtable, candidate, candidate_count, order, tree);
    auto end=std::chrono::high_resolution_clock::now();
    double filter_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[0]=filter_time_ns;
    int candidate_sum=0;
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) candidate_sum+=candidate_count[i];
    result[6]=candidate_sum;
    
    /*Edges ***edge_matrix=NULL;
    edge_matrix=new Edges**[query_graph.GetGraph()->GetNodeNum()];
    for(int i=0;i<query_graph.GetGraph()->GetNodeNum();++i) edge_matrix[i]=new Edges*[query_graph.GetGraph()->GetNodeNum()];
    
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_GraphOperations::BuildTables(*data_graph.GetGraph(), *query_graph.GetGraph(),candidate, candidate_count, edge_matrix,&matchtable);
    end=std::chrono::high_resolution_clock::now();
    double bulidtable_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[1]=bulidtable_time_ns;*/
    result[1]=0;
    
    
    int* query_order;
    start=std::chrono::high_resolution_clock::now();
    SubgraphMatch_Query::GenerateCRDPQueryPlan(*query_graph.GetGraph(), query_order, tree, order);
    end=std::chrono::high_resolution_clock::now();
    double queryplan_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    
    result[2]=queryplan_time_ns;
    
    size_t embedding_count;
    size_t call_count=0;
    start=std::chrono::high_resolution_clock::now();
    embedding_count=SubgraphMatch_Query::ExploreCRDPStyle(*data_graph.GetGraph(), *query_graph.GetGraph(), tree, candidate, candidate_count, query_order, 1000, call_count);
    end=std::chrono::high_resolution_clock::now();
    double query_time_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    result[3]=query_time_ns;
    result[4]=call_count;
    result[5]=matchtable.GetCurAllocateSize();
    result[7]=queryplan_time_ns+query_time_ns;
    
    delete[] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    delete[] query_order;
    
   
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


