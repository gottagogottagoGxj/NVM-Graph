//
//  UNDrict_Graph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/27.
//

#include"UNDrict_Graph.h"
UNDerict_Graph::UNDerict_Graph(Arena* arena): NodeTable(arena),NodeHash(32,4),MxNid(1){
    if(NodeTable->EndPtr()==NodeTable->BeginPtr()){
        HeadNode=AddExistNode();
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
                    MxNid=MxNid>(nid+1)?MxNid:(nid+1);
                }
                int deg=temp->GetDeg();
                for(int i=0;i<deg;++i){
                    if(temp->GetNbrNid(i)>=nid) EdgeNum++;
                }
            }
            temp++;
        }
    }
}
int UNDerict_Graph::AddNode(const int& nid){
    int newnid;
    if(nid==-1){newnid=MxNid;MxNid++;}
    else{
        if(IsNodeNid(nid)){return -1;}
        else{
            newnid=nid;
            MxNid=MxNid>(nid+1)?MxNid:(nid+1);
        }
    }
    uint64_t address;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        NvmNode* newnode=new(start) NvmNode(newnid);
        FreeNodeNum--;
        address=GetNodeLocation(newnode);
    }
    else{
        NvmNode * newnode=new(NodeTable) NvmNode(newnid);
        address=GetNodeLocation(newnode);
    }
    NodeHash.Add(newnid,address);
    return newnid;
}
int UNDerict_Graph::AddNode(const int& nid,const char* data){
    int newnid;
    if(nid==-1){newnid=MxNid;MxNid++;}
    else{
        if(IsNodeNid(nid)){return -1;}
        else{
            newnid=nid;
            MxNid=MxNid>(nid+1)?MxNid:(nid+1);
        }
    }
    uint64_t address;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        NvmNode* newnode=new(start) NvmNode(newnid,data);
        FreeNodeNum--;
        address=GetNodeLocation(newnode);
    }
    else{
        NvmNode * newnode=new(NodeTable) NvmNode(newnid,data);
        address=GetNodeLocation(newnode);
    }
    NodeHash.Add(newnid,address);
    return newnid;
}

UNDerict_Graph::NvmNode* UNDerict_Graph::AddExistNode(const int& nid,const uint64_t& prev){
    NvmNode* newnode;
    if(FreeNodeNum>0){
        uint64_t location=FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        char* start=NodeTable->HeadPtr()+location;
        newnode=new(start) NvmNode(nid,prev);
        FreeNodeNum--;
    }
    else{
        newnode=new(NodeTable) NvmNode(nid,prev);
    }
    return newnode;
}

void UNDerict_Graph::DeleteNode(const uint64_t &location){
    NvmNode* node=GetNodePtr(location);
    node->SetNextNodeAddre(FreeNodeLocation);
    node->SetFlag(false);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void UNDerict_Graph::DelNode(const int &nid){
    uint64_t location;
    if(NodeHash.Find(nid,location)){
        NvmNode* node=GetNodePtr(location);
        while(node->GetNextNodeAddre()!=0){
            uint64_t temp=node->GetNextNodeAddre();
            DeleteNode(location);
            location=temp;
            node=GetNodePtr(location);
        }
        DeleteNode(location);
    }
}

