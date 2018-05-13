#ifndef Syncqueue_h


#define Syncqueue_h

//function: ͬ����������
/*
���ܣ���ͬ���������н�����������������⡣

���ܣ�
��һ����������������Ʒ����Щ��Ʒ���ṩ�����ɸ�������ȥ���ѡ�
Ϊ��ʹ�����ߺ��������ܲ���ִ�У�������֮������һ���ж���������Ļ���ء�
�����߽��������Ĳ�Ʒ����һ���������У������߿��Դӻ�������ȡ�߲�Ʒ�������ѡ�
���������ߺ������߶����첽��ʽ���еģ������Ǳ��뱣��ͬ����
�������������ߵ�һ���յĻ�������ȡ��Ʒ��Ҳ��������������һ���Ѿ�װ����Ʒ����δ��ȡ�ߵĻ�������Ͷ�Ų�Ʒ��

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

	std::list<T> m_queue; // ������,������ʵ��
	std::mutex m_mutex; // ������  
	std::condition_variable m_not_empty; // ��Ϊ�յ���������
	std::condition_variable m_not_full; // û��������������

	int m_max_size; // ͬ����������size

	bool m_stop;  // ֹͣ�ı�־,Ĭ������false

private:

	// ���л�û��
	bool not_full()const
	{
		bool full = m_queue.size() >= m_max_size;
		if (full == true) {
			cout << "�������Ѿ����ˣ���Ҫ�ȴ�....." << endl;
		}
		if (full == false) return true;
		else return false;
	}
	// ���в�Ϊ�� 
	bool not_empty()const
	{
		bool empty = m_queue.empty();
		if (empty == true) {
			cout << "������Ϊ�գ���Ҫ�ȴ�...�첽����߳� id Ϊ��";
			cout << this_thread::get_id() << endl;
		}
		if (empty == true) return false;
		else return true;
	}

	//�����¼�����
	template<typename TT>
	void Add(TT && x)
	{
		unique_lock<mutex>locker(m_mutex);
		//��Ҫֹͣ ���� �������������ִ�У�����wait
		m_not_full.wait(locker, [this]{return m_stop || not_full(); });

		if (m_stop == true)  return; //�����Ҫ��ֹ�� return

		//����ֹ����������ӵ�ͬ������
		m_queue.push_back(forward<TT>(x));

		//�����̶߳��в�Ϊ�գ������߳�ȥȡ����
		m_not_empty.notify_one();

	}

public:
	// ��ʼ�������е����Ԫ�ظ������Ƿ���Ҫ��ֹ
	Syncqueue(int max_size) : m_max_size(max_size), m_stop(false)
	{

	}

	// ��ֵ����������Ϊ��&���� Ϊ�˺���ֵ���֣���ֵ����������Ϊ��&&����
	// ���������������,���������汾����ֵ����ֵ����
	void add(const T & x)
	{
		Add(x);
	}
	// ��ֵ����
	void add(T && x)
	{
		Add(forward<T>(x));
	}
	// �� Add ������
	void Out( list<T>& list)
	{
		unique_lock<mutex>locker(m_mutex); // ������

		// ֹͣ���߲��վͼ���ִ��,����wait
		m_not_empty.wait(locker, [this]{ return m_stop || not_empty(); });
		
		if (m_stop == true) return ;

		// move��������Էǳ��򵥵ķ�ʽ����ֵ����ת��Ϊ��ֵ����
		// һ�μ�����һ��ȡ�������е���������

		list = move(m_queue);  //ͨ���ƶ����� m_queue ת�Ƶ� list,�����ǿ���
		m_not_full.notify_one(); //�����߳�ȥ�������
	}

	// ÿ�λ�ȡһ�����ݣ�Ч�ʵ�
	void Out(T & t)
	{
		unique_lock<mutex> locker(m_mutex);
		m_not_full.wait(locker, [this]{ return m_stop || not_empty(); });
		
		if (m_stop == true) return;

		t = m_queue.front(); //ȡ��һ��
		m_queue.pop_front();
		m_not_full.notify_one();
	}

	//���û�����ֹ����
	void Stop()
	{
		{
			lock_guard<mutex>locker(m_mutex);
			m_stop = true; //����Ҫֹͣ��־ ��Ϊ true
			// ִ�е��⣬lock_guard�ͷ���
		}
		//�������еȴ����̣߳���if(m_stop)ʱΪ�棬Ȼ������˳�
		//�����ѵ��߳�ֱ�ӻ�ȡ��
		m_not_empty.notify_all(); 
		m_not_full.notify_all();
	}
	// �ж϶����Ƿ�Ϊ��
	bool is_empty()
	{
		lock_guard<mutex>locker(m_mutex);
		return m_queue.empty();
	}

	// �ж϶����Ƿ�����
	bool is_full()
	{
		lock_guard<mutex> locker(m_mutex);
		return m_max_size == m_queue.size();
	}

	// ��ѯ���еĴ�С
	int query_size()
	{
		lock_guard<mutex> locker(m_mutex);
		return m_queue.size();
	}

};

#endif