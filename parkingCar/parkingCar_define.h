/***************************************
 * parkingCar_define.h 
 * ����: ��Ϸ������ �����빦�ܺ궨��
 * ����: �׵籩��
 * ʱ��: 2018-01-02 17:45:00
 * ��ע: 
 ***************************************/

#pragma once
#ifndef parkingCar_define_H
#define parkingCar_define_H

#pragma comment(lib, "MSIMG32.LIB")		// 20171222: png����͸��

/***************************
******** �������忪ʼ ********
***************************/

// ------ ��ѧ������ ------
#define PI 3.14159265358979
#define ACC_SPEED 1.6		// ���ٶ�
#define DEC_SPEED 1.5		// ���ٶ�
#define MAX_SPEED 16		// ����ٶ�
#define EPSINON 0.000001
#define APS 90				// ÿ���ƶ��Ƕ�

// ------ ��Դ�ռ䳣�� ------
#define MAX_CARS 10			// ���������
#define MAX_BGS 15			// ��󱳾���
#define MAX_OBJECTS 15		// �����ͼ��
#define MAX_MAPS 15			// ����ͼ��

// ------ ��Ϸ���ó��� ------
#define O_WIDTH 1280		// ԭʼ������
#define O_HEIGHT 720		// ԭʼ����߶�
#define O_BKCOLOR BLACK		// ͼƬ������ɫ
#define CONFIG_FILE (TEXT("parkingCar.ini"))

// ------ ���Ƽ�ֵ���� ------
#define CMD_UP 1
#define CMD_DOWN 2
#define CMD_LEFT 4
#define CMD_RIGHT 8
#define CMD_QUIT 16

// ------ �����Ժ궨�� ------
#define _ax(x) ((int)(commConfig->UI_wZoomRate * x + 0.5))
#define _ay(y) ((int)(commConfig->UI_hZoomRate * y + 0.5))

/***************************
******** ����������� ********
***************************/


/***************************
***** �Զ����������Ϳ�ʼ ******
***************************/

// ------ ����״̬�ṹ�� ------
typedef struct Car {
	int orgResId;				// ����ԭʼ��ͼ��Դ ID
	int x, y;					// ��ͼԭ������
	int realX, realY;		// ���ĵ�����
	double angle;				// �Ƕ�
	double speed;				// �ٶ�
	int isqian, iszuo, isgas;	// ����������״̬
} Car;

// ------ ��Ϸ���ýṹ�� ------
typedef struct PC_Config {
	char Pro_name[50],			// ��Ʒ����
		Pro_ver[50],			// ��Ʒ�汾
		Pro_verCode[50];		// ��Ʒ�汾����
	double UI_wZoomRate,		// �������ű���
		UI_hZoomRate;			// �������ű���
	int UI_RefRate,				// ˢ���� (SleepInterval)
		UI_Width,				// ������
		UI_Height;				// ����߶�
	char UI_Lang[50],			// ���Դ��� zh-CN
		UI_LangDesc[50],		// ��������
		UI_Font[50];			// ��������
} PC_Config;

// ------ ������ť�ṹ�� ------
typedef struct Button {
	int resId;					// ��Դ ID (g_objIMG)
	char name[50];				// ���� (btn_*)
	int x, y;					// ��ͼԭ������
	int status;					// ״̬ (0-���� 1-����ƹ� 2-��갴�� 3-����)
	RECT r;						// ��������Χ (��ʾ������)
	char func[50];				// �󶨺�����
} Button;

// ------ ����ָ�붨�� ------
typedef void(*pFunc)();

/***************************
***** �Զ����������ͽ��� ******
***************************/

using namespace std;

#endif