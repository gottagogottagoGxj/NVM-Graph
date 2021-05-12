//
//  SubgraphMatch_Query.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/5/8.
//

#ifndef SubgraphMatch_Query_h
#define SubgraphMatch_Query_h
#include"SubgraphMatch_Graph.h"
#include <queue>

auto extendable_vertex_compare = [](std::pair<std::pair<int, int>, int> l, std::pair<std::pair<int, int>, int> r) {
    if (l.first.second == 1 && r.first.second != 1) {
        return true;
    }
    else if (l.first.second != 1 && r.first.second == 1) {
        return false;
    }
    else
    {
        return l.second > r.second;
    }
};

typedef std::priority_queue<std::pair<std::pair<int, int>, int>, std::vector<std::pair<std::pair<int, int>, int>>,
        decltype(extendable_vertex_compare)> dpiso_min_pq;


class SubgraphMatch_Query{
public:
    static void GenerateCFLQueryPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Edges ***edge_matrix,int *&order, int *&pivot, TreeNode *tree, int *bfs_order, int *candidates_count);
    static size_t ExploreGraph(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int **candidates,int *candidates_count, int *order, int *pivot, size_t output_limit_num, size_t &call_count);
    static size_t ExecuteQuery(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int **candidates,int *candidates_count, int *order, int *pivot, size_t output_limit_num, size_t &call_count);
    static void GenerateDSPisoQueryPlan(const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int *&order, int *&pivot,TreeNode *tree,int *bfs_order, int *candidates_count, int **&weight_array);
    static size_t ExploreDPisoStyle(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree,Edges ***edge_matrix, int **candidates, int *candidates_count,int **weight_array, int *order, size_t output_limit_num,size_t &call_count);
    static void GenerateCFLDPQueryPlan(const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int *&order, int *&pivot,TreeNode *tree,int *bfs_order, int *candidates_count, int **&weight_array);
    
    static void GenerateCRDPQueryPlan(const UNDirect_UNWeight_Graph& query_graph, int *&order, TreeNode *tree,int *bfs_order);
    static size_t ExploreCRDPStyle(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree, int **candidates, int *candidates_count, int *order, size_t output_limit_num,size_t &call_count);
    
private:
    static void estimatePathEmbeddsingsNum(std::vector<int> &path, Edges ***edge_matrix, std::vector<size_t> &estimated_embeddings_num);
    static int generateNoneTreeEdgesCount(const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree_node, std::vector<int> &path);
    static void generateCorePaths(const SubgraphMatch_Graph& query_graph, TreeNode *tree_node, int cur_vertex, std::vector<int> &cur_core_path, std::vector<std::vector<int>> &core_paths);
    static void generateTreePaths(const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree_node, int cur_vertex,std::vector<int> &cur_tree_path, std::vector<std::vector<int>> &tree_paths);
    static void generateLeaves(const UNDirect_UNWeight_Graph& query_graph, std::vector<int> &leaves);
    static void generateBN(const UNDirect_UNWeight_Graph& query_graph, int *order, int *pivot, int **&bn, int *&bn_count);
    static void generateBN(const UNDirect_UNWeight_Graph& query_graph, int *order, int **&bn, int *&bn_count);
    static void allocateBuffer(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, int *candidates_count, int *&idx,int *&idx_count, int *&embedding, int *&idx_embedding, int *&temp_buffer,int **&valid_candidate_idx, bool *&visited_vertices);
    static void releaseBuffer(int query_vertices_num, int *idx, int *idx_count, int *embedding, int *idx_embedding,int *temp_buffer, int **valid_candidate_idx, bool *visited_vertices, int **bn=NULL,int *bn_count=NULL);
    static void generateValidCandidateIndex(const UNDirect_UNWeight_Graph& data_graph, int depth, int *embedding, int *idx_embedding,int *idx_count, int **valid_candidate_index, Edges ***edge_matrix,bool *visited_vertices, int **bn, int *bn_cnt, int *order, int *pivot,int **candidates);
    static void generateValidCandidateIndex(int depth, int *idx_embedding, int *idx_count, int **valid_candidate_index,Edges ***edge_matrix, int **bn, int *bn_cnt,int *order,int *pivot,int *&temp_buffer);
    static void generateValidCandidateIndex(int depth, int *idx_embedding, int *idx_count, int **valid_candidate_index,Edges ***edge_matrix, int **bn, int *bn_cnt,int *order,int *&temp_buffer);
    static void generateValidCandidateIndex(int u, int *idx_embedding, int *idx_count, int *&valid_candidate_index,Edges ***edge_matrix, int *bn, int bn_cnt, int *&temp_buffer);
    static void updateExtendableVertex(int *idx_embedding, int *idx_count, int **valid_candidate_index, Edges ***edge_matrix, int *&temp_buffer, int **weight_array, TreeNode *tree, int mapped_vertex, int *extendable, std::vector<dpiso_min_pq> &vec_rank_queue, const UNDirect_UNWeight_Graph& query_graph);
    static void restoreExtendableVertex(TreeNode *tree, int unmapped_vertex,int *extendable);
    
    
    static void updateExtendableVertex(int *embedding,int** candidates,int* candidates_count, int *idx_count, int **valid_candidate,  TreeNode *tree, int mapped_vertex, int *extendable, std::vector<dpiso_min_pq> &vec_rank_queue, const UNDirect_UNWeight_Graph& query_graph, const UNDirect_UNWeight_Graph& data_graph);
    static void generateValidCandidates(int u, int *embedding,  int** candidates, int* candidates_count,int *idx_count, int *valid_candidate, int *bn, int bn_cnt, const UNDirect_UNWeight_Graph& data_graph);
    
};



void SubgraphMatch_Query::GenerateCFLQueryPlan(const SubgraphMatch_Graph& data_graph, const SubgraphMatch_Graph& query_graph, Edges ***edge_matrix,int *&order, int *&pivot, TreeNode *tree, int *bfs_order, int *candidates_count) {
    int query_vertices_num = query_graph.GetGraph()->GetNodeNum();
    int root_vertex = bfs_order[0];
    order = new int[query_vertices_num];
    pivot = new int[query_vertices_num];
    std::vector<bool> visited_vertices(query_vertices_num, false);

    std::vector<std::vector<int>> core_paths;
    std::vector<std::vector<std::vector<int>>> forests;
    std::vector<int> leaves;

    generateLeaves(*query_graph.GetGraph(), leaves);
    if (query_graph.GetCoreValue(root_vertex) > 1) {
        std::vector<int> temp_core_path;
        generateCorePaths(query_graph, tree, root_vertex, temp_core_path, core_paths);
        for (int i = 0; i < query_vertices_num; ++i) {
            int cur_vertex = i;
            if (query_graph.GetCoreValue(cur_vertex)> 1) {
                std::vector<std::vector<int>> temp_tree_paths;
                std::vector<int> temp_tree_path;
                generateTreePaths(*query_graph.GetGraph(), tree, cur_vertex, temp_tree_path, temp_tree_paths);
                if (!temp_tree_paths.empty()) {
                    forests.emplace_back(temp_tree_paths);
                }
            }
        }
    }
    else {
        std::vector<std::vector<int>> temp_tree_paths;
        std::vector<int> temp_tree_path;
        generateTreePaths(*query_graph.GetGraph(), tree, root_vertex, temp_tree_path, temp_tree_paths);
        if (!temp_tree_paths.empty()) {
            forests.emplace_back(temp_tree_paths);
        }
    }

    // Order core paths.
    int selected_vertices_count = 0;
    order[selected_vertices_count++] = root_vertex;
    visited_vertices[root_vertex] = true;

    if (!core_paths.empty()) {
        std::vector<std::vector<size_t>> paths_embededdings_num;
        std::vector<int> paths_non_tree_edge_num;
        for (auto& path : core_paths) {
            int non_tree_edge_num = generateNoneTreeEdgesCount(*query_graph.GetGraph(), tree, path);
            paths_non_tree_edge_num.push_back(non_tree_edge_num + 1);

            std::vector<size_t> path_embeddings_num;
            estimatePathEmbeddsingsNum(path, edge_matrix, path_embeddings_num);
            paths_embededdings_num.emplace_back(path_embeddings_num);
        }

        // Select the start path.
        double min_value = std::numeric_limits<double>::max();
        int selected_path_index = 0;

        for (int i = 0; i < core_paths.size(); ++i) {
            double cur_value = paths_embededdings_num[i][0] / (double) paths_non_tree_edge_num[i];

            if (cur_value < min_value) {
                min_value = cur_value;
                selected_path_index = i;
            }
        }


        for (int i = 1; i < core_paths[selected_path_index].size(); ++i) {
            order[selected_vertices_count] = core_paths[selected_path_index][i];
            pivot[selected_vertices_count] = core_paths[selected_path_index][i - 1];
            selected_vertices_count += 1;
            visited_vertices[core_paths[selected_path_index][i]] = true;
        }

        core_paths.erase(core_paths.begin() + selected_path_index);
        paths_embededdings_num.erase(paths_embededdings_num.begin() + selected_path_index);
        paths_non_tree_edge_num.erase(paths_non_tree_edge_num.begin() + selected_path_index);

        while (!core_paths.empty()) {
            min_value = std::numeric_limits<double>::max();
            selected_path_index = 0;

            for (int i = 0; i < core_paths.size(); ++i) {
                int path_root_vertex_idx = 0;
                for (int j = 0; j < core_paths[i].size(); ++j) {
                    int cur_vertex = core_paths[i][j];

                    if (visited_vertices[cur_vertex])
                        continue;

                    path_root_vertex_idx = j - 1;
                    break;
                }

                double cur_value = paths_embededdings_num[i][path_root_vertex_idx] / (double)candidates_count[core_paths[i][path_root_vertex_idx]];
                if (cur_value < min_value) {
                    min_value = cur_value;
                    selected_path_index = i;
                }
            }

            for (int i = 1; i < core_paths[selected_path_index].size(); ++i) {
                if (visited_vertices[core_paths[selected_path_index][i]])
                    continue;

                order[selected_vertices_count] = core_paths[selected_path_index][i];
                pivot[selected_vertices_count] = core_paths[selected_path_index][i - 1];
                selected_vertices_count += 1;
                visited_vertices[core_paths[selected_path_index][i]] = true;
            }

            core_paths.erase(core_paths.begin() + selected_path_index);
            paths_embededdings_num.erase(paths_embededdings_num.begin() + selected_path_index);
        }
    }

    // Order tree paths.
    for (auto& tree_paths : forests) {
        std::vector<std::vector<size_t>> paths_embededdings_num;
        for (auto& path : tree_paths) {
            std::vector<size_t> path_embeddings_num;
            estimatePathEmbeddsingsNum(path, edge_matrix, path_embeddings_num);
            paths_embededdings_num.emplace_back(path_embeddings_num);
        }

        while (!tree_paths.empty()) {
            double min_value = std::numeric_limits<double>::max();
            int selected_path_index = 0;

            for (int i = 0; i < tree_paths.size(); ++i) {
                int path_root_vertex_idx = 0;
                for (int j = 0; j < tree_paths[i].size(); ++j) {
                    int cur_vertex = tree_paths[i][j];

                    if (visited_vertices[cur_vertex])
                        continue;

                    path_root_vertex_idx = j == 0 ? j : j - 1;
                    break;
                }

                double cur_value = paths_embededdings_num[i][path_root_vertex_idx] / (double)candidates_count[tree_paths[i][path_root_vertex_idx]];
                if (cur_value < min_value) {
                    min_value = cur_value;
                    selected_path_index = i;
                }
            }

            for (int i = 0; i < tree_paths[selected_path_index].size(); ++i) {
                if (visited_vertices[tree_paths[selected_path_index][i]])
                    continue;

                order[selected_vertices_count] = tree_paths[selected_path_index][i];
                pivot[selected_vertices_count] = tree_paths[selected_path_index][i - 1];
                selected_vertices_count += 1;
                visited_vertices[tree_paths[selected_path_index][i]] = true;
            }

            tree_paths.erase(tree_paths.begin() + selected_path_index);
            paths_embededdings_num.erase(paths_embededdings_num.begin() + selected_path_index);
        }
    }

    // Order the leaves.
    while (!leaves.empty()) {
        double min_value = std::numeric_limits<double>::max();
        int selected_leaf_index = 0;

        for (int i = 0; i < leaves.size(); ++i) {
            int vertex = leaves[i];
            double cur_value = candidates_count[vertex];

            if (cur_value < min_value) {
                min_value = cur_value;
                selected_leaf_index = i;
            }
        }

        if (!visited_vertices[leaves[selected_leaf_index]]) {
            order[selected_vertices_count] = leaves[selected_leaf_index];
            pivot[selected_vertices_count] = tree[leaves[selected_leaf_index]].parent_;
            selected_vertices_count += 1;
            visited_vertices[leaves[selected_leaf_index]] = true;
        }
        leaves.erase(leaves.begin() + selected_leaf_index);
    }
}



void SubgraphMatch_Query::estimatePathEmbeddsingsNum(std::vector<int> &path, Edges ***edge_matrix, std::vector<size_t> &estimated_embeddings_num) {
    assert(path.size() > 1);
    std::vector<size_t> parent;
    std::vector<size_t> children;

    estimated_embeddings_num.resize(path.size() - 1);
    Edges& last_edge = *edge_matrix[path[path.size() - 2]][path[path.size() - 1]];
    children.resize(last_edge.vertex_count_);

    size_t sum = 0;
    for (int i = 0; i < last_edge.vertex_count_; ++i) {
        children[i] = last_edge.offset_[i + 1] - last_edge.offset_[i];
        sum += children[i];
    }

    estimated_embeddings_num[path.size() - 2] = sum;

    for (int i = path.size() - 2; i >= 1; --i) {
        int begin = path[i - 1];
        int end = path[i];

        Edges& edge = *edge_matrix[begin][end];
        parent.resize(edge.vertex_count_);

        sum = 0;
        for (int j = 0; j < edge.vertex_count_; ++j) {

            size_t local_sum = 0;
            for (int k = edge.offset_[j]; k < edge.offset_[j + 1]; ++k) {
                int nbr = edge.edge_[k];
                local_sum += children[nbr];
            }

            parent[j] = local_sum;
            sum += local_sum;
        }

        estimated_embeddings_num[i - 1] = sum;
        parent.swap(children);
    }
}

int SubgraphMatch_Query::generateNoneTreeEdgesCount(const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree_node, std::vector<int> &path) {
    int non_tree_edge_count = query_graph.GetNI(path[0]).GetDeg() - tree_node[path[0]].children_count_;

    for (int i = 1; i < path.size(); ++i) {
        int vertex = path[i];
        non_tree_edge_count += query_graph.GetNI(vertex).GetDeg() - tree_node[vertex].children_count_ - 1;
    }

    return non_tree_edge_count;
}

void SubgraphMatch_Query::generateCorePaths(const SubgraphMatch_Graph& query_graph, TreeNode *tree_node, int cur_vertex, std::vector<int> &cur_core_path, std::vector<std::vector<int>> &core_paths) {
    TreeNode& node = tree_node[cur_vertex];
    cur_core_path.push_back(cur_vertex);

    bool is_core_leaf = true;
    for (int i = 0; i < node.children_count_; ++i) {
        int child = node.children_[i];
        if (query_graph.GetCoreValue(child)>1) {
            generateCorePaths(query_graph, tree_node, child, cur_core_path, core_paths);
            is_core_leaf = false;
        }
    }

    if (is_core_leaf) {
        core_paths.emplace_back(cur_core_path);
    }
    cur_core_path.pop_back();
}

void SubgraphMatch_Query::generateTreePaths(const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree_node, int cur_vertex,std::vector<int> &cur_tree_path, std::vector<std::vector<int>> &tree_paths) {
    TreeNode& node = tree_node[cur_vertex];
    cur_tree_path.push_back(cur_vertex);

    bool is_tree_leaf = true;
    for (int i = 0; i < node.children_count_; ++i) {
        int child = node.children_[i];
        if (query_graph.GetNI(child).GetDeg() > 1) {
            generateTreePaths(query_graph, tree_node, child, cur_tree_path, tree_paths);
            is_tree_leaf = false;
        }
    }

    if (is_tree_leaf && cur_tree_path.size() > 1) {
        tree_paths.emplace_back(cur_tree_path);
    }
    cur_tree_path.pop_back();
}

void SubgraphMatch_Query::generateLeaves(const UNDirect_UNWeight_Graph& query_graph, std::vector<int> &leaves) {
    for (int i = 0; i < query_graph.GetNodeNum(); ++i) {
        int cur_vertex = i;
        if (query_graph.GetNI(cur_vertex).GetDeg() == 1) {
            leaves.push_back(cur_vertex);
        }
    }
}



size_t SubgraphMatch_Query::ExploreGraph(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int **candidates,int *candidates_count, int *order, int *pivot, size_t output_limit_num, size_t &call_count) {
    // Generate the bn.
    int **bn;
    int *bn_count;
    generateBN(query_graph, order, pivot, bn, bn_count);

    // Allocate the memory buffer.
    int *idx;
    int *idx_count;
    int *embedding;
    int *idx_embedding;
    int *temp_buffer;
    int **valid_candidate_idx;
    bool *visited_vertices;
    allocateBuffer(data_graph, query_graph, candidates_count, idx, idx_count, embedding, idx_embedding,temp_buffer, valid_candidate_idx, visited_vertices);
    // Evaluate the query.
    size_t embedding_cnt = 0;
    int cur_depth = 0;
    int max_depth = query_graph.GetNodeNum();
    int start_vertex = order[0];

    idx[cur_depth] = 0;
    idx_count[cur_depth] = candidates_count[start_vertex];

    for (int i = 0; i < idx_count[cur_depth]; ++i) {
        valid_candidate_idx[cur_depth][i] = i;
    }

    while (true) {
        while (idx[cur_depth] < idx_count[cur_depth]) {
            int valid_idx = valid_candidate_idx[cur_depth][idx[cur_depth]];
            int u = order[cur_depth];
            int v = candidates[u][valid_idx];

            embedding[u] = v;
            idx_embedding[u] = valid_idx;
            visited_vertices[v] = true;
            idx[cur_depth] += 1;

            if (cur_depth == max_depth - 1) {
                embedding_cnt += 1;
                visited_vertices[v] = false;
                if (embedding_cnt >= output_limit_num) {
                    goto EXIT;
                }
            } else {
                call_count += 1;
                cur_depth += 1;
                idx[cur_depth] = 0;
                generateValidCandidateIndex(data_graph, cur_depth, embedding, idx_embedding, idx_count,valid_candidate_idx,edge_matrix, visited_vertices, bn, bn_count, order, pivot, candidates);
            }
        }

        cur_depth -= 1;
        if (cur_depth < 0)
            break;
        else
            visited_vertices[embedding[order[cur_depth]]] = false;
    }


    // Release the buffer.
    EXIT:
    releaseBuffer(max_depth, idx, idx_count, embedding, idx_embedding, temp_buffer, valid_candidate_idx,
                  visited_vertices,
                  bn, bn_count);

    return embedding_cnt;
}


size_t SubgraphMatch_Query::ExecuteQuery(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int **candidates,int *candidates_count, int *order, int *pivot, size_t output_limit_num, size_t &call_count) {
    // Generate the bn.
    int **bn;
    int *bn_count;
    generateBN(query_graph, order, pivot, bn, bn_count);

    // Allocate the memory buffer.
    int *idx;
    int *idx_count;
    int *embedding;
    int *idx_embedding;
    int *temp_buffer;
    int **valid_candidate_idx;
    bool *visited_vertices;
    allocateBuffer(data_graph, query_graph, candidates_count, idx, idx_count, embedding, idx_embedding,temp_buffer, valid_candidate_idx, visited_vertices);
    // Evaluate the query.
    size_t embedding_cnt = 0;
    int cur_depth = 0;
    int max_depth = query_graph.GetNodeNum();
    int start_vertex = order[0];

    idx[cur_depth] = 0;
    idx_count[cur_depth] = candidates_count[start_vertex];

    for (int i = 0; i < idx_count[cur_depth]; ++i) {
        valid_candidate_idx[cur_depth][i] = i;
    }

    while (true) {
        while (idx[cur_depth] < idx_count[cur_depth]) {
            int valid_idx = valid_candidate_idx[cur_depth][idx[cur_depth]];
            int u = order[cur_depth];
            int v = candidates[u][valid_idx];

            embedding[u] = v;
            idx_embedding[u] = valid_idx;
            visited_vertices[v] = true;
            idx[cur_depth] += 1;

            if (cur_depth == max_depth - 1) {
                embedding_cnt += 1;
                visited_vertices[v] = false;
                if (embedding_cnt >= output_limit_num) {
                    goto EXIT;
                }
            } else {
                call_count += 1;
                cur_depth += 1;
                idx[cur_depth] = 0;
                generateValidCandidateIndex(cur_depth, idx_embedding,idx_count, valid_candidate_idx, edge_matrix, bn, bn_count, order, pivot, temp_buffer);
            }
        }

        cur_depth -= 1;
        if (cur_depth < 0)
            break;
        else
            visited_vertices[embedding[order[cur_depth]]] = false;
    }


    // Release the buffer.
    EXIT:
    releaseBuffer(max_depth, idx, idx_count, embedding, idx_embedding, temp_buffer, valid_candidate_idx,
                  visited_vertices,
                  bn, bn_count);

    return embedding_cnt;
}




void SubgraphMatch_Query::generateBN(const UNDirect_UNWeight_Graph& query_graph, int *order, int *pivot, int **&bn, int *&bn_count) {
    int query_vertices_num = query_graph.GetNodeNum();
    bn_count = new int[query_vertices_num];
    std::fill(bn_count, bn_count + query_vertices_num, 0);
    bn = new int *[query_vertices_num];
    for (int i = 0; i < query_vertices_num; ++i) {
        bn[i] = new int[query_vertices_num];
    }

    std::vector<bool> visited_vertices(query_vertices_num, false);
    visited_vertices[order[0]] = true;
    for (int i = 1; i < query_vertices_num; ++i) {
        int vertex = order[i];
        
        auto vertex_iter=query_graph.GetNI(vertex);
        auto temp_iter=vertex_iter;
        do{
            temp_iter=vertex_iter;
            int nbrs_cnt;
            const int *nbrs = vertex_iter.GetCurNbr(nbrs_cnt);
            for (int j = 0; j < nbrs_cnt; ++j) {
                int nbr = nbrs[j];

                if (visited_vertices[nbr] && nbr != pivot[i]) {
                    bn[i][bn_count[i]++] = nbr;
                }
            }
            vertex_iter.ToNextNode();
        }while(!temp_iter.IsNodeEnd());

        visited_vertices[vertex] = true;
    }
}
void SubgraphMatch_Query::generateBN(const UNDirect_UNWeight_Graph& query_graph, int *order, int **&bn, int *&bn_count) {
    int query_vertices_num = query_graph.GetNodeNum();
    bn_count = new int[query_vertices_num];
    std::fill(bn_count, bn_count + query_vertices_num, 0);
    bn = new int *[query_vertices_num];
    for (int i = 0; i < query_vertices_num; ++i) {
        bn[i] = new int[query_vertices_num];
    }

    std::vector<bool> visited_vertices(query_vertices_num, false);
    visited_vertices[order[0]] = true;
    for (int i = 1; i < query_vertices_num; ++i) {
        int vertex = order[i];

        auto vertex_iter=query_graph.GetNI(vertex);
        auto temp_iter=vertex_iter;
        do{
            temp_iter=vertex_iter;
            int nbrs_cnt;
            const int *nbrs = vertex_iter.GetCurNbr(nbrs_cnt);
            for (int j = 0; j < nbrs_cnt; ++j) {
                int nbr = nbrs[j];

                if (visited_vertices[nbr]) {
                    bn[i][bn_count[i]++] = nbr;
                }
        }
            vertex_iter.ToNextNode();
        }while(!temp_iter.IsNodeEnd());

        visited_vertices[vertex] = true;
    }
}
void SubgraphMatch_Query::allocateBuffer(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, int *candidates_count, int *&idx,int *&idx_count, int *&embedding, int *&idx_embedding, int *&temp_buffer,int **&valid_candidate_idx, bool *&visited_vertices) {
    
    int query_vertices_num = query_graph.GetNodeNum();
    int data_vertices_num = data_graph.GetNodeNum();
    int max_candidates_num = candidates_count[0];

    for (int i = 1; i < query_vertices_num; ++i) {
        int cur_vertex = i;
        int cur_candidate_num = candidates_count[cur_vertex];

        if (cur_candidate_num > max_candidates_num) {
            max_candidates_num = cur_candidate_num;
        }
    }

    idx = new int[query_vertices_num];
    idx_count = new int[query_vertices_num];
    embedding = new int[query_vertices_num];
    idx_embedding = new int[query_vertices_num];
    visited_vertices = new bool[data_vertices_num];
    temp_buffer = new int[max_candidates_num];
    valid_candidate_idx = new int *[query_vertices_num];
    for (int i = 0; i < query_vertices_num; ++i) {
        valid_candidate_idx[i] = new int[max_candidates_num];
    }

    std::fill(visited_vertices, visited_vertices + data_vertices_num, false);
}

void SubgraphMatch_Query::releaseBuffer(int query_vertices_num, int *idx, int *idx_count, int *embedding, int *idx_embedding,int *temp_buffer, int **valid_candidate_idx, bool *visited_vertices, int **bn,int *bn_count) {
    delete[] idx;
    delete[] idx_count;
    delete[] embedding;
    delete[] idx_embedding;
    delete[] visited_vertices;
    delete[] temp_buffer;
    for (int i = 0; i < query_vertices_num; ++i) {
        delete[] valid_candidate_idx[i];
    }

    delete[] valid_candidate_idx;
    if(bn_count!=NULL) delete[] bn_count;
    if(bn!=NULL){
        for (int i = 0; i < query_vertices_num; ++i) {
            delete[] bn[i];
        }
        delete[] bn;
    }
}

void SubgraphMatch_Query::generateValidCandidateIndex(const UNDirect_UNWeight_Graph& data_graph, int depth, int *embedding, int *idx_embedding,int *idx_count, int **valid_candidate_index, Edges ***edge_matrix,bool *visited_vertices, int **bn, int *bn_cnt, int *order, int *pivot,int **candidates) {
    int u = order[depth];
    int pivot_vertex = pivot[depth];
    int idx_id = idx_embedding[pivot_vertex];
    Edges &edge = *edge_matrix[pivot_vertex][u];
    int count = edge.offset_[idx_id + 1] - edge.offset_[idx_id];
    int *candidate_idx = edge.edge_ + edge.offset_[idx_id];

    int valid_candidate_index_count = 0;

    if (bn_cnt[depth] == 0) {
        for (int i = 0; i < count; ++i) {
            int temp_idx = candidate_idx[i];
            int temp_v = candidates[u][temp_idx];

            if (!visited_vertices[temp_v])
                valid_candidate_index[depth][valid_candidate_index_count++] = temp_idx;
        }
    } else {
        for (int i = 0; i < count; ++i) {
            int temp_idx = candidate_idx[i];
            int temp_v = candidates[u][temp_idx];

            if (!visited_vertices[temp_v]) {
                bool valid = true;

                for (int j = 0; j < bn_cnt[depth]; ++j) {
                    int u_bn = bn[depth][j];
                    int u_bn_v = embedding[u_bn];

                    if (!data_graph.IsEdge(temp_v, u_bn_v)) {
                        valid = false;
                        break;
                    }
                }

                if (valid)
                    valid_candidate_index[depth][valid_candidate_index_count++] = temp_idx;
            }
        }
    }

    idx_count[depth] = valid_candidate_index_count;
}

void SubgraphMatch_Query::generateValidCandidateIndex(int depth, int *idx_embedding, int *idx_count, int **valid_candidate_index,Edges ***edge_matrix, int **bn, int *bn_cnt,int *order,int *pivot,int *&temp_buffer){
    int u = order[depth];
    int pivot_vertex = pivot[depth];
    int idx_id = idx_embedding[pivot_vertex];
    Edges &edge = *edge_matrix[pivot_vertex][u];
    int count = edge.offset_[idx_id + 1] - edge.offset_[idx_id];
    int *candidate_idx = edge.edge_ + edge.offset_[idx_id];
    
    memcpy(valid_candidate_index[depth], candidate_idx, count*sizeof(int));
    int valid_candidate_index_count = count;
    int temp_count;
    for(int i=0;i<bn_cnt[depth];++i){
        int current_bn=bn[depth][i];
        Edges& current_edge=*edge_matrix[current_bn][u];
        int current_index_id=idx_embedding[current_bn];
        int current_candidate_count=current_edge.offset_[current_index_id+1]-current_edge.offset_[current_index_id];
        int* current_candidate=current_edge.edge_+current_edge.offset_[current_index_id];
        SubgraphMatch_GraphOperations::ComputeCandidateSetIntersect(current_candidate, current_candidate_count, valid_candidate_index[depth], valid_candidate_index_count, temp_buffer, temp_count);
        std::swap(temp_buffer, valid_candidate_index[depth]);
        valid_candidate_index_count=temp_count;
    }
    idx_count[depth]=valid_candidate_index_count;
}

void SubgraphMatch_Query::generateValidCandidateIndex(int depth, int *idx_embedding, int *idx_count, int **valid_candidate_index,Edges ***edge_matrix, int **bn, int *bn_cnt,int *order,int *&temp_buffer){
    int u = order[depth];
    int previous_bn = bn[depth][0];
    int previous_index_id = idx_embedding[previous_bn];
    int valid_candidates_count = 0;
    Edges& previous_edge = *edge_matrix[previous_bn][u];

    valid_candidates_count = previous_edge.offset_[previous_index_id + 1] - previous_edge.offset_[previous_index_id];
    int* previous_candidates = previous_edge.edge_ + previous_edge.offset_[previous_index_id];

    memcpy(valid_candidate_index[depth], previous_candidates, valid_candidates_count * sizeof(int));

    int temp_count;
    for (int i = 1; i < bn_cnt[depth]; ++i) {
        int current_bn = bn[depth][i];
        Edges& current_edge = *edge_matrix[current_bn][u];
        int current_index_id = idx_embedding[current_bn];

        int current_candidates_count = current_edge.offset_[current_index_id + 1] - current_edge.offset_[current_index_id];
        int* current_candidates = current_edge.edge_ + current_edge.offset_[current_index_id];

        SubgraphMatch_GraphOperations::ComputeCandidateSetIntersect(current_candidates, current_candidates_count, valid_candidate_index[depth], valid_candidates_count,temp_buffer, temp_count);

        std::swap(temp_buffer, valid_candidate_index[depth]);
        valid_candidates_count = temp_count;
    }

    idx_count[depth] = valid_candidates_count;
}


void SubgraphMatch_Query::GenerateDSPisoQueryPlan(const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int *&order, int *&pivot,TreeNode *tree,int *bfs_order, int *candidates_count, int **&weight_array) {
    int query_vertices_num = query_graph.GetNodeNum();
    order = new int[query_vertices_num];
    pivot = new int[query_vertices_num];

    for (int i = 0; i < query_vertices_num; ++i) {
        order[i] = bfs_order[i];
    }

    for (int i = 1; i < query_vertices_num; ++i) {
        pivot[i] = tree[order[i]].parent_;
    }

    // Compute weight array.
    weight_array = new int*[query_vertices_num];
    for (int i = 0; i < query_vertices_num; ++i) {
        weight_array[i] = new int[candidates_count[i]];
        std::fill(weight_array[i], weight_array[i] + candidates_count[i], std::numeric_limits<int>::max());
    }

    for (int i = query_vertices_num - 1; i >= 0; --i) {
        int vertex = order[i];
        TreeNode& node = tree[vertex];
        bool set_to_one = true;

        for (int j = 0; j < node.fn_count_; ++j) {
            int child = node.fn_[j];
            TreeNode& child_node = tree[child];

            if (child_node.bn_count_ == 1) {
                set_to_one = false;
                Edges& cur_edge = *edge_matrix[vertex][child];
                for (int k = 0; k < candidates_count[vertex]; ++k) {
                    int cur_candidates_count = cur_edge.offset_[k + 1] - cur_edge.offset_[k];
                    int* cur_candidates = cur_edge.edge_ + cur_edge.offset_[k];

                    int weight = 0;

                    for (int l = 0; l < cur_candidates_count; ++l) {
                        int candidates = cur_candidates[l];
                        weight += weight_array[child][candidates];
                    }

                    if (weight < weight_array[vertex][k])
                        weight_array[vertex][k] = weight;
                }
            }
        }

        if (set_to_one) {
            std::fill(weight_array[vertex], weight_array[vertex] + candidates_count[vertex], 1);
        }
    }
}
void SubgraphMatch_Query::GenerateCFLDPQueryPlan(const UNDirect_UNWeight_Graph& query_graph, Edges ***edge_matrix, int *&order, int *&pivot,TreeNode *tree,int *bfs_order, int *candidates_count, int **&weight_array) {
    int query_vertices_num = query_graph.GetNodeNum();
    order = new int[query_vertices_num];
    pivot = new int[query_vertices_num];

    for (int i = 0; i < query_vertices_num; ++i) {
        order[i] = bfs_order[i];
    }

    for (int i = 1; i < query_vertices_num; ++i) {
        pivot[i] = tree[order[i]].parent_;
    }
    
    // 修改CFL生成的BFS树为DAG图
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

        auto u_iter=query_graph.GetNI(u);
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

    // Compute weight array.
    weight_array = new int*[query_vertices_num];
    for (int i = 0; i < query_vertices_num; ++i) {
        weight_array[i] = new int[candidates_count[i]];
        std::fill(weight_array[i], weight_array[i] + candidates_count[i], std::numeric_limits<int>::max());
    }

    for (int i = query_vertices_num - 1; i >= 0; --i) {
        int vertex = order[i];
        TreeNode& node = tree[vertex];
        bool set_to_one = true;

        for (int j = 0; j < node.fn_count_; ++j) {
            int child = node.fn_[j];
            TreeNode& child_node = tree[child];

            if (child_node.bn_count_ == 1) {
                set_to_one = false;
                Edges& cur_edge = *edge_matrix[vertex][child];
                for (int k = 0; k < candidates_count[vertex]; ++k) {
                    int cur_candidates_count = cur_edge.offset_[k + 1] - cur_edge.offset_[k];
                    int* cur_candidates = cur_edge.edge_ + cur_edge.offset_[k];

                    int weight = 0;

                    for (int l = 0; l < cur_candidates_count; ++l) {
                        int candidates = cur_candidates[l];
                        weight += weight_array[child][candidates];
                    }

                    if (weight < weight_array[vertex][k])
                        weight_array[vertex][k] = weight;
                }
            }
        }

        if (set_to_one) {
            std::fill(weight_array[vertex], weight_array[vertex] + candidates_count[vertex], 1);
        }
    }
}


size_t SubgraphMatch_Query::ExploreDPisoStyle(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree,Edges ***edge_matrix, int **candidates, int *candidates_count,int **weight_array, int *order, size_t output_limit_num,size_t &call_count) {
    int max_depth = query_graph.GetNodeNum();

    int *extendable = new int[max_depth];
    for (int i = 0; i < max_depth; ++i) {
        extendable[i] = tree[i].bn_count_;
    }


    // Allocate the memory buffer.
    int *idx;
    int *idx_count;
    int *embedding;
    int *idx_embedding;
    int *temp_buffer;
    int **valid_candidate_idx;
    bool *visited_vertices;
    allocateBuffer(data_graph, query_graph, candidates_count, idx, idx_count, embedding, idx_embedding,
                   temp_buffer, valid_candidate_idx, visited_vertices);

    // Evaluate the query.
    size_t embedding_cnt = 0;
    int cur_depth = 0;

    int start_vertex = order[0];
    std::vector<dpiso_min_pq> vec_rank_queue;

    for (int i = 0; i < candidates_count[start_vertex]; ++i) {
        int v = candidates[start_vertex][i];
        embedding[start_vertex] = v;
        idx_embedding[start_vertex] = i;
        visited_vertices[v] = true;

        vec_rank_queue.emplace_back(dpiso_min_pq(extendable_vertex_compare));
        updateExtendableVertex(idx_embedding, idx_count, valid_candidate_idx, edge_matrix, temp_buffer, weight_array,
                               tree, start_vertex, extendable,
                               vec_rank_queue, query_graph);

        int u = vec_rank_queue.back().top().first.first;
        vec_rank_queue.back().pop();


        call_count += 1;
        cur_depth += 1;
        order[cur_depth] = u;
        idx[u] = 0;
        while (cur_depth > 0) {
            while (idx[u] < idx_count[u]) {
                int valid_idx = valid_candidate_idx[u][idx[u]];
                v = candidates[u][valid_idx];

                if (visited_vertices[v]) {
                    idx[u] += 1;
                    continue;
                }
                embedding[u] = v;
                idx_embedding[u] = valid_idx;
                visited_vertices[v] = true;
                idx[u] += 1;


                if (cur_depth == max_depth - 1) {
                    embedding_cnt += 1;
                    visited_vertices[v] = false;
                    if (embedding_cnt >= output_limit_num) {
                        goto EXIT;
                    }
                } else {
                    call_count += 1;
                    cur_depth += 1;
                    vec_rank_queue.emplace_back(vec_rank_queue.back());
                    updateExtendableVertex(idx_embedding, idx_count, valid_candidate_idx, edge_matrix, temp_buffer,
                                           weight_array, tree, u, extendable,
                                           vec_rank_queue, query_graph);

                    u = vec_rank_queue.back().top().first.first;
                    vec_rank_queue.back().pop();
                    idx[u] = 0;
                    order[cur_depth] = u;

                }
            }

            cur_depth -= 1;
            vec_rank_queue.pop_back();
            u = order[cur_depth];
            visited_vertices[embedding[u]] = false;
            restoreExtendableVertex(tree, u, extendable);
        }
    }

    // Release the buffer.
    EXIT:
    releaseBuffer(max_depth, idx, idx_count, embedding, idx_embedding, temp_buffer, valid_candidate_idx,
                  visited_vertices);

    return embedding_cnt;
}
void SubgraphMatch_Query::updateExtendableVertex(int *idx_embedding, int *idx_count, int **valid_candidate_index, Edges ***edge_matrix, int *&temp_buffer, int **weight_array, TreeNode *tree, int mapped_vertex, int *extendable, std::vector<dpiso_min_pq> &vec_rank_queue, const UNDirect_UNWeight_Graph& query_graph) {
    TreeNode &node = tree[mapped_vertex];
    for (int i = 0; i < node.fn_count_; ++i) {
        int u = node.fn_[i];
        extendable[u] -= 1;
        if (extendable[u] == 0) {
            generateValidCandidateIndex(u, idx_embedding, idx_count, valid_candidate_index[u], edge_matrix, tree[u].bn_,
                                        tree[u].bn_count_, temp_buffer);

            int weight = 0;
            for (int j = 0; j < idx_count[u]; ++j) {
                int idx = valid_candidate_index[u][j];
                weight += weight_array[u][idx];
            }
            vec_rank_queue.back().emplace(std::make_pair(std::make_pair(u, query_graph.GetNI(u).GetDeg()), weight));
        }
    }
}
void SubgraphMatch_Query::generateValidCandidateIndex(int u, int *idx_embedding, int *idx_count, int *&valid_candidate_index,Edges ***edge_matrix, int *bn, int bn_cnt, int *&temp_buffer) {
    int previous_bn = bn[0];
    Edges &previous_edge = *edge_matrix[previous_bn][u];
    int previous_index_id = idx_embedding[previous_bn];

    int previous_candidates_count =
            previous_edge.offset_[previous_index_id + 1] - previous_edge.offset_[previous_index_id];
    int *previous_candidates = previous_edge.edge_ + previous_edge.offset_[previous_index_id];

    int valid_candidates_count = 0;
    for (int i = 0; i < previous_candidates_count; ++i) {
        valid_candidate_index[valid_candidates_count++] = previous_candidates[i];
    }

    int temp_count;
    for (int i = 1; i < bn_cnt; ++i) {
        int current_bn = bn[i];
        Edges &current_edge = *edge_matrix[current_bn][u];
        int current_index_id = idx_embedding[current_bn];

        int current_candidates_count =
                current_edge.offset_[current_index_id + 1] - current_edge.offset_[current_index_id];
        int *current_candidates = current_edge.edge_ + current_edge.offset_[current_index_id];

        SubgraphMatch_GraphOperations::ComputeCandidateSetIntersect(current_candidates, current_candidates_count, valid_candidate_index, valid_candidates_count, temp_buffer, temp_count);

        std::swap(temp_buffer, valid_candidate_index);
        valid_candidates_count = temp_count;
    }

    idx_count[u] = valid_candidates_count;
}

void SubgraphMatch_Query::restoreExtendableVertex(TreeNode *tree, int unmapped_vertex,int *extendable) {
    TreeNode &node = tree[unmapped_vertex];
    for (int i = 0; i < node.fn_count_; ++i) {
        int u = node.fn_[i];
        extendable[u] += 1;
    }
}





void SubgraphMatch_Query::GenerateCRDPQueryPlan(const UNDirect_UNWeight_Graph& query_graph, int *&order, TreeNode *tree,int *bfs_order) {
    int query_vertices_num = query_graph.GetNodeNum();
    order = new int[query_vertices_num];

    for (int i = 0; i < query_vertices_num; ++i) {
        order[i] = bfs_order[i];
    }
    
    // 修改CFL生成的BFS树为DAG图
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

        auto u_iter=query_graph.GetNI(u);
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


size_t SubgraphMatch_Query::ExploreCRDPStyle(const UNDirect_UNWeight_Graph& data_graph, const UNDirect_UNWeight_Graph& query_graph, TreeNode *tree, int **candidates, int *candidates_count, int *order, size_t output_limit_num,size_t &call_count) {
    int max_depth = query_graph.GetNodeNum();

    int *extendable = new int[max_depth];
    for (int i = 0; i < max_depth; ++i) {
        extendable[i] = tree[i].bn_count_;
    }


    // Allocate the memory buffer.
    int *idx;
    int *idx_count;
    int *embedding;
    int *idx_embedding;
    int *temp_buffer;
    int **valid_candidate;
    bool *visited_vertices;
    allocateBuffer(data_graph, query_graph, candidates_count, idx, idx_count, embedding, idx_embedding,
                   temp_buffer, valid_candidate, visited_vertices);

    // Evaluate the query.
    size_t embedding_cnt = 0;
    int cur_depth = 0;

    int start_vertex = order[0];
    std::vector<dpiso_min_pq> vec_rank_queue;

    for (int i = 0; i < candidates_count[start_vertex]; ++i) {
        int v = candidates[start_vertex][i];
        embedding[start_vertex] = v;
        idx_embedding[start_vertex] = i;
        visited_vertices[v] = true;

        vec_rank_queue.emplace_back(dpiso_min_pq(extendable_vertex_compare));
        updateExtendableVertex(embedding, candidates, candidates_count, idx_count, valid_candidate, tree, start_vertex, extendable, vec_rank_queue, query_graph, data_graph);

        int u = vec_rank_queue.back().top().first.first;
        vec_rank_queue.back().pop();


        call_count += 1;
        cur_depth += 1;
        order[cur_depth] = u;
        idx[u] = 0;
        while (cur_depth > 0) {
            while (idx[u] < idx_count[u]) {
                v = valid_candidate[u][idx[u]];

                if (visited_vertices[v]) {
                    idx[u] += 1;
                    continue;
                }
                embedding[u] = v;
                visited_vertices[v] = true;
                idx[u] += 1;


                if (cur_depth == max_depth - 1) {
                    embedding_cnt += 1;
                    visited_vertices[v] = false;
                    if (embedding_cnt >= output_limit_num) {
                        goto EXIT;
                    }
                } else {
                    call_count += 1;
                    cur_depth += 1;
                    vec_rank_queue.emplace_back(vec_rank_queue.back());
                    updateExtendableVertex(embedding, candidates, candidates_count, idx_count, valid_candidate, tree, u, extendable, vec_rank_queue, query_graph, data_graph);

                    u = vec_rank_queue.back().top().first.first;
                    vec_rank_queue.back().pop();
                    idx[u] = 0;
                    order[cur_depth] = u;

                }
            }

            cur_depth -= 1;
            vec_rank_queue.pop_back();
            u = order[cur_depth];
            visited_vertices[embedding[u]] = false;
            restoreExtendableVertex(tree, u, extendable);
        }
    }

    // Release the buffer.
    EXIT:
    releaseBuffer(max_depth, idx, idx_count, embedding, idx_embedding, temp_buffer, valid_candidate,
                  visited_vertices);
    
    delete[] extendable;
    return embedding_cnt;
}
void SubgraphMatch_Query::updateExtendableVertex(int *embedding,int** candidates,int* candidates_count, int *idx_count, int **valid_candidate,  TreeNode *tree, int mapped_vertex, int *extendable, std::vector<dpiso_min_pq> &vec_rank_queue, const UNDirect_UNWeight_Graph& query_graph, const UNDirect_UNWeight_Graph& data_graph) {
    TreeNode &node = tree[mapped_vertex];
    for (int i = 0; i < node.fn_count_; ++i) {
        int u = node.fn_[i];
        extendable[u] -= 1;
        if (extendable[u] == 0) {
            generateValidCandidates(u, embedding, candidates, candidates_count, idx_count, valid_candidate[u], tree[u].bn_, tree[u].bn_count_, data_graph);

            vec_rank_queue.back().emplace(std::make_pair(std::make_pair(u, query_graph.GetNI(u).GetDeg()), idx_count[u]));
        }
    }
}
void SubgraphMatch_Query::generateValidCandidates(int u, int *embedding,  int** candidates,  int* candidates_count,int *idx_count, int *valid_candidate, int *bn, int bn_cnt, const UNDirect_UNWeight_Graph& data_graph) {
    
    idx_count[u] = 0;

    for (int i = 0; i < candidates_count[u]; ++i) {
        int v = candidates[u][i];
        bool valid = true;

        for (int j = 0; j < bn_cnt; ++j) {
            int u_nbr = bn[j];
            int u_nbr_v = embedding[u_nbr];

            if (!data_graph.IsEdge(v, u_nbr_v)) {
                valid = false;
                break;
            }
        }

        if (valid) {
                valid_candidate[idx_count[u]++] = v;
        }
        
    }
}





#endif /* SubgraphMatch_Query_h */
