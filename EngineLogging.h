#include <iostream>
#include <fstream>

		

class LoggingClass
{
public:
	template<typename T>
	void Log(T t)
	{
		mLogFile << t << std::endl;
	}


	template<typename T, typename... Args>
	void Log(T t, Args... args)
	{
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
#define EngineLog(...)  LoggingClass * Logger = LoggingClass::GetInstance();  \
	Logger->Log(__VA_ARGS__);
#else
#define EngineLog(...) do {} while(0);
#endif 