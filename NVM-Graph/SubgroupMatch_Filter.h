//
//  SubgroupMatch_Filter.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/4/28.
//

#ifndef SubgroupMatch_Filter_h
#define SubgroupMatch_Filter_h

#include<vector>
#include"SubgraphMatch_Graph.h"


class SubgraphMatch_Filter{
public:
    static void ComputeCandidateWithLDF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer=NULL);
    static void ComputeCandidateWithNLF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer=NULL);
    
    static void GenerateCFLFilterPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, TreeNode *&tree,int *&order, int &level_count, int *&level_offset);
    
private:
    static void AllocateBuffer(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph,Arena* table, int* candidate_count,int** candidate);
    static int SelectCFLFilterStartVertex(const SubgraphMatch_Graph &data_graph, const SubgraphMatch_Graph& query_graph);
    
};

void SubgraphMatch_Filter::ComputeCandidateWithLDF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer){
    auto QueryVertexIter=query_graph.GetGraph()->GetNI(query_vertex);
    const int* QueryVertexLabelArray=QueryVertexIter.GetData();
    const int QueryVertexLabel=QueryVertexLabelArray[0];
    int QueryVertexDegree=QueryVertexIter.GetDeg();
    count=0;
    int data_vertex_num;
    const int* data_vertex=data_graph.GetVertexsByLabel(QueryVertexLabel, data_vertex_num);
    if(buffer==NULL){
        for(int i=0;i<data_vertex_num;++i){
            int datavertex=data_vertex[i];
            if(data_graph.GetGraph()->GetNI(datavertex).GetDeg()>=QueryVertexDegree)
                count+=1;
        }
    }else{
        for(int i=0;i<data_vertex_num;++i){
            int datavertex=data_vertex[i];
            if(data_graph.GetGraph()->GetNI(datavertex).GetDeg()>=QueryVertexDegree){
                buffer[count++]=datavertex;
            }
        }
    }
}

void SubgraphMatch_Filter:: ComputeCandidateWithNLF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer){
    auto QueryVertexIter=query_graph.GetGraph()->GetNI(query_vertex);
    const int* QueryVertexLabelArray=QueryVertexIter.GetData();
    int QueryVertexLabel=QueryVertexLabelArray[0];
    int QueryVertexDegree=QueryVertexIter.GetDeg();
    count=0;
    int data_vertex_num;
    const int* data_vertex=data_graph.GetVertexsByLabel(QueryVertexLabel, data_vertex_num);
    for(int i=0;i<data_vertex_num;++i){
        int datavertex=data_vertex[i];
        if(data_graph.GetGraph()->GetNI(datavertex).GetDeg()>=QueryVertexDegree){
            int queryvertexlabelnum,datavertexlablenum;
            const int* querynlf=query_graph.GetVertexNLF(query_vertex,queryvertexlabelnum);
            const int* datanlf=data_graph.GetVertexNLF(datavertex, datavertexlablenum);
            if(datavertexlablenum>=queryvertexlabelnum){
                int i=0,j=0;
                bool flag=true;
                while(i<queryvertexlabelnum && j<datavertexlablenum){
                    if(querynlf[i]<datanlf[j]){flag=false;break;}
                    if(querynlf[i]>datanlf[j]) j++;
                    else{
                        if(querynlf[i+queryvertexlabelnum]>datanlf[j+datavertexlablenum]){
                            flag=false;
                            break;
                        }
                        else{
                            i++;
                            j++;
                        }
                    }
                }
                if(flag){
                    if(buffer!=NULL) buffer[count]=datavertex;
                    count++;
                }
            }
        }
        }
}

void SubgraphMatch_Filter::AllocateBuffer(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph,Arena* table, int* candidate_count,int** candidate){
    int query_vertex_num=query_graph.GetGraph()->GetNodeNum();
    int candidate_num=data_graph.GetMaxLabelFrequency();
    candidate_count=new int[query_vertex_num];
    memset(candidate_count, 0, sizeof(int)*query_vertex_num);
    for(int i=0;i<query_vertex_num;++i){
        candidate[i]=(int*)table->AllocateBytes(sizeof(int)*candidate_num);
    }
}


void SubgraphMatch_Filter::GenerateCFLFilterPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, TreeNode *&tree,int *&order, int &level_count, int *&level_offset){
    int query_vertices_num = query_graph.GetGraph()->GetNodeNum();
    int start_vertex = SelectCFLFilterStartVertex(data_graph, query_graph);
    SubgraphMatch_GraphOperations::BfsTraversal(query_graph, start_vertex, tree, order);

    std::vector<int> order_index(query_vertices_num);
    for (int i = 0; i < query_vertices_num; ++i) {
        int query_vertex = order[i];
        order_index[query_vertex] = i;
    }

    level_count = -1;
    level_offset = new int[query_vertices_num + 1];

    for (int i = 0; i < query_vertices_num; ++i) {
        int u = order[i];
        tree[u].under_level_count_ = 0;
        tree[u].bn_count_ = 0;
        tree[u].fn_count_ = 0;

        if (tree[u].level_ != level_count) {
            level_count += 1;
            level_offset[level_count] = 0;
        }

        level_offset[level_count] += 1;
        
        auto nodeiter=query_graph.GetGraph()->GetNI(u);
        auto tempiter=nodeiter;
        
        do{
            tempiter==nodeiter;
            int u_nbrs_count;
            const int* u_nbrs = nodeiter.GetCurNbr(u_nbrs_count);
            for (int j = 0; j < u_nbrs_count; ++j) {
                int u_nbr = u_nbrs[j];

                if (tree[u].level_ == tree[u_nbr].level_) {
                    if (order_index[u_nbr] < order_index[u]) {
                        tree[u].bn_[tree[u].bn_count_++] = u_nbr;
                    }
                    else {
                    tree[u].fn_[tree[u].fn_count_++] = u_nbr;
                    }
                }
                else if (tree[u].level_ > tree[u_nbr].level_) {
                    tree[u].bn_[tree[u].bn_count_++] = u_nbr;
                }
                else {
                    tree[u].under_level_[tree[u].under_level_count_++] = u_nbr;
                }
            }
            nodeiter.ToNextNode();
        }while(!tempiter.IsNodeEnd());
    }

    level_count += 1;

    int prev_value = 0;
    for (int i = 1; i <= level_count; ++i) {
        int temp = level_offset[i];
        level_offset[i] = level_offset[i - 1] + prev_value;
        prev_value = temp;
    }
    level_offset[0] = 0;
}


int SubgraphMatch_Filter::SelectCFLFilterStartVertex(const SubgraphMatch_Graph &data_graph, const SubgraphMatch_Graph& query_graph) {
    auto rank_compare = [](std::pair<int, double> l, std::pair<int, double> r) {
        return l.second < r.second;
    };

    std::priority_queue<std::pair<int, double>, std::vector<std::pair<int, double>>, decltype(rank_compare)> rank_queue(rank_compare);

    // Compute the ranking.
    for (int i = 0; i < query_graph.GetGraph()->GetNodeNum(); ++i) {
        int query_vertex = i;

        if ( query_graph.Get2CoreSize()==0 || query_graph.GetCoreValue(query_vertex) > 1) {
            auto vertexiter=query_graph.GetGraph()->GetNI(query_vertex);
            const int* label = vertexiter.GetData();
            int degree = vertexiter.GetDeg();
            int frequency = data_graph.GetLabelFrequency(label[0]);
            double rank = frequency / (double) degree;
            rank_queue.push(std::make_pair(query_vertex, rank));
        }
    }

    // Keep the top-3.
    while (rank_queue.size() > 3) {
        rank_queue.pop();
    }

    int start_vertex = 0;
    double min_score = data_graph.GetMaxLabelFrequency() + 1;

    while (!rank_queue.empty()) {
        int query_vertex = rank_queue.top().first;
        int count;
        ComputeCandidateWithNLF(data_graph, query_graph, query_vertex, count);
        double cur_score = count / (double) query_graph.GetGraph()->GetNI(query_vertex).GetDeg();

        if (cur_score < min_score) {
            start_vertex = query_vertex;
            min_score = cur_score;
        }
        rank_queue.pop();
    }

    return start_vertex;
}




#endif /* SubgroupMatch_Filter_h */
