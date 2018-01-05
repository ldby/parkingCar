/***************************************
 * parkingCar_mapTool.cpp
 * 名称: 地图转换程序
 * 作者: 雷电暴雨
 * 版本: 1.0.1.131
 * 时间: 2018-01-05 13:38:00
 * 备注: MileStone 3
 ***************************************/

#include <stdlib.h>
#include <easyx.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

int main(int argc, char *argv[]) {
	IMAGE img_mapRange;
	//char szImgPath[MAX_PATH] = "D:\\Projects\\parkingCar_alpha\\Debug\\map1_range.bmp";
	char szImgPath[MAX_PATH] = "";
	int srcW = 0, srcH = 0;
	int i = 0, j = 0;
	DWORD *pImgBuffer, *pXc;
	COLORREF pixelColor = 0;
	FILE *fp;

	if (argc <= 1 && strcmp(szImgPath, "") == 0) {
		printf("Error: Map range file not defined!\n");
	}
	else {
		if(strcmp(szImgPath, "") == 0) strcpy_s(szImgPath, argv[1]);
		if (_access(szImgPath, 4) == -1) {
			printf("Error: Cannot read file %s\n", szImgPath);
		}
		else {
			loadimage(&img_mapRange, szImgPath);
			srcW = img_mapRange.getwidth();
			srcH = img_mapRange.getheight();
			if (!srcW || !srcH) {
				printf("Error: load image %s failed\n", szImgPath);
			}
			else {
				pImgBuffer = GetImageBuffer(&img_mapRange);
				fopen_s(&fp, "result.txt", "w+");
				if (!fp) {
					printf("Error: Cannot open file result.txt");
				}
				else {
					initgraph(srcW, srcH);
					pXc = GetImageBuffer();
					for (i = 0; i < srcH; i++) {
						for (j = 0; j < srcW; j++) {
							pixelColor = pImgBuffer[srcW * i + j];
							//pixelColor = (0xff0000 - (pixelColor & 0xff0000)) | (0x00ff00 - (pixelColor & 0x00ff00)) | (0x0000ff - (pixelColor & 0x0000ff));
							if (pixelColor == 0x000000) {
								fprintf_s(fp, "0", pixelColor);
							}
							else if (pixelColor == 0xff0000) {
								fprintf_s(fp, "9");
							}
							else {
								fprintf_s(fp, "1");
							}
							pXc[i * srcW + j] = pImgBuffer[srcW * i + j];
						}
						//fprintf_s(fp, "\n");
					}
					fclose(fp);
					FlushBatchDraw();
					getchar();
					closegraph();
					printf("Done.");
				}
			}
		}
	}
	return 0;
}