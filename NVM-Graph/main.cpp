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
    Arena dataarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024);
    Arena queryarena(UNDirect_UNWeight_Graph::GetNodeSize(),1024*1024);
    UNDirect_UNWeight_Graph DataGraph(&dataarena),QueryGraph(&queryarena);
    const char* querygraphpath="/Users/jiangqingyuejinren/Desktop/参考代码/子图匹配/SubgraphMatching-master/test/sample_dataset/query2_positive.graph";
    const char* datagraphpath="/Users/jiangqingyuejinren/Desktop/参考代码/子图匹配/SubgraphMatching-master/test/sample_dataset/test_case_1.graph";
    LoadSubgraphMatchHPRDData(DataGraph,datagraphpath);
    LoadSubgraphMatchHPRDData(QueryGraph,querygraphpath);
  
    Arena matchtable(0,1024*1024);
    
    SubgraphMatch_Graph DataMatchGraph(&DataGraph, &matchtable, &matchtable);
    
    SubgraphMatch_Graph QueryMatchGraph(&QueryGraph,&matchtable,&matchtable);
    
    QueryMatchGraph.BuildCoreTable();
    
    int ** candidate;
    int* candidate_count;
    int* order;
    TreeNode* tree;
    SubgraphMatch_Filter::CFLFilter(DataMatchGraph, QueryMatchGraph, &matchtable, candidate, candidate_count, order, tree);
    
    for(int i=0;i<QueryGraph.GetNodeNum();++i){
        cout<<candidate_count[i]<<":";
        for(int j=0;j<candidate_count[i];++j){
            cout<<candidate[i][j]<<" ";
        }
        cout<<endl;
    }
    
    delete [] candidate;
    delete[] candidate_count;
    delete[] order;
    delete[] tree;
    
    
    
    
   
}

void LoadSubgraphMatchHPRDData(UNDirect_UNWeight_Graph& graph,const char* Path){
    ifstream datafile(Path);
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


