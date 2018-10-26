/*
 *
 *   mem pool test
 *   create by tuligen on 2018.10.23
 *
 */

#include "MemoryPool.h"
#include "MemAllocConfig.h"

#include "poco/AutoPtr.h"
#include "poco/RefCountedObject.h"

#pragma region          memory pool

void MemoryPoolTest()
{
    const int testlen = 1e6 / 4;
    
    
    time_t now = clock();
    {
        std::list<int>                    defaultvt;
        for(int i = 0; i < testlen; ++i)
        {
            defaultvt.push_back(i);
        }
    }
    time_t cur = clock();
    cout << " default vector cast : " << (cur - now) /(float)CLOCKS_PER_SEC << endl;
    
    {
        const size_t blocklen = 4096;// sizeof(int) * testlen;
        std::list<int, MemoryPool<int,blocklen> >  memvt;
        for(int i = 0; i < testlen; ++i)
        {
            memvt.push_back(i);
        }
    }
    
    now = clock();
    cout << " memory vector cast : " << (now - cur) /(float)CLOCKS_PER_SEC << endl;
    
    cout << "thank you." << endl;
    
    
    //替换vector的内存效率 并没有提高多少
}

#pragma unregion        memory pool


#pragma region nedmalloc pool


class Object{Object(int x){}};
//ned object
class NedObject : public TLG::BaseAllocatedObject
{
public:
    
protected:
    int _x[100];
};
//nor obj
class NorObject
{
public:
    
protected:
    int x[100];
};


void NedPoolTest()
{
    const int testlen = 1e6;
    
    time_t now = clock();
    
    for(int i = 0; i < testlen; ++i)
    {
        NorObject *obj = new NorObject[10];
        delete [] obj;
    }
    
    time_t cur = clock();
    cout << " normal object cast : " << (cur - now) /(float)CLOCKS_PER_SEC << endl;
    
    for(int i = 0; i < testlen; ++i)
    {
        NedObject *obj = new NedObject[10];
        delete [] obj;
    }
    
    now = clock();
    cout << " ned    objecct cast : " << (now - cur) /(float)CLOCKS_PER_SEC << endl;
}


class A : public Poco::RefCountedObject
{
public:
    
};

void PocoTest()
{
//    Poco::AutoPtr<int> pt(new int(10));
    A *a = new A;
    Poco::AutoPtr<A> b(a,true);
    cout << a->referenceCount() << endl;
    
}

#pragma unregion nedmalloc pool

int memtest(void)
{
    cout << " ============== memory pool ================ " << endl;
    MemoryPoolTest();

    cout << " ============== ned    pool ================ " << endl;
    NedPoolTest();

    PocoTest();
    return 0;
}

