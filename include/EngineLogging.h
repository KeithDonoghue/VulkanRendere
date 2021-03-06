#ifndef ENGINE_LOGGING_HDR
#define ENGINE_LOGGING_HDR 1

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

// Flip Here to toggle on in Release or off in Debug regardless of per file changes
//#define ENGINE_LOGGING_ENABLED 1


#ifndef ENGINE_LOGGING_ENABLED
// FLip in here to maintain per file defines.
#define ENGINE_LOGGING_ENABLED _DEBUG
//#define ENGINE_LOGGING_ENABLED 1
#endif


#if ENGINE_LOGGING_ENABLED
#define EngineLog(...) {	\
	LoggingClass * Logger = LoggingClass::GetInstance();  \
	Logger->Log(__VA_ARGS__, __FUNCTION__, __FILE__, __LINE__);	\
	}
#else
#define EngineLog(...) do {} while(0);
#endif 

#endif // ENGINE_LOGGING_HDR
