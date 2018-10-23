//
//  NedPoolingPolicy.h
//  XCodeCppTest
//
//  Created by TuLigen on 16/7/19.
//  Copyright © 2016年 TuLigen. All rights reserved.
//

#ifndef NedPoolingPolicy_hpp
#define NedPoolingPolicy_hpp
#include "stdlib.h"
namespace TLG
{
    /// ned pool
    class NedPoolingPolicy
    {
    public:
        /**
         *  分配内存
         *
         *  @param sz 大小
         *  @return  地址
         */
        static  void* allocBytes( size_t sz);
        /**
         *  释放
         *
         *  @param ptr 内存地址
         */
        static  void deallocBytes(void *ptr);
        /**
         *  获取一次分配的最大内存块
         *
         *  @return 大小
         */
        static  size_t getMaxAllocationSize(void);

    };
}

#endif /* NedPoolingPolicy_hpp */
