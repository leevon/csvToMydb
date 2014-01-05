#ifndef	_DEF_H
#define	_DEF_H

#include "syncutil.h"
#include "dbprepare.h"


#define	MAX_FIELD_LEN				256
#define	MAX_RECORD_NUM		100
#define	MAX_SQL_LEN		1024
#define	MAX_FEE_ITEM	5

#define	PROC_CTRL_RUN				0
#define	PROC_CTRL_EXIT				1
#define	PROC_CTRL_RESET			2

#define	ERROR_CODE_NORMAL               "Ebill000"
#define	ERROR_CODE_FIELDEMPTY               "Ebill001"
//--------------------------------------
#define TABNAME_LEN 30
//--------------------------------------
typedef struct _TABLE_INFO
{
  DCI_SQL stmtSelectSql;
  DCI_SQL stmtInsertInfo;
  char tableName[TABNAME_LEN];
  char chkSql[MAX_SQL_LEN];
  char chk[MAX_RECORD_NUM];
  char sqlStr[MAX_SQL_LEN];
  char map[MAX_RECORD_NUM];
  struct _TABLE_INFO *pNext;
  long istNum;
}typeTableInfo;

typedef struct _INSERTLIST_INFO
{
  DCI dci;
  char tableList[MAX_FIELD_LEN];
  int tableNum;
  typeTableInfo *pHead;
} typeInsertInfo;
//--------------------------------------

//typedef struct  _DCI_INFO
//{
//	DCI 				        dci;
//	DCI_SQL				stmtGetConfNo;
//	DCI_SQL				stmtNewUser;
//	DCI_SQL				stmtGetContact;
//	DCI_SQL				stmtImpContact;
//	DCI_SQL				stmtGetCid;
//	DCI_SQL				stmtImpCid;
//	DCI_SQL				stmtGetCtrlFlag;
//	DCI_SQL				stmtGetLastId;
//	
//}DCI_INFO;

//typedef struct _CUSTOMER_INFO
//{
//    char    szCid[64+1];
//    char    szCompanyName[128+1];
//    char    szConferenNo[64+1];
//    char    szContactName[64+1];
//    char    szAddr[128+1];
//    char    szPhone[30+1];
//    char    szEmail[128+1];
//    char    szPostCode[32+1];
//    char    szTableName[32+1];
//    char    szBillingResource[64+1];
//    char    szContactId[32+1];
//    char    szLastId[32+1];
//    
//    //Product_User Related Information.
//    int       nMeetingTypeId;
//    int       nWelcTmplId;
//    int       nStatusId;
//    int       nFlag;
//    
//}CUSTOMER_INFO;

//typedef struct CUSTOMER_INFO* PCUSTOMER_INFO;


//typedef DCI_INFO* PDCI_INFO;

typedef struct _SYNC_CFG
{
	//The configuration file name
	char szIniFile[MAX_PATH_LEN + 1];
	char szSqlFileName[MAX_PATH_LEN + 1];

	char szProcLog[MAX_PATH_LEN + 1];
	char szRunLog[MAX_PATH_LEN + 1];
	char szLogTmp[MAX_PATH_LEN + 1];

	char szInDir[MAX_PATH_LEN + 1];
	char szOutDir[MAX_PATH_LEN + 1];
	char szBakDir[MAX_PATH_LEN + 1];
	char szTmpDir[MAX_PATH_LEN + 1];
	char szInCdrCtrl[MAX_PATH_LEN + 1];
	char szOutCdrCtrl[MAX_PATH_LEN + 1];
	char szRejDir[MAX_PATH_LEN+1];
	char szKeyCtrlFile[MAX_PATH_LEN + 1];
	char szFieldSep[20+1];
	//char szFieldNum[20+1]; //add by vince_lee
	
	//The database type
	char szDbType[10 + 1];
	//The database host name
	char szDbServer[20+1];
	//The database instance name.
	char szDbName[30+1];
	//The database listening port.
	unsigned int nPort;
	//The crypt login file used to login database server
	char szLoginFile[128+1];
	//The user name decrypt from login file
	char szUserName[128+1];
	//The user password decrypt from login file
	char szPassWord[128+1];
	char szCtrlFlag[128+1];
	int nInterval;
}SYNC_CFG;
typedef SYNC_CFG* PSYNC_CFG;


typedef	struct _SYNC_DATA
{
	/* Global Data Area */
	//DCI_INFO dciInfo;
	SYNC_CFG syncCfg;
	typeInsertInfo insertInfo;
	//CUSTOMER_INFO custInfo;
	
	//The state of breakpoint
	int nBreakState;
	// The temporary file pointer to store the rate record
	FILE * fpTmpOut;
	// The error file pointer to store the error record
	FILE * fpErrLog;

	char szFileName[MAX_PATH_LEN + 1];
	//The call detail record
	char szRecord[MAX_RECORD_LEN + 1];
 
  char** szRecArr;
  char **szFiled;
	//The error code
	char szErrCode[16 + 1];
	
	//The start datetime of current file
	char szStartTime[20 + 1];
	//The end datetime of current file
	char szEndTime[20 + 1];
	// The current batch
	char szCurrBatch[14 + 1];
	
	int nRecordLen;	
	//The total number of record
	long lTotalRec;
	//The number of valid record
	long lValidRec;
	//The number of error record
	long lErrorRec;

	//The file handle of current CDR
	int nProcHandle;
	//The error file handle
	int nErrHandle;


} SYNC_DATA;
typedef SYNC_DATA* PSYNC_DATA;


#endif
