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
using namespace std;
int atoi(const char* decString) {
int length=strlen(decString);
int quan=1;
int result=0;
for(int i=length-1;i>=0;i--){
int cur=0;
switch(decString[i]){
    case '0': cur=0;break;
    case '1': cur=1;break;
    case '2': cur=2;break;
    case '3': cur=3;break;
    case '4': cur=4;break;
    case '5': cur=5;break;
    case '6': cur=6;break;
    case '7': cur=7;break;
    case '8': cur=8;break;
    case '9': cur=9;break;
}
    result+=cur*quan;
    quan*=10;

}
    return result;

}
typedef struct Node{
    unsigned int data;  //节点数据
    unsigned int nextSize;//后继节点数量
    struct Node **     next; //保存后继节点指针的数组
} *GraphNode;

int calculateMcCabe(GraphNode root){
    int result=0;
    set<GraphNode> curset;
    queue<GraphNode> q;
    q.push(root);
    while(!q.empty()){
        int length=q.size();
        for(int i=0;i<length;++i){
            curset.insert(q.front());
            if(q.front()->nextSize>1) result+=1;
            for(int j=0;j<q.front()->nextSize;++j){
                if(curset.find(q.front()->next[j])==curset.end())
                   {   q.push(q.front()->next[j]);
                       curset.insert(q.front()->next[j]);
                   }
            }
            q.pop();
        }
        
    }
    
    return result;
}




int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World yes !\n";
    string s="123";
    int sum=atoi(s.c_str());
    cout<<sum;
    return 0;
}
