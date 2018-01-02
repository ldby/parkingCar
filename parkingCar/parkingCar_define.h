/***************************************
 * parkingCar_define.h 
 * 名称: 游戏主程序 常量与功能宏定义
 * 作者: 雷电暴雨
 * 时间: 2018-01-02 17:45:00
 * 备注: 
 ***************************************/

#pragma once
#ifndef parkingCar_define_H
#define parkingCar_define_H

#pragma comment(lib, "MSIMG32.LIB")		// 20171222: png背景透明

/***************************
******** 常量定义开始 ********
***************************/

// ------ 数学物理常量 ------
#define PI 3.14159265358979
#define ACC_SPEED 1.6		// 加速度
#define DEC_SPEED 1.5		// 减速度
#define MAX_SPEED 16		// 最大速度
#define EPSINON 0.000001
#define APS 90				// 每秒移动角度

// ------ 资源空间常量 ------
#define MAX_CARS 10			// 最大车种类数
#define MAX_BGS 15			// 最大背景数
#define MAX_OBJECTS 15		// 最大贴图数
#define MAX_MAPS 15			// 最大地图数

// ------ 游戏配置常量 ------
#define O_WIDTH 1280		// 原始画面宽度
#define O_HEIGHT 720		// 原始画面高度
#define O_BKCOLOR BLACK		// 图片公共底色
#define CONFIG_FILE (TEXT("parkingCar.ini"))

// ------ 控制键值常量 ------
#define CMD_UP 1
#define CMD_DOWN 2
#define CMD_LEFT 4
#define CMD_RIGHT 8
#define CMD_QUIT 16

// ------ 功能性宏定义 ------
#define _ax(x) ((int)(commConfig->UI_wZoomRate * x + 0.5))
#define _ay(y) ((int)(commConfig->UI_hZoomRate * y + 0.5))

/***************************
******** 常量定义结束 ********
***************************/


/***************************
***** 自定义数据类型开始 ******
***************************/

// ------ 车辆状态结构体 ------
typedef struct Car {
	int orgResId;				// 车体原始贴图资源 ID
	int x, y;					// 贴图原点坐标
	int realX, realY;		// 中心点坐标
	double angle;				// 角度
	double speed;				// 速度
	int isqian, iszuo, isgas;	// 辅助调试用状态
} Car;

// ------ 游戏设置结构体 ------
typedef struct PC_Config {
	char Pro_name[50],			// 产品名称
		Pro_ver[50],			// 产品版本
		Pro_verCode[50];		// 产品版本代号
	double UI_wZoomRate,		// 横向缩放比率
		UI_hZoomRate;			// 纵向缩放比率
	int UI_RefRate,				// 刷新率 (SleepInterval)
		UI_Width,				// 画面宽度
		UI_Height;				// 画面高度
	char UI_Lang[50],			// 语言代号 zh-CN
		UI_LangDesc[50],		// 语言名称
		UI_Font[50];			// 字体名称
} PC_Config;

// ------ 交互按钮结构体 ------
typedef struct Button {
	int resId;					// 资源 ID (g_objIMG)
	char name[50];				// 名称 (btn_*)
	int x, y;					// 贴图原点坐标
	int status;					// 状态 (0-正常 1-鼠标移过 2-鼠标按下 3-禁用)
	RECT r;						// 矩形区域范围 (显示文字用)
	char func[50];				// 绑定函数名
} Button;

// ------ 函数指针定义 ------
typedef void(*pFunc)();

/***************************
***** 自定义数据类型结束 ******
***************************/

using namespace std;

#endif