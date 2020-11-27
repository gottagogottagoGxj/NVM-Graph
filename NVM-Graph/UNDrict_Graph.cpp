//
//  UNDrict_Graph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/27.
//

#include"UNDrict_Graph.h"
UNDerict_Graph::UNDerict_Graph(Arena* arena): NodeTable(arena),NodeHash(32,4){
    if(NodeTable->EndPtr()==NodeTable->BeginPtr()){
        HeadNode=AddExistNode(0);
        MxNid=1;
        EdgeNum=0;
        FreeNodeLocation=0;
        FreeNodeNum=0;
    }
    else{
        HeadNode=new(NodeTable->HeadPtr()) NvmNode(0);
        NvmNode* temp=HeadNode+1;
        const char* end=NodeTable->EndPtr();
        const char* begin=NodeTable->BeginPtr();
        while((char*)temp<end){
            if(!temp->GetFlag()){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=(char*)temp-begin;
                FreeNodeNum++;
            }
            else {
                int nid=temp->GetId();
                if(temp->GetPrevNodeAddre()==0){
                    uint64_t location=(char*)temp-begin;
                    NodeHash.Add(nid,location);
                    MxNid=MxNid>nid?MxNid:nid;
                }
                int deg=temp->GetDeg();
                for(int i=0;i<deg;++i){
                    if(temp->GetNbrNid(i)>=nid) EdgeNum++;
                }
            }
        }
    }
}
