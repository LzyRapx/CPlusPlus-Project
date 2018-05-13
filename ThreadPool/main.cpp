
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
	ThreadPool pool(4); //�̳߳ش���4���̣߳��첽���ʱ��������Ҫ�ȵȴ�
	

	//��������ͬ������̲߳������̳߳����������

	//���ͬ�����߳�ID����lambda���ʽ��ʾ,ÿ���̴߳���ʮ������
	thread thread_one([&pool]
	{
		for (int i = 0; i < 10; i++)
		{
			auto thdId = this_thread::get_id();
			pool.add_task([thdId]
			{ //���������ã���ʱ�����������ڣ�������locker��ʱ��
				lock_guard<mutex> locker(mtx);
				cout << "ͬ�����߳�һ���߳�ID�� " << thdId << endl;
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
				cout << "ͬ�����̶߳����߳�ID�� " << thdId << endl;
				cout << "ID = " << this_thread::get_id() << endl;
			});
		}
	});
	this_thread::sleep_for(chrono::seconds(2));
	getchar();

	//ֹͣ�̳߳�
	pool.Stop();
	cout << "finish" << endl;
	//�ȴ�ͬ����������߳�ִ����
	thread_one.join();
	thread_two.join();

	std:cout << "finish" << std::endl;
	system("pause");
	return 0;
}