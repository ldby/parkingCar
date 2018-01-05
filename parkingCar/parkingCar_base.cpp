/***************************************
 * parkingCar_base.cpp
 * ����: ��Ϸ������ �����Ժ���
 * ����: �׵籩��
 * ʱ��: 2018-01-05 13:01:00
 * ��ע:
 ***************************************/

#ifndef parkingCar_base
#define parkingCar_base

// =================== ���������Ժ�����ʼ ===================

/**************************************
 callFunc �����Զ��庯���ӿ�
 ������
	const string &str �Զ��庯����
 ����ֵ����
 ��ע�����ɵ����޲������������޷���ֵ
 **************************************/
void _callFunc(const string &str) {
	static string _modName = "callFunc";

	char szLogTmp[255] = "";

	if (strFuncMap.count(str)) {
		(*strFuncMap[str])();
	}
	else {
		sprintf_s(szLogTmp, "callFunc failed! Unsupported function name: %s", str.c_str());
		_log(_modName, szLogTmp, 3002, 4);
	}
}

/**************************************
 setFontStyle ͨ���������� (�������� 4)
 ������
	LOGFONT *f ����ṹ��ָ��
	int size �����С (Ĭ�� 16)
	UINT color ������ɫ���� (Ĭ�� WHITE)
 ����ֵ����
 **************************************/
void _setFont(LOGFONT *f, int size = 16, UINT color = WHITE) {
	settextcolor(color);
	gettextstyle(f);
	f->lfHeight = size;
	strcpy_s(f->lfFaceName, commConfig->UI_Font);
	f->lfQuality = ANTIALIASED_QUALITY;
	settextstyle(f);
}

/**************************************
 getButtonPosition ���㽻����ť��ʾλ��
 ������
	const char *sceneCfgPath ��������·��
	Button *btn ������ť�ṹ��ָ��
 ����ֵ����
 **************************************/
void _getBtnPos(const char *szSceneCfgPath, Button *btn, int btnId) {
	static string _modName = "getBtnPos";

	char szBtnTag[255] = "", szLogTmp[255];

	sprintf_s(szBtnTag, "btn%d", btnId + 1);
	btn->resId = GetPrivateProfileInt(szBtnTag, "resId", 0, szSceneCfgPath);
	GetPrivateProfileString(szBtnTag, "name", "", btn->name, 50, szSceneCfgPath);
	btn->x = (int)(_ax(GetPrivateProfileInt(szBtnTag, "x", 0, szSceneCfgPath)) + 0.5);
	btn->y = (int)(_ay(GetPrivateProfileInt(szBtnTag, "y", 0, szSceneCfgPath)) + 0.5);
	GetPrivateProfileString(szBtnTag, "func", "", btn->func, 50, szSceneCfgPath);
	btn->r.left = btn->x;
	btn->r.top = btn->y;
	btn->r.right = btn->x + g_objIMG[btn->resId].getwidth();
	btn->r.bottom = btn->y + (g_objIMG[btn->resId].getheight() / 4);
	btn->status = 0;

	sprintf_s(szLogTmp, "btn name = %s, range = {%d, %d, %d, %d}", btn->name, btn->r.left, btn->r.top, btn->r.right, btn->r.bottom);
	_log(_modName, szLogTmp);
}

/**************************************
 initConfig ��ʼ����Ϸ����
 ��������
 ����ֵ����
 **************************************/
void _initConfig() {
	static string _modName = "initConfig";
	
	// ��ȡ����ǰ����·��
	GetModuleFileName(NULL, g_szExePath, MAX_PATH);
	char *pFind = strrchr(g_szExePath, '\\');
	if (!pFind)
	{
		_log(_modName, "Get g_szExePath failed", 1001, 5, 1);
	}
	*pFind = '\0';
	_log(_modName, string("g_szExePath = ") + string(g_szExePath));

	// ��ʼ����־��¼�ļ�
	_initLogFile();

	// ��ʼ�������ļ�����Ϸ����
	strcpy_s(g_szIniPath, g_szExePath);
	strcat_s(g_szIniPath, "\\");
	strcat_s(g_szIniPath, CONFIG_FILE);
	_log(_modName, "g_szIniPath = " + string(g_szIniPath));

	commConfig = (PC_Config *)malloc(sizeof(PC_Config));
	if (!commConfig) {
		_log(_modName, "Failed to apply memory for commConfig", 1004, 5, 1);
	}
	GetPrivateProfileString("Product", "Pro_Name", "", commConfig->Pro_name, 50, g_szIniPath);
	GetPrivateProfileString("Product", "Pro_Ver", "", commConfig->Pro_ver, 50, g_szIniPath);
	GetPrivateProfileString("Product", "Pro_VerCode", "", commConfig->Pro_verCode, 50, g_szIniPath);
	commConfig->UI_RefRate = GetPrivateProfileInt("UI", "UI_RefRate", -1, g_szIniPath);

	commConfig->UI_Width = GetPrivateProfileInt("UI", "UI_Width", -1, g_szIniPath);
	commConfig->UI_Height = GetPrivateProfileInt("UI", "UI_Height", -1, g_szIniPath);
	commConfig->UI_wZoomRate = commConfig->UI_Width / (double)O_WIDTH;
	commConfig->UI_hZoomRate = commConfig->UI_Height / (double)O_HEIGHT;

	GetPrivateProfileString("UI", "UI_Lang", "zh-CN", commConfig->UI_Lang, 50, g_szIniPath);
	GetPrivateProfileString("UI", "UI_Font", "����", commConfig->UI_Font, 50, g_szIniPath);

	if (!strcmp(commConfig->Pro_name, "") || 
		!strcmp(commConfig->Pro_ver, "") || 
		!strcmp(commConfig->Pro_verCode, "")) {
		_log(_modName, "Config error in Product section", 1005, 5, 1);
	}
	if (-1 == commConfig->UI_RefRate || 
		-1 == commConfig->UI_Width || 
		-1 == commConfig->UI_Height) {
		_log(_modName, "Config error in UI section", 1006, 5, 1);
	}

	// ��ʼ����־��¼�ȼ�
	_initDebug(GetPrivateProfileInt("Product", "Pro_Debug", -1, g_szIniPath));

	// ��ʼ����������
	_initLangCfg();

	_log(_modName, "Finished init config");
}

/**************************************
 drawLoadingInfo ���Ƽ�����Ϣ����
 ������
	int isAllDone �Ƿ�ȫ���������
	char *szResName ���ڼ��ص���Դ����
	int now ��ǰ���ص�������Դ�ĵڼ���
	int cnt ��ǰ������Դ����
 ����ֵ����
 ��ע������ g_bgIMG[0] ������ɺ����
 **************************************/
void _drawLoadingInfo(int isAllDone, char *szResName = "", int now = 1, int cnt = 1) {
	static string _modName = "drawLoadingInfo";
	LOGFONT f;
	char szBuffer[255];

	BeginBatchDraw();
	cleardevice();
	_setFont(&f, 28);

	if (isAllDone) {
		sprintf_s(szBuffer, _L("ui_loaded"));
	}
	else {
		sprintf_s(szBuffer, _L("ui_loading"), _L(szResName), now, cnt);
	}

	RECT r = {
		(int)(_ax(880) + 0.5), 
		(int)(_ay(650) + 0.5), 
		(int)(_ax(1250) + 0.5), 
		(int)(_ay(680) + 0.5)
	};
	putimage(0, 0, &g_bgIMG[0]);
	setbkmode(TRANSPARENT);
	drawtext(szBuffer, &r, DT_LEFT | DT_SINGLELINE);
	EndBatchDraw();
}

/**************************************
 batchLoadImages ��������ĳ��ͼƬ
 ������
	IMAGE *IMG_Arr ����ͼƬ IMAGE ����ָ��
	string res_path ����ͼƬ��Դ·��ǰ׺
	char *res_name ����ͼƬ��Դ����
	int res_max ����ͼƬ��Դ�ռ�����
	int isNeedZoom �Ƿ���Ҫ���� Ĭ��Ϊ0��
 ����ֵ����
 **************************************/
void _batchLoadImg(IMAGE *IMG_Arr, string res_path, char *szResName, int res_max) {
	static string _modName = "batchLoadImg";

	int i, cnt;
	char szResCfgName[255], szResFilePath[MAX_PATH];
	char szLogTmp[255], szCfgValTmp[255];
	IMAGE imgTmp;

	// ��ȡ��Դ���������������ռ�ȽϷ�ֹ���
	strcpy_s(szResCfgName, "Res_");
	strcat_s(szResCfgName, szResName);
	cnt = GetPrivateProfileInt(szResCfgName, "Cnt", 0, g_szIniPath);
	if (cnt > res_max) {
		sprintf_s(szLogTmp, "Resource amount exceeded! res_cfgName = %s, res_max = %d, res_cnt = %d", szResCfgName, res_max, cnt);
		_log(_modName, szLogTmp, 2006, 5, 1);
	}

	// ��������
	res_path.append(szResName);
	res_path.append("\\");
	for (i = 0; i < cnt; i++) {
		sprintf_s(szCfgValTmp, "Path%d", i + 1);
		GetPrivateProfileString(szResCfgName, szCfgValTmp, "", szCfgValTmp, 255, g_szIniPath);
		if (!strcmp(szCfgValTmp, "")) {
			sprintf_s(szLogTmp, "Empty filename of %s #%d", szResCfgName, i + 1);
			_log(_modName, szLogTmp, 2007, 3);
		}
		else {
			strcpy_s(szResFilePath, res_path.c_str());
			strcat_s(szResFilePath, szCfgValTmp);
			if (_access(szResFilePath, 4) == -1) {
				sprintf_s(szLogTmp, "Load %s (#%d of %s) failed", szResFilePath, i + 1, szResCfgName);
				_log(_modName, szLogTmp, 2008, 3);
			}

			loadimage(&IMG_Arr[i], szResFilePath);
			if (fabs(commConfig->UI_wZoomRate - 1.0) > EPSINON || fabs(commConfig->UI_hZoomRate - 1.0) > EPSINON) {
				if (commConfig->UI_wZoomRate - 1.0 > EPSINON || commConfig->UI_hZoomRate - 1.0 > EPSINON) {
					loadimage(
						&imgTmp, 
						szResFilePath, 
						(int)(_ax(IMG_Arr[i].getwidth()) + 0.5), 
						(int)(_ay(IMG_Arr[i].getheight()) + 0.5), 
						true);
				}
				else {
					ZoomImage(&imgTmp, &IMG_Arr[i], commConfig->UI_wZoomRate, true, commConfig->UI_hZoomRate);
				}
				IMG_Arr[i] = imgTmp;
			}
			sprintf_s(szLogTmp, "Loaded image: %s, w = %d, h = %d", szResFilePath, IMG_Arr[i].getwidth(), IMG_Arr[i].getheight());
			_log(_modName, szLogTmp);
			_drawLoadingInfo(0, szResCfgName, i + 1, cnt);
		}
	}
}

/**************************************
 putimage2 ֧��͸������ɫ�Ļ�ͼ (putImgByGDI)
 ������
	int x ��ͼԭ�������
	int y ��ͼԭ��������
	int w ͼ����
	int h ͼ��߶�
	IMAGE img ͼ�����
	UINT bkcolor ����ɫ Ĭ��BLACK
 ����ֵ����
 ��ע������������ͼ������ʹ��
 **************************************/
void _putimage(int x, int y, int w, int h, IMAGE img, UINT bkcolor = BLACK)
{
	// ��ȡ��ͼ���ں� IMAGE ����ľ����TransparentBlt ������Ҫ��
	HDC dstDC = GetImageHDC();
	HDC srcDC = GetImageHDC(&img);
	// ʹ�� Windows GDI ����ʵ��͸��λͼ
	TransparentBlt(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bkcolor);
	// ʹ GDI ������Ч
	FlushBatchDraw();
}

/**************************************
 getCommand ��ȡ����״̬
 ��������
 ����ֵ��
	int ����״̬λ��������
 ��ע��������λ������ж�
 **************************************/
int _getCommand()
{
	int c = 0;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)		c |= CMD_LEFT;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)	c |= CMD_RIGHT;
	if (GetAsyncKeyState(VK_UP) & 0x8000)		c |= CMD_UP;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)		c |= CMD_DOWN;
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)	c |= CMD_QUIT;
	return c;
}

/**************************************
 initSceneConfig ��ʼ����������
 ������
	int sceneId ���� ID
	char *szSceneCfgPath ��������·��ָ��
 ����ֵ����
 **************************************/
void _initSceneConfig(int sceneId, char *szSceneCfgPath, char *mapData = NULL) {
	static string _modName = "initSceneConfig";
	char szLogTmp[500], szPathTmp[MAX_PATH];
	FILE *fp;
	int i = 0, j = 0;

	sprintf_s(szSceneCfgPath, MAX_PATH, "%s\\res\\cfg_scene\\scene%d.ini", g_szExePath, sceneId);
	if (_access(szSceneCfgPath, 4) == -1) {
		sprintf_s(szLogTmp, "Cannot access scene config file! Path = %s", szSceneCfgPath);
		_log(_modName, szLogTmp, 3001, 5, 1);
	}

	if (mapData) {
		sprintf_s(szPathTmp, "%s\\res\\map\\map%d.dat", g_szExePath, sceneId);

		fopen_s(&fp, szPathTmp, "r");
		if (!fp) {
			sprintf_s(szLogTmp, "Cannot read map data file! Path = %s", szPathTmp);
			_log(_modName, szLogTmp, 3002, 5, 1);
		}
		while ((mapData[i++] = fgetc(fp)) != EOF);
	}
}

// =================== ���������Ժ������� ===================

#endif