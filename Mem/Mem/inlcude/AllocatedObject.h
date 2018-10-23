//
//  AllocatedObject.h
//  XCodeCppTest
//
//  Created by TuLigen on 16/7/19.
//  Copyright © 2016年 TuLigen. All rights reserved.
//

#ifndef AllocatedObject_hpp
#define AllocatedObject_hpp
#include "stdlib.h"
namespace TLG {
    template<typename Alloc>
    /// 内存分配对象
    class AllocatedObject
    {
    public:
        void* operator new( size_t sz )
        {
           return Alloc::allocBytes(sz);
        }
        
        void operator delete(void *obj)
        {
            Alloc::deallocBytes(obj);
        }
        
        void operator delete( void *obj, size_t sz)
        {
            Alloc::deallocBytes(obj);
        }
        
        void* operator new[]( size_t sz )
        {
            return Alloc::allocBytes(sz);
        }
        
        void operator delete[] ( void *obj )
        {
            Alloc::deallocBytes(obj);
        }
        
        void operator delete[]( void *obj, size_t sz)
        {
            Alloc::deallocBytes(obj);
        }
    };
}


#endif /* AllocatedObject_hpp */
