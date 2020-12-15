//
//  Derict_Graph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/12/1.
//

//#include"Derict_Graph.h"

/*Derict_graph::Derict_graph(Arena* arena):NodeTable(arena),NodeHash(32, 4),MxNid(1),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0){
    if(NodeTable->EndPtr()!=NodeTable->BeginPtr()){
        NvmNode* temp=(NvmNode*)(NodeTable->BeginPtr());
        const char* end=NodeTable->EndPtr();
        while((char*)temp<end){
            if(temp->GetFlag()==-1){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=GetNodeLocation(temp);
                FreeNodeNum++;
            }
            else{
                if(temp->GetFlag()==1){
                    uint64_t location=GetNodeLocation(temp);
                    NodeHash.Add(temp->GetId(), location);
                    MxNid=MxNid>(temp->GetId()+1)?MxNid:(temp->GetId()+1);
                }
                EdgeNum+=temp->GetOutDeg();
            }
            temp++;
        }
    }
    
}

int Derict_graph::AddNode(const int nid,const char* data){
    int curnid;
    uint64_t curlocation;
    if(nid==-1){curnid=MxNid;MxNid++;}
    else{
        if(NidIsExist(nid)) return -1;
        if(nid==MxNid) MxNid++;
        curnid=nid;
    }
    if(FreeNodeNum>0){
        curlocation=FreeNodeLocation;
        NvmNode* old=GetNodePtr(FreeNodeLocation);
        FreeNodeLocation=old->GetNextNodeAddre();
        FreeNodeNum--;
        char* node=NodeTable->HeadPtr()+curlocation;
        new(node) NvmNode(curnid,data);
    }
    else{
        curlocation=NodeTable->EndPtr()-NodeTable->BeginPtr();
        new(NodeTable) NvmNode(curnid,data);
    }
    NodeHash.Add(curnid, curlocation);
    return curnid;
}

void Derict_graph::DeleteNode(const uint64_t &location){
    NvmNode* curnode=GetNodePtr(location);
    curnode->SetNextNodeAddre(FreeNodeLocation);
    curnode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Derict_graph::DelNode(const int nid){
    uint64_t location;
    if(NodeHash.Find(nid, location)){
        NvmNode* temp=GetNodePtr(location);
        while(temp->GetNextNodeAddre()!=0){
            uint64_t old=location;
            location=temp->GetNextNodeAddre();
            temp=GetNodePtr(location);
            DeleteNode(old);
        }
        DeleteNode(location);
    }
}

Derict_graph::NvmNode* Derict_graph::AddExistNode(const int &nid,const int& flag){
    NvmNode* newnode;
    if(FreeNodeNum>0){
        char* curptr=NodeTable->HeadPtr()+FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        FreeNodeNum--;
        newnode=new(curptr)NvmNode(nid,flag);
    }
    else{
        newnode=new(NodeTable)NvmNode(nid,flag);
    }
    return newnode;
}

void Derict_graph::AddInEdgeToNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0){
        if(curnode->AddInNid(nid)){
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddInNid(nid))
    {
        NvmNode* newnode=AddExistNode(curnode->GetId());
        newnode->AddInNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

void Derict_graph::AddOutEdgeToNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0)
    {
        if(curnode->AddOutNid(nid))
        {
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddOutNid(nid))
    {
        NvmNode* newnode=AddExistNode(curnode->GetId());
        newnode->AddOutNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

int Derict_graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return -1;
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid) AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}
int Derict_graph::AddEdge2(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    if(SrcNid!=DstNid) AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}

void Derict_graph::AddEdgeUnCheck(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
}

void Derict_graph::DelInEdgeOfNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    NvmNode* prevnode=curnode;
    while(!curnode->DeleteInNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Derict_graph::DelOutEdgeOfNode(const uint64_t &location, const int &nid){
    NvmNode* curnode=GetNodePtr(location);
    NvmNode* prevnode=curnode;
    while(!curnode->DeleteOutNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Derict_graph::DelEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) return;
    DelOutEdgeOfNode(location1, DstNid);
    DelInEdgeOfNode(location2, SrcNid);
    EdgeNum--;
}
*/
