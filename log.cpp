#include "log.h"

using namespace std;


Log& Log::create(const std::string& n, const std::string& p, bool mult_thread)
{
	static Log log(n, p, mult_thread);
	return log;
}

Log::Log(const std::string& n, const std::string& p, 
		bool mult_thread): 
	name(n), path(p), file(NULL), using_mult_thread(mult_thread) 
{
	next_time = time(NULL);	
	next_time = (next_time / DAY_SECONDS + 1) * DAY_SECONDS;

	if(using_mult_thread)
	{
		pthread_mutex_init(&mutex, NULL);
	}
	open_new_file();
}

bool Log::need_open_new_file()
{
	time_t now_time = time(NULL);

	return now_time >= next_time;
}

int Log::open_new_file()
{
	std::string full_path = path + "/" + name;
	std::string day;
	get_year_month_day(day);   

	full_path += "_" + day + ".log";
	file = fopen(full_path.c_str(), "a+");				
	if(file == NULL)
	{
		return -1;
	}
	return 0;
}

void Log::release_file()
{
	fclose(file);
	file = NULL;
}

Log::~Log()
{
	release_file();
	if(using_mult_thread)
	{		
		pthread_mutex_destroy(&mutex);
		using_mult_thread = false;
	}
}

void Log::update_next_time()
{
	next_time += DAY_SECONDS;
}

int Log::print(const char* file_name, int line, 
		const char* func, const char* fmt, ...)
{
	if(using_mult_thread)
	{
		pthread_mutex_lock(&mutex);
	}

	if(need_open_new_file())
	{
		release_file();
		if(open_new_file())
		{
			return -1;
		}
		update_next_time();
	}

	if(file == NULL)
	{
		return -2;
	}

	std::string day, hour, tm;
	get_year_month_day(day);
	get_hour_min_sec(hour);

	tm = day + " " + hour; 
	fprintf(file, "[%s]", tm.c_str());
	fprintf(file, "[%s:%d]", file_name, line);
	fprintf(file, "[%s]: ", func);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(file, fmt, ap);
	va_end(ap);

	fprintf(file, "\n");
	fflush(file);

	if(using_mult_thread)
	{
		pthread_mutex_unlock(&mutex);
	}
	return 0;
}

void Log::get_year_month_day(std::string& res)
{
	time_t cur_time = time(NULL);
	struct tm * timeinfo = localtime(&cur_time);

	char buffer[] = {"yyyy-mm-dd"};
	strftime(buffer, sizeof(buffer), "%F", timeinfo);
	res = std::string(buffer);
}

void Log::get_hour_min_sec(std::string& res)
{
	time_t cur_time = time(NULL);
	struct tm * timeinfo = localtime(&cur_time);

	char buffer[] = {"hh:mm:ss"};
	strftime(buffer, sizeof(buffer), "%T", timeinfo);
	res = std::string(buffer);
}


Log* LogContainer::log = NULL;
const string LogContainer::log_path = ".";
const bool LogContainer::using_mult_thread = true; 

Log* LogContainer::get()
{
	return log;
}

Log* LogContainer::create(const char* name)
{
	return log = &Log::create(name, log_path, using_mult_thread); 
}
