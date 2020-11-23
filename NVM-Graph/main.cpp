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
#include"Arena.h"
#include"hashtable.h"
#include"MurmurHash.h"
using namespace std;


int main(){
    int num=32;
    CuckooHash::HashTable hash(num,4);
    for(int i=1;i<1000;i++){
        if(!hash.Add(i, i)){
            cout<<i<<endl;
            num*=2;
            while (!hash.ReSize(num)) {
                num*=2;
            }
            cout<<hash.GetBucketNum()<<endl;
        }
    }
    cout<<hash.LoadFactor()<<endl;
    for(int i=1;i<1000;++i){
        uint64_t location=0;
        hash.Find(i, location);
        cout<<location<<endl;
    }
    return 0;
}
