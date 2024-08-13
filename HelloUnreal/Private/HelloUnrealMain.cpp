/* ----------------------------------------- Co - responding Header						*/
//#include "HelloUnreal.h"
/* ----------------------------------------- C / C++, STL, Platform Dependent Header	*/
//#include <iostream>
//#include <thread>
//
//using std::cout;
//using std::endl;
//
///* ----------------------------------------- Third-Party Header							*/
///* ----------------------------------------- Unreal Engine Module Header				*/
//#include "UObject/UObjectBase.h"
//
//#include "RequiredProgramMainCPPInclude.h"
///* ----------------------------------------- Project Module Header						*/
//
//
//DEFINE_LOG_CATEGORY_STATIC(LogHelloUnreal, Log, All);
//
//IMPLEMENT_APPLICATION(HelloUnreal, "HelloUnreal");
//
//void HelloThread()
//{
//	std::cout << "Hello Thread" << std::endl;
//}
//
//void TestCPP()
//{
//	std::thread t(HelloThread);
//
//
//	std::cout << "Hello Main" << std::endl;
//
//	t.join();
//}
//
////https://unrealcommunity.wiki/multi-threading:-how-to-create-threads-in-ue4-0bsy2g96
//
////https://unrealcommunity.wiki/multi-threading:-task-graph-system-pah8k101
//
////https://unrealcommunity.wiki/multithreading-with-frunnable-2a4xuf68
//
//void TestUnreal(int32 ArgC, TCHAR* ArgV[])
//{
//	FTaskTagScope TaskTagScope(ETaskTag::EGameThread);
//
//	GEngineLoop.PreInit(ArgC, ArgV);
//
//	// Make sure all UObject classes are registered and default properties have been initialized
//	ProcessNewlyLoadedUObjects();
//
//	// Tell the module manager it may now process newly-loaded UObjects when new C++ modules are loaded
//	FModuleManager::Get().StartProcessingNewlyLoadedObjects();
//
//	// loop while the server does the rest
//	while (!IsEngineExitRequested())
//	{
//		BeginExitIfRequested();
//
//		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
//		FStats::AdvanceFrame(false);
//		FTSTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
//
//		{
//			UE_LOG(LogHelloUnreal, Display, TEXT("Hello Unreal, %ld"), GFrameCounter);
//		}
//
//		FPlatformProcess::Sleep(0.01);
//		GFrameCounter++;
//	}
//
//	FCoreDelegates::OnExit.Broadcast();
//	//FSlateApplication::Shutdown();
//	FModuleManager::Get().UnloadModulesAtShutdown();
//
//	GEngineLoop.AppPreExit();
//	GEngineLoop.AppExit();
//}
//
//class FMyRunnable : public FRunnable
//{
//private:
//	bool bStop;
//
//public:
//	virtual bool Init(void) override
//	{
//		bStop = false;
//		return true;
//	}
//	virtual uint32 Run(void) override
//	{
//		for (int32 i = 0; i < 10 && !bStop; i++)
//		{
//			UE_LOG(LogHelloUnreal, Display, TEXT("Hello Unreal, %d"), i);
//		}
//		return 0;
//	}
//	virtual void Stop(void) override
//	{
//		bStop = true;
//	}
//	virtual void Exit(void) override
//	{
//	}
//};
//
////int32 GValue = 0;
//TAtomic<int32> GValue = 0;
//
//TArray<int32> GArray;
//FCriticalSection GMutex;
//
//class FSpinLock
//{
//public:
//	void Lock()
//	{
//		bool Expected = false;
//		bool Desired = true;
//
//		while (Locked.CompareExchange(Expected, Desired) == false)
//		{
//			Expected = false;
//		}
//
//	}
//	void UnLock()
//	{
//		Locked.Store(false);
//
//	}
//private:
//	TAtomic<bool> Locked = 0;
//};
//FSpinLock GSpinLock;
//
//INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
//{
//#if defined(ParentProjectName)
//	const FString ProjectDir = FString::Printf(TEXT("../../Program/%s/"), FApp::GetProjectName());
//	FPlatformMisc::SetOverrideProjectDir(ProjectDir);
//#endif
//
//	FTaskTagScope TaskTagScope(ETaskTag::EGameThread);
//
//	GEngineLoop.PreInit(ArgC, ArgV);
//
//	// Make sure all UObject classes are registered and default properties have been initialized
//	ProcessNewlyLoadedUObjects();
//
//	// Tell the module manager it may now process newly-loaded UObjects when new C++ modules are loaded
//	FModuleManager::Get().StartProcessingNewlyLoadedObjects();
//
//	if (FPlatformProcess::SupportsMultithreading())
//	{
//		cout << "MultiThreading Enabled" << endl;;
//
//		const int32 NumberOfCoresIncludingHyperthreads = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
//		const int32 NumberOfWorkerThreadsToSpawn = FPlatformMisc::NumberOfWorkerThreadsToSpawn();
//		//cout << "NumberOfCoresIncludingHyperthreads = " << NumberOfCoresIncludingHyperthreads << endl;
//		//cout << "NumberOfWorkerThreadsToSpawn = " << NumberOfWorkerThreadsToSpawn << endl;
//
//		/* ~Thread */
//		//{
//		//	TArray<FMyRunnable*> Runnables;
//		//	TArray<FRunnableThread*> Threads;
//		//	for(int32 Index = 0; Index < 10; ++Index)
//		//	{
//		//		FMyRunnable* MyRunnable = new FMyRunnable;
//		//		FRunnableThread* MyThread = FRunnableThread::Create(MyRunnable, TEXT("MyRunnable[%d]"), Index);
//		//
//		//		const uint32 ThreadID							= MyThread->GetThreadID();
//		//		const FString& ThreadName						= MyThread->GetThreadName();
//		//		const FRunnableThread::ThreadType ThreadType	= MyThread->GetThreadType();
//		//		const EThreadPriority ThreadPrioirty			= MyThread->GetThreadPriority();
//		//		const uint32 TlsSlot							= MyThread->GetTlsSlot();
//		//
//		//		Runnables.Add(MyRunnable);
//		//		Threads.Add(MyThread);
//		//
//		//	}
//		//
//		//	for (int32 Index = 0; Index < 10; ++Index)
//		//	{
//		//		Threads[Index]->WaitForCompletion();
//		//		delete Threads[Index];
//		//		delete Runnables[Index];
//		//	}
//		//}
//		/* ~Thread End */
//
//		/* ~Atomic */
//		{
//
//			class FAddRunnable : public FRunnable
//			{
//			public:
//				virtual uint32 Run() override
//				{
//					for (int32 Index = 0; Index < 1'000'000; ++Index)
//					{
//						++GValue;
//						//GValue.IncrementExchange();
//					}
//					return 0;
//				}
//			};
//			class FSubRunnable : public FRunnable
//			{
//			public:
//				virtual uint32 Run() override
//				{
//					for (int32 Index = 0; Index < 1'000'000; ++Index)
//					{
//						--GValue;
//						//GValue.IncrementExchange();
//
//					}
//					return 0;
//				}
//			};
//
//			FRunnable* AddRunnable = new FAddRunnable;
//			FRunnableThread* AddThread = FRunnableThread::Create(AddRunnable, TEXT("AddRunnable"));
//
//			FRunnable* SubRunnable = new FSubRunnable;
//			FRunnableThread* SubThread = FRunnableThread::Create(SubRunnable, TEXT("SubRunnable"));
//
//			AddThread->WaitForCompletion();
//			SubThread->WaitForCompletion();
//
//			cout << "GValue = " << GValue << endl;
//
//			delete AddThread;
//			delete AddRunnable;
//			delete SubThread;
//			delete SubRunnable;
//		}
//		/* ~Atomic End */
//
//		/* ~Lock*/
//		{
//
//
//			class FAddRunnable : public FRunnable
//			{
//			public:
//				virtual uint32 Run() override
//				{
//					for (int32 Index = 0; Index < 10'000; ++Index)
//					{
//						//GMutex.Lock();
//						//GArray.Add(Index);
//						//GMutex.Unlock();
//
//
//						FScopeLock Lock(&GMutex); 
//						GArray.Add(Index);
//
//						GSpinLock.Lock();
//						GArray.Add(Index);
//						GSpinLock.UnLock();
//					}
//					return 0;
//				}
//			};
//
//			FRunnable* AddRunnable0 = new FAddRunnable;
//			FRunnableThread* Thread0 = FRunnableThread::Create(AddRunnable0, TEXT("Thread0"));
//			FRunnable* AddRunnable1 = new FAddRunnable;
//			FRunnableThread* Thread1 = FRunnableThread::Create(AddRunnable1, TEXT("Thread1"));
//			FRunnable* AddRunnable2 = new FAddRunnable;
//			FRunnableThread* Thread2 = FRunnableThread::Create(AddRunnable2, TEXT("Thread2"));
//
//			Thread0->WaitForCompletion();
//			Thread1->WaitForCompletion();
//			Thread2->WaitForCompletion();
//
//			delete Thread0;
//			delete Thread1;
//			delete Thread2;
//			delete AddRunnable0;
//			delete AddRunnable1;
//			delete AddRunnable2;
//
//			cout << "GArray.Num() = " << GArray.Num() << endl;
//
//
//		}
//		/* ~Lock*/
//
//
//		{
//
//
//
//
//
//		}
//
//	}
//
//
//	FCoreDelegates::OnExit.Broadcast();
//	//FSlateApplication::Shutdown();
//	FModuleManager::Get().UnloadModulesAtShutdown();
//
//	GEngineLoop.AppPreExit();
//	GEngineLoop.AppExit();
//
//
//	system("pause");
//	return 0;
//}


// Copyright Epic Games, Inc. All Rights Reserved.
/* ----------------------------------------- Co - responding Header						*/
#include "HelloUnreal.h"
/* ----------------------------------------- C / C++, STL, Platform Dependent Header	*/
#include <iostream>
/* ----------------------------------------- Third-Party Header							*/
/* ----------------------------------------- Unreal Engine Module Header				*/
#include "RequiredProgramMainCPPInclude.h"
/* ----------------------------------------- Project Module Header						*/
#include "Tests/TestMultiThreading.h"

IMPLEMENT_APPLICATION(LogHelloUnreal, "LogHelloUnreal");

class UEGTestPrinter : public ::testing::EmptyTestEventListener
{
	virtual void OnTestStart(const ::testing::TestInfo& TestInfo)
	{
		UE_LOG(LogHelloUnreal, Verbose, TEXT("Test %s.%s Starting"), *FString(TestInfo.test_case_name()), *FString(TestInfo.name()));
	}

	virtual void OnTestPartResult(const ::testing::TestPartResult& TestPartResult)
	{
		if (TestPartResult.failed())
		{
			UE_LOG(LogHelloUnreal, Error, TEXT("FAILED in %s:%d\n%s"), *FString(TestPartResult.file_name()), TestPartResult.line_number(), *FString(TestPartResult.summary()))
		}
		else
		{
			UE_LOG(LogHelloUnreal, Verbose, TEXT("Succeeded in %s:%d\n%s"), *FString(TestPartResult.file_name()), TestPartResult.line_number(), *FString(TestPartResult.summary()))
		}
	}

	virtual void OnTestEnd(const ::testing::TestInfo& TestInfo)
	{
		UE_LOG(LogHelloUnreal, Verbose, TEXT("Test %s.%s Ending"), *FString(TestInfo.test_case_name()), *FString(TestInfo.name()));
	}
};


INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{

	// COMMENT::asdasd
	// MyComment::asdads

	// DvTODO::as
	// Comment:: as

	//Bug: asd


	/*
	FTaskTagScope Scope(ETaskTag::EGameThread);
	ON_SCOPE_EXIT
	{
		LLM(FLowLevelMemTracker::Get().UpdateStatsPerFrame());
		RequestEngineExit(TEXT("Exiting"));
		FEngineLoop::AppPreExit();
		FModuleManager::Get().UnloadModulesAtShutdown();
		FEngineLoop::AppExit();
	};
	
	if (int32 Ret = GEngineLoop.PreInit(ArgC, ArgV))
	{
		return Ret;
	}
	
	//UE_SCOPED_LOG_EVENT_DISPLAY(LogHelloUnreal);
	
	//(LogBlankProgram.GetCategoryName() == LogTemp.GetCategoryName()) ? A = 10 : int B = 10;
	
	//UE_SCOPED_LOG_DISPLAY(LogBlankProgram, "Hello BlankProgram #0");
	//UE_SCOPED_LOG_DISPLAY(LogBlankProgram, "Hello BlankProgram #1");
	{
		//(LogBlankProgram != LogTemp);//
	
		//? _GET_UE_SCOPED_LOG(LogBlankProgram, ELogVerbosity::Display, true, ) : _LOG_OVERRIDE(LogTemp, ELogVerbosity::Display, "Hello LogTemp");;
	
		//const FString Name = ParentProjectName;
		//UE_SCOPED_LOG_DISPLAY(LogTemp, "Hello LogTemp, %s", *Name);;
		//DvScopedLoggerHelperInternal::LogOverride(LogTemp, ELogVerbosity::Display, "Hello LogTemp");;;
	}
	*/

	/*
	FTaskTagScope Scope(ETaskTag::EGameThread);
	TRACE_BOOKMARK(TEXT("DefaultMain"));
	{
	}
	TRACE_BOOKMARK(TEXT("Initializing"));
	{
		if (int32 Ret = GEngineLoop.PreInit(ArgC, ArgV))
		{
			// Error
			return Ret;
		}
	}


	TRACE_BOOKMARK(TEXT("Tick loop starting"));
	{

	}
	TRACE_BOOKMARK(TEXT("Tick loop end"));

	FEngineLoop::AppExit();
	*/







	// start up the main loop
	GEngineLoop.PreInit(ArgC, ArgV);
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();

	::testing::InitGoogleTest(&ArgC, ArgV);

	// Add a UE-formatting printer
	::testing::TestEventListeners& Listeners = ::testing::UnitTest::GetInstance()->listeners();
	Listeners.Append(new UEGTestPrinter);

	ensure(RUN_ALL_TESTS() == 0);

	FEngineLoop::AppPreExit();
	FModuleManager::Get().UnloadModulesAtShutdown();
	FEngineLoop::AppExit();
	FPlatformMisc::RequestExit(false);

	return system("pause");

}

TEST(HelloUnrealTests, HelloUnrealTest0)
{
	SUCCEED();
}



