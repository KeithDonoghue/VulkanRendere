#include <iostream>
#include <fstream>
#include <mutex>
		

class LoggingClass
{
	static std::recursive_mutex mtx;
public:
	template<typename T>
	void Log(T t)
	{
		std::lock_guard<std::recursive_mutex> lock(mtx);
		mLogFile << t << std::endl;
	}


	template<typename T, typename... Args>
	void Log(T t, Args... args)
	{
		std::lock_guard<std::recursive_mutex> lock(mtx);

		mLogFile << t << " ";
		Log(args...);
	}

	
	static LoggingClass * GetInstance();
	~LoggingClass();

private:
	LoggingClass();
	static LoggingClass * sInstance;
	std::ofstream mLogFile;
};





#if ENGINE_LOGGING_ENABLED
#define EngineLog(...) {	\
	LoggingClass * Logger = LoggingClass::GetInstance();  \
	Logger->Log(__VA_ARGS__);	\
	}
#else
#define EngineLog(...) do {} while(0);
#endif 