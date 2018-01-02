/***************************************
 * parkingCar_zoomImage.h 
 * ����: ͼƬ���Ź���ͷ�ļ�
 * ����: �׵籩��
 * ʱ��: 2018-01-02 16:20:00
 * ��ע: 
 ***************************************/

#pragma once
#ifndef parkingCar_zoomImage_H
#define parkingCar_zoomImage_H

void ZoomImage(IMAGE* dstImg, IMAGE* srcImg, double ZoomRate, bool HighsrcImguality = false, double ZoomRate2 = 0)
{
	// ����д�ڶ����Ų�����Ĭ�Ϻ͵�һ���
	if (ZoomRate2 == 0) {
		ZoomRate2 = ZoomRate;
	}

	// �������ű����趨Ŀ��ͼ���С
	dstImg->Resize((int)(srcImg->getwidth()*ZoomRate), (int)(srcImg->getheight()*ZoomRate2));

	// �ֱ��ԭͼ���Ŀ��ͼ���ȡָ��
	DWORD* M = GetImageBuffer(dstImg);
	DWORD* N = GetImageBuffer(srcImg);

	// ѡ���������ʹ��˫���Բ�ֵ�㷨
	if (HighsrcImguality)
	{
		for (int i = 0; i<dstImg->getheight(); i++)
		{
			for (int j = 0; j<dstImg->getwidth(); j++)
			{
				// ���Ŀ��ͼ���Ӧ���ص���ԭͼ�ĸ������겢ȡ��
				int X_ = (int)((j + 0.5) / ZoomRate - 0.5);
				int Y_ = (int)((i + 0.5) / ZoomRate2 - 0.5);

				// ����ȡ��������A1(X,Y), A2(X+1,Y), A3(X,Y+1), A4(X+1,Y+1)�����������ٽ�4�������ɫƽ��ֵ��
				M[j + i*dstImg->getwidth()] = RGB(
					(
						GetRValue(N[X_ + Y_*srcImg->getwidth()]) + 
						GetRValue(N[(X_ + 1) + Y_*srcImg->getwidth()]) + 
						GetRValue(N[X_ + (Y_ + 1)*srcImg->getwidth()]) + 
						GetRValue(N[(X_ + 1) + (Y_ + 1)*srcImg->getwidth()])
					) / 4,
					(
						GetGValue(N[X_ + Y_*srcImg->getwidth()]) + 
						GetGValue(N[(X_ + 1) + Y_*srcImg->getwidth()]) + 
						GetGValue(N[X_ + (Y_ + 1)*srcImg->getwidth()]) + 
						GetGValue(N[(X_ + 1) + (Y_ + 1)*srcImg->getwidth()])
					) / 4,
					(
						GetBValue(N[X_ + Y_*srcImg->getwidth()]) + 
						GetBValue(N[(X_ + 1) + Y_*srcImg->getwidth()]) + 
						GetBValue(N[X_ + (Y_ + 1)*srcImg->getwidth()]) + 
						GetBValue(N[(X_ + 1) + (Y_ + 1)*srcImg->getwidth()])
					) / 4
				);
			}
		}
	}
	else { // ѡ��������򰴳��淽������
		for (int i = 0; i < dstImg->getheight(); i++)
			for (int j = 0; j < dstImg->getwidth(); j++)
				// ����Ŀ��ͼ�����ص�λ��������ԭͼ�����ص㸳ֵ
				M[j + i*dstImg->getwidth()] = N[(int)(j / ZoomRate) + (int)(i / ZoomRate2) * srcImg->getwidth()];
	}
}

#endif