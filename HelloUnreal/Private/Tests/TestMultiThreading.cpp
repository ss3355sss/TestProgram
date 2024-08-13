#include "TestMultiThreading.h"

#include "iostream"

#include "Async.h"
#include "CircularQueue.h"
#include "RunnableThread.h"
#include "ThreadManager.h"
#include "ThreadSafeBool.h"
#include "Future.h"
#include "Misc/ScopeRWLock.h"

using std::cout;
using std::endl;

/*class FTrivialClass
{
public:
	FString Name;

public:
	// Constructor
	FTrivialClass(const FString& InName = TEXT("None")) : Name(InName)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Constructor (%s)", *Name);
	}
	// Copy Constructor
	FTrivialClass(const FTrivialClass& InObject) : Name(InObject.Name)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Copy Constructor (%s)", *Name);
	}
	// Move Constructor
	FTrivialClass(FTrivialClass&& InObject) : Name(InObject.Name)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Move Constructor (%s)", *Name);
		InObject.Name = TEXT("None");

	}

	// Destructor
	~FTrivialClass()
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Destructor (%s)", *Name);
		Name = TEXT("None");
	}

public:
	// Copy Assignment Operator
	FTrivialClass& operator=(const FTrivialClass& InRhs)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Copy Assignment Operator (%s)", *Name);
		if (this != &InRhs)
		{
			Name = InRhs.Name;
		}
		return *this;
	}
	// Move Assignment Operator
	FTrivialClass& operator=(FTrivialClass&& InRhs)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Move Assignment Operator (%s)", *Name);
		if (this != &InRhs)
		{
			Name = InRhs.Name;
			InRhs.Name = TEXT("None");
		}
		return *this;
	}

public:
	void Print(const FString& InTag)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "%s : Name = %s", *InTag, *Name);
	}
};
struct FNonTrivialClass
{
public:
	TSharedPtr<FString> Name = nullptr;
public:
	// Constructor
	FNonTrivialClass() = default;
	//FNonTrivialClass(const FString& InName = TEXT("None")) : Name(MakeShareable(InName))
	//{
	//	//UE_SCOPED_LOG_DISPLAY(LogTemp, "Constructor (%s)", *Name);
	//}
	// Copy Constructor
	//FNonTrivialClass(const FTrivialClass& InObject) : Name(InObject.Name)
	//{
	//	UE_SCOPED_LOG_DISPLAY(LogTemp, "Copy Constructor (%s)", *Name);
	//}
	//// Move Constructor
	//FNonTrivialClass(FTrivialClass&& InObject) : Name(InObject.Name)
	//{
	//	UE_SCOPED_LOG_DISPLAY(LogTemp, "Move Constructor (%s)", *Name);
	//	InObject.Name = TEXT("None");
	//
	//}
	//
	//// Destructor
	//~FNonTrivialClass()
	//{
	//	UE_SCOPED_LOG_DISPLAY(LogTemp, "Destructor (%s)", *Name);
	//	Name = TEXT("None");
	//}

public:
	//// Copy Assignment Operator
	//FTrivialClass& operator=(const FTrivialClass& InRhs)
	//{
	//	UE_SCOPED_LOG_DISPLAY(LogTemp, "Copy Assignment Operator (%s)", *Name);
	//	if (this != &InRhs)
	//	{
	//		Name = InRhs.Name;
	//	}
	//	return *this;
	//}
	//// Move Assignment Operator
	//FTrivialClass& operator=(FTrivialClass&& InRhs)
	//{
	//	UE_SCOPED_LOG_DISPLAY(LogTemp, "Move Assignment Operator (%s)", *Name);
	//	if (this != &InRhs)
	//	{
	//		Name = InRhs.Name;
	//		InRhs.Name = TEXT("None");
	//	}
	//	return *this;
	//}
};

class FSimpleThread : public FRunnable
{
public:
	FString          Name = TEXT("None");
	FRunnableThread* RunnableThread = nullptr;
	FThreadSafeBool  bStop = false;
public:
	FSimpleThread(const FString& InName) : Name(InName)
	{
		RunnableThread = FRunnableThread::Create(this,                                   /*=InRunnerable													#1#
			*Name,                                  /*=InThreadName													#1#
			0,                                      /*=InStackSize													#1#
			TPri_Normal,                            /*=InThreadPri													#1#
			FPlatformAffinity::GetNoAffinityMask(), /*=InThreadAffinityMask	(CPU 선호도, 어떤 CPU에 작업을 할당할지)	#1#
			EThreadCreateFlags::None                /*=InCreateFlags												#1#
		);
		Log(__FUNCTION__);
	}
	virtual ~FSimpleThread() override
	{
		if (RunnableThread)
		{
			RunnableThread->WaitForCompletion();
			delete RunnableThread;
			RunnableThread = nullptr;
			Log(__FUNCTION__);
		}
	}

public:
	// FRunnable Interface
	virtual bool Init() override
	{
		Log(__FUNCTION__);
		return true;
	}
	virtual uint32 Run() override
	{
		while (!bStop)
		{
			FPlatformProcess::Sleep(1 /*=Sec#1#);
			Log(__FUNCTION__);
		}
		return 0;
	}
	virtual void Exit() override
	{
		Log(__FUNCTION__);
	}
	virtual void Stop() override
	{
		bStop = true;
		if (RunnableThread)
		{
			RunnableThread->WaitForCompletion();
		}
	}

private:
	void Log(const char* Action)
	{
		uint32  CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
		FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);

		if (RunnableThread)
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "%s@%s[%d] - %s,%d, %s", *Name,
				*CurrentThreadName,
				CurrentThreadId,
				*RunnableThread->GetThreadName(),
				RunnableThread->GetThreadID(),
				ANSI_TO_TCHAR(Action)
			);
		}
		else
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "%s@%s[%d] - %s,%d, %s", *Name,
				*CurrentThreadName,
				CurrentThreadId,
				TEXT("NULL"),
				0,
				ANSI_TO_TCHAR(Action)
			);
		}
	}
};
#define SAFE_DELETE(Ptr)  if (Ptr) { delete Ptr; Ptr = nullptr; }
static void DumpAllThreads(const char* Log)
{
	FThreadManager::Get().ForEachThread([=](uint32 ThreadID, FRunnableThread* Thread)
	{
		UE_SCOPED_LOG_DISPLAY(LogTemp, "%s: %s,%u", ANSI_TO_TCHAR(Log), *Thread->GetThreadName(), ThreadID);
	});
}
static FString LexToString(ENamedThreads::Type ThreadInfo)
{
	ENamedThreads::Type ThreadIndex = ENamedThreads::GetThreadIndex(ThreadInfo);
	if (ThreadIndex == ENamedThreads::AnyThread)
	{
		const TCHAR* TaskPriority = ENamedThreads::GetTaskPriority(ThreadInfo) == ENamedThreads::NormalTaskPriority ? TEXT("Normal") : TEXT("High");
		const TCHAR* ThreadPriStrs[] = { TEXT("Normal"), TEXT("High"), TEXT("Low") };
		const TCHAR* ThreadPriority = ThreadPriStrs[ENamedThreads::GetThreadPriorityIndex(ThreadInfo)];

		return FString::Printf(TEXT("%s Priority Task on %s Priority Worker"), TaskPriority, ThreadPriority);
	}
	else
	{
		const TCHAR* NamedThreadsStrs[] = { TEXT("Stats"), TEXT("RHI"), TEXT("Audio"), TEXT("Game"), TEXT("Rendering") };
		const TCHAR* QueueStr = ENamedThreads::GetQueueIndex(ThreadInfo) == ENamedThreads::MainQueue ? TEXT("Main") : TEXT("Local");
		return FString::Printf(TEXT("%s (%s Queue)"), NamedThreadsStrs[ThreadIndex], QueueStr);
	}
}
static void AsyncLog(const char* Action)
{
	int32 ThreadId = FPlatformTLS::GetCurrentThreadId();
	FString ThreadName = FThreadManager::Get().GetThreadName(ThreadId);
	UE_LOG(LogTemp, Display, TEXT("%s[%d], %s"), *ThreadName, ThreadId, ANSI_TO_TCHAR(Action));
}


void Test_ThreadPool()
{
	class FQueuedWork : public IQueuedWork
	{
	public:
		FString Name;
	public:
		FQueuedWork(const FString& InName) : Name(InName)
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "Create QueuedWork (Name = %s)", *Name);
		}

		virtual ~FQueuedWork() override
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "Destroy QueuedWork (Name = %s)", *Name);
		}

		// 스레드 풀의 스레드에서 실행
		virtual void DoThreadedWork() override
		{
			const double TimeOut = 5.0; /*=Sec #1#

			const double StartTime = FPlatformTime::Seconds();
			while(FPlatformTime::Seconds() - StartTime < TimeOut)
			{
				FPlatformProcess::Sleep(0.0f);
			}

			UE_SCOPED_LOG_DISPLAY(LogTemp, "Work Done (Name = %s)", *Name);
			//FPlatformTime::Seconds();

			//FPlatformProcess::Sleep(0.2);
			// 작업이 끝나면 생성된 Worker 객체가 해제되거나 호출자에게 넘겨져 소멸될 수 있습니다.
			delete this;
		}

		// 작업 포기
		virtual void Abandon() override
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "Work Abandoned (Name = %s)", *Name);
			// 작업이 중단되어 생성된 Worker 객체가 해제되며, 이 객체는 호출자에게 소멸을 위해 제공될 수도 있습니다.
			delete this;
		}
	};

	{
		// Create A Thread Pool
		FQueuedThreadPool* Pool = FQueuedThreadPool::Allocate();
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Create MyPool");
		Pool->Create(5, 0, TPri_Normal, TEXT("MyPool"));

		{
			UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Add Work");
			int WokerNum = 100;
			for (int32 Index = 0; Index < WokerNum; ++Index)
			{
				// Worker will be deleted when the job is done
				Pool->AddQueuedWork(new FQueuedWork(TEXT("Work") + FString::FromInt(Index)));
			}
		}

		// Ticks
		{
			UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Tick Start");
			int TickCount = 64;
			for (int i = 0; i < TickCount; ++i)
			{
				// Consume
				UE_SCOPED_LOG_DISPLAY(LogHelloUnreal, "Tick[%d]...", i);
				FPlatformProcess::Sleep(0.1);
			}
		}

		// Destroy, if work is not done, call Abandon
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Destroy Pool");
		Pool->Destroy();
		delete Pool;
	}

}




void TestMultiThreadingFoundation0()
{
	const int32 NumberOfCoresIncludingHyperthreads = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	const int32 NumberOfWorkerThreadsToSpawn       = FPlatformMisc::NumberOfWorkerThreadsToSpawn();
	UE_SCOPED_LOG_DISPLAY(LogTemp, "# of Core (including HyperThread) = %d", FPlatformMisc::NumberOfCoresIncludingHyperthreads());
	UE_SCOPED_LOG_DISPLAY(LogTemp, "# of WorkerThreads to spawn(including HyperThread) = %d", FPlatformMisc::NumberOfWorkerThreadsToSpawn());



	// Create Threads
	FSimpleThread* SimpleThread1 = new FSimpleThread(TEXT("SimpleThread1"));
	FSimpleThread* SimpleThread2 = new FSimpleThread(TEXT("SimpleThread2"));

	DumpAllThreads(__FUNCTION__);

	// Ticks
	int TickCount = 100;
	for (int i = 0; i < TickCount; ++i)
	{
		// Consume
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Tick[%d] ........ ", i);
		FPlatformProcess::Sleep(0.1);
	}

	// Stop Thread
	SimpleThread1->Stop();
	SimpleThread2->Stop();

	// Destroy Threads
	SAFE_DELETE(SimpleThread1);
	SAFE_DELETE(SimpleThread2);

}

void TestTAtomic()
{
	TAtomic<int32> Counter;
	Counter++;
	int32 Read = Counter.Load();	// Atomic Read
	Counter.Store(10);				// Atomic Write

	FThreadSafeCounter Counter2;
	Counter2.Increment();			// FPlatformAtomics::InterlockedIncrement
	Counter2.Decrement();			// FPlatformAtomics::InterlockedDecrement
	if (Counter2.GetValue() == 0)	// FPlatformAtomics::AtomicRead
	{
		// Do Something
	}

	//FThreadSafeCounter64 Counter3;
	//FThreadSafeBool      AtomicBool;


	//::MoveTemp()
	//	::Move

	//std::chrono::milliseconds(100);
	//100ms;
	//FPlatformProcess::Sleep(100 /*=Sec#1#);

	FRWLock ValuesLock;
	{
		FRWScopeLock ScopedLock(ValuesLock, SLT_ReadOnly);
	}
	{
		ValuesLock.WriteLock();
		ValuesLock.WriteUnlock();
	}
}

void Test_FEvent()
{
	FEvent* SyncEvent = nullptr;

	Async(EAsyncExecution::Thread, [&SyncEvent]()
		{
			FPlatformProcess::Sleep(3);
			if (SyncEvent)
			{
				// 다른 스레드에서 이벤트 트리거하기
				SyncEvent->Trigger();
				UE_LOG(LogTemp, Display, TEXT("Trigger ....."));
			}
		});

	// 이벤트 객체 만들기
	SyncEvent = FPlatformProcess::GetSynchEventFromPool(true /*=bIsManualReset#1#);
	// 이벤트가 트리거될 때까지 기다리기(무한 대기)
	SyncEvent->Wait((uint32)-1);
	// 이벤트 객체 해제하기
	FPlatformProcess::ReturnSynchEventToPool(SyncEvent);

	UE_LOG(LogTemp, Display, TEXT("Over ....."));
}




void Test_ThreadPoolAndAsyncTask0()
{
	class FSimpleQueuedWorker : public IQueuedWork
	{
	public:
		FSimpleQueuedWorker(const FString& Name) : WorkerName(Name) {
			Log(__FUNCTION__);
		}

		virtual ~FSimpleQueuedWorker() override {
			Log(__FUNCTION__);
		}

		// 스레드 풀의 스레드에서 실행
		virtual void DoThreadedWork() override {
			FPlatformProcess::Sleep(0.2);
			Log(__FUNCTION__);
			// 작업이 끝나면 생성된 Worker 객체가 해제되거나 호출자에게 넘겨져 소멸될 수 있습니다.
			delete this;
		}

		// 작업 포기
		virtual void Abandon() override {
			Log(__FUNCTION__);
			// 작업이 중단되어 생성된 Worker 객체가 해제되며, 이 객체는 호출자에게 소멸을 위해 제공될 수도 있습니다.
			delete this;
		}

		void Log(const char* Action) {
			uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
			FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
			UE_LOG(LogTemp, Display, TEXT("%s@%s[%d] - %s"),
				*WorkerName, *CurrentThreadName, CurrentThreadId, ANSI_TO_TCHAR(Action));
		}

	public:
		FString WorkerName;
	};


	{
		// Create A Thread Pool
		FQueuedThreadPool* Pool = FQueuedThreadPool::Allocate();
		Pool->Create(5, 0, TPri_Normal, TEXT("SimpleThreadPool"));

		int WokerNum = 100;
		for (int i = 0; i < WokerNum; ++i)
		{
			FString Name = TEXT("Worker") + FString::FromInt(i);

			// Worker will be deleted when the job is done
			Pool->AddQueuedWork(new FSimpleQueuedWorker(Name));
		}

		// Ticks
		int TickCount = 20;
		for (int i = 0; i < TickCount; ++i)
		{
			// Consume
			UE_LOG(LogTemp, Display, TEXT("Tick[%d] ........ "), i);
			FPlatformProcess::Sleep(0.1);
		}

		// Destroy, if work is not done, call Abandon
		Pool->Destroy();
		delete Pool;
	}

	class SimpleExampleTask : public FNonAbandonableTask
	{
		friend class FAsyncTask<SimpleExampleTask>;

		int32 ExampleData;
		float WorkingTime;

	public:
		SimpleExampleTask(int32 InExampleData, float TheWorkingTime = 1)
			: ExampleData(InExampleData), WorkingTime(TheWorkingTime) { }

		~SimpleExampleTask() {
			Log(__FUNCTION__);
		}

		// 执行任务（必须实现）
		void DoWork() {
			// do the work...
			FPlatformProcess::Sleep(WorkingTime);
			Log(__FUNCTION__);
		}

		// 用时统计对应的ID（必须实现）
		FORCEINLINE TStatId GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
		}

		void Log(const char* Action)
		{
			uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
			FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
			UE_LOG(LogTemp, Display, TEXT("%s[%d] - %s, ExampleData=%d"), *CurrentThreadName, CurrentThreadId,
				ANSI_TO_TCHAR(Action), ExampleData);
		}
	};

	{
		(new FAutoDeleteAsyncTask<SimpleExampleTask>(1000))->StartBackgroundTask();

		// 在当前线程中执行
		(new FAutoDeleteAsyncTask<SimpleExampleTask>(2000))->StartSynchronousTask();
	}


}



struct FMyItem
{
public:
	uint32 Id;
	FString Name;
public:
	FMyItem(uint32 TheId = 0, const FString& TheName = TEXT("Item")): Id(TheId), Name(TheName)
	{
	}
};

TQueue<FMyItem, EQueueMode::Spsc> ItemsQueue;
void Test_Queue()
{
	// Single Producer
	Async(EAsyncExecution::Thread, []()
	{
		for (uint32 Id = 1; ; Id++)
		{
			// Produce Item
			ItemsQueue.Enqueue(FMyItem(Id, "Item"));
			UE_LOG(LogTemp, Display, TEXT("Produce: %d,%s"), Id, TEXT("Item"));
		}
	});
	// Single Consumer
	Async(EAsyncExecution::Thread, []()
	{
		while (true)
		{
			if (!ItemsQueue.IsEmpty())
			{
				// Consume Item
				FMyItem Item;
				ItemsQueue.Dequeue(Item);
				UE_LOG(LogTemp, Display, TEXT("Consume: %d,%s"), Item.Id, *Item.Name);
			}
		}
	});
}

void Test_CircularQueue()
{
	// SPSC only
	int32 Capacity = 100;
	TCircularQueue<uint32> Queue(Capacity);
	TAtomic<bool> bStop = false;


}

void Test_MPMCQueue()
{
}

void Test_DoubleBufferQueue()
{
}




/**~ Task Graph #1#
void Test_TaskGraph()
{
	// 1회성 작업 정의 (FireAndForget)
	class FFireAndForgetTask
	{
	public:
		// Custom Params
		FString TaskName;
		int     SomeArgument;
		float   TimeOut;

	public:
		FFireAndForgetTask(const TCHAR* InName, int InSomeArgument, float InWorkingTime = 1.0f) : TaskName(InName), SomeArgument(InSomeArgument), TimeOut(InWorkingTime)
		{
			Log(__FUNCTION__);
		}
		~FFireAndForgetTask()
		{
			Log(__FUNCTION__);
		}

	public:
		FORCEINLINE TStatId GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FGraphTaskSimple, STATGROUP_TaskGraphTasks);
		}

	public:
		static ENamedThreads::Type GetDesiredThread()
		{
			return ENamedThreads::AnyThread;
		}

		static ESubsequentsMode::Type GetSubsequentsMode()
		{
			return ESubsequentsMode::FireAndForget;	// FireAndForget: 일회성 작업, 종속성 없음
		}

	public:
		void DoTask(ENamedThreads::Type ThreadInfo, const FGraphEventRef& MyCompletionGraphEvent)
		{
			UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "ThreadInfo = %s", *LexToString(ThreadInfo));
			// The arguments are useful for setting up other tasks. Do work here, probably using SomeArgument.
			const double StartTime = FPlatformTime::Seconds();
			while (true)
			{
				const double ElpasedTime = FPlatformTime::Seconds() - StartTime;
				if (ElpasedTime > TimeOut)
					break;

				UE_SCOPED_LOG_DISPLAY(LogHelloUnreal, "Do Something - %5.2lf", ElpasedTime);
				FPlatformProcess::Sleep(1.0f);
			}
			//Log(__FUNCTION__);
		}

	private:
		void Log(const char* Action)
		{
			UE_SCOPED_LOG_DISPLAY(LogTemp, "%s - TaskName = %s, SomeArgument = %d", ANSI_TO_TCHAR(Action), *TaskName, SomeArgument);

			//uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
			//FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
			//UE_LOG(LogTemp, Display, TEXT("%s@%s[%d] - %s, SomeArgument=%d"), *TaskName, *CurrentThreadName, CurrentThreadId, ANSI_TO_TCHAR(Action), SomeArgument);
		}
	};

	// 종속성을 지원하는 작업 정의 (TrackSubsequents)
	class FTrackSubsequentsTask : public FFireAndForgetTask
	{
	public:
		using FFireAndForgetTask::FFireAndForgetTask;

	public:
		FORCEINLINE TStatId GetStatId() const
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(FGraphTask, STATGROUP_TaskGraphTasks);
		}
	public:
		static ENamedThreads::Type GetDesiredThread()
		{
			return ENamedThreads::AnyThread;
		}

		static ESubsequentsMode::Type GetSubsequentsMode()
		{
			return ESubsequentsMode::TrackSubsequents;	// TrackSubsequents - 종속성 검사 지원
		}
	};

	/**~ 0. Create And Dispatch Task (TGraphTask) #1#
	{
		
	}
	//// 创建一个任务并在后台AnyThread中执行
	//FGraphEventRef GraphEvent = FFunctionGraphTask::CreateAndDispatchWhenReady([]()
	//{
	//	UE_LOG(LogTemp, Display, TEXT("Main task"));
	//	FPlatformProcess::Sleep(5.f); // pause for a bit to let waiting start
	//});
	//check(!GraphEvent->IsComplete());
	//
	//// 在主线程中等待该任务完成
	//GraphEvent->Wait(ENamedThreads::GameThread);
	//UE_LOG(LogTemp, Display, TEXT("Over1 ..."));
	//
	//
	//// 同时创建多个任务
	//FGraphEventArray Tasks;
	//for (int i = 0; i < 10; ++i)
	//{
	//	Tasks.Add(FFunctionGraphTask::CreateAndDispatchWhenReady([i]()
	//	{
	//		UE_LOG(LogTemp, Display, TEXT("Task %d"), i);
	//	}));
	//}
	//
	//// 在主线程中等待所有任务完成
	//FTaskGraphInterface::Get().WaitUntilTasksComplete(MoveTemp(Tasks), ENamedThreads::GameThread);
	//UE_LOG(LogTemp, Display, TEXT("Over2 ..."));







	/**~ 1. Custom Task #1#

	/**~ 2. OneTime Task (FireAndForgetTask) #1#
	{
		// 일회성 작업을 생성하고 실행하여 작업이 끝나면 자동으로 삭제합니다.
		FGraphEventRef Task0 = TGraphTask<FFireAndForgetTask>::CreateTask(nullptr /*=Prerequisites#1#, ENamedThreads::AnyThread /*=CurrentThreadIfKnown#1#).ConstructAndDispatchWhenReady(TEXT("MyTask1") /*=InName#1#, 1024/*=InSomeArgument#1#, 3.0f/*=InWorkingTime#1#);
		//Task0->Wait();

		// 작업을 생성하지만 실행을 위해 작업 그래프에 넣지 않습니다.
		//if(TGraphTask<FFireAndForgetTask>* Task = TGraphTask<FFireAndForgetTask>::CreateTask().ConstructAndHold(TEXT("SimpleTask2"), 20000))
		//{
		//	// 작업을 잠금 해제하고 TaskGraph에 넣어 실행합니다.
		//	UE_LOG(LogTemp, Display, TEXT("Task is Unlock to Run..."));
		//	Task->Unlock();
		//	Task = nullptr;
		//}
	}
	/**~ 3. Sequentially dependent tasks #1#

	/**~ 4. Gather/Fence Task #1#
	//{
	//	auto TaskA = TGraphTask<FTask>::CreateTask().ConstructAndDispatchWhenReady(TEXT("TaskA"), 1, 2);
	//	auto TaskB = TGraphTask<FTask>::CreateTask().ConstructAndDispatchWhenReady(TEXT("TaskB"), 2, 1);
	//
	//}

	/**~ 5. Delegate Task (FSimpleDelegateGraphTask) #1#
	//{
	//	// Simple Delegate
	//	FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(FSimpleDelegateGraphTask::FDelegate::CreateLambda([]()
	//	{
	//		uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
	//		FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
	//		UE_LOG(LogTemp, Display, TEXT("%s[%d] - Simple Delegate"), *CurrentThreadName, CurrentThreadId);
	//	}),TStatId());
	//
	//
	//	// Delegate
	//	FGraphEventRef GraphEventRef1 = FDelegateGraphTask::CreateAndDispatchWhenReady(FDelegateGraphTask::FDelegate::CreateLambda([](ENamedThreads::Type InCurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	//	{
	//		FPlatformProcess::Sleep(3.0);
	//		uint32  CurrentThreadId   = FPlatformTLS::GetCurrentThreadId();
	//		FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
	//		UE_LOG(LogTemp, Display, TEXT("%s[%d] - Delegate, %d"), *CurrentThreadName, CurrentThreadId, InCurrentThread);
	//	}), TStatId());
	//	FTaskGraphInterface::Get().WaitUntilTaskCompletes(GraphEventRef1, ENamedThreads::GameThread);
	//}

	/**~ 6. Function Task (FFunctionGraphTask) #1#
	//{
	//	// Fuction with no params
	//	FGraphEventRef GraphEventRef0 = FFunctionGraphTask::CreateAndDispatchWhenReady([]()
	//	{
	//		uint32  CurrentThreadId   = FPlatformTLS::GetCurrentThreadId();
	//		FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
	//		UE_LOG(LogTemp, Display, TEXT("%s[%d] - Fuction with no params"), *CurrentThreadName, CurrentThreadId);
	//	}, TStatId());
	//
	//	// Fuction with params (void(ENamedThreads::Type, const FGraphEventRef&))
	//	FFunctionGraphTask::CreateAndDispatchWhenReady([](ENamedThreads::Type InCurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	//	{
	//		FPlatformProcess::Sleep(3.0);
	//		uint32  CurrentThreadId   = FPlatformTLS::GetCurrentThreadId();
	//		FString CurrentThreadName = FThreadManager::Get().GetThreadName(CurrentThreadId);
	//		UE_LOG(LogTemp, Display, TEXT("%s[%d] - Function with params, %d"), *CurrentThreadName, CurrentThreadId, InCurrentThread);
	//	}, TStatId())->Wait(ENamedThreads::GameThread);
	//
	//	//UE_LOG(LogTemp, Display, TEXT("Over ......"));
	//}
}

/**~ Async Series #1#
/*
 *	Async Series는 함수의 비동기 실행을 지원하고
 *	Future 객체(함수 반환값 포함)를 반환합니다. -> TFuture<ResultType> Future
 *
 *	Callable Signiture				-> TResultType();
 *	CompletionCallback Signiture	-> void();
 *
 *	어떤 Concurrency 메커니즘 결정은 파라미터로 제어할 수 있습니다.
 *	EAsyncExecution::TaskGraph							Execute in Task Graph (for short running tasks).
 *	EAsyncExecution::TaskGraphMainThread(=GameThread)	Execute in Task Graph on the main thread (for short running tasks).
 *	EAsyncExecution::Thread								Execute in separate thread if supported (for long running tasks).
 *	EAsyncExecution::ThreadIfForkSafe					Execute in separate thread if supported or supported post fork (see FForkProcessHelper::CreateThreadIfForkSafe) (for long running tasks).
 *	EAsyncExecution::ThreadPool							Execute in global queued thread pool.
 *	EAsyncExecution::LargeThreadPool					Execute in large global queued thread pool.

 #1#
void Test_AsyncSeries()
{
	constexpr double TimeOut = 5.0f;
	//TUniqueFunction<double()> OnAsyncFunc = [&TimeOut]()
	TFunction<double()> OnAsyncFunc = [&TimeOut]()	
	{
		const double StartTime   = FPlatformTime::Seconds();
		double       ElpasedTime = 0.0f;
		while (true)
		{
			ElpasedTime = FPlatformTime::Seconds() - StartTime;
			if (ElpasedTime > TimeOut)
				break;

			UE_SCOPED_LOG_DISPLAY(LogTemp, "Do Sub Task  - %5.2lf", ElpasedTime);
			FPlatformProcess::Sleep(1.0f);
		}
		return ElpasedTime;
	};

	auto WaitTask = [](TSharedFuture<double> InFuture)
	{
		const double StartTime   = FPlatformTime::Seconds();
		double       ElpasedTime = 0.0f;
		while (true)
		{
			ElpasedTime = FPlatformTime::Seconds() - StartTime;
			if (InFuture.IsReady())
			{
				//"OnAsyncFunc"가 완료 되었음.
				UE_SCOPED_LOG_DISPLAY(LogTemp, "SubTask Complete! SubTask's Complete ElapsedTime is %5.2lf", InFuture.Get());
				break;
			}
			else
			{
				UE_SCOPED_LOG_DISPLAY(LogTemp, "SubTask Not Complete! (%5.2lf)", ElpasedTime);
			}
			FPlatformProcess::Sleep(0.5f);
		}
	};


	// Begin Test
	//{
	//	UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Test Async #0");
	//
	//	EAsyncExecution AsyncExecution = EAsyncExecution::Thread;
	//	//TFuture<double> Future0 = Async(AsyncExecution, MoveTemp(OnAsyncFunc) /*=CompletionCallback)#1#);
	//	TSharedFuture<double> Future = Async(AsyncExecution,	/*=Execution			#1#
	//	                                OnAsyncFunc,			/*=Callable				#1#
	//	                                nullptr					/*=CompletionCallback)	#1#
	//	);
	//	UE_SCOPED_LOG_DISPLAY(LogHelloUnreal, "Waiting for the promise...");
	//
	//
	//	/**~ Waits Funcs #1#
	//	// Future.Wait();								// Wait Infinitly
	//	// Future.WaitFor(FTimespan::FromSeconds(5.0));	// Wait 5 sec
	//	// Future.WaitUntil(FDateTime(2022, 1, 1));		// Wait until 2022/01/01
	//	WaitTask(Future);
	//}

	//{
	//	UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Test AsyncThread #0");
	//
	//	// EAsyncExecution::Thread 와 동일하지만, Thread의 spec(=StackSize, ThreadPriority)을 정의할 수 있음
	//	TSharedFuture<double> Future = AsyncThread(OnAsyncFunc, /*=Callable							#1#
	//	                                           0 /*=StackSize(0 means default size)	#1#,
	//	                                           TPri_Normal /*=ThreadPriority					#1#,
	//	                                           nullptr /*=CompletionCallback)				#1#
	//	);
	//	WaitTask(Future);
	//}
	
	{
		UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Test AsyncPool #0");
		TSharedFuture<double> Future = AsyncPool(*GThreadPool,					/*=ThreadPool				#1#
		                                         OnAsyncFunc,					/*=Callable				#1#
		                                         nullptr,        /*=CompletionCallback)	#1#
		                                         EQueuedWorkPriority::Normal		/*=QueuedWorldPriority	#1#
		);
		WaitTask(Future);
	}
	
	{
		UE_SCOPED_LOG_EVENT_DISPLAY_F(LogHelloUnreal, "Test AsyncTask #0");
	
		TUniqueFunction<void()> AsyncTaskFunction = [&TimeOut]()
		{
			const double StartTime   = FPlatformTime::Seconds();
			double       ElpasedTime = 0.0f;
			while (true)
			{
				ElpasedTime = FPlatformTime::Seconds() - StartTime;
				if (ElpasedTime > TimeOut)
					break;
	
				UE_SCOPED_LOG_DISPLAY(LogTemp, "Do Sub Task  - %5.2lf", ElpasedTime);
				FPlatformProcess::Sleep(1.0f);
			}
			return ElpasedTime;
		};
	
		// No Return; fire-and-forget asynchronous functions
		AsyncTask(ENamedThreads::AnyThread,   /*=ThreadType	#1#
		          MoveTemp(AsyncTaskFunction) /*=Callable		#1#
		);
	}

}

void Test_FutureAndPromise()
{
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


	TPromise<bool/*=ResultType#1#> Promise;
	TFuture<bool/*=ResultType#1#>  Future = Promise.GetFuture();	// Share Future


	FFunctionGraphTask::CreateAndDispatchWhenReady([&Promise]()
	{
		// "SomeTask"
		FPlatformProcess::Sleep(3/*=Sec#1#);
		UE_SCOPED_LOG_DISPLAY(LogTemp, "Do the promise");
		Promise.SetValue(true);
	});


	UE_SCOPED_LOG_DISPLAY(LogTemp, "waiting for the promise...");
	// Future.Wait();								// Wait Infinitly
	Future.WaitFor(FTimespan::FromSeconds(5.0));	// Wait 5 sec
	// Future.WaitUntil(FDateTime(2022, 1, 1));		// Wait until 2022/01/01



	if (Future.IsReady())
	{
		// 이 시점까지 "SomeTask"가 완료 되었음.
		UE_SCOPED_LOG_DISPLAY(LogTemp, "keep the promise, future is %d", Future.Get());
	}
	else
	{
		// 이 시점까지 "SomeTask"가 완료 되지 않았음.
		UE_SCOPED_LOG_DISPLAY(LogTemp, "break the promise");
		Promise.SetValue(false);
	}


}*/