#include <iostream>
#include <typeinfo>
#include <ctime>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "log.h" 

using namespace std;

void* thr_fn(void* arg)
{
	int n = 20;
	for(int i = 0; i < n; ++i)
	{
		DEBUG_LOG("12345");
	}
	return (void*)0;
}

void test_mult_thread()
{
	const int n = 5;
	pthread_t tid[n];
	for(int i = 0; i < n; ++i)
	{
		pthread_create(&tid[i], NULL, thr_fn, NULL);
	}
	
	for(int i = 0; i < n; ++i)
	{
		pthread_join(tid[i], NULL);
	}
}


void test_print()
{
	DEBUG_LOG("int: %d, str: %s, double: %lf", 1, "hello world", 1.5);
}

int main()
{
	INIT_LOG("test_log");
	test_print();
	test_mult_thread();	
	return 0;
}
