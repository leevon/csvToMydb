/*
 *file name : dealFunc.c
 *des : 
 *date : 2013-11-18
 *author : liwq (286570361)
 *notes :
 *       2013-11-18   liwq      create files
 */

/**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dealFunc.h"
#include "public.h"

int getnumchr(char *pBegin, char *pEnd, char chr)
{
  int iNum = 0;
  char *pChr = pBegin;
  while(pChr < pEnd)
  {
    if (*pChr == chr)
    {
      ++iNum;
    }
    ++pChr;
  }
  return iNum;
}
char *mystrrchr(char *pBegin, char *loc, char chr)
{
  char tmpStr[128] = {0};
  strcpy(tmpStr, pBegin);
  *(tmpStr + (loc-pBegin)) = 0;
  char *pChr = strrchr(tmpStr, chr);
  if (!pChr) return NULL;
  return pBegin + (pChr - tmpStr);
}


//int substr(char *pDes, const char *pSrc, int begin, int len)
//{
//  if (!pSrc || !pDes) return 0;
//  if (len < 0)        return 0;
//
//
//  int srcLen = strlen(pSrc);
//  if (0 == srcLen) return 0;
//
//
//  if (begin >= 0)
//  {
//    if (begin+len > srcLen)
//    {
//      len = srcLen - begin;
//    }
//    strncpy(pDes, pSrc+begin, len);
//  }
//  else
//  {
//    int tmp = srcLen+begin-len+1;
//    if (tmp < 0) 
//    {
//      tmp = 0;
//      len = srcLen + begin + 1;
//    }
//    strncpy(pDes, pSrc+tmp, len);
//  }
//  *(pDes+len) = 0;
//  return len;
//}


//add[1|2]
char* myadd(char *des, const char *pFuncStr,  char *pSrc[])
{
  if (!des || !pFuncStr || !pSrc) return NULL;
  char funcStr[32] = {0};
  int index1 = 0;
  int index2 = 0;
  char *pIndex1 = strstr(pFuncStr, "add[");
  if (!pIndex1) return NULL;
  strcpy(funcStr, pIndex1+4);


  char *pIndex2 = strchr(funcStr, '|');
  if (!pIndex2) return NULL;


  char *pEnd = strchr(pIndex2+1, ']');
  if (!pEnd) return NULL;


  *pIndex2 = 0;
  index1 = atoi(funcStr);
  
  *pEnd = 0;
  index2 = atoi(pIndex2+1);


  strcpy(des, pSrc[index1]);
  strcat(des, pSrc[index2]);


  return des;
}
/**
 *sub[1|1|3]
 *abcdef
 */
char* mysub(char *des, const char *pFuncStr,  char *pSrc[])
{
  if (!des || !pFuncStr || !pSrc) return NULL;
  char funcStr[32] = {0};
  int index = 0;
  int begin = 0;
  int len = 0;
  char *pAdd = strstr(pFuncStr, "sub[");
  strcpy(funcStr, pAdd+4);
  char *pIndex = strchr(funcStr, '|');
  if (!pIndex) return NULL;
  char *pBegin = strchr(pIndex+1, '|');
  if (!pBegin) return NULL;
  char *pLen = strchr(pBegin+1, ']');
  if (!pLen)   return NULL;
  *pIndex = 0;
  index = atoi(funcStr);
  *pBegin = 0;
  begin = atoi(pIndex+1);
  *pLen = 0;
  len = atoi(pBegin+1);
  substr(des, pSrc[index], begin, len);
  return des;
}
//if[1|2|3|3]
char* myife(char *des, const char *pFuncStr,  char *pStr[])
{
  if (!des || !pFuncStr || !pStr) return NULL;


  char funcStr[32] = {0};
  int index1 = 0;
  char locStr2[16] = {0};
  int index2 = 0;
  int index3 = 0;
  int index4 = 0;
  
  char *pInd1 = strstr(pFuncStr, "ife[");
  if (!pInd1) return NULL;
  strcpy(funcStr, pInd1+4);


  char *pInd2 = strchr(funcStr, '|');
  if (!pInd2) return NULL;
  
  char *pInd3 = strchr(pInd2+1, '|');
  if (!pInd3) return NULL;


  char *pInd4 = strchr(pInd3+1, '|');
  if (!pInd4) return NULL;


  *pInd2 = 0;
  index1 = atoi(funcStr);


  *pInd3 = 0;
  if (0==strncmp("&", pInd2+1, 1))
  {
    strcpy(locStr2, pInd2+2);
  }
  else
  {
    index2 = atoi(pInd2+1);
    strcpy(locStr2, pStr[index2]);
  }

  if(0 == strcmp(pStr[index1], locStr2))
  {
    *pInd4 = 0;
    if (0 == strncmp("&", pInd3+1, 1))
    {
      strcpy(des, pInd3+2);
    }
    else
    {
      index3 = atoi(pInd3+1);
      strcpy(des, pStr[index3]);
    }
  }
  else
  {
    char *pEnd = strchr(pInd4+1, ']');
    if (!pEnd) return NULL;
    *pEnd = 0;
    if (0 == strncmp("&", pInd4+1, 1))
    {
      strcpy(des, pInd4+2);
    }
    else
    {
      index4 = atoi(pInd4+1);
      strcpy(des, pStr[index4]);
    }
  }
  return des;
}
char *mygetstr(char *des, const char *pFuncStr,  char *pStr[])
{
  char funcStr[128] = {0};
  strcpy(funcStr, pFuncStr);

  char *pFirstBegin = strrchr(funcStr, '[');
  if (!pFirstBegin) return NULL;

  char *pFirstEnd = strchr(pFirstBegin, ']');
  if (!pFirstEnd) return NULL;

  pFirstBegin -= 3;


  char *pSecondBegin = mystrrchr(funcStr, pFirstBegin, '[');
  if (!pSecondBegin)
  {
    if(0 == strncmp("ife", pFirstBegin, 3))
    {
      myife(des, pFirstBegin, pStr);
      return des;
    }
    else if (0 == strncmp("add", pFirstBegin, 3))
    {
      myadd(des, pFirstBegin, pStr);
      return des;
    }
    else if (0 == strncmp("sub", pFirstBegin, 3))
    {
      mysub(des, pFirstBegin, pStr);
      return des;
    }
  }
  else
  {
    char subRes[32] = {0};


    if (0 == strncmp("ife", pFirstBegin, 3))
    {
      myife(subRes, pFirstBegin, pStr);
    }
    else if (0 == strncmp("add", pFirstBegin, 3))
    {
      myadd(subRes, pFirstBegin, pStr);
    }
    else if (0 == strncmp("sub", pFirstBegin, 3))
    {
      mysub(subRes, pFirstBegin, pStr);
    }
    
    char *pOtherEnd = NULL;
    char *pOtherBegin = NULL;
    char otherVal[32] = {0};
    while(pSecondBegin)
    {
      pOtherEnd = mystrrchr(funcStr, pFirstBegin, ']');
      if (pOtherEnd)
      {
        if (pSecondBegin < pOtherEnd)
        {
          int beginNum = getnumchr(pSecondBegin, pOtherEnd, ']');
          pOtherBegin = pSecondBegin;
          while(beginNum--)
          {
            pOtherBegin = mystrrchr(funcStr, pOtherBegin, '[');
          }
          pOtherBegin -= 3;
          pSecondBegin = mystrrchr(funcStr, pOtherBegin, '[');
          char otherSubStr[128] = {0};
          strncpy(otherSubStr, pOtherBegin, pOtherEnd-pOtherBegin+1);
          mygetstr(otherVal, otherSubStr, pStr);
        }
      }
      pSecondBegin -= 3;
      #ifdef debug
        printf("val:%s\n", pSecondBegin);
      #endif
      long fstEnd = pFirstEnd - pSecondBegin;
      char subFunc[128] = {0};
      strncpy(subFunc, pSecondBegin, 128);
      if (0 == strncmp("ife", subFunc, 3))
      {
        char *pIdx2 = strchr(pSecondBegin+fstEnd, '|');
        if (!pIdx2) return NULL;
        long addIdx2 = pIdx2 - pSecondBegin;


        char *pIdx3 = strchr(pIdx2+1, '|');
        if (!pIdx3) return NULL;
        long addIdx3 = pIdx3 - pSecondBegin;


        char *pIdx4 = strchr(pIdx3+1, '|');
        if (!pIdx4) return NULL;
        long addIdx4 = pIdx4 - pSecondBegin;


        char *pEnd = strchr(pIdx4+1, ']');
        if (!pEnd) return NULL;
        pFirstEnd = pEnd;
        long end = pEnd - pSecondBegin;

        char tmpLoc2[16] = {0};
        *(subFunc+addIdx3) = 0;
        if (0 == strncmp("&", subFunc+addIdx2+1, 1))
        {
          strcpy(tmpLoc2, subFunc+addIdx2+2);
        }
        else
        {
          int idx2 = atoi(subFunc+addIdx2+1);
          strcpy(tmpLoc2, pStr[idx2]);
        }


        if (0 == strcmp(subRes, tmpLoc2))
        {
          *(subFunc+addIdx4) = 0;
          if (0 == strncmp("&", subFunc+addIdx3+1, 1))
          {
            strcpy(subRes, subFunc+addIdx3+2);
          }
          else
          {
            int idx3 = atoi(subFunc+addIdx3+1);
            strcpy(subRes, pStr[idx3]);
          }
        }
        else
        {
          *(subFunc+end) = 0;
          if (0 == strncmp("&", subFunc+addIdx4+1, 1))
          {
            strcpy(subRes, subFunc+addIdx4+2);
          }
          else
          {
            int idx4 = atoi(subFunc+addIdx4+1);
            strcpy(subRes, pStr[idx4]);
          }
        }
      }
      else if (0 == strncmp("add", subFunc, 3))
      {
        int otherIdx = 0;
        char *pIdx1 = strchr(pSecondBegin, '|');
        if (!pIdx1) return NULL;

        if (pOtherEnd && (pOtherEnd > pSecondBegin))
        {
          char addStrTmp[512] = {0};
          strcpy(addStrTmp, otherVal);
          strcat(addStrTmp, subRes);
          memset(subRes, 0x00, sizeof(subRes));
          strcpy(subRes, addStrTmp);
          pFirstEnd = strchr(pFirstEnd+1, ']');
        }
        else if (pIdx1 > pFirstBegin)
        {
          char *pIdx2 = strchr(pFirstEnd, '|');
          if (!pIdx2) return NULL;
          long addIdx2 = pIdx2 - pSecondBegin;

          char *pEnd = strchr(pIdx2+1, ']');
          if (!pEnd) return NULL;
          long addEnd = pEnd - pSecondBegin;
          pFirstEnd = pEnd;

          *(subFunc+addEnd) = 0;
          otherIdx = atoi(subFunc+addIdx2+1);
          strcat(subRes, pStr[otherIdx]);
        }
        else
        {
          char *pbg = strchr(pSecondBegin, '[');
          if (!pbg) return NULL;
          long addBg = pbg - pSecondBegin;
          long addIdx1 = pIdx1 - pSecondBegin;
          *(subFunc+addIdx1) = 0;
          otherIdx = atoi(subFunc+addBg+1);
          char addStrTmp[512] = {0};
          strcpy(addStrTmp, pStr[otherIdx]);
          strcat(addStrTmp, subRes);
          memset(subRes, 0x00, sizeof(subRes));
          strcpy(subRes, addStrTmp);
          pFirstEnd += 1;
        }
      }
      else if (0 == strncmp("sub", subFunc, 3))
      {
        char *pidx2 = strchr(pSecondBegin+fstEnd, '|');
        if (!pidx2) return NULL;
        long addIdx2 = pidx2 - pSecondBegin;

        char *pidx3 = strchr(pidx2+1, '|');
        if (!pidx3) return NULL;
        long addIdx3 = pidx3 - pSecondBegin;

        char *pEnd = strchr(pidx3+1, ']');
        if (!pEnd) return NULL;
        pFirstEnd = pEnd;
        long addEnd = pEnd - pSecondBegin;

        *(subFunc+addIdx3) = 0;
        int start = atoi(subFunc + addIdx2 + 1);
        
        *(subFunc+addEnd) = 0;
        int len = atoi(subFunc+addIdx3+1);
        char suttmp[32] = {};
        substr(suttmp, subRes, start, len);
        strcpy(subRes, suttmp);
      }
      pFirstBegin = pSecondBegin;
      pSecondBegin = mystrrchr(funcStr, pFirstBegin, '[');
    }
    strcpy(des, subRes);
  }
  return des;
}


int testMain(void)
{
  char *srcStr[8] = {"vince_lee", "jay_", "chou", "jay_chou", "jay_chou", "23", "24", "25"};
  char des[80] = {0};
  
  mygetstr(des, "ife[3|4|5|6]", srcStr);
  printf("the des1_1:%s\n", des);
  mygetstr(des, "ife[2|3|5|6]", srcStr);
  printf("the des1_2:%s\n", des);

  mygetstr(des, "add[1|2]", srcStr);
  printf("the des1_3:%s\n", des);
  
  mygetstr(des, "ife[add[1|2]|3|5|6]", srcStr);
  printf("the des1_4:%s\n", des);
  
  mygetstr(des, "sub[3|4|4]", srcStr);
  printf("the des1_5:%s\n", des);
  
  mygetstr(des, "ife[sub[3|4|4]|2|5|6]", srcStr);
  printf("the des1_6:%s\n", des);
  
  mygetstr(des, "add[sub[3|0|4]|2]", srcStr);
  printf("the des2_1:%s\n", des);
  mygetstr(des, "add[1|sub[3|4|4]]", srcStr);
  printf("the des2_1:%s\n", des);

  mygetstr(des, "add[add[sub[3|0|4]|2]|add[1|sub[3|4|4]]]", srcStr);
  printf("the des3_1:%s\n", des);
  
  mygetstr(des, "sub[add[add[sub[3|0|4]|2]|add[1|sub[3|4|4]]]|0|8]", srcStr);
  printf("the des3_2:%s\n", des);
  
  mygetstr(des, "ife[sub[add[add[sub[3|0|4]|2]|add[1|sub[3|4|4]]]|0|8]|3|5|6]", srcStr);
  printf("the des3_3:%s\n", des);
  
  mygetstr(des, "ife[0|&vince_lee|1|2]", srcStr);
  printf("the des3_4:%s\n", des);
  
  mygetstr(des, "ife[0|&vince_lee|&1|&2]", srcStr);
  printf("the des3_5:%s\n", des);
  
  mygetstr(des, "ife[add[1|2]|&jay_chou|&1|&2]", srcStr);
  printf("the des3_6:%s\n", des);
  return 0;
}
