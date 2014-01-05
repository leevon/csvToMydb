#ifndef _DCI_H_
#define _DCI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#define MY_TRUE                     1
#define MY_FALSE                    0
#define MY_SUCCEED              0   
#define MY_FAIL                     1

#define DCI_MAX_FIELD                               100
#define MAX_SQL_LEN                             1024

#define DCI_MAX_VALUE_LEN                   512
#define DCI_MAX_DATE_LEN                        21
#define DCI_MAX_DIGITAL_LEN             30
#define DCI_MAX_BUFF_LEN                        4096
#define MAX_STMT_NUM                            20


#define DCI_TYPE_STRING         0
#define DCI_TYPE_INT            1
#define DCI_TYPE_DATE         2
#define DCI_TYPE_FLOAT        3
#define DCI_TYPE_DOUBLE     4
#define DCI_TYPE_LONG         5


typedef struct _DCI_SQL
{
    MYSQL_STMT*                             hStmt;
    char                                    szSql[MAX_SQL_LEN+1];

    MYSQL_BIND                              paramBind[DCI_MAX_FIELD];
    char*                                    param[DCI_MAX_FIELD];
    char                                    paramType[DCI_MAX_FIELD];
    unsigned long                           paramLen[DCI_MAX_FIELD];
    int                                     paramNum;


    MYSQL_BIND                              resultBind[DCI_MAX_FIELD];
    char                                    resultBuff[DCI_MAX_FIELD][DCI_MAX_VALUE_LEN];
    char                                    resultType[DCI_MAX_FIELD];
    int                                     resultNum;
    char                                    bufferFlag;

    unsigned long                           real_length[DCI_MAX_FIELD];

    int                                     resFieldNum;

    //Error inforation
    char                                    szErrInfo[256]; 
    //Error number
    int                                     nErrNo;

}DCI_SQL;
typedef DCI_SQL* PDCI_SQL;

//Database call interface struct
typedef struct _DCI
{
    //DBHANDLE              dbHandle;
    MYSQL*          dbHandle;

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


    //Error inforation
    char szErrInfo[1024]; 
    //Error number
    int nErrNo;

}DCI;
typedef DCI* PDCI;

extern int dbStmtConnect(DCI* pDci);
extern int dbStmtInit( DCI* pDci, DCI_SQL* pDciSql );
extern int dbStmtPrepare(DCI* pDci, DCI_SQL* pDciSql);
extern int dbStmtQuery( DCI_SQL* pDciSql );
extern int dbStmtInsert( DCI_SQL* pDciSql );
extern int dbStmtPing( DCI_SQL* pDciSql );
extern int dbStmtFetch( DCI_SQL* pDciSql);
extern int dbStmtFree( DCI_SQL* pDciSql );
extern int dbStmtClose(DCI* pDci);

#endif

