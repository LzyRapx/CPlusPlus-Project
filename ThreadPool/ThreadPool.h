#ifndef ThreadPool_h

#define ThreadPool_h

#include <cstdio>
#include <stack>
#include <vector>
#include <queue>
#include <iostream>
#include <cstdio>
#include <mutex> // 互斥算法避免多个线程同时访问共享资源。这会避免数据竞争，并提供线程间的同步支持。
#include <condition_variable> //条件变量是允许多个线程相互交流的同步原语。它允许一定量的线程等待（可以定时）另一线程的提醒，然后再继续。条件变量始终关联到一个互斥。
#include <functional>
#include <future>
#include <thread>
#include <list>
#include <atomic>

//using namespace std;

#include "Syncqueue.h"

using namespace std;

const int max_task = 100; // 最大任务数

class ThreadPool 
{
	

private:
	// 线程中的函数对象
	using Task = function<void()>;
	// 处理任务的线程池,用list保存
	list < shared_ptr<thread> > m_thread_pool;
	// 同步队列
	Syncqueue<Task> m_queue;
	// 是否停止的标志
	atomic_bool m_running;
	// call_once的参数
	once_flag m_flag;

private:
	//线程池开始，预先创建包含numThreads 个线程的线程池
	void Start(int num_thread)
	{
		m_running = true;

		// 创建线程池
		for (int i = 0; i < num_thread; i++) {
			//智能指针管理，给出线程函数&ThreadPool::RunInThread 和对应参数this
			m_thread_pool.push_back(make_shared<thread>(&ThreadPool::RunInThread, this));
		}
	}
	void RunInThread()
	{
		while (m_running == true)
		{
			list<Task>list;
			m_queue.Out(list);
			for (auto & Task : list)
			{
				if (m_running == true) return;
				Task();
			}
		}
	}
	//终止线程池，销毁池中的所有线程
	void stop_thread_pool()
	{
		m_queue.Stop(); // 让同步队列中的线程终止
		m_running = false;
		for (auto thread : m_thread_pool)
		{
			if (thread) thread->join();
		}
		m_thread_pool.clear();
	}
public:
	//任务类型，这里是无参数无返回值,可以修改为任何类型的范型函数模板
	

	// hardware_concurrency CPU核数 当默认线程数
	ThreadPool(int num_thread = thread::hardware_concurrency()) : m_queue(max_task)
	{
		Start(num_thread); // 开始
	}
	// 析构函数
	~ThreadPool()
	{
		Stop(); // 如果没有停止时，则主动终止线程池
	}

	//终止线程池,销毁池中所有线程
	void Stop()
	{
		//保证多线程情况下只调用一次stop_Thread_pool
		call_once(m_flag, [this] { stop_thread_pool(); });
	}

	// 同步服务层：往同步队列中添加任务,两个版本
	// 同步服务层：往同步队列中添加任务，重载一下
	// 左值引用
	void add_task(const Task& task) {
		m_queue.add(task);
	}
	void add_task(Task&& task) {
		m_queue.add(forward<Task>(task));
	}

};

#endif
