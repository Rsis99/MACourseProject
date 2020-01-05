#pragma once

#include <math.h>
#include "stdafx.h"

#define MAX_FLOAT 999999.99
#define MIN_FLOAT 0.0000001

//����ģ�����DistTemp
class DistTemp
{
public:
	virtual int GetSize() = 0;//��ȡģ��Ԫ�ظ���
	virtual float GetDist(int i) = 0;//��ȡ��ǰԪ�ص�center�ľ���
	virtual int GetOfX(int i) = 0;//��ȡ��ǰԪ�ع���center�����x����
	virtual int GetOfY(int i) = 0;//��ȡ��ǰԪ�ع���center�����y����
};

//5��5ģ��
class DistTemp_5 : public DistTemp
{
private:
	float RelDistTemp_5[25];

public:
	int GetSize() { return 25; }
	float GetDist(int i) { return RelDistTemp_5[i]; }
	int GetOfX(int i) { return i % 5 - 2; }
	int GetOfY(int i) { return i / 5 - 2; }

	DistTemp_5()
	{
		for (int i = 0; i < 25; i++)
		{
			int x = GetOfX(i);
			int y = GetOfY(i);
			RelDistTemp_5[i] = sqrt(long double(x * x + y * y));
		}
	}
};

//�˱���ģ��
class DistTemp_octagon: public DistTemp
{
private:
	float RelDistTemp_octagon[49];

public:
	int GetSize() { return 49; }
	float GetDist(int i) { return RelDistTemp_octagon[i]; }
	int GetOfX(int i) { return i % 7 - 3; }
	int GetOfY(int i) { return i / 7 - 3; }

	DistTemp_octagon()
	{
		for (int i = 0; i < 49; i++)
		{
			int x = GetOfX(i);
			int y = GetOfY(i);
			long double temp_dis = sqrt(long double(x * x + y * y));
			if (temp_dis = 0) {
				RelDistTemp_octagon[i] = 0;
			}
			else if (temp_dis <= 1)
			{
				RelDistTemp_octagon[i] = 1;
			}
			else if (temp_dis < 2.7) {
				RelDistTemp_octagon[i] = 2;
			}
			else if (temp_dis < 3.5) {
				RelDistTemp_octagon[i] = 3;
			}
			else RelDistTemp_octagon[i] = MAX_FLOAT;
			//RelDistTemp_octagon[i] = sqrt(long double(x * x + y * y));
		}
	}
};

//13��13ŷʽģ��
class DistTemp_13 : public DistTemp
{
private:
	float RelDistTemp_13[169];

public:
	int GetSize() { return 169; }
	float GetDist(int i) { return RelDistTemp_13[i]; }
	int GetOfX(int i) { return i % 13 - 6; }
	int GetOfY(int i) { return i / 13 - 6; }

	DistTemp_13()
	{
		for (int i = 0; i < 169; i++)
		{
			int x = GetOfX(i);
			int y = GetOfY(i);
			RelDistTemp_13[i] = sqrt(long double(x * x + y * y));
		}
	}
};

///��һ��ʵϰ
//��ɫ
int BmpReverse(const char *InBmpName, const char *OutBmpName);
//����
int BmpOverlay(const char *InBmpName1, const char *InBmpName2, const char *OutBmpName);
//�ֲ�����
int BmpFocal(const char *InBmpName, DistTemp *FocalTemp, const char *OutBmpName);
//��ʽת��
int Bmp256to32b(const char *InBmpName, const char *OutBmpName);

///�ڶ���ʵϰ
//����任
int DistTran256(const char *InBmpName, DistTemp *CurrDisT, int mode, const char * DisBmpName, const char * LocBmpName);

///������ʵϰ
//�߽磨Vͼ������
bool GenerateBondaries(const char *InBmpName, const char *OutBmpName);
//����������
bool GenerateBuffer(const char * InBmpName, int buffer_width, const char *OutBmpName);
//ճ���任
bool AdhesionTransform(const char * InBmpName, int radius, const char *OutBmpName);
//ʴ�ϱ任
bool ErosionTransform(const char *InBmpName, int radius, const char *OutBmpName);
//��̬����
bool AxisTransform(const char *InBmpName, const char *OutBmpName);