//
//  SubgraphMatch_Graph.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/4/26.
//

#ifndef SubgraphMatch_Graph_h
#define SubgraphMatch_Graph_h

#include<unordered_map>
#include<map>
#include<queue>
#include<memory>
#include"Arena.h"
#include "UNDirect_UNWeight_Graph.h"

class TreeNode {
public:
    int id_;
    int parent_;
    int level_;
    int under_level_count_;
    int children_count_;
    int bn_count_;
    int fn_count_;
    int* under_level_;
    int* children_;
    int* bn_;
    int* fn_;
    size_t estimated_embeddings_num_;
public:
    TreeNode() {
        id_ = 0;
        under_level_ = NULL;
        bn_ = NULL;
        fn_ = NULL;
        children_ = NULL;
        parent_ = 0;
        level_ = 0;
        under_level_count_ = 0;
        children_count_ = 0;
        bn_count_ = 0;
        fn_count_ = 0;
        estimated_embeddings_num_ = 0;
    }

    ~TreeNode() {
        delete[] under_level_;
        delete[] bn_;
        delete[] fn_;
        delete[] children_;
    }

    void initialize(const int size) {
        under_level_ = new int[size];
        bn_ = new int[size];
        fn_ = new int[size];
        children_ = new int[size];
    }
};



class SubgraphMatch_Graph{
private:
    int Max_Degree;
    int Label_Count;
    int Max_Label_Frequency;
    std::unordered_map<int,int> Label_Frequency;
    
    int* Reverse_Index;
    size_t* Reverse_Index_Offset;
    
    Arena* NLF;
    std::unordered_map<int,size_t> NLF_Offset;
    
    int Core_Length;
    int* Core_Table;
    
    const UNDirect_UNWeight_Graph* Graph;
    
public:
    SubgraphMatch_Graph(const UNDirect_UNWeight_Graph* graph,Arena* reverseindex,Arena* nlf);
    ~SubgraphMatch_Graph(){
        delete[] Reverse_Index_Offset;
        if(Core_Table!=NULL) delete[] Core_Table;
    }
    
    const int GetLabelFrequency(const int Label)const{
        return Label_Frequency.find(Label)==Label_Frequency.end()? 0: Label_Frequency.at(Label);
    }
    const int* GetVertexsByLabel(const int Label,int& Count)const{
        Count=Reverse_Index_Offset[Label+1]-Reverse_Index_Offset[Label];
        return Reverse_Index +Reverse_Index_Offset[Label];
    }
    const int* GetVertexNLF(const int Nid, int& LabelNum)const{
        if(NLF_Offset.find(Nid)==NLF_Offset.end()){
            LabelNum=0;
            return NULL;
        }
        size_t offset=NLF_Offset.at(Nid);
        const int* temp=(int*)(NLF->BeginPtr()+offset);
        LabelNum=temp[0];
        return temp+1;
    }
    
    const int GetCoreValue(const int nid)const{return Core_Table[nid];}
    const int Get2CoreSize()const{return Core_Length;}
    
    const int GetLabelCount()const{return Label_Count;}
    const int GetMaxLabelFrequency()const{return Max_Label_Frequency;}
    const int GetMaxVertexDegree()const{return Max_Degree;}
    const UNDirect_UNWeight_Graph* GetGraph()const{return Graph;}
    
    void BuildCoreTable();
private:
    void BuildReverseIndex(Arena* reverseindex);
    void BuildNLF();
    
    
};



class SubgraphMatch_GraphOperations {
public:
    static void GetKCore(const SubgraphMatch_Graph& graph, int *core_table);
    static void BfsTraversal(const SubgraphMatch_Graph& graph, int root_vertex, TreeNode *&tree, int *&bfs_order);
    static void DfsTraversal(TreeNode* tree, int root_vertex, int node_num, int* &dfs_order);
private:
    static void dfs(TreeNode* tree, int cur_vertex, int* dfs_order, int& count);
};


void SubgraphMatch_GraphOperations::GetKCore(const SubgraphMatch_Graph& graph, int *core_table) {
    int vertices_count = graph.GetGraph()->GetNodeNum();
    int max_degree = graph.GetMaxVertexDegree();

    int* vertices = new int[vertices_count];          // Vertices sorted by degree.
    int* position = new int[vertices_count];          // The position of vertices in vertices array.
    int* degree_bin = new int[max_degree + 1];      // Degree from 0 to max_degree.
    int* offset = new int[max_degree + 1];          // The offset in vertices array according to degree.

    std::fill(degree_bin, degree_bin + (max_degree + 1), 0);

    for (int i = 0; i < vertices_count; ++i) {
        int degree = graph.GetGraph()->GetNI(i).GetDeg();
        core_table[i] = degree;
        degree_bin[degree] += 1;
    }

    int start = 0;
    for (int i = 0; i < max_degree + 1; ++i) {
        offset[i] = start;
        start += degree_bin[i];
    }

    for (int i = 0; i < vertices_count; ++i) {
        int degree = graph.GetGraph()->GetNI(i).GetDeg();
        position[i] = offset[degree];
        vertices[position[i]] = i;
        offset[degree] += 1;
    }

    for (int i = max_degree; i > 0; --i) {
        offset[i] = offset[i - 1];
    }
    offset[0] = 0;

    for (int i = 0; i < vertices_count; ++i) {
        int v = vertices[i];
        
        auto vertexiter=graph.GetGraph()->GetNI(v);
        auto tempiter=vertexiter;
        
        do{
            tempiter=vertexiter;
            int count;
            const int* neighbors =vertexiter.GetCurNbr(count);
            for(int j = 0; j < count; ++j) {
                int u = neighbors[j];
                
                if (core_table[u] > core_table[v]) {

                    // Get the position and vertex which is with the same degree
                    // and at the start position of vertices array.
                    int cur_degree_u = core_table[u];
                    int position_u = position[u];
                    int position_w = offset[cur_degree_u];
                    int w = vertices[position_w];

                    if (u != w) {
                        // Swap u and w.
                        position[u] = position_w;
                        position[w] = position_u;
                        vertices[position_u] = w;
                        vertices[position_w] = u;
                    }

                    offset[cur_degree_u] += 1;
                    core_table[u] -= 1;
                }
            }
            vertexiter.ToNextNode();
        }while(!tempiter.IsNodeEnd());
    }

    delete[] vertices;
    delete[] position;
    delete[] degree_bin;
    delete[] offset;
}

void SubgraphMatch_GraphOperations::BfsTraversal(const SubgraphMatch_Graph &graph, int root_vertex, TreeNode *&tree, int *&bfs_order){
    int vertex_num = graph.GetGraph()->GetNodeNum();

    std::queue<int> bfs_queue;
    std::vector<bool> visited(vertex_num, false);

    tree = new TreeNode[vertex_num];
    for (int i = 0; i < vertex_num; ++i) {
        tree[i].initialize(vertex_num);
    }
    bfs_order = new int[vertex_num];

    int visited_vertex_count = 0;
    bfs_queue.push(root_vertex);
    visited[root_vertex] = true;
    tree[root_vertex].level_ = 0;
    tree[root_vertex].id_ = root_vertex;

    while(!bfs_queue.empty()) {
        const int u = bfs_queue.front();
        bfs_queue.pop();
        bfs_order[visited_vertex_count++] = u;
        
        auto nodeiter=graph.GetGraph()->GetNI(u);
        auto tempiter=nodeiter;
        
        do{
            tempiter=nodeiter;
            int u_nbrs_count;
            const int* u_nbrs = nodeiter.GetCurNbr(u_nbrs_count);
            for (int i = 0; i < u_nbrs_count; ++i) {
                int u_nbr = u_nbrs[i];

                if (!visited[u_nbr]) {
                    bfs_queue.push(u_nbr);
                    visited[u_nbr] = true;
                    tree[u_nbr].id_ = u_nbr;
                    tree[u_nbr].parent_ = u;
                    tree[u_nbr].level_ = tree[u] .level_ + 1;
                    tree[u].children_[tree[u].children_count_++] = u_nbr;
                }
            }
            nodeiter.ToNextNode();
        }while(!tempiter.IsNodeEnd());
    }
}

void SubgraphMatch_GraphOperations::DfsTraversal(TreeNode *tree, int root_vertex, int node_num, int *&dfs_order){
    dfs_order = new int[node_num];
    int count = 0;
    dfs(tree, root_vertex, dfs_order, count);
}

void SubgraphMatch_GraphOperations::dfs(TreeNode *tree, int cur_vertex, int *dfs_order, int &count){
    dfs_order[count++] = cur_vertex;
    
    for (int i = 0; i < tree[cur_vertex].children_count_; ++i) {
        dfs(tree, tree[cur_vertex].children_[i], dfs_order, count);
    }
}






SubgraphMatch_Graph::SubgraphMatch_Graph(const UNDirect_UNWeight_Graph* graph,Arena* reverseindex,Arena* nlf):Graph(graph),Max_Degree(0),Label_Count(0),Max_Label_Frequency(0),NLF(nlf),Core_Length(0),Core_Table(NULL){
    int Max_Label=0;
    auto iter=Graph->BegNI();
    while(!iter.IsEnd()){
        if(iter.GetDeg()>Max_Degree) Max_Degree=iter.GetDeg();
        const int* Label=iter.GetData();
        int i=0;
        while(i<NodeDatalengthDef && Label[i]!=-1){
            if(Label_Frequency.find(Label[i])==Label_Frequency.end()){
                Label_Frequency[Label[i]]=0;
                if(Label[i]>Max_Label) Max_Label=Label[i];
            }
            Label_Frequency[Label[i]]+=1;
            i++;
        }
        iter++;
    }
    Label_Count=Label_Frequency.size()>(Max_Label+1)? Label_Frequency.size(): Max_Label+1;
    for(auto element:Label_Frequency){
        if(element.second>Max_Label_Frequency) Max_Label_Frequency=element.second;
    }
    BuildReverseIndex(reverseindex);
    BuildNLF();
}

void SubgraphMatch_Graph::BuildReverseIndex(Arena* reverseindex){
    Reverse_Index_Offset=new size_t[Label_Count+1];
    Reverse_Index_Offset[0]=0;
    size_t total=0;
    for(int i=0;i<Label_Count;++i){
        Reverse_Index_Offset[i+1]=total;
        total+=Label_Frequency[i];
    }
    Reverse_Index=(int*)reverseindex->AllocateBytes(total*sizeof(int));
    auto iter=Graph->BegNI();
    while(!iter.IsEnd()){
        const int* Label=iter.GetData();
        int i=0;
        while(i<NodeDatalengthDef && Label[i]!=-1){
            int label=Label[i];
            Reverse_Index[Reverse_Index_Offset[label+1]++]=iter.GetId();
            i++;
        }
        iter++;
    }
}
void SubgraphMatch_Graph::BuildNLF(){
    auto iter=Graph->BegNI();
    while(!iter.IsEnd()){
        std::map<int,int> labelfrequency;
        auto curnode=iter;
        auto temp=curnode;
        do{
            temp=curnode;
            int deg;
            const int* nbr=curnode.GetCurNbr(deg);
            for(int i=0;i<deg;++i){
                const int* nbrlabel=Graph->GetNI(nbr[i]).GetData();
                int j=0;
                while(j<NodeDatalengthDef && nbrlabel[j]!=-1){
                    if(labelfrequency.find(nbrlabel[j])==labelfrequency.end()) labelfrequency[nbrlabel[j]]=0;
                    labelfrequency[nbrlabel[j]]+=1;
                    j++;
                }
            }
            curnode.ToNextNode();
        }while(!temp.IsNodeEnd());
        int nbrlabelsize=labelfrequency.size();
        char* allocatechar=NLF->AllocateBytes((nbrlabelsize*2+1)*sizeof(int));
        NLF_Offset[iter.GetId()]=allocatechar-NLF->BeginPtr();
        int* NodeNLF=(int*)allocatechar;
        NodeNLF[0]=nbrlabelsize;
        int i=1;
        for(auto iter:labelfrequency) {NodeNLF[i]=iter.first;i++;}
        for(auto iter:labelfrequency) {NodeNLF[i]=iter.second;i++;}
        iter++;
    }
}

void SubgraphMatch_Graph::BuildCoreTable(){
    Core_Table=new int[Graph->GetNodeNum()];
    SubgraphMatch_GraphOperations::GetKCore(*this, Core_Table);
    for(int i=0;i<Graph->GetNodeNum();++i){
        if(Core_Table[i]>1) Core_Length+=1;
    }
}




#endif /* SubgraphMatch_Graph_h */
