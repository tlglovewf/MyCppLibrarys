//
//  main.cpp
//  PocoTest
//
//  Created by TuLigen on 2018/10/26.
//  Copyright © 2018年 TuLigen. All rights reserved.
//

#include <iostream>

#include <Poco/AutoPtr.h>
#include <Poco/RefCountedObject.h>
#include <Poco/MemoryPool.h>

#include <Poco/Thread.h>
#include <Poco/Timer.h>
#include <Poco/Runnable.h>


#include <Poco/Task.h>
#include <Poco/TaskManager.h>
#include <Poco/TaskNotification.h>
#include <Poco/Observer.h>


#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#pragma region    foundation test

//object pointer test
class A : public Poco::RefCountedObject
{
public:
};

void RefTest()
{
    A *a = new A;
    std::cout << a->referenceCount() << std::endl;
    Poco::AutoPtr<A> pA(a,true);//true 引用计数  +1
    std::cout << pA->referenceCount() << std::endl;
    Poco::AutoPtr<A> pB(a);    //false +-1
    std::cout << pB->referenceCount() << std::endl;
    
}

void MemoryTest()
{
    Poco::MemoryPool pool(1024);
    
    char *buffer = reinterpret_cast<char*>(pool.get());
    
    std::cin.read(buffer, pool.blockSize());
    
    std::streamsize n = std::cin.gcount();
    
    std::string str(buffer,n);
    
    pool.release(buffer);
    
}

#pragma unregion


#pragma region  thread test

class TestThread : public Poco::Runnable,public Poco::RefCountedObject
{
public:
    virtual void run()
    {
        std::cout << "poco test." << std::endl;
    }
};


void ThreadTest()
{
    TestThread runable;
    Poco::Thread thread;
    thread.start(runable);
    thread.join();
    Poco::Thread::sleep(1000);
    std::cout << "end thread." << std::endl;
}


class TimerFunc
{
public:
    void click(Poco::Timer &t)
    {
        static int i = 0;
        std::cout << i++ << std::endl;
    }
};

void TimerTest()
{
    Poco::Timer timer(0,250);
    TimerFunc func;
    timer.start(Poco::TimerCallback<TimerFunc>(func,&TimerFunc::click));
    
    Poco::Thread::sleep(5000);
    
    timer.stop();
}

#pragma unregion



#pragma region   task test

class SimpleTask : public Poco::Task
{
public:
    SimpleTask(const std::string &name):Poco::Task(name){}
    
    void runTask()
    {
        for (int i = 0; i < 100; ++i)
        {
            setProgress(float(i)/100); // report progress
            if (sleep(1000))
                break;
        }
    }
};



class ProgressHandler
{
public:
    void onProgress(Poco::TaskProgressNotification* pNf)
    {
        std::cout << pNf->task()->name()
        << " progress: " << pNf->progress() << std::endl;
        pNf->release();
    }
    void onFinished(Poco::TaskFinishedNotification* pNf)
    {
        std::cout << pNf->task()->name() << " finished." << std::endl;
        pNf->release();
    }
};



void  TaskTest()
{
    Poco::TaskManager tm;
    ProgressHandler pm;
    tm.addObserver(
                   Poco::Observer<ProgressHandler, Poco::TaskProgressNotification>
                   (pm, &ProgressHandler::onProgress)
                   );
    tm.addObserver(
                   Poco::Observer<ProgressHandler, Poco::TaskFinishedNotification>
                   (pm, &ProgressHandler::onFinished)
                   );
    tm.start(new SimpleTask("Task 1")); // tm takes ownership
    tm.start(new SimpleTask("Task 2"));
    tm.joinAll();

}


#pragma unregion


#pragma region

#pragma unregion

int main(int argc, const char * argv[]) {
    // insert code here...
    
//    RefTest();
//    
//    ThreadTest();
//    
//    TimerTest();
    
//    TaskTest();
    
    MemoryTest();
    
    return 0;
}
