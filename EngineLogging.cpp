#include "EngineLogging.h"
#include <iostream>
#include <fstream>



//Constructor
LoggingClass::LoggingClass()
{
	mLogFile.open("Logs/ErrorLog.txt");
}




//Destructor
LoggingClass::~LoggingClass()
{
	mLogFile.close();
}





LoggingClass * LoggingClass::GetInstance()
{
	if (sInstance == nullptr)
	{
		sInstance = new LoggingClass();
	}

	return sInstance;
}


LoggingClass * LoggingClass::sInstance = nullptr;