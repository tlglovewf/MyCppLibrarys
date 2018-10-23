//
//  NedPoolingPolicy.cpp
//  XCodeCppTest
//
//  Created by TuLigen on 16/7/19.
//  Copyright © 2016年 TuLigen. All rights reserved.
//

#include "NedPoolingPolicy.h"
#include <algorithm>
#define ABORT_ON_ASSERT_FAILURE 0
#include "nedmalloc.c"
namespace TLG
{
    
    nedalloc::nedpool *pool = NULL;
    void* s_poolFootprint = reinterpret_cast<void*>(0xBB1AA45A);
    
    /**
     *  分配内存
     *
     *  @param sz 大小
     *  @return  地址
     */
    void* NedPoolingPolicy::allocBytes( size_t sz)
    {
        if(NULL == pool )
        {
            pool = nedalloc::nedcreatepool(0, 1);
            nedalloc::nedpsetvalue(pool, s_poolFootprint);
        }
        return nedalloc::nedpmalloc(pool, sz);
    }
    /**
     *  释放
     *
     *  @param ptr 内存地址
     */
    void NedPoolingPolicy::deallocBytes(void *ptr)
    {
        if( NULL != pool )
        {
            void *footpt = nedalloc::nedgetvalue(&pool, ptr);
            if(footpt == s_poolFootprint)
            {
                nedalloc::nedpfree(pool,ptr);
            }
        }
    }
    /**
     *  获取一次分配的最大内存块
     *
     *  @return 大小
     */
    size_t NedPoolingPolicy::getMaxAllocationSize(void)
    {
        return std::numeric_limits<size_t>::max();
    }
}
