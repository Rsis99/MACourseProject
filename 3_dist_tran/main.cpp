// 3_dist_tran.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include "transform.h"


int main()
{
	//BmpReverse("tst0.bmp", "1_reverse.bmp");

	//BmpOverlay("tst0.bmp", "tst1.bmp", "2_overlay.bmp");

	//DistTemp_octagon FocalT;
	//BmpFocal("tst1.bmp", &FocalT, "3_focal.bmp");

	//Bmp256to32b("tst0.bmp", "4_256to32b.bmp");

	//DistTemp_5 EDis5T;
	//DistTran256("tst1.bmp", &EDis5T, 0, "5_Dis51.bmp", "5_Loc51.bmp");
	
	//DistTemp_octagon EDis8T;
	//DistTran256("tst1.bmp", &EDis8T, 0, "6_Dis81.bmp", "6_Loc81.bmp");

	//DistTemp_13 EDis13T;
	//DistTran256("tst0.bmp", &EDis13T, 0, "7_Dis13.bmp", "7_Loc13.bmp");

	//GenerateBondaries("5_Loc51.bmp", "8_boundary1.bmp");

	//GenerateBuffer("5_Dis51.bmp", 15, "9_buffer1.bmp");
	//GenerateBuffer("Dis_5.bmp", 15, "bufbon_1.bmp");

	//AdhesionTransform("tst2.bmp", 15, "10_AdhesionTransform.bmp");
	//ErosionTransform("tst2.bmp", 15, "11_ErosionTransform.bmp");

	//AxisTransform("tst3.bmp", "12_AxisTransform.bmp");

	printf("Success!");
    return 0;
}

