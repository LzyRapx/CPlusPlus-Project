#ifndef ThreadPool_h

#define ThreadPool_h

#include <cstdio>
#include <stack>
#include <vector>
#include <queue>
#include <iostream>
#include <cstdio>
#include <mutex> // �����㷨�������߳�ͬʱ���ʹ�����Դ�����������ݾ��������ṩ�̼߳��ͬ��֧�֡�
#include <condition_variable> //�����������������߳��໥������ͬ��ԭ�������һ�������̵߳ȴ������Զ�ʱ����һ�̵߳����ѣ�Ȼ���ټ�������������ʼ�չ�����һ�����⡣
#include <functional>
#include <future>
#include <thread>
#include <list>
#include <atomic>

//using namespace std;

#include "Syncqueue.h"

using namespace std;

const int max_task = 100; // ���������

class ThreadPool 
{
	

private:
	// �߳��еĺ�������
	using Task = function<void()>;
	// ����������̳߳�,��list����
	list < shared_ptr<thread> > m_thread_pool;
	// ͬ������
	Syncqueue<Task> m_queue;
	// �Ƿ�ֹͣ�ı�־
	atomic_bool m_running;
	// call_once�Ĳ���
	once_flag m_flag;

private:
	//�̳߳ؿ�ʼ��Ԥ�ȴ�������numThreads ���̵߳��̳߳�
	void Start(int num_thread)
	{
		m_running = true;

		// �����̳߳�
		for (int i = 0; i < num_thread; i++) {
			//����ָ����������̺߳���&ThreadPool::RunInThread �Ͷ�Ӧ����this
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
	//��ֹ�̳߳أ����ٳ��е������߳�
	void stop_thread_pool()
	{
		m_queue.Stop(); // ��ͬ�������е��߳���ֹ
		m_running = false;
		for (auto thread : m_thread_pool)
		{
			if (thread) thread->join();
		}
		m_thread_pool.clear();
	}
public:
	//�������ͣ��������޲����޷���ֵ,�����޸�Ϊ�κ����͵ķ��ͺ���ģ��
	

	// hardware_concurrency CPU���� ��Ĭ���߳���
	ThreadPool(int num_thread = thread::hardware_concurrency()) : m_queue(max_task)
	{
		Start(num_thread); // ��ʼ
	}
	// ��������
	~ThreadPool()
	{
		Stop(); // ���û��ֹͣʱ����������ֹ�̳߳�
	}

	//��ֹ�̳߳�,���ٳ��������߳�
	void Stop()
	{
		//��֤���߳������ֻ����һ��stop_Thread_pool
		call_once(m_flag, [this] { stop_thread_pool(); });
	}

	//ͬ������㣺��ͬ���������������,�����汾
	// ͬ������㣺��ͬ�������������������һ��
	// ��ֵ����
	void add_task(const Task& task) {
		m_queue.add(task);
	}
	void add_task(Task&& task) {
		m_queue.add(forward<Task>(task));
	}

};

#endif