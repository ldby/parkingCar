/***************************************
 * parkingCar_debug.cpp
 * 名称: 游戏调试及日志记录相关
 * 作者: 雷电暴雨
 * 时间: 2018-01-02 17:44:00
 * 备注: 
 ***************************************/

#pragma once
#ifndef parkingCar_debug
#define parkingCar_debug

/**************************************
 freeCommonResources 释放公共资源内存空间
 参数：无
 返回值：无
 **************************************/
void _freeCommonResources() {
	if (commConfig) free(commConfig);
	if (graphicHwnd) closegraph();
}

/**************************************
 log 记录日志
 参数：
 	string modName	模块名
 	string msg		提示信息
 	int errCode		错误代码，默认为0
 	int level		日志级别，默认为1
 	int isExit		是否退出，默认为0
 返回值：无
 备注：全局日志处理
 **************************************/

void _log(string modName, string msg, int errCode = 0, int level = 1, int isExit = 0) {
	string types[5] = { "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" };
	string optBuffer = "";
	char szDbgInfo[1000], szTemp[255];
	UINT uIcon = MB_ICONINFORMATION;
	time_t today;
	tm *nowTime = new tm();
	FILE *logFile = NULL;

	if (level == 3) uIcon = MB_ICONWARNING;
	else if (level >= 4) uIcon = MB_ICONERROR;
	if (level > 5) level = 5;

	time(&today);
	localtime_s(nowTime, &today);
	strftime(szTemp, 255, "%Y-%m-%d %H:%M:%S", nowTime);

	optBuffer += "[";
	optBuffer.append(szTemp);
	optBuffer += "][";
	optBuffer.append(types[level - 1]);
	optBuffer += "][" + modName + "]";

	if (errCode != 0) {
		sprintf_s(szTemp, "[%d]", errCode);
		optBuffer.append(szTemp);
	}
	optBuffer += " " + msg;

	strcpy_s(szDbgInfo, optBuffer.c_str());
	strcat_s(szDbgInfo, "\n");
	OutputDebugString(szDbgInfo);

	if (g_logLevel > 0 && level >= g_logLevel) {
		if (!fopen_s(&logFile, g_szLogPath, "a+")) {
			fprintf_s(logFile, "%s\n", optBuffer.c_str());
			fclose(logFile);
		}
	}

	if (level > 2) {
		sprintf_s(szTemp, "error_%d", errCode);
		strcpy_s(szTemp, _L(szTemp));
		if (!strcmp(szTemp, "")) {
			MessageBox(graphicHwnd, msg.c_str(), commConfig->Pro_name, uIcon);
		}
		else if (g_logLevel < 3) {
			strcat_s(szTemp, "\n\n");
			strcat_s(szTemp, msg.c_str());
			MessageBox(graphicHwnd, szTemp, commConfig->Pro_name, uIcon);
		}
		else {
			MessageBox(graphicHwnd, szTemp, commConfig->Pro_name, uIcon);
		}
	}

	if (isExit) {
		_freeCommonResources();
		exit(errCode);
	}
}

/**************************************
 initDebugInfoLevel 设置全局日志级别
 参数：
 	int isDebug	是否为调试模式
 返回值：无
 **************************************/
void _initDebug(int isDebug) {
	static string _modName = "initDebug";

	char szLogTmp[255];

	if (g_logLevel != -1) {
		if (isDebug) {
			g_logLevel = 1;
		}
		else {
			g_logLevel = 3;
		}
	}
	sprintf_s(szLogTmp, "Log level set to %d", g_logLevel);
	_log(_modName, szLogTmp);
}

/**************************************
 initLogFile 初始化日志记录文件
 参数：无
 返回值：无
 **************************************/
void _initLogFile() {
	static string _modName = "initLogFile";
	time_t today;
	tm *nowTime = new tm();
	FILE *logFile;
	char szLogFileName[MAX_PATH] = "";

	// 初始化日志文件
	time(&today);
	localtime_s(nowTime, &today);
	strftime(szLogFileName, MAX_PATH, "parkingCar_%Y_%m_%d_%H_%M_%S.log", nowTime);
	strcpy_s(g_szLogPath, g_szExePath);
	strcat_s(g_szLogPath, "\\logs\\");

	if (_access(g_szLogPath, 6) == -1 && _mkdir(g_szLogPath) == -1) {
		_log(_modName, string("Create log file dir failed, path = ") + string(g_szLogPath), 1002, 3);
		g_logLevel = -1;
	}
	else {
		strcat_s(g_szLogPath, szLogFileName);
		;
		if (fopen_s(&logFile, g_szLogPath, "w+")) {
			_log(_modName, string("Create log file failed, path = ") + string(g_szLogPath), 1003, 3);
			g_logLevel = -1;
		}
		else {
			fclose(logFile);
			_log(_modName, string("Log file ready, path = ") + string(g_szLogPath));
		}
	}
}

/**************************************
 generateDebugInformation 生成实时调试信息
 参数：
	Car *tCar 车辆结构体指针
 返回值：
	string 调试文本内容
 **************************************/
string _genDebugInfo(Car *tCar) {
	char buffer[80] = "", str[1000] = "";
	strcat_s(str, commConfig->Pro_name);
	strcat_s(str, " ");
	strcat_s(str, commConfig->Pro_verCode);
	strcat_s(str, "\n");
	strcat_s(str, "clientVersion: ");
	strcat_s(str, commConfig->Pro_ver);
	strcat_s(str, "\n");
	strcat_s(str, "==== DEBUG INFO ====\n");
	strcat_s(str, "Status of car\n");
	sprintf_s(buffer, "X: %d\nY: %d\nAngle: %.6lf\nSpeed: %.6lf\n", tCar->x, tCar->y, tCar->angle, tCar->speed);
	strcat_s(str, buffer);
	sprintf_s(buffer, "realX: %d\nrealY: %d\n", tCar->realX, tCar->realY);
	strcat_s(str, buffer);
	sprintf_s(buffer, "isGas: %d\nisqian: %d\niszuo: %d\n", tCar->isgas, tCar->isqian, tCar->iszuo);
	strcat_s(str, buffer);
	return string(str);
}

#endif