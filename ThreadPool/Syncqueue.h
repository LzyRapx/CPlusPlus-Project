#ifndef Syncqueue_h


#define Syncqueue_h

//function: 同步阻塞队列
/*
功能：用同步阻塞队列解决生产者消费者问题。

介绍：
有一个生产者在生产产品，这些产品将提供给若干个消费者去消费。
为了使生产者和消费者能并发执行，在两者之间设置一个有多个缓冲区的缓冲池。
生产者将它生产的产品放入一个缓冲区中，消费者可以从缓冲区中取走产品进行消费。
所有生产者和消费者都是异步方式运行的，但它们必须保持同步。
即不允许消费者到一个空的缓冲区中取产品，也不允许生产者向一个已经装满产品且尚未被取走的缓冲区中投放产品。

*/
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>

using namespace std;

//#endif

//const int max_size = 100;

template<typename T>
class Syncqueue
{
private:

	std::list<T> m_queue; // 缓冲区,用链表实现
	std::mutex m_mutex; // 互斥量  
	std::condition_variable m_not_empty; // 不为空的条件变量
	std::condition_variable m_not_full; // 没有满的条件变量

	int m_max_size; // 同步队列最大的size

	bool m_stop;  // 停止的标志,默认是是false

private:

	// 队列还没满
	bool not_full()const
	{
		bool full = m_queue.size() >= m_max_size;
		if (full == true) {
			cout << "缓冲区已经满了，需要等待....." << endl;
		}
		if (full == false) return true;
		else return false;
	}
	// 队列不为空 
	bool not_empty()const
	{
		bool empty = m_queue.empty();
		if (empty == true) {
			cout << "缓冲区为空，需要等待...异步层的线程 id 为：";
			cout << this_thread::get_id() << endl;
		}
		if (empty == true) return false;
		else return true;
	}

	//范型事件函数
	template<typename TT>
	void Add(TT && x)
	{
		unique_lock<mutex>locker(m_mutex);
		//需要停止 或者 不满则继续往下执行，否则wait
		m_not_full.wait(locker, [this]{return m_stop || not_full(); });

		if (m_stop == true)  return; //如果需要终止就 return

		//不终止，把任务添加到同步队列
		m_queue.push_back(forward<TT>(x));

		//提醒线程队列不为空，唤醒线程去取数据
		m_not_empty.notify_one();

	}

public:
	// 初始化：队列的最大元素个数，是否需要终止
	Syncqueue(int max_size) : m_max_size(max_size), m_stop(false)
	{

	}

	// 左值的声明符号为”&”， 为了和左值区分，右值的声明符号为”&&”。
	// 往队列中添加任务,重载两个版本，左值和右值引用
	void add(const T & x)
	{
		Add(x);
	}
	// 右值引用
	void add(T && x)
	{
		Add(forward<T>(x));
	}
	// 和 Add 反操作
	void Out( list<T>& list)
	{
		unique_lock<mutex>locker(m_mutex); // 互斥量

		// 停止或者不空就继续执行,不用wait
		m_not_empty.wait(locker, [this]{ return m_stop || not_empty(); });
		
		if (m_stop == true) return ;

		// move这个函数以非常简单的方式将左值引用转换为右值引用
		// 一次加锁，一下取出队列中的所有数据

		list = move(m_queue);  //通过移动，将 m_queue 转移到 list,而不是拷贝
		m_not_full.notify_one(); //唤醒线程去添加任务
	}

	// 每次获取一个数据，效率低
	void Out(T & t)
	{
		unique_lock<mutex> locker(m_mutex);
		m_not_full.wait(locker, [this]{ return m_stop || not_empty(); });
		
		if (m_stop == true) return;

		t = m_queue.front(); //取出一个
		m_queue.pop_front();
		m_not_full.notify_one();
	}

	//让用户能终止任务
	void Stop()
	{
		{
			lock_guard<mutex>locker(m_mutex);
			m_stop = true; //将需要停止标志 置为 true
			// 执行到这，lock_guard释放锁
		}
		//唤醒所有等待的线程，到if(m_stop)时为真，然后相继退出
		//被唤醒的线程直接获取锁
		m_not_empty.notify_all(); 
		m_not_full.notify_all();
	}
	// 判断队列是否为空
	bool is_empty()
	{
		lock_guard<mutex>locker(m_mutex);
		return m_queue.empty();
	}

	// 判断队列是否满了
	bool is_full()
	{
		lock_guard<mutex> locker(m_mutex);
		return m_max_size == m_queue.size();
	}

	// 查询队列的大小
	int query_size()
	{
		lock_guard<mutex> locker(m_mutex);
		return m_queue.size();
	}

};

#endif