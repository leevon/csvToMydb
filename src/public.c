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
#include "public.h"

/*********
*守护进程方式
*
***************************************/
void daemonInit(void)
{
  int i;
  if(fork() > 0)  exit(0);
  sleep(1);
  setsid();
  signal(SIGHUP,SIG_IGN);
  if(fork() > 0)  exit(0);
  sleep(1);
  for(i = 0; i < 3; i++)
  {
    close(i);
  }
}

/*********
*判断字符串是否都为数字
*
***************************************/
int isNumber(char * sNumber)
{
  if (!sNumber) return 0;
  int i;
  for(i=0; i<strlen(sNumber); ++i)
  {
    if(0 == isdigit(sNumber[i]))
    {
      return 0;
    }
  }
  return 1;
}

/*********
*剔除字符串左边的空格制表符等
*
***************************************/
char* ltrim(char *s)
{
  if (!s) return NULL;

  char *ptmp = s;
  int tmpLen = strlen(s);
  while(isspace(*ptmp))
  {
    ++ptmp;
  }
  if (tmpLen > strlen(ptmp))
  {
    memcpy(s, ptmp, strlen(ptmp)+1);
  }
  return s;
}

/*********
*剔除字符串右边的空格制表符等
*
***************************************/
char* rtrim(char *s)
{
  if (!s) return NULL;

  char *ptmp = s;
  int len = strlen(s);
  while (len)
  {
    if (isspace(*(ptmp+len-1)))
    {
      *(ptmp+len-1) = '\0';
      --len;
    }
    else 
    {
      break;
    }
  }
  return s;
}

/*********
*剔除字符串左右的空格制表符等
*
**************************************/
char* trim(char *s)
{
  if (!s) return NULL;

  char *ptmp = s;
  int len = strlen(s);
  int tmpLen = len;
  while (len)
  {
    if (isspace(*(ptmp+len-1)))
    {
      *(ptmp+len-1) = '\0';
      --len;
    }
    else 
    {
      break;
    }
  }
  while(isspace(*ptmp))
  {
    ++ptmp;
  }
  if (tmpLen > strlen(ptmp))
  {
    memcpy(s, ptmp, strlen(ptmp)+1);
  }
  return s;
}

/*********
*取系统时间，格式：YYYYMMDDHH24MISS
*
*
***************************************/
char* getSysDate(char * sysDate)
{
  if (!sysDate) return NULL;

  time_t sec;
  struct tm *t;

  sec = time(NULL);
  t = localtime(&sec);

  sprintf(sysDate,"%04d%02d%02d%02d%02d%02d", \
                  t->tm_year+1900,t->tm_mon+1,\
                  t->tm_mday, t->tm_hour,t->tm_min,t->tm_sec);
  sysDate[14]='\0';
  return sysDate;
}
/*********
*是否为合法日期YYYYMMDD
*
***************************************/
int isRightfulDate(char * sDate)
{
  if (!sDate || strlen(sDate)!=8) return 0;

  int iYear,iMonth,iDay;
  char sBuffer[5];

  if (!isNumber(sDate)) return 0;

  memcpy(sBuffer,sDate,4);
  sBuffer[4] = 0;
  iYear = atoi(sBuffer);
  memcpy(sBuffer,sDate+4,2);
  sBuffer[2] = 0;
  iMonth = atoi(sBuffer);
  memcpy(sBuffer,sDate+6,2);
  sBuffer[2] = 0;
  iDay = atoi(sBuffer);
  if (iYear <= 1990)
  {
    return 0;
  }
  if (iMonth>12 || iDay>31 || iMonth==0 || iDay==0)
  {
    return 0;
  }
  if ((iMonth==4 || iMonth==6 || iMonth==9 || iMonth==11) && iDay>30)
  {
    return 0;
  }
  if (iMonth==2)
  {
    if ((iYear%100!=0 && iYear%4==0) || (iYear%400==0 && iYear%4==0))
    {
      if (iDay > 29) return 0;
    }
    else
    {
      if (iDay > 28) return 0;
    }
  }
  return 1;
}

/*********
*判断是否为合法时间hh24miss
*
***************************************/
int isTime(char * sTime)
{
  if (!sTime || strlen(sTime)!=6) return 0;

  char sBuffer[5];
  int iValue;

  if (!isNumber(sTime))  return 0;

  memcpy(sBuffer,sTime,2);
  sBuffer[2] = 0;
  iValue = atoi(sBuffer);
  if (!(iValue>=0 && iValue <=23)) return 0;

  memcpy(sBuffer,&sTime[2],2);
  sBuffer[2] = 0;
  iValue = atoi(sBuffer);
  if (!(iValue>=0 && iValue <=59)) return 0;

  memcpy(sBuffer,&sTime[4],2);
  sBuffer[2] = 0;
  iValue = atoi(sBuffer);
  if (!(iValue>=0 && iValue <=59)) return 0;

  return 1;
}

/*********
*判断是否为合法日期串yyyymmddhh24miss
*
***************************************/
int isDatetime(const char * sDatetime)
{
  if (!sDatetime || strlen(sDatetime)!=14) return 0;

  char sDate[8+1];
  char sTime[6+1];

  if( strlen(sDatetime) != 14 ) return 0;

  memcpy(sDate,sDatetime,8);
  sDate[8] = 0;
  if (!isRightfulDate(sDate))  return 0;

  memcpy(sTime,&sDatetime[8],6);
  sTime[6] = 0;
  if (!isTime(sTime)) return 0;

  return 1;
}

/*********
*14位字符串时间从1970年1月1日00时00分00秒所经过的秒数
*
***************************************/
long time2sec(const char *src)
{
  if (!src || !isDatetime(src)) return 0L;

  struct tm tmpTm;
  memset(&tmpTm,0x00,sizeof(struct tm));

  sscanf(src,"%4d%2d%2d%2d%2d%2d",&tmpTm.tm_year, \
             &tmpTm.tm_mon, &tmpTm.tm_mday,       \
             &tmpTm.tm_hour, &tmpTm.tm_min, &tmpTm.tm_sec);
  tmpTm.tm_year -= 1900;
  tmpTm.tm_mon -= 1;

  return mktime(&tmpTm);
}

/*********
*14位字符串秒数差值
*
**************************************/
long diffsec(const char *src1, const char *src2)
{
  return time2sec(src1) - time2sec(src2);
}

/*********
*字母转换为大写
*
***************************************/
char* strupr(char * lwrstr)
{
  if (!lwrstr) return NULL;

  char * pcStr;
  pcStr = lwrstr ;
  while (*pcStr)
  {
    if (*pcStr>=97 && *pcStr<=122)
    {
      *pcStr = *pcStr - 32;
    }

    ++pcStr;
  }
  return lwrstr;
}

/*********
*字母转换为小写
*
***************************************/
char* strlwr(char * lwrstr)
{
  if (!lwrstr) return NULL;

  char * pcStr;
  pcStr = lwrstr ;
  while (*pcStr)
  {
    if ( *pcStr>=65 && *pcStr<=90 )
    {
      *pcStr = *pcStr + 32;
    }
    pcStr++;
  }
  return lwrstr;
}

/*********
*简单不对称加密解密
*
***************************************/
char* cryptStr(char * oStr,char * sStr)
{
  #define _keyLen_  5
  char key[_keyLen_+1]="\001\002\003\004\005";
  int p = (int)_keyLen_;
  int k;
  for (k=0; k<strlen(sStr); ++k)
  {
   oStr[k]=sStr[k]^key[k%p--];
   if (!p) p = (int)_keyLen_;
  }
  oStr[strlen(sStr)] = '\0';
  return oStr;
}

/*********
*分离文件全路径的路径名和文件名
*
***************************************/
int getExePathAndName( const char *in_ptrArgv0,char *out_exename, char *out_exepath)
{
  const char *ptrChar;

  if( in_ptrArgv0 == NULL )
  {
    printf( "SearchExeFileNameAndPath_<Input_Parameter_Error\n" );
    return -1;
  }

  ptrChar = strrchr( in_ptrArgv0, '/' );

  if( ptrChar != NULL )
  {
    strncpy(out_exename, ptrChar+1, strlen(ptrChar+1));
    strncpy(out_exepath, in_ptrArgv0, strlen(in_ptrArgv0)-strlen(ptrChar)+1);
  }
  else
  {
    strncpy( out_exename, in_ptrArgv0, strlen( in_ptrArgv0 ) );
    strncpy( out_exepath, "./", 2);
  }

  return 0;
}

/*********
*获得串中第n个ch的指针
***************************************/
static void pstrchr( char * str, char ch, int n, char * p )
{
  char * tmp =  NULL;
  int i = 0;
  int k = 0;
  if(!str) return;
  tmp = strchr(str, ch);
  if( !tmp )	return;
  i++;
  for(k=i; k<n; k++)
  {
    tmp = strchr(tmp+1, ch);
    if(!tmp)
    {
      return;
    }
  }
  strcpy(p, tmp);
}

/*********
*获得子串，sp是分隔子符，n 表示第n个子串(从1开始计数)，substr存放获得的子串
***************************************/
void getsubstr(char *str, char sp, int n, char *substr)
{
  char p0[STRLEN];
  char p1[STRLEN];

  memset(p0, 0, sizeof(p0));
  memset(p1, 0, sizeof(p1));

  if(!str || n<=0)	return;
  if(n == 1)
  {
    strcpy(p0, str);
  }
  else
  {
    pstrchr( str, sp, n-1, p0 );
  }
  pstrchr( str, sp, n, p1 );
  if( p0 && p1 )
  {
    if( n == 1 )
    {
      strncpy( substr, p0, strlen( p0 ) - strlen( p1 ) );
    }
    else
    {
      strncpy( substr, p0+1, strlen( p0+1 ) - strlen( p1 ) );
    }
  }
}

/*********
*返回目标字符串中的子串个数
***************************************/
int getstrnum(char *dest ,char *searchstr)
{
  char *strtmp,*strtmp2;
  int i=0;
  if (strstr(dest,searchstr)==NULL)
  {
    return 0;
  }
  strtmp=dest;
  while ((strtmp2=strstr((char *)strtmp,searchstr))!=NULL)
  {
    strtmp=strtmp2+strlen(searchstr);
    i++;
  }
  return i;
}

/*********
*替换目标字符串中的子串
***************************************/
int strreplace(char *dest,char *searchstr,char *replacestr)
{
  char *sTmphead, *sTmptail;
  int ilen = 0, ipos = 0;
  int iserlen, ireplen, inum;

  if (strstr(dest,searchstr) == NULL)
  {
    return 0;
  }

  inum = getstrnum(dest,searchstr);

  iserlen = strlen(searchstr);
  ireplen = strlen(replacestr);
  ilen = strlen(dest) + (ireplen-iserlen)*inum + 1;
  if ((sTmphead=(char *)malloc(ilen)) == NULL)
  {
    perror("replace malloc error");
    return -1;
  }

  memset(sTmphead, 0, ilen);

  ilen = strstr(dest, searchstr) - dest;
  memcpy(sTmphead,dest,ilen);
  strncat(sTmphead,replacestr,strlen(replacestr));
  ipos=strstr(dest,searchstr)-dest+strlen(searchstr);
  while ((sTmptail=strstr(dest+ipos,searchstr)) != NULL)
  {
    strncat(sTmphead,dest+ipos,sTmptail-(dest+ipos));
    strncat(sTmphead,replacestr,strlen(replacestr));
    ipos=strstr(dest+ipos,searchstr)-dest+strlen(searchstr);
  }
  strncat(sTmphead,dest+ipos,strlen(dest+ipos));
  strcpy(dest,sTmphead);

  free(sTmphead);

  return 0;
}

/****
**char *strrstr(str1, str2) - find last occurrence of string in string
**Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
**
**Exit:
*       returns a pointer to the last occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
**********************************************************************/
char *strrstr(const char *str1,const char *str2)
{
  size_t ilen1 = 0;
  size_t ilen2 = 0;
  char *cp1 = (char*)str1;
  char *cp2 = (char*)str2;
  if (NULL == cp2)
    return (cp1);
  while (*cp1)
  {
    ilen1++;
    if ( !*(cp1+1) ) break;
    cp1++;
  }
  while (*cp2)
  {
    ilen2++;
    if ( !*(cp2+1) ) break;
    cp2++;
  }
  while (ilen1 > 0)
  {
    size_t item1 = ilen1;
    size_t item2 = ilen2;
    char * ctem1 = cp1;
    char * ctem2 = cp2;
    while ( (item1>0) && (item2>0) && !(*ctem1-*ctem2) )
    {
      item1--;
      item2--;
      ctem1--;
      ctem2--;
    }

    if (item2 <= 0)
      return (ctem1+1);

    ilen1--;
    cp1--;
  }
  return (NULL);
}

/**
 *截取字符串
 *begin<0 从后第几位开始向前截取
 *begin>=0 从前第几位开始向后截取
 */
int substr(char *pDes, const char *pSrc, int begin, int len)
{
  if (!pSrc || !pDes) return 0;
  if (len < 0)        return 0;

  int srcLen = strlen(pSrc);
  if (0 == srcLen) return 0;

  if (begin >= 0)
  {
    if (begin+len > srcLen)
    {
      len = srcLen - begin;
    }
    strncpy(pDes, pSrc+begin, len);
  }
  else
  {
    int tmp = srcLen+begin-len+1;
    if (tmp < 0) 
    {
      tmp = 0;
      len = srcLen + begin + 1;
    }
    strncpy(pDes, pSrc+tmp, len);
  }
  *(pDes+len) = 0;
  return len;
}
