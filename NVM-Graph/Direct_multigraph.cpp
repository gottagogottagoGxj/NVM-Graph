//
//  Direct_multigraph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/14.
//

#include"Direct_multigraph.h"

Direct_multigraph::Direct_multigraph(Arena* node,Arena* edge):Nodetable(node),EdgeTable(edge),NodeHash(32,4),EdgeHash(32,4),MxNid(1),MxEid(1),NodeNum(0),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0),FreeEdgeNum(0),FreeEdgeLocation(0){
    if(Nodetable->EndPtr()!=Nodetable->BeginPtr()){
        
    }
    
}





