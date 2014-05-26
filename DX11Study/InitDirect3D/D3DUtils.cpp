#include "stdafx.h"
#include "D3DUtils.h"
#include <cstdio>
#include <cstdarg>

float RGB256(UINT c)
{
	return 1.0f / 256.0f * c;
}

void Log(const char* foramt, ...)
{
	va_list args;			//����һ��va_list���͵ı������������浥������  
	va_start(args, foramt);	//ʹargsָ��ɱ�����ĵ�һ������  
	vprintf(foramt, args);  //������vprintf�ȴ�V��  
	va_end(args);			//�����ɱ�����Ļ�ȡ  
}

void DisplayVector(float x, float y, float z)
{
	Log("(%f, %f, %f)\n", x, y, z);
}