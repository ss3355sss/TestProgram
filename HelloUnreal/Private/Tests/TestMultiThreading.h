#pragma once
#include "HelloUnreal.h"
#include "Runnable.h"


/**~ UE's multithreading foundation */
class FProducerRunnable : public FRunnable
{
public:
	virtual bool Init(void) override
	{
		//bStop = false;
		return true;
	}
	virtual uint32 Run(void) override
	{
		//for (int32 i = 0; i < 10 && !bStop; i++)
		//{
		//	UE_LOG(LogHelloUnreal, Display, TEXT("Hello Unreal, %d"), i);
		//}
		return 0;
	}
	virtual void Stop(void) override
	{
		//bStop = true;
	}
	virtual void Exit(void) override
	{
	}
};



void Test_ThreadPool();
void TestMultiThreadingFoundation0();
// TAtomic
void TestTAtomic();

// FTest
void Test_FEvent();

// Test Queues
void Test_Queue();
void Test_CircularQueue();
void Test_MPMCQueue();
void Test_DoubleBufferQueue();



// TPromize

// ParallelFor

// ThreadPoolAndAsyncTask
void Test_ThreadPoolAndAsyncTask0();

// TaskGraph
void Test_TaskGraph();

// TFuture
void Test_FutureAndPromise();

// Async
void Test_AsyncSeries();
