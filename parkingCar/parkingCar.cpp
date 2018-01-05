/***************************************
 * parkingCar.cpp
 * 名称: 游戏主程序
 * 作者: 雷电暴雨
 * 版本: 1.0.1.131
 * 时间: 2018-01-05 13:36:00
 * 备注: MileStone 3
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
 ****** 全局变量定义开始 ******
 ***************************/

static map<string, pFunc> strFuncMap;		// 公开函数对应表
static HWND graphicHwnd = NULL;				// 图形设备句柄
PC_Config *commConfig = NULL;				// 游戏设置

static char g_szExePath[MAX_PATH] = "";		// 运行目录
static char g_szIniPath[MAX_PATH] = "";		// 配置文件路径
static char g_szLangPath[MAX_PATH] = "";	// 语言包路径
static char g_szLogPath[MAX_PATH] = "";		// 日志文件路径
static int g_logLevel = 0;					// 日志等级

IMAGE g_carIMG[MAX_CARS];					// 车体图片资源
IMAGE g_bgIMG[MAX_BGS];						// 背景资源
IMAGE g_objIMG[MAX_OBJECTS];				// 贴图资源
IMAGE g_mapIMG[MAX_MAPS];					// 地图资源
// AUDIO *AUDIO_LIST;						// 音频资源
IMAGE g_myCarIMG;							// 控制车体贴图

/***************************
 ****** 全局变量定义结束 ******
 ***************************/

#include "parkingCar_lang.cpp"
#include "parkingCar_debug.cpp"
#include "parkingCar_base.cpp"

// =================== 游戏逻辑与控制开始 ===================

/**************************************
 loadResources 加载各类资源
 参数：无
 返回值：无
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

	// 检查资源目录结构完整性
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

	// 加载背景资源
	_batchLoadImg(g_bgIMG, resPath, "bg", MAX_BGS);
	// 加载车体资源
	_batchLoadImg(g_carIMG, resPath, "car", MAX_CARS);
	// 加载物件资源
	_batchLoadImg(g_objIMG, resPath, "object", MAX_OBJECTS);
	// 加载地图资源
	_batchLoadImg(g_mapIMG, resPath, "map", MAX_MAPS);
	
	_drawLoadingInfo(1);
	Sleep(1000);
}

/**************************************
 reDrawScene 重画游戏各关卡场景
 参数：
	IMAGE *img_map 地图图片指针
	IMAGE *img_myCar 车体原始图片指针
	IMAGE *img_realMyCar 车体旋转后图片指针
	Car *tCar 车辆指针
 返回值：无
 **************************************/
void reDrawScene(IMAGE *img_map, Car *tCar, int isChangeAngel = 0, char *mapData = NULL, const int mapLen = 0) {
	char debugInfo[1000];
	LOGFONT f;
	DWORD *pCarImg;
	int i, j, carW, carH, nowX, nowY, nowOx, nowOy, nowOpos, coveredPx = 0, outPx = 0;
	int isCrash = 0;
	
	BeginBatchDraw();
	cleardevice();
	putimage(0, 0, commConfig->UI_Width, commConfig->UI_Height, img_map, 0, 0);

	// 首先根据 angle 旋转车体 并设置背景为黑色
	if (isChangeAngel) {
		rotateimage(&g_myCarIMG, &g_carIMG[tCar->orgResId], tCar->angle / 180 * PI, O_BKCOLOR, true);
	}
	tCar->x = tCar->realX - (g_myCarIMG.getwidth() / 2.0);
	tCar->y = tCar->realY - (g_myCarIMG.getheight() / 2.0);

	if (mapData) {
		carW = g_myCarIMG.getwidth();
		carH = g_myCarIMG.getheight();
		if (isChangeAngel || strlen(tCar->range) <= 1) {
			pCarImg = GetImageBuffer(&g_myCarIMG);
			tCar->bodyPx = 0;
			for (i = 0, nowY = (int)(tCar->y); i < carH; i++, nowY++) {
				for (j = 0, nowX = (int)(tCar->x); j < carW; j++, nowX++) {
					if (pCarImg[i * carW + j] == 0x000000) {
						tCar->range[i * carW + j] = '0';
					}
					else {
						tCar->range[i * carW + j] = '1';
						tCar->bodyPx++;
						if (nowX < 0 || nowX > commConfig->UI_Width || nowY < 0 || nowY > commConfig->UI_Height) {
							outPx++;
							continue;
						}
						else {
							nowOx = (int)(nowX / commConfig->UI_wZoomRate + 0.5);
							nowOy = (int)(nowY / commConfig->UI_hZoomRate + 0.5);
							nowOpos = nowOy * O_WIDTH + nowOx;
							if (mapData[nowOy * O_WIDTH + nowOx] == '0') {
								isCrash = 1;
								break;
							}
							else if (mapData[nowOy * O_WIDTH + nowOx] == '9') {
								coveredPx++;
							}
						}
					}
				}
				if (isCrash) break;
			}
		}
		else {
			for (i = 0, nowY = (int)(tCar->y); i < carH; i++, nowY++) {
				for (j = 0, nowX = (int)(tCar->x); j < carW; j++, nowX++) {
					if (tCar->range[i * carW + j] == '1') {
						if (nowX < 0 || nowX > commConfig->UI_Width || nowY < 0 || nowY > commConfig->UI_Height) {
							outPx++;
							continue;
						}
						else {
							nowOx = (int)(nowX / commConfig->UI_wZoomRate + 0.5);
							nowOy = (int)(nowY / commConfig->UI_hZoomRate + 0.5);
							nowOpos = nowOy * O_WIDTH + nowOx;
							if (mapData[nowOy * O_WIDTH + nowOx] == '0') {
								isCrash = 1;
								break;
							}
							else if (mapData[nowOy * O_WIDTH + nowOx] == '9') {
								coveredPx++;
							}
						}
					}
				}
				if (isCrash) break;
			}
		}
		tCar->outPx = outPx;
		tCar->coveredPx = coveredPx;
	}
	
	_putimage(
		(int)(tCar->x + 0.5),
		(int)(tCar->y + 0.5),
		(int)(_ax(g_myCarIMG.getwidth()) + 0.5),
		(int)(_ay(g_myCarIMG.getheight()) + 0.5),
		g_myCarIMG,
		O_BKCOLOR
	);
	//putimage((int)(tCar->x + 0.5), (int)(tCar->y + 0.5), &g_myCarIMG);

	// 调试模式下 绘制调试信息
	if (g_logLevel < 3) {
		_setFont(&f);
		_genDebugInfo(tCar, debugInfo);
		RECT r = { 0, 0, commConfig->UI_Width, commConfig->UI_Height };
		setbkmode(TRANSPARENT);
		drawtext(debugInfo, &r, DT_LEFT);

		setfillcolor(RED);
		fillcircle((int)(tCar->realX + 0.5), (int)(tCar->realY + 0.5), 3);
	}
	EndBatchDraw();

	if (isCrash) {
		MessageBox(graphicHwnd, "You crashed!", "Game over", MB_ICONERROR);
		_callFunc("game_exit");
	}
	if (coveredPx && tCar->bodyPx && abs(coveredPx - tCar->bodyPx) < 5) {
		MessageBox(graphicHwnd, "You win!", "Game clear", MB_ICONINFORMATION);
		_callFunc("game_exit");
	}
	if (abs(tCar->bodyPx - outPx) < (int)(tCar->bodyPx * 0.2)) {
		MessageBox(graphicHwnd, "You out!", "Game over", MB_ICONINFORMATION);
		_callFunc("game_exit");
	}
}

void updateCar(Car *tCar, int isGas, int isqian, int iszuo) {
	double _angle = 0, _speed = 0;
	double _x = 0, _y = 0;

	// 计算当前车中心点的坐标
	tCar->realX = g_myCarIMG.getwidth() / 2.0 + tCar->x;
	tCar->realY = g_myCarIMG.getheight() / 2.0 + tCar->y;

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
		tCar->realX += _x;
		tCar->realY += _y;

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
		_setFont(&f, (int)(_ay(20) + 0.5));

		putimage(0, 0, &g_bgIMG[t]);

		for (i = 0; i < btnCnt; i++) {
			srcY = btns[i].status * (int)(_ay(50) + 0.5);
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
	char *mapData = (char *)malloc(O_WIDTH * O_HEIGHT * sizeof(char));
	int mapLen = 0;
	int i = 0;
	int isChangeAngel = 0;

	_initSceneConfig(sceneId, szSceneCfgPath, mapData);
	mapLen = strlen(mapData);
	mapResId = GetPrivateProfileInt("map", "id", 0, szSceneCfgPath);
	tCar.orgResId = GetPrivateProfileInt("car", "myResId", 0, szSceneCfgPath);
	tCar.realX = (double)GetPrivateProfileInt("car", "myrealX", 0, szSceneCfgPath);
	tCar.realY = (double)GetPrivateProfileInt("car", "myrealY", 0, szSceneCfgPath);
	GetPrivateProfileString("car", "myAngle", "0", szTemp, 255, szSceneCfgPath);
	sscanf_s(szTemp, "%lf", &tCar.angle);
	tCar.speed = 0;
	tCar.isqian = tCar.iszuo = tCar.isgas = 0;

	rotateimage(&g_myCarIMG, &g_carIMG[tCar.orgResId], tCar.angle / 180 * PI, O_BKCOLOR, true);
	tCar.x = tCar.realX - (g_myCarIMG.getwidth() / 2.0);
	tCar.y = tCar.realY - (g_myCarIMG.getheight() / 2.0);
	tCar.range = (char *)malloc(sizeof(char) * O_WIDTH * O_HEIGHT);
	tCar.range[0] = '\0';
	tCar.bodyPx = 0;

	reDrawScene(&g_mapIMG[mapResId], &tCar, 0, mapData);

	sprintf_s(szTemp, "g_myCarIMG width = %d, height = %d, tCar.x = %.4lf, tCar.y = %.4lf", g_myCarIMG.getwidth(), g_myCarIMG.getheight(), tCar.x, tCar.y);

	while (1) {
		keyStat = _getCommand();
		if (keyStat & CMD_UP) {
			if (keyStat & CMD_LEFT) {
				tCar.angle += (double)APS * (commConfig->UI_RefRate / 1000.0) * (tCar.speed / MAX_SPEED);
				isChangeAngel = 1;
			}
			else if (keyStat & CMD_RIGHT) {
				tCar.angle -= (double)APS * (commConfig->UI_RefRate / 1000.0) * (tCar.speed / MAX_SPEED);
				isChangeAngel = 1;
			}
		}

		if (keyStat & CMD_DOWN) {
			if (keyStat & CMD_LEFT) {
				tCar.angle += (double)APS * (commConfig->UI_RefRate / 1000.0) * (tCar.speed / MAX_SPEED);
				isChangeAngel = 1;
			}
			else if (keyStat & CMD_RIGHT) {
				tCar.angle -= (double)APS * (commConfig->UI_RefRate / 1000.0) * (tCar.speed / MAX_SPEED);
				isChangeAngel = 1;
			}
		}
		updateCar(&tCar, (keyStat & CMD_UP || keyStat & CMD_DOWN), keyStat & CMD_UP, keyStat & CMD_LEFT);
		reDrawScene(&g_mapIMG[mapResId], &tCar, isChangeAngel, mapData, mapLen);

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
* 参数：无
* 返回值：无
* 备注：初始化公开函数指针表
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