#ifndef __LOG__
#define __LOG__

#include <iostream>
#include <string>
#include <stdio.h>
#include <ctime>
#include <stdarg.h>
#include <pthread.h>


class Log
{
private:
	enum 
	{
		DAY_SECONDS = 24 * 60 * 60	
	};

	std::string name;
	std::string path; 
	
	time_t next_time;  //下一次新建日志的时间
	FILE* file;

	bool using_mult_thread;
	pthread_mutex_t mutex;

private:
	Log(const std::string& n, const std::string& p = ".", 
			bool mult_thread = false);
	~Log();
	Log(const Log&);
	Log operator=(const Log&);
	
public:
	static Log& create(const std::string& n, const std::string& p = ".", 
			bool mult_thread = false);

	bool need_open_new_file();
	int open_new_file();
	void release_file();
	void update_next_time();
	int print(const char* file_name, int line, 
			const char* func, const char* fmt, ...);

private:
	void get_year_month_day(std::string& res);
	void get_hour_min_sec(std::string& res);	
};

class LogContainer
{
private:	
	static Log* log;
	static const std::string log_path;
	static const bool using_mult_thread;

public:
	static Log* get();
	static Log* create(const char*);
};


#define INIT_LOG(name) \
	do \
	{ \
		if(LogContainer::get() == NULL) \
		{ \
			LogContainer::create(name); \
		} \
	} while(0)


#define DEBUG_LOG(...) \
	do \
	{ \
		Log* p = LogContainer::get(); \
		p->print(__FILE__, __LINE__, __FUNCTION__ ,__VA_ARGS__); \
	} while(0)

#endif
	
