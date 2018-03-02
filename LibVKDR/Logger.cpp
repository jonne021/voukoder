#include <cstdio>
#include <vector>
#include <sstream>
#include <Windows.h>
#include "Version.h"
#include "Logger.h"

using namespace LibVKDR;

static inline string getTimeStamp()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime); //unsafe

	strftime(buffer, sizeof(buffer), "[%d-%m-%Y %I:%M:%S] ", timeinfo);
	
	return string(buffer);
}

Logger::Logger()
{
	AvCallback<void(void*, int, const char*, va_list)>::func = bind(&Logger::avCallback, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4);

	av_log_set_level(AV_LOG_DEBUG);
	av_log_set_callback(AvCallback<void(void*, int, const char*, va_list)>::callback);

	wstring tempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
	{
		tempPath = wcharPath;

		wstringstream filepath;
		filepath << wstring(wcharPath);
		filepath << "voukoder.log";

		ofs.open(filepath.str(), ofstream::out | ofstream::trunc);
		ofs << LIB_VKDR_APPNAME << endl << "by Daniel Stankewitz" << endl << endl;
	}
}

vector<string> Logger::getLastEntries(int lines)
{
	if (messages.size() < lines)
		lines = (int)messages.size();

	return vector<string>(messages.end() - lines, messages.end());
}

void Logger::message(const string message)
{
	messages.push_back(message);

	ofs << getTimeStamp();
	ofs << message << endl;

	OutputDebugStringA(message.c_str());
}

void Logger::avCallback(void *, int level, const char * szFmt, va_list varg)
{
	char logbuf[2000];
	vsnprintf(logbuf, sizeof(logbuf), szFmt, varg);
	logbuf[sizeof(logbuf) - 1] = '\0';

	message(trim(string(logbuf)));
}