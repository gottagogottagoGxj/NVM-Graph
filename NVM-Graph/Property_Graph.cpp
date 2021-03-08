//
//  Property_Graph.cpp
//  NVM-Graph
//
//  Created by 江清月近人 on 2021/3/7.
//

/*#include "Property_Graph.h"

inline Property_Graph::Node::Node(const int nid,const char* data):Nid(nid),InNidVNum(0),OutNidVNum(0),Flag(1),NextNode(0){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline Property_Graph::Node::Node(MOut& Out){
    Out.Load(Nid);
    Out.Load(Data, NodeDatalengthDef);
    Out.Load(InNidVNum);
    Out.Load(OutNidVNum);
    Out.Load(InNidV, InOutEdgeNumDef);
    Out.Load(OutNidV, InOutEdgeNumDef);
    Out.Load(Flag);
    Out.Load(NextNode);
}
inline void Property_Graph::Node::Save(MIn& In){
    In.Save(Nid);
    In.Save(Data, NodeDatalengthDef);
    In.Save(InNidVNum);
    In.Save(OutNidVNum);
    In.Save(InNidV, InOutEdgeNumDef);
    In.Save(OutNidV, InOutEdgeNumDef);
    In.Save(Flag);
    In.Save(NextNode);
}
inline void Property_Graph::Node::SetData(const char* data){
    size_t length=strlen(data);
    if(length>=NodeDatalengthDef) length=NodeDatalengthDef-1;
    memcpy(Data, data, length);
    Data[length]='\0';
}
inline bool Property_Graph::Node::IsInNid(const int& nid)const{
    for(int i=0;i<InNidVNum;++i){
        if(InNidV[i]==nid) return true;
    }
    return false;
}
inline bool Property_Graph::Node::IsOutNid(const int& nid)const{
    for(int i=0;i<OutNidVNum;++i){
        if(OutNidV[i]==nid) return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddInNid(const int& nid){
    if(InNidVNum<InOutEdgeNumDef){
        InNidV[InNidVNum]=nid;
        InNidVNum++;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddOutNid(const int& nid){
    if(OutNidVNum<InOutEdgeNumDef){
        OutNidV[OutNidVNum]=nid;
        OutNidVNum++;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddInNid(const int* nid,const int& nidN){
    if(InNidVNum+nidN<=InOutEdgeNumDef){
        memcpy(InNidV, nid, sizeof(int)*nidN);
        InNidVNum+=nidN;
        return true;
    }
    return false;
}
inline bool Property_Graph::Node::AddOutNid(const int* nid,const int& nidN){
    if(OutNidVNum+nidN<=InOutEdgeNumDef){
        memcpy(OutNidV, nid, sizeof(int)*nidN);
        OutNidVNum+=nidN;
        return true;
    }
    return false;
}
inline void Property_Graph::Node::DeleteInNidN(const int & NodeN){
    for(int i=NodeN;i<(InNidVNum-1);++i) {InNidV[i]=InNidV[i+1];}
    InNidVNum--;
}
inline void Property_Graph::Node::DeleteOutNidN(const int& NodeN){
    for(int i=NodeN;i<(OutNidVNum-1);++i) {OutNidV[i]=OutNidV[i+1];}
    OutNidVNum--;
}
inline bool Property_Graph::Node::DeleteInNid(const int& nid){
    for(int i=0;i<InNidVNum;++i){
        if(InNidV[i]==nid){
            for(int j=i;j<(InNidVNum-1);++j){InNidV[j]=InNidV[j+1];}
            InNidVNum--;
            return true;
        }
    }
    return false;
}
inline bool Property_Graph::Node::DeleteOutNid(const int& nid){
    for(int i=0;i<OutNidVNum;++i){
        if(OutNidV[i]==nid){
            for(int j=i;j<(OutNidVNum-1);++j){OutNidV[j]=OutNidV[j+1];}
            OutNidVNum--;
            return true;
        }
    }
    return false;
}
bool Property_Graph::Node::AddInNidSort(const int& nid,const bool asc){
    if(InNidVNum<InOutEdgeNumDef){
        int index=SearchBinRight(InNidV, 0, InNidVNum, nid, Comparison(asc));
        for(int i=InNidVNum;i>index;--i) InNidV[i]=InNidV[i-1];
        InNidV[index]=nid;
        InNidVNum++;
        return true;
    }
    return false;
}
bool Property_Graph::Node::AddOutNidSort(const int& nid,const bool asc){
    if(OutNidVNum<InOutEdgeNumDef){
        int index=SearchBinRight(OutNidV, 0, OutNidVNum, nid, Comparison(asc));
        for(int i=OutNidVNum;i>index;--i) OutNidV[i]=OutNidV[i-1];
        OutNidV[index]=nid;
        OutNidVNum++;
        return true;
    }
    return false;
}
bool Property_Graph::Node::DeleteInNidSort(const int& nid,const bool asc){
    int index=SearchBin(InNidV, 0, InNidVNum, nid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<InNidVNum;++i) InNidV[i]=InNidV[i+1];
        InNidVNum--;
        return  true;
    }
    return false;
}
bool Property_Graph::Node::DeleteOutNidSort(const int& nid,const bool asc){
    int index=SearchBin(OutNidV, 0, OutNidVNum, nid, Comparison(asc));
    if(index>=0){
        for(int i=index;i<OutNidVNum;++i) OutNidV[i]=OutNidV[i+1];
        OutNidVNum--;
        return true;
    }
    return false;
}
int Property_Graph::NodeIter::GetInDeg()const{
    int InDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        InDeg+=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    InDeg+=temp->GetNextNodeAddre();
    return InDeg;
}
int Property_Graph::NodeIter::GetOutDeg()const{
    int OutDeg=0;
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        OutDeg+=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    OutDeg+=temp->GetNextNodeAddre();
    return OutDeg;
}
bool Property_Graph::NodeIter::IsNbrNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsNbrNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsNbrNid(nid);
}
bool Property_Graph::NodeIter::IsInNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsInNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNid(nid);
}
bool Property_Graph::NodeIter::IsOutNid(const int& nid)const{
    Node* temp=CurNode;
    while(temp->GetNextNodeAddre()!=0){
        if(temp->IsOutNid(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutNid(nid);
}
int Property_Graph::NodeIter::GetInNid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while(n>temp->GetInDeg() && temp->GetNextNodeAddre()!=0){
        n-=temp->GetInDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetInDeg()) return -1;
    return temp->GetInNid(n-1);
}
int Property_Graph::NodeIter::GetOutNid(const int edgeN)const{
    int n=edgeN;
    Node* temp=CurNode;
    while (n>temp->GetOutDeg()&& temp->GetNextNodeAddre()!=0) {
        n-=temp->GetOutDeg();
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    if(n>temp->GetOutDeg()) return -1;
    return temp->GetOutNid(n-1);
}
bool Property_Graph::NodeIter::IsNbrNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInNidSort(nid)||temp->IsOutNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNidSort(nid)||temp->IsOutNidSort(nid);
}
bool Property_Graph::NodeIter::IsInNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsInNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsInNidSort(nid);
}
bool Property_Graph::NodeIter::IsOutNidSort(const int& nid)const{
    Node* temp=CurNode;
    while (temp->GetNextNodeAddre()!=0) {
        if(temp->IsOutNidSort(nid)) return true;
        temp=(Node*)(Begin+temp->GetNextNodeAddre());
    }
    return temp->IsOutNidSort(nid);
}

Property_Graph::Property_Graph(Arena* arena,Arena* AttrNameN,Arena* AttrIndexN,Arena* AttrValueN,Arena* AttrNameE,Arena* AttrIndexE,Arena* AttrValueE):NodeTable(arena),NodeHash(32, 4),MxNid(1),NodeNum(0),EdgeNum(0),FreeNodeNum(0),FreeNodeLocation(0),AttrN(AttrNameN, AttrIndexN, AttrValueN),AttrE(AttrNameE, AttrIndexE, AttrValueE){
    if(NodeTable->EndPtr()!=NodeTable->BeginPtr()){
        Node* temp=(Node*)(NodeTable->BeginPtr());
        const char* end=NodeTable->EndPtr();
        while((char*)temp<end){
            if(temp->GetFlag()==-1){
                temp->SetNextNodeAddre(FreeNodeLocation);
                FreeNodeLocation=GetNodeLocation(temp);
                FreeNodeNum++;
            }
            else{
                if(temp->GetFlag()==1){
                    NodeNum++;
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

int Property_Graph::AddNode(const int nid,const char* data){
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
        Node* old=GetNodePtr(FreeNodeLocation);
        FreeNodeLocation=old->GetNextNodeAddre();
        FreeNodeNum--;
        char* node=NodeTable->HeadPtr()+curlocation;
        new(node) Node(curnid,data);
    }
    else{
        curlocation=NodeTable->EndPtr()-NodeTable->BeginPtr();
        new(NodeTable) Node(curnid,data);
    }
    NodeHash.Add(curnid, curlocation);
    NodeNum++;
    return curnid;
}
Property_Graph::Node* Property_Graph::AddExistNode(const int &nid,const int& flag){
    Node* newnode;
    if(FreeNodeNum>0){
        char* curptr=NodeTable->HeadPtr()+FreeNodeLocation;
        FreeNodeLocation=GetNodePtr(FreeNodeLocation)->GetNextNodeAddre();
        FreeNodeNum--;
        newnode=new(curptr)Node(nid,flag);
    }
    else{
        newnode=new(NodeTable)Node(nid,flag);
    }
    return newnode;
}

void Property_Graph::DeleteNode(const uint64_t &location){
    Node* curnode=GetNodePtr(location);
    curnode->SetNextNodeAddre(FreeNodeLocation);
    curnode->SetFlag(-1);
    FreeNodeLocation=location;
    FreeNodeNum++;
}
void Property_Graph::DelNode(const int nid){
    uint64_t location;
    if(!NodeHash.Find(nid, location)) return;
    Node* temp=GetNodePtr(location);
    do{
        for(int i=0;i<temp->GetInDeg();++i){
            int SrcNid=temp->GetInNid(i);
            uint64_t SrcNodeLocation;
            if(NodeHash.Find(SrcNid, SrcNodeLocation)){
                DelOutEdgeOfNode(SrcNodeLocation, nid);
                AttrE.DelAttrDat(SrcNid, nid);
            }
        }
        for(int i=0;i<temp->GetOutDeg();++i){
            int DstNid=temp->GetOutNid(i);
            uint64_t DstNodeLocation;
            if(NodeHash.Find(DstNid, DstNodeLocation)){
                DelInEdgeOfNode(DstNodeLocation, nid);
                AttrE.DelAttrDat(nid, DstNid);
            }
        }
        uint64_t old=location;
        location=temp->GetNextNodeAddre();
        temp=GetNodePtr(location);
        DeleteNode(old);
    }while(temp->GetNextNodeAddre()!=0);
    AttrN.DelAttrDat(nid);
    NodeHash.Delete(nid);
    NodeNum--;
}



void Property_Graph::AddInEdgeToNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    while(curnode->GetNextNodeAddre()!=0){
        if(curnode->AddInNid(nid)){
            //curnode->AddInNidSort(nid)
            return;
        }
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(!curnode->AddInNid(nid))
    {
        Node* newnode=AddExistNode(curnode->GetId());
        newnode->AddInNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

void Property_Graph::AddOutEdgeToNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
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
        Node* newnode=AddExistNode(curnode->GetId());
        newnode->AddOutNid(nid);
        curnode->SetNextNodeAddre(GetNodeLocation(newnode));
    }
}

int Property_Graph::AddEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) ||!NodeHash.Find(DstNid, location2)) return -1;
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}
int Property_Graph::AddEdge2(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1)){AddNode(SrcNid);NodeHash.Find(SrcNid, location1);}
    if(!NodeHash.Find(DstNid, location2)){AddNode(DstNid);NodeHash.Find(DstNid, location2);}
    if(GetNI(SrcNid).IsOutNid(DstNid)) return 0;
    AddOutEdgeToNode(location1, DstNid);
    AddInEdgeToNode(location2, SrcNid);
    EdgeNum++;
    return 1;
}



void Property_Graph::DelInEdgeOfNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    Node* prevnode=curnode;
    while(!curnode->DeleteInNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Property_Graph::DelOutEdgeOfNode(const uint64_t &location, const int &nid){
    Node* curnode=GetNodePtr(location);
    Node* prevnode=curnode;
    while(!curnode->DeleteOutNid(nid) && curnode->GetNextNodeAddre()!=0){
        prevnode=curnode;
        curnode=GetNodePtr(curnode->GetNextNodeAddre());
    }
    if(curnode->GetFlag()==0 && curnode->GetDeg()==0){
        prevnode->SetNextNodeAddre(curnode->GetNextNodeAddre());
        DeleteNode(GetNodeLocation(curnode));
    }
}
void Property_Graph::DelEdge(const int &SrcNid, const int &DstNid){
    uint64_t location1,location2;
    if(!NodeHash.Find(SrcNid, location1) || !NodeHash.Find(DstNid, location2)) return;
    DelOutEdgeOfNode(location1, DstNid);
    DelInEdgeOfNode(location2, SrcNid);
    AttrE.DelAttrDat(SrcNid, DstNid);
    EdgeNum--;
}

inline const char* Property_Graph::GetNData(const int &Nid) const{
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        return GetNodePtr(location)->GetData();
    }
    return NULL;
}
void Property_Graph::SetNData(const int &Nid, const char *Data){
    uint64_t location;
    if(NodeHash.Find(Nid, location)){
        GetNodePtr(location)->SetData(Data);
    }
}
bool Property_Graph::AddAttrDatN(const int &Nid, const int &AttrId, const char *Val){
    if(!NidIsExist(Nid)) return false;
    return AttrN.AddAttrDat(Nid, AttrId, Val);
}
bool Property_Graph::AddAttrDatN(const int &Nid, const char *AttrName, const char *Val){
    if(NidIsExist(Nid)) return false;
    return AttrN.AddAttrDat(Nid, AttrName, Val);
}
bool Property_Graph::GetAttrDatN(const int &Nid, const int &AttrId, char *Val)const{
    if(NidIsExist(Nid)) return false;
    return AttrN.GetAttrDat(Nid, AttrId, Val);
}
bool Property_Graph::GetAttrDatN(const int& Nid,const char* AttrName,char* Val)const{
    if(!NidIsExist(Nid)) return false;
    return AttrN.GetAttrDat(Nid, AttrName, Val);
}
void Property_Graph::DelAttrDatN(const int &Nid, const int &AttrId){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid, AttrId);}
}
void Property_Graph::DelAttrDatN(const int &Nid, const char *AttrName){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid, AttrName);}
}
void Property_Graph::DelAttrDatN(const int &Nid){
    if(NidIsExist(Nid)){AttrN.DelAttrDat(Nid);}
}

bool Property_Graph::AddAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId, const char *Val){
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.AddAttrDat(SrcNid, DstNid, AttrId, Val);
}
bool Property_Graph::AddAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName, const char *Val){
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.AddAttrDat(SrcNid, DstNid, AttrName, Val);
}
bool Property_Graph::GetAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId, char *Val)const{
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.GetAttrDat(SrcNid, DstNid, AttrId, Val);
}
bool Property_Graph::GetAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName, char *Val)const{
    if(!EdgeIsExist(SrcNid, DstNid)) return false;
    return AttrE.GetAttrDat(SrcNid, DstNid, AttrName, Val);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid, const int &AttrId){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid, AttrId);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid, const char *AttrName){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid, AttrName);
}
void Property_Graph::DelAttrDatE(const int &SrcNid, const int &DstNid){
    if(EdgeIsExist(SrcNid, DstNid)) AttrE.DelAttrDat(SrcNid, DstNid);
}
*/
