#include "stdafx.h"
#include "transform.h"
#include <stdio.h>
#include <Windows.h>


///反色处理
int BmpReverse(const char *InBmpName, const char *OutBmpName)
{
	//文件读取
	FILE *in_bmp;
	errno_t err = fopen_s(&in_bmp, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}

	BITMAPFILEHEADER bmp_head;
	BITMAPINFOHEADER bmp_info;
	unsigned clr_tab[256];

	fread(&bmp_head, sizeof(BITMAPFILEHEADER), 1, in_bmp);
	fread(&bmp_info, sizeof(BITMAPINFOHEADER), 1, in_bmp);
	fread(clr_tab, sizeof(unsigned), 256, in_bmp);

	int bmp_width = bmp_info.biWidth;
	int bmp_height = bmp_info.biHeight;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//分配内存空间
	unsigned char ** reverse_mtx = new unsigned char *[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		reverse_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			reverse_mtx[i][j] = 0xFF;
		}
	}

	//反色处理
	unsigned char * line_buf = new unsigned char[bmp_width];
	for (int i = 0; i < bmp_height; i++)
	{
		fread(line_buf, sizeof(unsigned char), bmp_width, in_bmp);
		for (int j = 0; j < mtx_width; j++)
		{
			//if (line_buf[j] == 0xFF) continue;
			reverse_mtx[i][j] = 255 - line_buf[j];
			
		}
	}
	fclose(in_bmp);

	//图像输出
	FILE *ReverseBmp;
	errno_t err_loc = fopen_s(&ReverseBmp, OutBmpName, "wb");
	fwrite(&bmp_head, sizeof(BITMAPFILEHEADER), 1, ReverseBmp);
	fwrite(&bmp_info, sizeof(BITMAPINFOHEADER), 1, ReverseBmp);
	fwrite(clr_tab, 1024, 1, ReverseBmp);

	for (unsigned i = 0; i < bmp_height; i++)
	{
		fwrite(reverse_mtx[i], sizeof(unsigned char), mtx_width, ReverseBmp);
	}
	fclose(ReverseBmp);

	//扫尾
	for (unsigned i = 0; i < bmp_height; i++)
	{
		delete[] reverse_mtx[i];
		reverse_mtx[i] = NULL;
		delete[] line_buf;
		line_buf = NULL;
	}

	delete[] reverse_mtx;
	return 0;

}

///叠置处理
int BmpOverlay(const char *InBmpName1, const char *InBmpName2, const char *OutBmpName)
{
	FILE *file1, *file2;
	errno_t err1 = fopen_s(&file1, InBmpName1, "rb");
	errno_t err2 = fopen_s(&file2, InBmpName2, "rb");
	if (err1 != 0 || err2 !=0) {
		printf_s("文件打开失败！");
	}
	//FILE *file1 = fopen(InBmpName1, "rb");
	//FILE *file2 = fopen(InBmpName2, "rb");

	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fread(ClrTab, 1024, 1, file1);

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file2);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file2);
	fread(ClrTab, 1024, 1, file2);

	int bmp_height = BmpInfoHeader.biHeight;
	int bmp_width = BmpInfoHeader.biWidth;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//分配内存空间
	unsigned char **overlay_mtx = new unsigned char*[bmp_height];
	for (int i = 0; i < bmp_height; i++)
	{
		overlay_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			overlay_mtx[i][j] = 0xFF;
		}
	}

	//叠置操作
	unsigned char *line_buf1 = new unsigned char[bmp_width];
	unsigned char *line_buf2 = new unsigned char[bmp_width];
	for (int i = 0; i < bmp_height; i++)
	{
		fread(line_buf1, sizeof(unsigned char), bmp_width, file1);
		fread(line_buf2, sizeof(unsigned char), bmp_width, file2);
		for (int j = 0; j < bmp_width; j++)
		{
			if (line_buf1[j] == 0xFF && line_buf2[j] == 0xFF) continue;
			overlay_mtx[i][j] = line_buf1[j] + line_buf2[j];
		}
	}
	fclose(file1);
	fclose(file2);

	//图像输出
	FILE *OverlayBmp;
	errno_t err_loc = fopen_s(&OverlayBmp, OutBmpName, "wb");
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, OverlayBmp);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, OverlayBmp);
	fwrite(ClrTab, 1024, 1, OverlayBmp);

	for (unsigned i = 0; i < bmp_height; i++)
	{
		fwrite(overlay_mtx[i], sizeof(unsigned char), mtx_width, OverlayBmp);
	}
	fclose(OverlayBmp);

	//扫尾
	for (int i = 0; i < bmp_height; i++)
	{
		delete[] overlay_mtx[i];
		overlay_mtx[i] = NULL;
		delete[] line_buf1;
		line_buf1 = NULL;
		delete[] line_buf2;
		line_buf2 = NULL;
	}
	delete[] overlay_mtx;

	return 0;

}

///局部运算
int BmpFocal(const char *InBmpName, DistTemp *FocalTemp, const char *OutBmpName)
{
	//文件读取
	FILE *file;
	errno_t err = fopen_s(&file, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}

	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread(ClrTab, 1024, 1, file);

	int bmp_height = BmpInfoHeader.biHeight;
	int bmp_width = BmpInfoHeader.biWidth;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//初始值分配
	unsigned char **bmp_mtx = new unsigned char *[bmp_height];
	unsigned char * line_buf = new unsigned char [bmp_width];
	for (int i = 0; i < bmp_height; i++)
	{
		bmp_mtx[i] = new unsigned char[mtx_width];
		fread(line_buf, sizeof(unsigned char), bmp_width, file);
		for (int j = 0; j < mtx_width; j++)
		{
			if (j < bmp_width) bmp_mtx[i][j] = line_buf[j];
			else bmp_mtx[i][j] = 0xFF;
		}
	}
	fclose(file);

	unsigned char **focal_mtx = new unsigned char*[bmp_height];
	for (int i = 0; i < bmp_height; i++)
	{
		focal_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			focal_mtx[i][j] = 0xFF;
		}
	}

	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < bmp_width; x++)
		{
			//if (bmp_mtx[y][x] == 0xFF) continue;
			unsigned char cell_value = 0;

			for (int k = 0; k < FocalTemp->GetSize(); k++)
			{
				int off_x = FocalTemp->GetOfX(k);
				int off_y = FocalTemp->GetOfY(k);
				int temp_power = FocalTemp->GetDist(k);

				int curr_x = x + off_x;
				int curr_y = y + off_y;
				if ((curr_x < 0) || (curr_x > bmp_width - 1)) continue;
				if ((curr_y < 0) || (curr_y > bmp_height - 1)) continue;
				unsigned char tmp = bmp_mtx[y][x];
				cell_value += tmp * temp_power;
			}

			focal_mtx[y][x] = cell_value / (FocalTemp->GetSize());//平均后求得新值
		}
	}

	//输出图像
	FILE *FocalBmp;
	errno_t err_loc = fopen_s(&FocalBmp, OutBmpName, "wb");
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, FocalBmp);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, FocalBmp);
	fwrite(ClrTab, 1024, 1, FocalBmp);

	for (unsigned i = 0; i < bmp_height; i++)
	{
		fwrite(focal_mtx[i], sizeof(unsigned char), mtx_width, FocalBmp);
	}
	fclose(FocalBmp);

	//扫尾
	for (int i = 0; i < bmp_height; i++)
	{
		delete[] bmp_mtx[i];
		bmp_mtx[i] = NULL;
		delete[] focal_mtx[i];
		focal_mtx[i] = NULL;
	}
	delete[] bmp_mtx;
	delete[] focal_mtx;
	delete[] line_buf;

	return 0;
}

///256色转32位图
int Bmp256to32b(const char *InBmpName, const char *OutBmpName)
{
	// 1. 读取front_BMP
	// 1.1 读文件头
	FILE *InBmp;
	errno_t err = fopen_s(&InBmp, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}
	//FILE *InBmp = fopen(InBmpName, "rb");

	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned ClrTab[256];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, InBmp);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, InBmp);
	fread(ClrTab, sizeof(unsigned), 256, InBmp);


	int BmpWidth = BmpInfoHeader.biWidth;
	int BmpHeight = BmpInfoHeader.biHeight;
	int MtxWidth = (BmpWidth + 3) / 4 * 4;

	// 1.2 创建矩阵数据结构
	unsigned char ** BmpMtx = new unsigned char *[BmpHeight];
	for (unsigned y = 0; y < BmpHeight; y++)
	{
		BmpMtx[y] = new unsigned char[MtxWidth];
	}

	unsigned **Bmp32Mtx = new unsigned *[BmpHeight];
	for (unsigned y = 0; y < BmpHeight; y++)
	{
		Bmp32Mtx[y] = new unsigned[BmpWidth];
	}

	// 1.3 读位图数据
	for (unsigned y = 0; y < BmpHeight; y++)
	{
		fread(BmpMtx[y], sizeof(unsigned char), MtxWidth, InBmp);
	}
	fclose(InBmp);


	// 2. 矩阵数据运算
	for (unsigned y = 0; y < BmpHeight; y++)
		for (unsigned x = 0; x < BmpWidth; x++)
		{
			Bmp32Mtx[y][x] = ClrTab[BmpMtx[y][x]];
		}

	// 3. 写入新的位图文件

	FILE *OutBmp;
	errno_t err_loc = fopen_s(&OutBmp, OutBmpName, "wb");
	BmpFileHeader.bfOffBits = 54;
	BmpFileHeader.bfSize = sizeof(unsigned)*(BmpHeight*BmpWidth) + 54;
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, OutBmp);
	BmpInfoHeader.biBitCount = 32;
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, OutBmp);

	for (unsigned y = 0; y < BmpHeight; y++)
	{
		fwrite(Bmp32Mtx[y], sizeof(unsigned), BmpWidth, OutBmp);
	}
	fclose(OutBmp);

	// 4. 扫尾
	for (unsigned y = 0; y < BmpHeight; y++)
	{
		delete[]BmpMtx[y];
		BmpMtx[y] = NULL;
		delete[]Bmp32Mtx[y];
		Bmp32Mtx[y] = NULL;
	}
	delete[]BmpMtx;
	BmpMtx = NULL;
	delete[]Bmp32Mtx;
	Bmp32Mtx = NULL;

	return 0;
}

///距离变换
int DistTran256(const char *InBmpName, DistTemp *CurrDisT, int mode, const char * DisBmpName, const char * LocBmpName)
{
	//读取矩阵数据，初始化距离矩阵和分配矩阵
	FILE *in_bmp;
	errno_t err = fopen_s(&in_bmp, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}

	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned ClrTab[256];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, in_bmp);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, in_bmp);
	fread(ClrTab, sizeof(unsigned), 256, in_bmp);

	int bmp_width = BmpInfoHeader.biWidth;
	int bmp_height = BmpInfoHeader.biHeight;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//分配两个矩阵的内存空间
	unsigned char ** loc_mtx = new unsigned char *[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		loc_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++) loc_mtx[i][j] = 0;
	}

	float ** dis_mtx = new float*[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		dis_mtx[i] = new float[mtx_width];
		for (int j = 0; j < mtx_width; j++) dis_mtx[i][j] = 0.0;
	}

	//分配场矩阵初始化
	for (unsigned i = 0; i < bmp_height; i++)
	{
		fread(loc_mtx[i], sizeof(unsigned char), mtx_width, in_bmp);

	}
	fclose(in_bmp);

	//距离场矩阵初始化
	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < mtx_width; x++)
		{
			if (mode == 0)
			{
				if (loc_mtx[y][x] == 0xFF)
					dis_mtx[y][x] = MAX_FLOAT;//元素不存在的栅格赋无穷大值
				else dis_mtx[y][x] = 0.0;//元素存在的栅格赋为0
			}
			else if (mode == 1)
			{
				if (loc_mtx[y][x] == 0xFF)
					dis_mtx[y][x] = 0.0;
				else dis_mtx[y][x] = MAX_FLOAT;
			}
			else if (mode == 2)
			{
				if (loc_mtx[y][x] == 0)
					dis_mtx[y][x] = MAX_FLOAT;
				else dis_mtx[y][x] = 0.0;
			}
		}
	}

	

//两次扫描计算距离矩阵和分配矩阵
	//在主函数中建立距离模板
	//第一次扫描（从左上到右下）
	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < mtx_width; x++)
		{
			//遍历所有矩阵位置（距离矩阵值为0则跳过）
			if (fabs(dis_mtx[y][x]) < MIN_FLOAT) continue;
			//对任一(x,y)位置，遍历每一股模板元素
			float min_temp_dis = MAX_FLOAT;
			for (int i = 0; i < CurrDisT->GetSize() / 2 + 1; i++)
			{
				//取出当前模板元素的参数
				int elements_off_x = CurrDisT->GetOfX(i);
				int elements_off_y = CurrDisT->GetOfY(i);
				float elements_dist = CurrDisT->GetDist(i);

				if (elements_dist >= MAX_FLOAT) continue;
				//计算当前模板元素在大矩阵中的位置（dist_mtx_X,dist_mtx_Y）
				int dist_mtx_X = x + elements_off_x;
				int dist_mtx_Y = y + elements_off_y;
				//位置合法性检验
				if ((dist_mtx_X < 0) || (dist_mtx_X > mtx_width - 1)) continue;
				if ((dist_mtx_Y < 0) || (dist_mtx_Y > bmp_height - 1)) continue;
				//计算距离矩阵对应距离值+模板元素的距离值，比较最小值
				if (dis_mtx[dist_mtx_Y][dist_mtx_X] + elements_dist < min_temp_dis)
				{
					min_temp_dis = dis_mtx[dist_mtx_Y][dist_mtx_X]+elements_dist;
					dis_mtx[y][x] = min_temp_dis;
					loc_mtx[y][x] = loc_mtx[dist_mtx_Y][dist_mtx_X];
				}
			}
		}
	}

	//第二次扫描（从右下到左上）
	for (int y = bmp_height - 1; y >= 0; y--)

	//for (int y = 0; y < bmp_height; y++)
	{
		//for (int x = mtx_width - 1; x >= 0; x--)
		for (int x = 0; x < mtx_width; x++)
		{
			if (fabs(dis_mtx[y][x] < MIN_FLOAT)) continue;
			int min_temp_dis = MAX_FLOAT;
			for (int i = CurrDisT->GetSize() - 1; i >= CurrDisT->GetSize() / 2 + 1; i--)
			{
				int elements_off_x = CurrDisT->GetOfX(i);
				int elements_off_y = CurrDisT->GetOfY(i);
				int elements_dist = CurrDisT->GetDist(i);

				if (elements_dist >= MAX_FLOAT) continue;

				int dist_mtx_X = x + elements_off_x;
				int dist_mtx_Y = y + elements_off_y;

				if ((dist_mtx_X < 0) || (dist_mtx_X > mtx_width - 1)) continue;
				if ((dist_mtx_Y < 0) || (dist_mtx_Y > bmp_height - 1)) continue;

				if (dis_mtx[dist_mtx_Y][dist_mtx_X] + elements_dist < min_temp_dis)
				{
					min_temp_dis = dis_mtx[dist_mtx_Y][dist_mtx_X] + elements_dist;
					dis_mtx[y][x] = min_temp_dis;
					loc_mtx[y][x] = loc_mtx[dist_mtx_Y][dist_mtx_X];
				}
			}
		}
	}
	

//保存数据，释放空间
	//保存分配域bmp
	FILE *LocBmp;
	errno_t err_loc = fopen_s(&LocBmp, LocBmpName, "wb");
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, LocBmp);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, LocBmp);
	fwrite(ClrTab, 1024, 1, LocBmp);

	for (unsigned y = 0; y < bmp_height; y++)
	{
		fwrite(loc_mtx[y], sizeof(unsigned char), mtx_width, LocBmp);
	}
	fclose(LocBmp);

//保存距离场bmp
	FILE *DisBmp;
	errno_t err_dis = fopen_s(&DisBmp, DisBmpName, "wb");
	BmpFileHeader.bfOffBits = 54;
	BmpFileHeader.bfSize = sizeof(float)*(bmp_height*mtx_width) + 54;
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, DisBmp);
	BmpInfoHeader.biBitCount = 32;
	BmpInfoHeader.biSize = 40;
	BmpInfoHeader.biWidth = mtx_width;
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, DisBmp);

	for (int y = 0; y < bmp_height; y++)
	{
		fwrite(dis_mtx[y], sizeof(float), mtx_width, DisBmp);
	}
	fclose(DisBmp);



//扫尾
	for (unsigned y = 0; y < bmp_height; y++)
	{
		delete[] dis_mtx[y];
		dis_mtx[y] = NULL;
		delete[] loc_mtx[y];
		loc_mtx[y] = NULL;
	}

	delete[] dis_mtx;
	dis_mtx = NULL;
	delete[] loc_mtx;
	loc_mtx = NULL;

	return 0;
}

///边界生成
bool GenerateBondaries(const char *InBmpName, const char *OutBmpName)
{
	//读取矩阵数据，初始化距离矩阵和分配矩阵
	FILE *in_bmp;
	errno_t err = fopen_s(&in_bmp, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}

	BITMAPFILEHEADER bmp_head;
	BITMAPINFOHEADER bmp_info;
	unsigned clr_tab[256];

	fread(&bmp_head, sizeof(BITMAPFILEHEADER), 1, in_bmp);
	fread(&bmp_info, sizeof(BITMAPINFOHEADER), 1, in_bmp);
	fread(clr_tab, sizeof(unsigned), 256, in_bmp);

	int bmp_width = bmp_info.biWidth;
	int bmp_height = bmp_info.biHeight;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//分配内存空间
	unsigned char ** loc_mtx = new unsigned char *[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		loc_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++) loc_mtx[i][j] = 0xFF;
	}


	unsigned char ** bon_mtx = new unsigned char *[bmp_height];
	for (int i = 0; i < bmp_height; i++)
	{
		bon_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			bon_mtx[i][j] = 0xff;
		}
	}

	//读取分配场
	for (unsigned i = 0; i < bmp_height; i++)
	{
		fread(loc_mtx[i], sizeof(unsigned char), mtx_width, in_bmp);
	}
	fclose(in_bmp);

	//根据分配场生成边界
	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < mtx_width; x++)
		{
			if ((x == 0) || (y == 0) || (x == mtx_width - 1) || (y == bmp_height - 1))
			{
				bon_mtx[y][x] = 0;
				continue;
			}
			//if ((loc_mtx[y - 1][x] != loc_mtx[y + 1][x]) || (loc_mtx[y][x - 1] != loc_mtx[y][x + 1]))
			if ((loc_mtx[y][x] != loc_mtx[y + 1][x]) || (loc_mtx[y][x] != loc_mtx[y][x + 1]) || (loc_mtx[y][x] != loc_mtx[y - 1][x]) || (loc_mtx[y][x] != loc_mtx[y][x - 1]))
			{
				bon_mtx[y][x] = 0;
			}
			else bon_mtx[y][x] = 0xFF;
		}
	}
	//保存数据，释放空间
	//保存边界
	FILE *BonBmp;
	errno_t err_loc = fopen_s(&BonBmp, OutBmpName, "wb");
	fwrite(&bmp_head, sizeof(BITMAPFILEHEADER), 1, BonBmp);
	fwrite(&bmp_info, sizeof(BITMAPINFOHEADER), 1, BonBmp);
	fwrite(clr_tab, 1024, 1, BonBmp);

	for (unsigned y = 0; y < bmp_height; y++)
	{
		fwrite(bon_mtx[y], sizeof(unsigned char), mtx_width, BonBmp);
	}
	fclose(BonBmp);

	//扫尾
	for (unsigned y = 0; y < bmp_height; y++)
	{
		delete[] loc_mtx[y];
		loc_mtx[y] = NULL;
		delete[] bon_mtx[y];
		bon_mtx[y] = NULL;
	}

	delete[] loc_mtx;
	loc_mtx = NULL;
	delete[] bon_mtx;
	bon_mtx = NULL;

	return true;
}

///缓冲区生成
bool GenerateBuffer(const char * InBmpName, int buffer_width, const char *OutBmpName)
{
	//读取矩阵数据，初始化距离矩阵和分配矩阵
	FILE *in_bmp;
	errno_t err = fopen_s(&in_bmp, InBmpName, "rb");
	if (err != 0) {
		printf_s("文件打开失败！");
	}

	BITMAPFILEHEADER bmp_head;
	BITMAPINFOHEADER bmp_info;
	//unsigned clr_tab[256];

	fread(&bmp_head, sizeof(BITMAPFILEHEADER), 1, in_bmp);
	fread(&bmp_info, sizeof(BITMAPINFOHEADER), 1, in_bmp);
	//fread(clr_tab, sizeof(unsigned), 256, in_bmp);

	int bmp_width = bmp_info.biWidth;
	int bmp_height = bmp_info.biHeight;
	int mtx_width = (bmp_width + 3) / 4 * 4;

	//分配内存空间
	/*
	unsigned char ** loc_mtx = new unsigned char *[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		loc_mtx[i] = new unsigned char[mtx_width];
	}
	*/

	float ** dis_mtx = new float*[bmp_height];
	for (unsigned i = 0; i < bmp_height; i++)
	{
		dis_mtx[i] = new float[mtx_width];
	}

	unsigned char ** buf_mtx = new unsigned char *[bmp_height];
	for (int i = 0; i < bmp_height; i++)
	{
		buf_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			buf_mtx[i][j] = 0xff;
		}
	}

	unsigned char ** bufbon_mtx = new unsigned char *[bmp_height];
	for (int i = 0; i < bmp_height; i++)
	{
		bufbon_mtx[i] = new unsigned char[mtx_width];
		for (int j = 0; j < mtx_width; j++)
		{
			bufbon_mtx[i][j] = 0xff;
		}
	}

	//距离场矩阵初始化
	for (unsigned y = 0; y < bmp_height; y++)
	{
		fread(dis_mtx[y], sizeof(float), mtx_width, in_bmp);
	}
	fclose(in_bmp);


	//生成缓冲区
	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < mtx_width; x++)
		{
			if (dis_mtx[y][x] < buffer_width) buf_mtx[y][x] = 0;
			else buf_mtx[y][x] = 0xFF;
		}
	}
	
	
	//生成缓冲区边界
	for (int y = 0; y < bmp_height; y++)
	{
		for (int x = 0; x < mtx_width; x++)
		{
			if ((x == 0) || (y == 0) || (x == mtx_width - 1) || (y == bmp_height - 1))
			{
				bufbon_mtx[y][x] = 0;
				continue;
			}
			//if ((loc_mtx[y - 1][x] != loc_mtx[y + 1][x]) || (loc_mtx[y][x - 1] != loc_mtx[y][x + 1]))
			if ((buf_mtx[y][x] != buf_mtx[y + 1][x]) || (buf_mtx[y][x] != buf_mtx[y][x + 1]) || (buf_mtx[y][x] != buf_mtx[y - 1][x]) || (buf_mtx[y][x] != buf_mtx[y][x - 1]))
			{
				bufbon_mtx[y][x] = 0;
			}
			else bufbon_mtx[y][x] = 0xFF;
		}
	}
	

	//保存数据，释放空间
	FILE *file2;
	errno_t err_2 = fopen_s(&file2, "tst0.bmp", "rb");
	if (err_2 != 0) {
		printf_s("文件打开失败！");
	}
	BITMAPFILEHEADER IdxFileHeader;
	BITMAPINFOHEADER IdxInfoHeader;
	unsigned char ClrTab[256 * 4];// color table : r + b + g + alpha
	fread(&IdxFileHeader, sizeof(BITMAPFILEHEADER), 1, file2);
	fread(&IdxInfoHeader, sizeof(BITMAPINFOHEADER), 1, file2);
	fread(ClrTab, 1024, 1, file2);

	IdxFileHeader.bfSize = sizeof(unsigned char)*(bmp_height*mtx_width) + 54;
	//IdxInfoHeader.biWidth = mtx_width;
	//IdxInfoHeader.biHeight = bmp_height;


	//保存BufferB
	FILE *BufBmp;
	errno_t err_loc = fopen_s(&BufBmp, OutBmpName, "wb");
	fwrite(&IdxFileHeader, sizeof(BITMAPFILEHEADER), 1, BufBmp);
	fwrite(&IdxInfoHeader, sizeof(BITMAPINFOHEADER), 1, BufBmp);
	fwrite(ClrTab, 1024, 1, BufBmp);

	for (unsigned y = 0; y < bmp_height; y++)
	{
		fwrite(buf_mtx[y], sizeof(unsigned char), mtx_width, BufBmp);
	}
	fclose(BufBmp);

	/*
	//保存BufferBon
	FILE *BufBonBmp;
	errno_t err_loc = fopen_s(&BufBonBmp, OutBmpName, "wb");
	fwrite(&IdxFileHeader, sizeof(BITMAPFILEHEADER), 1, BufBonBmp);
	fwrite(&IdxInfoHeader, sizeof(BITMAPINFOHEADER), 1, BufBonBmp);
	fwrite(ClrTab, 1024, 1, BufBonBmp);

	for (unsigned y = 0; y < bmp_height; y++)
	{
		fwrite(bufbon_mtx[y], sizeof(unsigned char), mtx_width, BufBonBmp);
	}
	fclose(BufBonBmp);
	*/
	
	//扫尾
	for (unsigned y = 0; y < bmp_height; y++)
	{
		delete[] dis_mtx[y];
		dis_mtx[y] = NULL;
		delete[] buf_mtx[y];
		buf_mtx[y] = NULL;
	}

	delete[] dis_mtx;
	dis_mtx = NULL;
	delete[] buf_mtx;
	buf_mtx = NULL;
	return true;
}

///粘连变换
bool AdhesionTransform(const char * InBmpName, int radius, const char *OutBmpName)
{
	//粘连变换：先生成外距填补凹陷，再生成内距，保凸减凹

	//外距
	DistTemp_5 *pDisT = new DistTemp_5;
	DistTran256(InBmpName, pDisT, 0, "10_adDis1.bmp", "10_adLoc1.bmp");
	GenerateBuffer("10_adDis1.bmp", radius, "10_adBufferd1.bmp");

	//内距
	DistTran256("10_adBufferd1.bmp", pDisT, 1, "10_adDis2.bmp", "10_adLoc2.bmp");
	GenerateBuffer("10_adDis2.bmp", radius, "10_adBufferd2.bmp");
	BmpReverse("10_adBufferd2.bmp", OutBmpName);
	delete pDisT;

	return true;
}


///蚀断变换
bool ErosionTransform(const char *InBmpName, int radius, const char *OutBmpName)
{
	//蚀断变换：先生成内距，再生成内距，保凹减凸

	//内距
	DistTemp_5 *pDisT = new DistTemp_5;
	DistTran256(InBmpName, pDisT, 1, "11_eroDis1.bmp", "11_eroLoc1.bmp");
	GenerateBuffer("11_eroDis1.bmp", radius, "11_eroBufferd1.bmp");
	BmpReverse("11_eroBufferd1.bmp", "11_eroBufferd2.bmp");
	//外距
	DistTran256("11_eroBufferd2.bmp", pDisT, 0, "11_eroDis2.bmp", "11_eroLoc2.bmp");
	GenerateBuffer("11_eroDis2.bmp", radius, OutBmpName);
	
	delete pDisT;
	return true;
}

///形态中轴
bool AxisTransform(const char *InBmpName, const char *OutBmpName)
{
	DistTemp_5 *pDisT = new DistTemp_5;
	DistTran256(InBmpName, pDisT, 2, "12_axisDis.bmp", "12_axisLoc.bmp");
	GenerateBondaries("12_axisLoc.bmp", OutBmpName);

	delete pDisT;
	return true;
}
