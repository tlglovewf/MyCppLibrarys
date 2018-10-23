//
//  MemAllocConfig.h
//  XCodeCppTest
//
//  Created by TuLigen on 16/7/19.
//  Copyright © 2016年 TuLigen. All rights reserved.
//

#ifndef MemAllocConfig_h
#define MemAllocConfig_h

#if  1 // MY_ALLOC_ALLOCATOR == NED_POOLING_ALLOCATOR
#include "NedPoolingPolicy.h"
namespace TLG
{
    class CommonAllocPolicy : public NedPoolingPolicy
    {
    public:
        
    };
}

#endif

/**
 *  ...add more Alloc Policy
 */


#include "AllocatedObject.h"
namespace TLG
{
typedef AllocatedObject<CommonAllocPolicy>   BaseAllocatedObject;
}


#endif /* MemAllocConfig_h */
