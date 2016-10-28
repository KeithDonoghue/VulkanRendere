#include "EngineLogging.h"
#include <iostream>
#include <fstream>



//Constructor
LoggingClass::LoggingClass()
{
	mLogFile.open("ErrorLog.txt");
}




//Destructor
LoggingClass::~LoggingClass()
{
	mLogFile.close();
}



/*/
template<typename T>
void LoggingClass::Log(T t)
{
	mLogFile << t << std::endl;
}



template<typename T, typename... Args>
void LoggingClass::Log(T t, Args... args)
{
	mLogFile << t << " ";
	Log(args...);
}

*/


LoggingClass * LoggingClass::GetInstance()
{
	if (sInstance == nullptr)
	{
		sInstance = new LoggingClass();
	}

	return sInstance;
}


LoggingClass * LoggingClass::sInstance = nullptr;