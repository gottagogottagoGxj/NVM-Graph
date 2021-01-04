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
#include "UNDerict_Graph.h"
#include "Direct_graph.h"
#include"Direct_multigraph.h"
#include"Attr.h"
using namespace std;



int main(){
    Arena a(10,10240),b(10),c(10);
    Comparator<AttrIndexKey> compare;
    Attr attr(&a,compare,&b,&c);
    attr.AddAttrName("Name");
    attr.AddAttrName("Age");
    attr.AddAttrName("Sex");
    attr.AddAttrDat(1, "Name", "WangTingting");
    attr.AddAttrDat(1, "Age", "23");
    attr.AddAttrDat(1, "Sex", "Female");
    char data[100];
    attr.GetAttrDat(1, "Name", data);
    cout<<data<<endl;
    attr.GetAttrDat(1, "Age", data);
    cout<<data<<endl;
    attr.GetAttrDat(1, "Sex", data);
    cout<<data<<endl;
    attr.DelAttrDat(1);
    bool flag=attr.GetAttrDat(1, "Name", data);
    cout<<flag<<endl;
    
    return 0;
}
