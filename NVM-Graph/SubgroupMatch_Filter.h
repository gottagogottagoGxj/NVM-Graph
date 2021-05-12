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

#define INVALID_VERTEX_ID -1

class SubgraphMatch_Filter{
public:
    static void ComputeCandidateWithLDF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer=NULL);
    static void ComputeCandidateWithNLF(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph, const int& query_vertex,int& count,int* buffer=NULL);
    static bool LDFFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph,Arena* candidatetable, int **&candidates, int *&candidates_count);
    static bool NLFFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Arena* candidatetable, int **&candidates, int *&candidates_count);
    
    static void GenerateCFLFilterPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, TreeNode *&tree,int *&order, int &level_count, int *&level_offset);
    
    static bool CFLFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph &query_graph, Arena* candidatetable, int **&candidates, int *&candidates_count,int *&order, TreeNode *&tree);
    
    static void GenerateDPisoFilterPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, TreeNode *&tree,int *&order);
    static bool DPisoFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Arena* candidatetable,int **&candidates, int *&candidates_count,int *&order, TreeNode *&tree);
    
private:
    static void AllocateBuffer(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph,Arena* table ,int** &candidate ,int* &candidate_count);
    static int SelectCFLFilterStartVertex(const SubgraphMatch_Graph &data_graph, const SubgraphMatch_Graph& query_graph);
    static int SelectDPisoStartVertex(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph);
    static void GenerateCandidates(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, int query_vertex,int *pivot_vertices, int pivot_vertices_count, int **candidates,int *candidates_count, int *flag, int *updated_flag);
    static void PruneCandidates(const SubgraphMatch_Graph & data_graph, const SubgraphMatch_Graph & query_graph, int query_vertex,int *pivot_vertices, int pivot_vertices_count, int **candidates,int *candidates_count, int *flag, int *updated_flag);
    static void CompactCandidates(int **&candidates, int *&candidates_count, int query_vertex_num);
    static bool IsCandidateSetValid(int **&candidates, int *&candidates_count, int query_vertex_num);
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

bool SubgraphMatch_Filter::LDFFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph,Arena* candidatetable, int **&candidates, int *&candidates_count) {
    AllocateBuffer(data_graph, query_graph,candidatetable, candidates, candidates_count);

    for (int i = 0; i < query_graph.GetGraph()->GetNodeNum(); ++i) {
        ComputeCandidateWithLDF(data_graph, query_graph, i, candidates_count[i],candidates[i]);

        if (candidates_count[i] == 0) {
            return false;
        }
    }

    return true;
}

bool SubgraphMatch_Filter::NLFFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Arena* candidatetable, int **&candidates, int *&candidates_count) {
    AllocateBuffer(data_graph, query_graph, candidatetable,candidates, candidates_count);

    for (int i = 0; i < query_graph.GetGraph()->GetNodeNum(); ++i) {
        ComputeCandidateWithNLF(data_graph, query_graph, i, candidates_count[i],candidates[i]);

        if (candidates_count[i] == 0) {
            return false;
        }
    }

    return true;
}


void SubgraphMatch_Filter::AllocateBuffer(const SubgraphMatch_Graph& data_graph,const SubgraphMatch_Graph& query_graph,Arena* table ,int** &candidate ,int* &candidate_count){
    int query_vertex_num=query_graph.GetGraph()->GetNodeNum();
    int candidate_num=data_graph.GetMaxLabelFrequency();
    candidate_count=new int[query_vertex_num];
    candidate=new int*[query_vertex_num];
    memset(candidate_count, 0, sizeof(int)*query_vertex_num);
    for(int i=0;i<query_vertex_num;++i){
        candidate[i]=(int*)(table->AllocateBytes(sizeof(int)*candidate_num));
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
            tempiter=nodeiter;
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


void SubgraphMatch_Filter::GenerateCandidates(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, int query_vertex,int *pivot_vertices, int pivot_vertices_count, int **candidates,int *candidates_count, int *flag, int *updated_flag) {
    auto query_vertex_iter=query_graph.GetGraph()->GetNI(query_vertex);
    const int* query_vertex_label = query_vertex_iter.GetData();
    int query_vertex_degree = query_vertex_iter.GetDeg();
    
    int query_vertex_nlflabelnum;
    const int* query_vertex_nlf=query_graph.GetVertexNLF(query_vertex, query_vertex_nlflabelnum);
    
    int count = 0;
    int updated_flag_count = 0;
    for (int i = 0; i < pivot_vertices_count; ++i) {
        int pivot_vertex = pivot_vertices[i];

        for (int j = 0; j < candidates_count[pivot_vertex]; ++j) {
            int v = candidates[pivot_vertex][j];

            if (v == INVALID_VERTEX_ID)
                continue;
            auto viter=data_graph.GetGraph()->GetNI(v);
            auto tempiter=viter;
            do{
                tempiter=viter;
                int v_nbrs_count;
                const int* v_nbrs = viter.GetCurNbr(v_nbrs_count);

                for (int k = 0; k < v_nbrs_count; ++k) {
                    int v_nbr = v_nbrs[k];
                    auto v_nbr_iter=data_graph.GetGraph()->GetNI(v_nbr);
                    const int* v_nbr_label = v_nbr_iter.GetData();
                    int v_nbr_degree = v_nbr_iter.GetDeg();
                    if(flag[v_nbr]==count && v_nbr_degree>=query_vertex_degree){
                        bool v_nbr_flag=false;
                        int l=0;
                        while(l<NodeDatalengthDef && v_nbr_label[l]!=-1){
                            if(v_nbr_label[l]==query_vertex_label[0]){
                                v_nbr_flag=true;
                                break;
                            }
                            l++;
                        }
                        if(v_nbr_flag){
                            flag[v_nbr]+=1;
                            
                            if(count==0){
                                updated_flag[updated_flag_count++]=v_nbr;
                            }
                        }
                    }
            }
                viter.ToNextNode();
            }while(!tempiter.IsNodeEnd());
        }

        count += 1;
    }

    for (int i = 0; i < updated_flag_count; ++i) {
        int v = updated_flag[i];
        if (flag[v] == count) {
            int v_nlflabelnum;
            const int* v_nlf=data_graph.GetVertexNLF(v, v_nlflabelnum);
            if(v_nlflabelnum>=query_vertex_nlflabelnum){
                int j=0,k=0;
                bool v_flag=true;
                while(j<query_vertex_nlflabelnum && k<v_nlflabelnum){
                    if(query_vertex_nlf[j]< v_nlf[k]){
                        v_flag=false;
                        break;
                    }
                    if(query_vertex_nlf[j]> v_nlf[k]) k++;
                    else{
                        if(query_vertex_nlf[j+query_vertex_nlflabelnum]>v_nlf[k+v_nlflabelnum]){
                            v_flag=false;
                            break;
                        }
                        j++;
                        k++;
                    }
                    
                }
                if(v_flag) candidates[query_vertex][candidates_count[query_vertex]++]=v;
            }
        }
    }

    for (int i = 0; i < updated_flag_count; ++i) {
        int v = updated_flag[i];
        flag[v] = 0;
    }
}

void SubgraphMatch_Filter::PruneCandidates(const SubgraphMatch_Graph & data_graph, const SubgraphMatch_Graph & query_graph, int query_vertex,int *pivot_vertices, int pivot_vertices_count, int **candidates,int *candidates_count, int *flag, int *updated_flag) {
    auto query_vertex_iter=query_graph.GetGraph()->GetNI(query_vertex);
    const int* query_vertex_label = query_vertex_iter.GetData();
    int query_vertex_degree = query_vertex_iter.GetDeg();

    int count = 0;
    int updated_flag_count = 0;
    for (int i = 0; i < pivot_vertices_count; ++i) {
        int pivot_vertex = pivot_vertices[i];

        for (int j = 0; j < candidates_count[pivot_vertex]; ++j) {
            int v = candidates[pivot_vertex][j];

            if (v == INVALID_VERTEX_ID)
                continue;
            
            auto v_iter=data_graph.GetGraph()->GetNI(v);
            auto temp_iter=v_iter;
            
            do{
                temp_iter=v_iter;
                int v_nbrs_count;
                const int* v_nbrs = v_iter.GetCurNbr(v_nbrs_count);

                for (int k = 0; k < v_nbrs_count; ++k) {
                    int v_nbr = v_nbrs[k];
                    auto v_nbr_iter=data_graph.GetGraph()->GetNI(v_nbr);
                    const int* v_nbr_label = v_nbr_iter.GetData();
                    int v_nbr_degree = v_nbr_iter.GetDeg();

                    if (flag[v_nbr] == count && v_nbr_degree >= query_vertex_degree) {
                        bool v_nbr_flag=false;
                        int l=0;
                        while(l<NodeDatalengthDef && v_nbr_label[l]!=-1){
                            if(v_nbr_label[l]==query_vertex_label[0]){
                                v_nbr_flag=true;
                                break;
                            }
                            l++;
                        }
                        if(v_nbr_flag){
                            flag[v_nbr] += 1;

                            if (count == 0) {
                            updated_flag[updated_flag_count++] = v_nbr;
                            }
                        }
                    }
                }
                v_iter.ToNextNode();
            }while(!temp_iter.IsNodeEnd());
        }

        count += 1;
    }

    for (int i = 0; i < candidates_count[query_vertex]; ++i) {
        int v = candidates[query_vertex][i];
        if (v == INVALID_VERTEX_ID)
            continue;

        if (flag[v] != count) {
            candidates[query_vertex][i] = INVALID_VERTEX_ID;
        }
    }

    for (int i = 0; i < updated_flag_count; ++i) {
        int v = updated_flag[i];
        flag[v] = 0;
    }
}



bool SubgraphMatch_Filter::CFLFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph &query_graph, Arena* candidatetable, int **&candidates, int *&candidates_count,int *&order, TreeNode *&tree) {
    AllocateBuffer(data_graph, query_graph, candidatetable, candidates, candidates_count);
    int level_count;
    int* level_offset;
    GenerateCFLFilterPlan(data_graph, query_graph, tree, order, level_count, level_offset);

    int start_vertex = order[0];
    ComputeCandidateWithNLF(data_graph, query_graph, start_vertex, candidates_count[start_vertex], candidates[start_vertex]);

    int* updated_flag = new int[data_graph.GetGraph()->GetNodeNum()];
    int* flag = new int[data_graph.GetGraph()->GetNodeNum()];
    std::fill(flag, flag + data_graph.GetGraph()->GetNodeNum(), 0);

    // Top-down generation.
    for (int i = 1; i < level_count; ++i) {
        // Forward generation.
        for (int j = level_offset[i]; j < level_offset[i + 1]; ++j) {
            int query_vertex = order[j];
            TreeNode& node = tree[query_vertex];
            GenerateCandidates(data_graph, query_graph, query_vertex, node.bn_, node.bn_count_, candidates, candidates_count, flag, updated_flag);
        }

        // Backward prune.
        for (int j = level_offset[i + 1] - 1; j >= level_offset[i]; --j) {
            int query_vertex = order[j];
            TreeNode& node = tree[query_vertex];

            if (node.fn_count_ > 0) {
                PruneCandidates(data_graph, query_graph, query_vertex, node.fn_, node.fn_count_, candidates, candidates_count, flag, updated_flag);
            }
        }
    }

    // Bottom-up refinement.
    for (int i = level_count - 2; i >= 0; --i) {
        for (int j = level_offset[i]; j < level_offset[i + 1]; ++j) {
            int query_vertex = order[j];
            TreeNode& node = tree[query_vertex];

            if (node.under_level_count_ > 0) {
                PruneCandidates(data_graph, query_graph, query_vertex, node.under_level_, node.under_level_count_, candidates, candidates_count, flag, updated_flag);
            }
        }
    }


    CompactCandidates(candidates, candidates_count, query_graph.GetGraph()->GetNodeNum());

    delete[] updated_flag;
    delete[] flag;
    delete[] level_offset;
    return IsCandidateSetValid(candidates, candidates_count, query_graph.GetGraph()->GetNodeNum());
}

void SubgraphMatch_Filter::CompactCandidates(int **&candidates, int *&candidates_count, int query_vertex_num) {
    for (int i = 0; i < query_vertex_num; ++i) {
        int query_vertex = i;
        int next_position = 0;
        for (int j = 0; j < candidates_count[query_vertex]; ++j) {
            int data_vertex = candidates[query_vertex][j];

            if (data_vertex != INVALID_VERTEX_ID) {
                candidates[query_vertex][next_position++] = data_vertex;
            }
        }

        candidates_count[query_vertex] = next_position;
    }
}

bool SubgraphMatch_Filter::IsCandidateSetValid(int **&candidates, int *&candidates_count, int query_vertex_num) {
    for (int i = 0; i < query_vertex_num; ++i) {
        if (candidates_count[i] == 0)
            return false;
    }
    return true;
}


void SubgraphMatch_Filter::GenerateDPisoFilterPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, TreeNode *&tree,int *&order) {
    int start_vertex = SelectDPisoStartVertex(data_graph, query_graph);
    SubgraphMatch_GraphOperations::BfsTraversal(query_graph, start_vertex, tree, order);

    int query_vertices_num = query_graph.GetGraph()->GetNodeNum();
    std::vector<int> order_index(query_vertices_num);
    for (int i = 0; i < query_vertices_num; ++i) {
        int query_vertex = order[i];
        order_index[query_vertex] = i;
    }

    for (int i = 0; i < query_vertices_num; ++i) {
        int u = order[i];
        tree[u].under_level_count_ = 0;
        tree[u].bn_count_ = 0;
        tree[u].fn_count_ = 0;

        auto u_iter=query_graph.GetGraph()->GetNI(u);
        auto temp_iter=u_iter;
        
        do{
            temp_iter=u_iter;
            int u_nbrs_count;
            const int* u_nbrs = u_iter.GetCurNbr(u_nbrs_count);
            for (int j = 0; j < u_nbrs_count; ++j) {
                int u_nbr = u_nbrs[j];
                if (order_index[u_nbr] < order_index[u]) {
                    tree[u].bn_[tree[u].bn_count_++] = u_nbr;
                }
                else {
                    tree[u].fn_[tree[u].fn_count_++] = u_nbr;
                }
        }
            u_iter.ToNextNode();
        }while(!temp_iter.IsNodeEnd());
    }
}

int SubgraphMatch_Filter::SelectDPisoStartVertex(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph) {
    double min_score = data_graph.GetGraph()->GetNodeNum();
    int start_vertex = 0;

    for (int i = 0; i < query_graph.GetGraph()->GetNodeNum(); ++i) {
        int degree = query_graph.GetGraph()->GetNI(i).GetDeg();
        if (degree <= 1)
            continue;

        int count = 0;
        ComputeCandidateWithLDF(data_graph, query_graph, i, count);
        double cur_score = count / (double)degree;
        if (cur_score < min_score) {
            min_score = cur_score;
            start_vertex = i;
        }
    }

    return start_vertex;
}

bool SubgraphMatch_Filter::DPisoFilter(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Arena* candidatetable,int **&candidates, int *&candidates_count,int *&order, TreeNode *&tree) {
    if (!LDFFilter(data_graph, query_graph, candidatetable, candidates, candidates_count))
        return false;

    GenerateDPisoFilterPlan(data_graph, query_graph, tree, order);

    int query_vertices_num = query_graph.GetGraph()->GetNodeNum();
    int* updated_flag = new int[data_graph.GetGraph()->GetNodeNum()];
    int* flag = new int[data_graph.GetGraph()->GetNodeNum()];
    std::fill(flag, flag + data_graph.GetGraph()->GetNodeNum(), 0);

    // The number of refinement is k. According to the original paper, we set k as 3.
    for (int k = 0; k < 2; ++k) {
        if (k % 2 == 0) {
            for (int i = 1; i < query_vertices_num; ++i) {
                int query_vertex = order[i];
                TreeNode& node = tree[query_vertex];
                PruneCandidates(data_graph, query_graph, query_vertex, node.bn_, node.bn_count_, candidates, candidates_count, flag, updated_flag);
            }
        }
        else {
            for (int i = query_vertices_num - 2; i >= 0; --i) {
                int query_vertex = order[i];
                TreeNode& node = tree[query_vertex];
                PruneCandidates(data_graph, query_graph, query_vertex, node.fn_, node.fn_count_, candidates, candidates_count, flag, updated_flag);
            }
        }
    }
    CompactCandidates(candidates, candidates_count, query_graph.GetGraph()->GetNodeNum());

    delete[] updated_flag;
    delete[] flag;
    return IsCandidateSetValid(candidates, candidates_count, query_graph.GetGraph()->GetNodeNum());
}


#endif /* SubgroupMatch_Filter_h */
