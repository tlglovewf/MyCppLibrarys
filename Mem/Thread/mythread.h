//
//  thread.cpp
//  Mem
//
//  Created by TuLigen on 2018/10/23.
//  Copyright © 2018年 TuLigen. All rights reserved.
//

#include <thread>
#include <future>
#include <utility>

#include "poco/Thread.h"

int func(int x)
{
//    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "thread start !" << std::endl ;
    
    return x + 2;
}

void threadtest()
{
#if 0
    std::cout << "============= Thread ==============" << std::endl;
    std::thread t(func);
//    t.join();// 阻塞当前线程
//    t.detach();//交给后台处理
    std::cout << " end thread ! " << std::endl;
#else
//    std::packaged_task<int(int)> tsk(func);
//    std::future<int> fut = tsk.get_future();
//
//    std::thread(std::move(tsk),2).detach();
//
//    int value = fut.get();
//
//    std::cout << " The result is " << value << std::endl;
//
//    int a = 2;
//    int b = 3;
//    cout << "a + b = " << a + b << endl;

    
    std::promise<int> pr;
    std::thread t([](std::promise<int> &p){p.set_value_at_thread_exit(10);},std::ref(pr));
    std::future<int> f = pr.get_future();
   
    cout << f.get() << endl;
    t.detach();
#endif
}
