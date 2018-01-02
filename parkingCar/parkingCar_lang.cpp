/***************************************
 * parkingCar_lang.cpp
 * 名称: 游戏主程序 语言包相关
 * 作者: 雷电暴雨
 * 时间: 2018-01-02 17:43:00
 * 备注:
 ***************************************/

#pragma once
#ifndef parkingCar_lang
#define parkingCar_lang

#define _L(t) (_getLangStr(t).c_str())

void _initLangCfg() {
	static string _modName = "initLangCfg";
	
	char szCfgValTmp[255];
	// 准备语言配置
	strcpy_s(g_szLangPath, g_szExePath);
	strcat_s(g_szLangPath, "\\res\\cfg_lang\\");		// 已经连同检查过 res 文件夹
	strcat_s(g_szLangPath, commConfig->UI_Lang);
	strcat_s(g_szLangPath, ".ini");
	if (_access(g_szLangPath, 4) == -1) {
		MessageBox(graphicHwnd, "Language file init failed! Program will exit.", "Error", MB_ICONERROR);
		exit(1007);
		//_log(_modName, string("Language file init failed, path = ") + string(g_szLangPath), 1007, 5, 1);
	}
	GetPrivateProfileString("lang", "lang", "", szCfgValTmp, 255, g_szLangPath);
	if (strcmp(szCfgValTmp, commConfig->UI_Lang)) {
		MessageBox(graphicHwnd, "Language file may be broken", "Warning", MB_ICONWARNING);
	}
	GetPrivateProfileString("lang", "desc", "", commConfig->UI_LangDesc, 50, g_szLangPath);
}

/**************************************
 getLangStr 获取语言包中特定标签的文本
 参数：
 	const char *tag 文本标签
 返回值：
 	string 标签所对应的文本，若失败为空 ("")
 **************************************/
string _getLangStr(const char *tag) {
	char szTemp[255];
	if (strcmp(g_szLangPath, "") == 0) {
		return "";
	}
	GetPrivateProfileString("text", tag, "", szTemp, 255, g_szLangPath);
	return string(szTemp);
}

#endif