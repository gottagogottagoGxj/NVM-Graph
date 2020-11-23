//
//  InOut.h
//  NVM-Graph
//
//  Created by 江清月近人 on 2020/11/19.
//

#ifndef InOut_h
#define InOut_h
// 将数据存入内存或NVM
class MIn{
private:
    char * Bf;
    uint BfC,BfL;
public:
    MIn(char* _Bf, const uint _BfL):Bf(_Bf),BfL(_BfL),BfC(0){}
    int Len() const {return BfC;}
    int MaxLen()const{return BfL;}
    bool PutBf(const void* _Bf, const uint & _BfL);
    
    bool Save(const void* bf,const uint& bfl){return PutBf(bf, bfl);}
    bool Save(const int& Int){return PutBf(&Int, sizeof(int));}
    bool Save(const bool & Bool){return PutBf(&Bool, sizeof(bool));}
    bool Save(const uint& UInt){return PutBf(&UInt, sizeof(uint));}
    bool Save(const char& Ch){return PutBf(&Ch, sizeof(char));}
    bool Save(const int* IntV, const uint& length){return PutBf(IntV, sizeof(int)*length); }
    bool Save(const char* ChV,const uint& length){return PutBf(ChV,length);}
    bool Save(const uint64_t& UInt64){return PutBf(&UInt64, sizeof(uint64_t));}
};
bool MIn::PutBf(const void *_Bf, const uint &_BfL){
    if(BfC+_BfL>BfL) return false;
    for(int i=0;i<_BfL;++i){
        Bf[BfC++]=((char*)_Bf)[i];
    }
    return true;
}

//将内存/NVM中数据读取到类型对象
class MOut{
private:
    const char * Bf;
    uint BfC;
    const uint BfL;
public:
    MOut(const char* _Bf, const uint& _BfL):Bf(_Bf),BfL(_BfL),BfC(0){}
    int Len() const {return BfC;}
    int MaxLen()const{return BfL;}
    bool GetBf(void* _Bf,const uint& _BfL);
    bool Load(int & Int){return GetBf(&Int, sizeof(int));}
    bool Load(bool & Bool){return GetBf(&Bool, sizeof(bool));}
    bool Load(uint & UInt){return GetBf(&UInt, sizeof(uint));}
    bool Load(void* bf, const uint& bfl){return GetBf(bf, bfl);}
    bool Load(int * IntV, const uint& length){
        return GetBf(IntV, sizeof(int)*length);
    }
    bool Load(char* ChV,const uint& length){
        return GetBf(ChV, length);
    }
    bool Load(uint64_t& UInt64){return GetBf(&UInt64, sizeof(uint64_t));}
};
bool MOut::GetBf(void *_Bf, const uint &_BfL){
    if(BfC+_BfL>BfL) return false;
    for(int i=0;i<_BfL;++i){
        ((char*)_Bf)[i]=Bf[BfC++];
    }
    return true;
}

#endif /* InOut_h */
