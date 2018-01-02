/***************************************
 * parkingCar_zoomImage.h 
 * 名称: 图片缩放公共头文件
 * 作者: 雷电暴雨
 * 时间: 2018-01-02 16:20:00
 * 备注: 
 ***************************************/

#pragma once
#ifndef parkingCar_zoomImage_H
#define parkingCar_zoomImage_H

void ZoomImage(IMAGE* dstImg, IMAGE* srcImg, double ZoomRate, bool HighsrcImguality = false, double ZoomRate2 = 0)
{
	// 不填写第二缩放参数则默认和第一相等
	if (ZoomRate2 == 0) {
		ZoomRate2 = ZoomRate;
	}

	// 根据缩放比率设定目标图像大小
	dstImg->Resize((int)(srcImg->getwidth()*ZoomRate), (int)(srcImg->getheight()*ZoomRate2));

	// 分别对原图像和目标图像获取指针
	DWORD* M = GetImageBuffer(dstImg);
	DWORD* N = GetImageBuffer(srcImg);

	// 选择高质量则使用双线性插值算法
	if (HighsrcImguality)
	{
		for (int i = 0; i<dstImg->getheight(); i++)
		{
			for (int j = 0; j<dstImg->getwidth(); j++)
			{
				// 求出目标图像对应像素点在原图的浮点坐标并取整
				int X_ = (int)((j + 0.5) / ZoomRate - 0.5);
				int Y_ = (int)((i + 0.5) / ZoomRate2 - 0.5);

				// 根据取整坐标求A1(X,Y), A2(X+1,Y), A3(X,Y+1), A4(X+1,Y+1)即浮点坐标临近4个点的颜色平均值。
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
	else { // 选择低质量则按常规方法缩放
		for (int i = 0; i < dstImg->getheight(); i++)
			for (int j = 0; j < dstImg->getwidth(); j++)
				// 根据目标图像像素点位置逆推算原图像像素点赋值
				M[j + i*dstImg->getwidth()] = N[(int)(j / ZoomRate) + (int)(i / ZoomRate2) * srcImg->getwidth()];
	}
}

#endif