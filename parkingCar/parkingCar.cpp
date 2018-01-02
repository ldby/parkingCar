/***************************************
 * parkingCar.cpp
 * ����: ��Ϸ������
 * ����: �׵籩��
 * �汾��1.0.0.126
 * ʱ��: 2018-01-02 17:43:00
 * ��ע: MileStone 2
 ***************************************/

#include <iostream>
#include <map>
#include <direct.h>
#include <io.h>
#include <stdlib.h>
#include <string>
#include <graphics.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>
#include "parkingCar_ZoomImage.h"
#include "parkingCar_define.h"

/***************************
 ****** ȫ�ֱ������忪ʼ ******
 ***************************/

static map<string, pFunc> strFuncMap;		// ����������Ӧ��
static HWND graphicHwnd = NULL;				// ͼ���豸���
PC_Config *commConfig = NULL;				// ��Ϸ����

static char g_szExePath[MAX_PATH] = "";		// ����Ŀ¼
static char g_szIniPath[MAX_PATH] = "";		// �����ļ�·��
static char g_szLangPath[MAX_PATH] = "";	// ���԰�·��
static char g_szLogPath[MAX_PATH] = "";		// ��־�ļ�·��
static int g_logLevel = 0;					// ��־�ȼ�

IMAGE g_carIMG[MAX_CARS];					// ����ͼƬ��Դ
IMAGE g_bgIMG[MAX_BGS];						// ������Դ
IMAGE g_objIMG[MAX_OBJECTS];				// ��ͼ��Դ
IMAGE g_mapIMG[MAX_MAPS];					// ��ͼ��Դ
// AUDIO *AUDIO_LIST;						// ��Ƶ��Դ
IMAGE g_myCarIMG;							// ���Ƴ�����ͼ

/***************************
 ****** ȫ�ֱ���������� ******
 ***************************/

#include "parkingCar_lang.cpp"
#include "parkingCar_debug.cpp"
#include "parkingCar_base.cpp"

// =================== ��Ϸ�߼�����ƿ�ʼ ===================

/**************************************
 loadResources ���ظ�����Դ
 ��������
 ����ֵ����
 **************************************/
void loadResources() {
	static string _modName = "loadResources";
	string resPath = g_szExePath;
	IMAGE imgBuffer;
	char szBuffer[255], temp[MAX_PATH];

	if (g_logLevel < 3) {
		sprintf_s(
			szBuffer,
			"%s V%s_%s [%s %s %s] [DEBUG]",
			_L("ui_title"),
			commConfig->Pro_ver,
			commConfig->Pro_verCode,
			commConfig->UI_Lang,
			commConfig->UI_LangDesc,
			commConfig->UI_Font
		);
	}
	else {
		strcpy_s(szBuffer, _L("ui_title"));
	}
	SetWindowText(graphicHwnd, szBuffer);

	// �����ԴĿ¼�ṹ������
	resPath += "\\res\\";

	strcpy_s(temp, resPath.c_str());
	strcat_s(temp, "\\cfg_scene\\");
	if (_access(resPath.c_str(), 4) == -1) {
		_log(_modName, "Cannot access folder of scene config files", 2001, 5, 1);
	}
	strcpy_s(temp, resPath.c_str());
	strcat_s(temp, "\\bg\\");
	if (_access(resPath.c_str(), 4) == -1) {
		_log(_modName, "Cannot access folder of background images", 2002, 5, 1);
	}
	strcat_s(temp, "\\object\\");
	if (_access(resPath.c_str(), 4) == -1) {
		_log(_modName, "Cannot access folder of object images", 2003, 5, 1);
	}
	strcpy_s(temp, resPath.c_str());
	strcat_s(temp, "\\car\\");
	if (_access(resPath.c_str(), 4) == -1) {
		_log(_modName, "Cannot access folder of car images", 2004, 5, 1);
	}
	strcpy_s(temp, resPath.c_str());
	strcat_s(temp, "\\map\\");
	if (_access(resPath.c_str(), 4) == -1) {
		_log(_modName, "Cannot access folder of map images", 2005, 5, 1);
	}

	// ���ر�����Դ
	_batchLoadImg(g_bgIMG, resPath, "bg", MAX_BGS);
	// ���س�����Դ
	_batchLoadImg(g_carIMG, resPath, "car", MAX_CARS);
	// ���������Դ
	_batchLoadImg(g_objIMG, resPath, "object", MAX_OBJECTS);
	// ���ص�ͼ��Դ
	_batchLoadImg(g_mapIMG, resPath, "map", MAX_MAPS);
	
	_drawLoadingInfo(1);
	Sleep(1000);
}

/**************************************
 reDrawScene �ػ���Ϸ���ؿ�����
 ������
	IMAGE *img_map ��ͼͼƬָ��
	IMAGE *img_myCar ����ԭʼͼƬָ��
	IMAGE *img_realMyCar ������ת��ͼƬָ��
	Car *tCar ����ָ��
 ����ֵ����
 **************************************/
void reDrawScene(IMAGE *img_map, Car *tCar) {
	char debugInfo[1000];
	LOGFONT f;

	// ���ȸ��� angle ��ת���� �����ñ���Ϊ��ɫ
	rotateimage(&g_myCarIMG, &g_carIMG[tCar->orgResId], tCar->angle / 180 * PI, O_BKCOLOR, true);

	
	BeginBatchDraw();
	cleardevice();
	putimage(0, 0, commConfig->UI_Width, commConfig->UI_Height, img_map, 0, 0);

	tCar->x = (int)(tCar->realX - (g_myCarIMG.getwidth() / 2.0));
	tCar->y = (int)(tCar->realY - (g_myCarIMG.getheight() / 2.0));

	_putimage(tCar->x, tCar->y, _ax(g_myCarIMG.getwidth()), _ay(g_myCarIMG.getheight()), g_myCarIMG, O_BKCOLOR);

	_setFont(&f);
	strcpy_s(debugInfo, _genDebugInfo(tCar).c_str());
	RECT r = { 0, 0, commConfig->UI_Width, commConfig->UI_Height };
	setbkmode(TRANSPARENT);
	drawtext(debugInfo, &r, DT_LEFT);
	EndBatchDraw();
}

void updateCar(Car *tCar, int isGas, int isqian, int iszuo) {
	double _angle = 0, _speed = 0;
	double _x = 0, _y = 0;

	// ���㵱ǰ�����ĵ������
	tCar->realX = (int)(g_myCarIMG.getwidth() / 2.0 + tCar->x + 0.5);
	tCar->realY = (int)(g_myCarIMG.getheight() / 2.0 + tCar->y + 0.5);

	// (-180, 180]
	if (tCar->angle > 180) {
		tCar->angle = -(360 - tCar->angle);
	}
	else if (tCar->angle <= -180) {
		tCar->angle = 360 + tCar->angle;
	}
	if (isGas) {
		if (tCar->speed == 0) {
			tCar->speed = (isqian ? 1 : -1);
		}
		else if (tCar->speed > 0) {
			if (isqian) {
				if (tCar->speed < MAX_SPEED) {
					tCar->speed *= ACC_SPEED;
				}
				else {
					tCar->speed = MAX_SPEED;
				}
			}
			else {
				if (tCar->speed > 1) {
					tCar->speed /= DEC_SPEED;
				}
				else {
					tCar->speed = 0;
				}
			}
		}
		else if (tCar->speed < 0) {
			if (!isqian) {
				if (tCar->speed > -MAX_SPEED) {
					tCar->speed *= ACC_SPEED;
				}
				else {
					tCar->speed = -MAX_SPEED;
				}
			}
			else {
				if (tCar->speed < -1) {
					tCar->speed /= DEC_SPEED;
				}
				else {
					tCar->speed = 0;
				}
			}
		}
	}
	else {
		if (tCar->speed > 1 || tCar->speed < -1) {
			tCar->speed /= DEC_SPEED;
		}
		else {
			tCar->speed = 0;
		}
	}

	if (tCar->speed > 0 || tCar->speed < 0) {
		_x = -sin(tCar->angle / 180 * PI) * tCar->speed;
		_y = -cos(tCar->angle / 180 * PI) * tCar->speed;
		tCar->realX += (int)(_x + 0.5);
		tCar->realY += (int)(_y + 0.5);

		tCar->isqian = isqian;
		tCar->iszuo = iszuo;
		tCar->isgas = isGas;
	}
}

void initWelcomeScene() {
	static string _modName = "initWelcomeScene";
	static int _sceneId = 0;

	char szSceneCfgPath[MAX_PATH] = "", szFuncName[50] = "";
	char szTemp[50] = "";
	unsigned int t, srcY, btnCnt, i, funcStat = 0;
	MOUSEMSG msg;
	LOGFONT f;
	Button *btns;

	_initSceneConfig(_sceneId, szSceneCfgPath);
	t = GetPrivateProfileInt("bg", "id", 0, szSceneCfgPath);
	btnCnt = GetPrivateProfileInt("btn", "Cnt", 0, szSceneCfgPath);
	btns = (Button *)malloc(sizeof(Button) * btnCnt);
	for (i = 0; i < btnCnt; i++) {
		_getBtnPos(szSceneCfgPath, &btns[i], i);
	}

	while (1) {
		msg = GetMouseMsg();
		if (msg.mkLButton) {

		}
		for (i = 0; i < btnCnt; i++) {
			if (msg.x >= btns[i].r.left &&
				msg.x <= btns[i].r.right &&
				msg.y >= btns[i].r.top &&
				msg.y <= btns[i].r.bottom) {
				if (msg.mkLButton && !funcStat) {
					btns[i].status = 2;
					strcpy_s(szFuncName, btns[i].func);
					funcStat = 1;
				}
				else {
					btns[i].status = 1;
				}
			}
			else {
				if (btns[i].status == 1 || btns[i].status == 2) {
					btns[i].status = 0;
				}
			}
		}

		BeginBatchDraw();
		cleardevice();
		_setFont(&f, _ay(20));

		putimage(0, 0, &g_bgIMG[t]);

		for (i = 0; i < btnCnt; i++) {
			srcY = btns[i].status * _ay(50);
			putimage(btns[i].x, btns[i].y, btns[i].r.right - btns[i].x, btns[i].r.bottom - btns[i].y, &g_objIMG[btns[i].resId], 0, srcY);
			sprintf_s(szTemp, "ui_%s", btns[i].name);
			drawtext(_L(szTemp), &btns[i].r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		EndBatchDraw();

		if (funcStat) {
			_callFunc(string(szFuncName));
			funcStat = 0;
			strcpy_s(szFuncName, "");
		}
	}
}

void initGameScene(int sceneId) {
	static string _modName = "initGameScene";
	
	Car tCar;
	char szSceneCfgPath[MAX_PATH] = "";
	char szTemp[255];
	int keyStat = 0, mapResId = 0, myCarResId = 0;

	_initSceneConfig(sceneId, szSceneCfgPath);
	mapResId = GetPrivateProfileInt("map", "id", 0, szSceneCfgPath);
	tCar.orgResId = GetPrivateProfileInt("car", "myResId", 0, szSceneCfgPath);
	tCar.realX = GetPrivateProfileInt("car", "myrealX", 0, szSceneCfgPath);
	tCar.realY = GetPrivateProfileInt("car", "myrealY", 0, szSceneCfgPath);
	GetPrivateProfileString("car", "myAngle", "0", szTemp, 255, szSceneCfgPath);
	sscanf_s(szTemp, "%lf", &tCar.angle);
	tCar.speed = 0;
	tCar.isqian = tCar.iszuo = tCar.isgas = 0;

	rotateimage(&g_myCarIMG, &g_carIMG[tCar.orgResId], tCar.angle / 180 * PI, O_BKCOLOR, true);
	tCar.x = (int)(tCar.realX - (g_myCarIMG.getwidth() / 2.0) + 0.5);
	tCar.y = (int)(tCar.realY - (g_myCarIMG.getheight() / 2.0) + 0.5);

	reDrawScene(&g_mapIMG[mapResId], &tCar);

	sprintf_s(szTemp, "g_myCarIMG width = %d, height = %d, tCar.x = %d, tCar.y = %d", g_myCarIMG.getwidth(), g_myCarIMG.getheight(), tCar.x, tCar.y);

	while (1) {
		keyStat = _getCommand();
		if (keyStat & CMD_UP) {
			if (keyStat & CMD_LEFT) {
				tCar.angle += (double)APS * (commConfig->UI_RefRate / 1000.0);
			}
			else if (keyStat & CMD_RIGHT) {
				tCar.angle -= (double)APS * (commConfig->UI_RefRate / 1000.0);
			}
		}

		if (keyStat & CMD_DOWN) {
			if (keyStat & CMD_LEFT) {
				tCar.angle -= (double)APS * (commConfig->UI_RefRate / 1000.0);
			}
			else if (keyStat & CMD_RIGHT) {
				tCar.angle += (double)APS * (commConfig->UI_RefRate / 1000.0);
			}
		}
		updateCar(&tCar, (keyStat & CMD_UP || keyStat & CMD_DOWN), keyStat & CMD_UP, keyStat & CMD_LEFT);
		reDrawScene(&g_mapIMG[mapResId], &tCar);
		Sleep(commConfig->UI_RefRate);
	}

}

void game_start() {
	initGameScene(1);
}

void game_exit() {
	static string _modName = "game_exit";
	_log(_modName, "Game exit");
	_freeCommonResources();
	exit(0);
}

/***************************
* void initFuncMap
* ��������
* ����ֵ����
* ��ע����ʼ����������ָ���
****************************/
void initFuncMap() {
	strFuncMap["game_start"] = &game_start;
	strFuncMap["game_exit"] = &game_exit;
}

int main() {
	static string _modName = "main";
	initFuncMap();
	_initConfig();

	graphicHwnd = initgraph(commConfig->UI_Width, commConfig->UI_Height);
	loadResources();
	initWelcomeScene();

	_log(_modName, "Program exit");
	_freeCommonResources();
	return 0;
}