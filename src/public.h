/*
 *file name : public.h
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 *       2013-10-29   liwq      add function:getsubstr,getstrnum,strreplace
 *       2013-11-04   liwq      add function:strrstr
 */

/**********************************************************/
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>

#define STRLEN 1024
/*********
*�ػ����̷�ʽ
*
***************************************/
void daemonInit(void);

/**********
*�ж��ַ����Ƿ�Ϊ����
*
***************************************/
int isNumber(char * sNumber);

/*********
*�޳��ַ�����ߵĿո��Ʊ����
*
***************************************/
char* ltrim(char *s);

/*********
*�޳��ַ����ұߵĿո��Ʊ����
***************************************/
char* rtrim(char *s);

/*********
*�޳��ַ������ҵĿո��Ʊ����
***************************************/
char* trim(char *s);

/*********
*ȡϵͳʱ�䣬��ʽ��YYYYMMDDHH24MISS
***************************************/
char* getSysDate(char * sysDate);

/*********
*�Ƿ�Ϊ�Ϸ�����YYYYMMDD
***************************************/
int isRightfulDate(char * sDate);

//�ж��Ƿ�Ϊ�Ϸ�ʱ��hh24miss
int isTime(char * sTime);

//�ж��Ƿ�Ϊ�Ϸ����ڴ�yyyymmddhh24miss
int isDatetime(const char * sDatetime);

//14λ�ַ���ʱ���1970��1��1��00ʱ00��00��������������
long time2sec(const char *src);

//14λ�ַ���������ֵ
long diffsec(const char *src1, const char *src2);

//��ĸת��Ϊ��д
char* strupr(char * lwrstr);

//��ĸת��ΪСд
char* strlwr(char * lwrstr);

//�򵥲��ԳƼ��ܽ���
char* cryptStr(char * oStr,char * sStr);

//�����ļ�ȫ·����·�������ļ���
int getExePathAndName( const char *in_ptrArgv0,char *out_exename, char *out_exepath);

//����Ӵ���sp�Ƿָ��ӷ���n ��ʾ��n���Ӵ�(��1��ʼ����)��substr��Ż�õ��Ӵ�
void getsubstr(char *str, char sp, int n, char *substr);

//����Ŀ���ַ������Ӵ��ĸ���
int getstrnum(char *dest ,char *searchstr);

//�滻Ŀ���ַ����е��Ӵ�
int strreplace(char *dest,char *searchstr,char *replacestr);

//����Ŀ���ַ��������һ���Ӵ���ָ��
char *strrstr(const char *str1,const char *str2);

//��ȡ�ַ����������Ӵ�
int substr(char *pDes, const char *pSrc, int begin, int len);
#endif /*__PUBLIC_H__*/
