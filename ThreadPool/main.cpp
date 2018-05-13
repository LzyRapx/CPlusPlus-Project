
// main.cpp

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <cstdio>
#include <thread>
#include <stack>
#include <algorithm>

#include "ThreadPool.h"

using namespace std;

mutex mtx;

int main()
{
	ThreadPool pool(4); //线程池创建4个线程，异步层此时无任务需要先等待
	

	//创建两个同步层的线程不断往线程池中添加任务

	//输出同步层线程ID，用lambda表达式表示,每个线程处理十个任务
	thread thread_one([&pool]
	{
		for (int i = 0; i < 10; i++)
		{
			auto thdId = this_thread::get_id();
			pool.add_task([thdId]
			{ //大括号作用：临时变量的生存期，即控制locker的时间
				lock_guard<mutex> locker(mtx);
				cout << "同步层线程一的线程ID： " << thdId << endl;
				cout << "ID = " << this_thread::get_id() << endl;
			});
		}
	});

	thread thread_two([&pool]
	{
		for (int i = 0; i < 10; i++)
		{
			auto thdId = this_thread::get_id();
			pool.add_task([thdId]{
				lock_guard<mutex> locker(mtx);
				cout << "同步层线程二的线程ID： " << thdId << endl;
				cout << "ID = " << this_thread::get_id() << endl;
			});
		}
	});
	this_thread::sleep_for(chrono::seconds(2));
	getchar();

	//停止线程池
	pool.Stop();
	cout << "finish" << endl;
	//等待同步层的两个线程执行完
	thread_one.join();
	thread_two.join();

	std:cout << "finish" << std::endl;
	system("pause");
	return 0;
}