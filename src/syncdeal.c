#include <iconv.h>
#include "syncutil.h"
#include "syncdef.h"
#include "public.h"
#include "dealFunc.h"


/**
 * Get current log batch.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int getCurrBatch( PSYNC_DATA pSyncData )
{
    struct tm * tmTime;

    tmTime = localtime( &tmStartTime );

    memset( pSyncData->szCurrBatch, 0, sizeof( pSyncData->szCurrBatch ) );

    sprintf( pSyncData->szCurrBatch, "%04d%02d%02d%02d%02d%02d",
            tmTime->tm_year + 1900,	tmTime->tm_mon + 1,	tmTime->tm_mday,
            tmTime->tm_hour, tmTime->tm_min,	tmTime->tm_sec );

    return MY_SUCCEED;
}


/**
 * Open process log and temporary output file.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int openLogFile( PSYNC_DATA pSyncData )
{
    char szFilePath[MAX_PATH_LEN + 1];

    memset( szFilePath, 0, sizeof( szFilePath ) );

    sprintf( szFilePath, "%s/dbsync.%s.log", (pSyncData->syncCfg).szLogTmp, szSrcDir );

    pSyncData->nProcHandle = open( szFilePath, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
    if( -1 == pSyncData->nProcHandle )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Open process log: %s failed!\n", szFilePath );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    memset( szFilePath, 0, sizeof( szFilePath ) );

    sprintf( szFilePath, "%s/dbsync.%s.err", (pSyncData->syncCfg).szLogTmp, szSrcDir );

    pSyncData->fpErrLog = fopen( szFilePath, "wb" );
    if( NULL == pSyncData->fpErrLog  )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Open temporary output file: %s failed!\n", szFilePath );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Move the error file to reject directory.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int moveErrFile(PSYNC_DATA pSyncData)
{
    char szRejFile[MAX_PATH_LEN+1];
    char szInFile[MAX_PATH_LEN+1];
    szRejFile[0] = 0;
    szInFile[0] = 0;
    
    sprintf( szInFile, "%s/%s", pSyncData->syncCfg.szInDir, pSyncData->szFileName );
    sprintf( szRejFile, "%s/%s", pSyncData->syncCfg.szRejDir, pSyncData->szFileName );
    if( MY_SUCCEED != moveFile( szInFile, szRejFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Move file: %s to reject directory: %s failed!\n", szInFile, pSyncData->syncCfg.szRejDir );
        return MY_FAIL;
    }
    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Reject file: %s !\n", szInFile );
    
}


/**
 * Open output temporary file.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int openTmpOutFile( PSYNC_DATA pSyncData )
{
    char szFilePath[MAX_PATH_LEN + 1];

    if( NULL == pSyncData->fpTmpOut )
    {
        memset( szFilePath, 0, sizeof( szFilePath ) );

        sprintf( szFilePath, "%s/%s.tmp", pSyncData->syncCfg.szTmpDir, pSyncData->szFileName );

        pSyncData->fpTmpOut = fopen( szFilePath, "a" );


        if( NULL == pSyncData->fpTmpOut  )
        {
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Open temporary file: %s  failed!\n", szFilePath );
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
            return MY_FAIL;
        }
    }

    return MY_SUCCEED;
}

/**
 * Write error record to error file.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int writeErrRecord( PSYNC_DATA pSyncData )
{
    char szErrorInfo[MAX_RECORD_LEN + 1];

    memset( szErrorInfo, 0, sizeof( szErrorInfo ) );

    sprintf( szErrorInfo, "%s:%s:%s", pSyncData->szFileName, pSyncData->szErrCode, pSyncData->szRecord );

    if( strlen( szErrorInfo ) != fwrite( szErrorInfo, sizeof( char ), strlen( szErrorInfo ), pSyncData->fpErrLog ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Write error record to file failed!\n" );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
        return MY_FAIL;
    }
    fflush( pSyncData->fpErrLog );
    pSyncData->lErrorRec++;

    return MY_SUCCEED;
}


/**
 * Write process log.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */


int writeProcLog( PSYNC_DATA pSyncData )
{
    char szLogInfo[MAX_RECORD_LEN + 1];

    memset( szLogInfo, 0, sizeof( szLogInfo ) );

    sprintf( szLogInfo, "[%s]:%s:%s:T %ld:V %ld:E %ld\n",
            pSyncData->szFileName,
            pSyncData->szStartTime, pSyncData->szEndTime,
            pSyncData->lTotalRec, pSyncData->lValidRec, pSyncData->lErrorRec );

    if( strlen( szLogInfo ) != write( pSyncData->nProcHandle, szLogInfo, strlen( szLogInfo ) ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Write process log failed\n" );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Move output temporary file to formal directory and backup the source file.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int moveAndBakFile( PSYNC_DATA pSyncData )
{
    char szSrcFile[MAX_PATH_LEN + 1];
    char szDstFile[MAX_PATH_LEN + 1];

    memset( szSrcFile, 0, sizeof( szSrcFile ) );
    memset( szDstFile, 0, sizeof( szDstFile ) );

    sprintf( szSrcFile, "%s/%s", pSyncData->syncCfg.szInDir , pSyncData->szFileName );
    sprintf( szDstFile, "%s/%s", pSyncData->syncCfg.szBakDir, pSyncData->szFileName );

    if( MY_SUCCEED != moveFile( szSrcFile, szDstFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Move backup source file: %s failed!\n" , pSyncData->szFileName);
        return MY_FAIL;
    }

    return MY_SUCCEED;
}


/**
 * Move process log and error log files from temporary to formal directory
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int moveLogFile( PSYNC_DATA pSyncData )
{
    char szSrcFile[MAX_PATH_LEN + 1];
    char szDstFile[MAX_PATH_LEN + 1];

    memset( szSrcFile, 0, sizeof( szSrcFile ) );
    memset( szDstFile, 0, sizeof( szDstFile ) );

    sprintf( szSrcFile, "%s/dbsync.%s.log", (pSyncData->syncCfg).szLogTmp, szSrcDir );
    sprintf( szDstFile, "%s/logbak/dbsync%12.12s.%s.log", (pSyncData->syncCfg).szProcLog, pSyncData->szCurrBatch, szSrcDir );

    if( MY_SUCCEED != moveFile( szSrcFile, szDstFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Move temporary process log file %s to %s failed!\n", szSrcFile,  szDstFile);
        return MY_FAIL;
    }

    memset( szSrcFile, 0, sizeof( szSrcFile ) );
    memset( szDstFile, 0, sizeof( szDstFile ) );

    sprintf( szSrcFile, "%s/dbsync.%s.err", (pSyncData->syncCfg).szLogTmp, szSrcDir );
    sprintf( szDstFile, "%s/logbak/dbsync%12.12s.%s.err", (pSyncData->syncCfg).szProcLog, pSyncData->szCurrBatch, szSrcDir );

    if( MY_SUCCEED != moveFile( szSrcFile, szDstFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Move temporary error log file %s to %s failed!\n", szSrcFile, szDstFile );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Close current deal log and error log.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int closeLogFile( PSYNC_DATA pSyncData )
{
    if( -1 != pSyncData->nProcHandle )
    {
        close( pSyncData->nProcHandle );
        pSyncData->nProcHandle = -1;
    }

    if( NULL != pSyncData->fpErrLog )
    {
        fclose( pSyncData->fpErrLog );
        pSyncData->fpErrLog = NULL;
    }

    return MY_SUCCEED;
}


/**
 * Close the old process and error log files and move the old files.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int switchLogFile( PSYNC_DATA pSyncData )
{
    if( MY_SUCCEED != closeLogFile( pSyncData ) )
    {
        return MY_FAIL;
    }

    if( MY_SUCCEED != moveLogFile( pSyncData ) )
    {

        return MY_FAIL;
    }

    getCurrBatch( pSyncData );

    if( MY_SUCCEED != openLogFile( pSyncData ) )
    {
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Determine if it is the file that needs to be charged.
 *
 * @param PSYNC_DATA pSyncData The global variable contains all dbsync data.
 * @param char* szFileName The source file name.
 * @return boolean
 */

int isSyncFile( PSYNC_DATA pSyncData, char * szFileName )
{

    return MY_TRUE;
}

/**
 * Transfer the record string into array format.
 *
 * @param char* szRecord The call detail record string, splited by dedicated seperator.
 * @param char** szRecArr The record array.
 * @return MY_SUCCEED|MY_FAIL
 */

int record2Array(char* szRecord, char** szRecArr, char* szSep)
{
    char  *token,**str;
    int j;

    for (j = 0, str = &szRecord; ; j++) {
        token = strsep(str, szSep);
        if (token == NULL)
            break;
        TrimLeft(TrimRight(token));
        strcpy(szRecArr[j], token);

    }
    return MY_SUCCEED;
}

/**
 *不用注释掉 vince_lee at 20131111
int cidExists(PSYNC_DATA pSyncData, int* nExists )
{
    
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    pDciInfo->stmtGetCid.param[0] = (char*)(pCustInfo->szCid);
    pDciInfo->stmtGetCid.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtGetCid.paramLen[0]= strlen(pCustInfo->szCid);
    
    pDciInfo->stmtGetCid.paramNum = 1;
    pDciInfo->stmtGetCid.bufferFlag = '1';
    pDciInfo->stmtGetCid.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtQuery( &(pSyncData->dciInfo.stmtGetCid) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Query customer_id failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtGetCid.nErrNo, pSyncData->dciInfo.stmtGetCid.szErrInfo );
        return MY_FAIL;
    }
    
    if ( 0 == pDciInfo->stmtGetCid.resultNum )
    {
        *nExists = 0;
        
    }
    else
    {
        *nExists = 1;
    }
    return MY_SUCCEED;
    
}
*/

/**
 *不用注释掉 vince_lee at 20131111
int insertCust( PSYNC_DATA pSyncData )
{
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    int levelId = 4;
    
    pDciInfo->stmtImpCid.param[0] = (char*)(pCustInfo->szCid);
    pDciInfo->stmtImpCid.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[0]= strlen(pCustInfo->szCid);
    
    
    pDciInfo->stmtImpCid.param[1] = (char*)(&levelId);
    pDciInfo->stmtImpCid.paramType[1] = DCI_TYPE_LONG;
    pDciInfo->stmtImpCid.paramLen[1]= sizeof(levelId);
        
    pDciInfo->stmtImpCid.param[2] = (char*)(pCustInfo->szCompanyName);
    pDciInfo->stmtImpCid.paramType[2] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[2]= strlen(pCustInfo->szCompanyName);
    
    pDciInfo->stmtImpCid.param[3] = (char*)(pCustInfo->szContactName);
    pDciInfo->stmtImpCid.paramType[3] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[3]= strlen(pCustInfo->szContactName);
    
    pDciInfo->stmtImpCid.param[4] = (char*)(pCustInfo->szEmail);
    pDciInfo->stmtImpCid.paramType[4] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[4]= strlen(pCustInfo->szEmail);

    pDciInfo->stmtImpCid.param[5] = (char*)(pCustInfo->szPhone);
    pDciInfo->stmtImpCid.paramType[5] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[5]= strlen(pCustInfo->szPhone);

    pDciInfo->stmtImpCid.param[6] = (char*)(pCustInfo->szPostCode);
    pDciInfo->stmtImpCid.paramType[6] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[6]= strlen(pCustInfo->szPostCode);
    
    pDciInfo->stmtImpCid.param[7] = (char*)(pCustInfo->szAddr);
    pDciInfo->stmtImpCid.paramType[7] = DCI_TYPE_STRING;
    pDciInfo->stmtImpCid.paramLen[7]= strlen(pCustInfo->szAddr);

    pDciInfo->stmtImpCid.paramNum = 8;
    pDciInfo->stmtImpCid.bufferFlag = '1';
    pDciInfo->stmtGetContact.resFieldNum = 0;
    pDciInfo->stmtImpCid.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtInsert( &(pSyncData->dciInfo.stmtImpCid) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Insert customer_id failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtImpCid.nErrNo, pSyncData->dciInfo.stmtImpCid.szErrInfo );
        return MY_FAIL;
    }
    return MY_SUCCEED;
}
*/

/**
 *不用注释掉 vince_lee at 20131111
int contactExists( PSYNC_DATA pSyncData, int* nExists )
{
    //Check if contact exists.
    char szTmpPhone[32+1];
    char szTmpEmail[128+1];
    szTmpPhone[0]=0;
    szTmpEmail[0]=0;
    
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    memset( pCustInfo->szContactId, 0, sizeof(pCustInfo->szContactId ) );
    sprintf(szTmpPhone, "%s%s", "\%", pCustInfo->szPhone);
    
    pDciInfo->stmtGetContact.param[0] = (char*)(pCustInfo->szCid);
    pDciInfo->stmtGetContact.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtGetContact.paramLen[0]= strlen(pCustInfo->szCid);
    
    pDciInfo->stmtGetContact.param[1] = (char*)(szTmpPhone);
    pDciInfo->stmtGetContact.paramType[1] = DCI_TYPE_STRING;
    pDciInfo->stmtGetContact.paramLen[1]= strlen(szTmpPhone);
    
    pDciInfo->stmtGetContact.param[2] = (char*)(pCustInfo->szEmail);
    pDciInfo->stmtGetContact.paramType[2] = DCI_TYPE_STRING;
    pDciInfo->stmtGetContact.paramLen[2]= strlen(pCustInfo->szEmail);
    
    
    pDciInfo->stmtGetContact.paramNum = 3;
    pDciInfo->stmtGetContact.bufferFlag = '1';
    
    pDciInfo->stmtGetContact.resFieldNum = 1;
    pDciInfo->stmtGetContact.resultNum = 0;
    
    pDciInfo->stmtGetContact.resultType[0] = DCI_TYPE_STRING;

    if (MY_SUCCEED != dbStmtQuery( &(pSyncData->dciInfo.stmtGetContact) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Get contact failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtGetContact.nErrNo, pSyncData->dciInfo.stmtGetContact.szErrInfo );
        return MY_FAIL;
    }
    
    if ( 0 == pDciInfo->stmtGetContact.resultNum )
    {
        *nExists = 0;
    }
    else
    {
        strcpy( pCustInfo->szContactId, pDciInfo->stmtGetContact.resultBuff[0]);
        *nExists = 1;
        if( MY_SUCCEED != dbStmtFetch( &(pSyncData->dciInfo.stmtGetContact)))
        {
            MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Fetch contact failed, errno: %d, error: %s !\n\n",  pDciInfo->stmtGetContact.nErrNo, pDciInfo->stmtGetContact.szErrInfo);
            return  MY_FAIL;
        }
    }

    return MY_SUCCEED;

}
*/
/**
 *不用注释掉 vince_lee at 20131111
int insertContact ( PSYNC_DATA pSyncData )
{
    
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    memset(pCustInfo->szLastId, 0, sizeof(pCustInfo->szLastId));
    strcpy(pCustInfo->szTableName, "Contact");
    pDciInfo->stmtGetLastId.param[0] = (char*)(pCustInfo->szTableName);
    pDciInfo->stmtGetLastId.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtGetLastId.paramLen[0]= strlen(pCustInfo->szTableName);
    
    //Get last contact id
    pDciInfo->stmtGetLastId.paramNum = 1;
    pDciInfo->stmtGetLastId.bufferFlag = '1';
    
    pDciInfo->stmtGetLastId.resFieldNum = 11;
    pDciInfo->stmtGetLastId.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtQuery( &(pSyncData->dciInfo.stmtGetLastId) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Get contact last id failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtGetLastId.nErrNo, pSyncData->dciInfo.stmtGetLastId.szErrInfo );
        return MY_FAIL;
    }
    if( MY_SUCCEED != dbStmtFetch( &(pSyncData->dciInfo.stmtGetLastId)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Fetch contact last id failed, errno: %d, error: %s !\n\n",  pDciInfo->stmtGetLastId.nErrNo, pDciInfo->stmtGetLastId.szErrInfo);
        return  MY_FAIL;
    }
    
    strcpy( pCustInfo->szLastId, pDciInfo->stmtGetLastId.resultBuff[10]);

    if ( 0 == pCustInfo->szContactId[0] )
    {
        strcpy( pCustInfo->szContactId, pCustInfo->szLastId);
    }
    
    //Import a new contact
    pDciInfo->stmtImpContact.param[0] = (char*)(pCustInfo->szContactId);
    pDciInfo->stmtImpContact.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[0]= strlen(pCustInfo->szContactId);
    
    pDciInfo->stmtImpContact.param[1] = (char*)(pCustInfo->szCid);
    pDciInfo->stmtImpContact.paramType[1] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[1]= strlen(pCustInfo->szCid);
    
    pDciInfo->stmtImpContact.param[2] = (char*)(pCustInfo->szPhone);
    pDciInfo->stmtImpContact.paramType[2] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[2]= strlen(pCustInfo->szPhone);
    
    pDciInfo->stmtImpContact.param[3] = (char*)(pCustInfo->szEmail);
    pDciInfo->stmtImpContact.paramType[3] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[3]= strlen(pCustInfo->szEmail);
    
    pDciInfo->stmtImpContact.param[4] = (char*)(pCustInfo->szPostCode);
    pDciInfo->stmtImpContact.paramType[4] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[4]= strlen(pCustInfo->szPostCode);
    
    pDciInfo->stmtImpContact.param[5] = (char*)(pCustInfo->szAddr);
    pDciInfo->stmtImpContact.paramType[5] = DCI_TYPE_STRING;
    pDciInfo->stmtImpContact.paramLen[5]= 2*strlen(pCustInfo->szAddr);
    
    pDciInfo->stmtImpContact.paramNum = 6;
    pDciInfo->stmtImpContact.bufferFlag = '1';
    pDciInfo->stmtImpContact.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtInsert( &(pSyncData->dciInfo.stmtImpContact) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Insert new contact to contact failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtImpContact.nErrNo, pSyncData->dciInfo.stmtImpContact.szErrInfo );
        return MY_FAIL;
    }
    
    MyLog( LOG_SOURCE_DB, LOG_LEVEL_DEBUG, "Insert new contact to contact successful affected: [%d]\n", pDciInfo->stmtImpContact.resultNum);
    return MY_SUCCEED;

}
*/

/**
 *不用注释掉 vince_lee at 20131111
int confNoExists( PSYNC_DATA pSyncData, int* nExists )
{
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    pDciInfo->stmtGetConfNo.param[0] = (char*)(pCustInfo->szConferenNo);
    pDciInfo->stmtGetConfNo.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtGetConfNo.paramLen[0]= strlen(pCustInfo->szConferenNo);
    
    pDciInfo->stmtGetConfNo.paramNum = 1;
    pDciInfo->stmtGetConfNo.bufferFlag = '1';

    pDciInfo->stmtGetConfNo.resFieldNum = 11;
    pDciInfo->stmtGetConfNo.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtQuery( &(pSyncData->dciInfo.stmtGetConfNo) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Query conference no failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtGetConfNo.nErrNo, pSyncData->dciInfo.stmtGetConfNo.szErrInfo );
        return MY_FAIL;
    }
    
    if ( 0 == pDciInfo->stmtGetConfNo.resultNum )
    {
        *nExists = 0;
    }
    else
    {
        *nExists = 1;
    }
    
    return MY_SUCCEED;
    
    
}
*/

/**
 *不用注释掉 vince_lee at 20131111
int insertUser( PSYNC_DATA pSyncData )
{
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
        
    //Get Last user id in Product User table.
    strcpy(pCustInfo->szTableName, "Product_User");

    pDciInfo->stmtGetLastId.param[0] = (char*)(pCustInfo->szTableName);
    pDciInfo->stmtGetLastId.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtGetLastId.paramLen[0]= strlen(pCustInfo->szTableName);
    
    pDciInfo->stmtGetLastId.paramNum = 1;
    pDciInfo->stmtGetLastId.bufferFlag = '1';

    pDciInfo->stmtGetLastId.resFieldNum = 11;
    pDciInfo->stmtGetLastId.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtQuery( &(pSyncData->dciInfo.stmtGetLastId) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Query last user id failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtGetLastId.nErrNo, pSyncData->dciInfo.stmtGetLastId.szErrInfo );
        return MY_FAIL;
    }

    if( MY_SUCCEED != dbStmtFetch( &(pSyncData->dciInfo.stmtGetLastId)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Fetch last user id failed, errno: %d, error: %s !\n\n",  pDciInfo->stmtGetLastId.nErrNo, pDciInfo->stmtGetLastId.szErrInfo);
        return  MY_FAIL;
    }
    
    strcpy( pCustInfo->szLastId, pDciInfo->stmtGetLastId.resultBuff[10]); 

    //Insert new conference number information to product_user.
    pDciInfo->stmtNewUser.param[0] = (char*)(pCustInfo->szLastId);
    pDciInfo->stmtNewUser.paramType[0] = DCI_TYPE_STRING;
    pDciInfo->stmtNewUser.paramLen[0]= strlen(pCustInfo->szLastId);
    //pDciInfo->stmtNewUser.paramLen[0]= 0;
    

    pDciInfo->stmtNewUser.param[1] = (char*)&(pCustInfo->nMeetingTypeId);
    pDciInfo->stmtNewUser.paramType[1] = DCI_TYPE_LONG;
    pDciInfo->stmtNewUser.paramLen[1]= sizeof(pCustInfo->nMeetingTypeId);
    

    pDciInfo->stmtNewUser.param[2] = (char*)&(pCustInfo->nWelcTmplId);
    pDciInfo->stmtNewUser.paramType[2] = DCI_TYPE_LONG;
    pDciInfo->stmtNewUser.paramLen[2]= sizeof(pCustInfo->nWelcTmplId);

    pDciInfo->stmtNewUser.param[3] = (char*)&(pCustInfo->szConferenNo);
    pDciInfo->stmtNewUser.paramType[3] = DCI_TYPE_STRING;
    pDciInfo->stmtNewUser.paramLen[3]= strlen(pCustInfo->szConferenNo);


    pDciInfo->stmtNewUser.param[4] = (char*)&(pCustInfo->szBillingResource);
    pDciInfo->stmtNewUser.paramType[4] = DCI_TYPE_STRING;
    pDciInfo->stmtNewUser.paramLen[4]= strlen(pCustInfo->szBillingResource);
    
    
    pDciInfo->stmtNewUser.param[5] = (char*)&(pCustInfo->nStatusId);
    pDciInfo->stmtNewUser.paramType[5] = DCI_TYPE_LONG;
    pDciInfo->stmtNewUser.paramLen[5]= sizeof(pCustInfo->nStatusId);
    

    pDciInfo->stmtNewUser.param[6] = (char*)&(pCustInfo->szContactId);
    pDciInfo->stmtNewUser.paramType[6] = DCI_TYPE_STRING;
    pDciInfo->stmtNewUser.paramLen[6]= strlen(pCustInfo->szContactId);
    
    pDciInfo->stmtNewUser.param[7] = (char*)&(pCustInfo->nFlag);
    pDciInfo->stmtNewUser.paramType[7] = DCI_TYPE_LONG;
    pDciInfo->stmtNewUser.paramLen[7]= sizeof(pCustInfo->nFlag);


    pDciInfo->stmtNewUser.paramNum = 8;
    pDciInfo->stmtNewUser.resultNum = 0;
    
    if (MY_SUCCEED != dbStmtInsert( &(pSyncData->dciInfo.stmtNewUser) ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Insert new user to product_user failed, errno: %d, error: %s !\n\n", pSyncData->dciInfo.stmtNewUser.nErrNo, pSyncData->dciInfo.stmtNewUser.szErrInfo );
        return MY_FAIL;
    }
    
    MyLog( LOG_SOURCE_DB, LOG_LEVEL_DEBUG, "Insert new user to product_user successful affected: [%d]\n", pDciInfo->stmtNewUser.resultNum);
    return MY_SUCCEED;

}
*/

/**
 *重写了 by vince_lee at 20131111
 */
 /*
int dealRecord( PSYNC_DATA pSyncData )
{

    CUSTOMER_INFO* pCustInfo = &(pSyncData->custInfo);
    
    int nExists = 0;
    pCustInfo->nMeetingTypeId = 1;
    pCustInfo->nWelcTmplId = 1;
    pCustInfo->nStatusId = 1;
    pCustInfo->nFlag = 1;
    char szRecord[MAX_RECORD_LEN + 1];
    memset(szRecord, 0, sizeof(szRecord));
    
    strcpy(szRecord, pSyncData->szRecord);
    record2Array(szRecord, pSyncData->szRecArr, pSyncData->syncCfg.szFieldSep);
    
    strcpy(pCustInfo->szCid, pSyncData->szRecArr[0]);
    
    TrimRight(TrimLeft(pCustInfo->szCid));
    if ( pCustInfo->szCid[0] == 0 )
    {
        strcpy(pSyncData->szErrCode, ERROR_CODE_FIELDEMPTY);
        return MY_SUCCEED;
    }

    strcpy(pCustInfo->szCompanyName, pSyncData->szRecArr[1]);
    TrimRight(TrimLeft(pCustInfo->szCompanyName));
    
    strcpy(pCustInfo->szConferenNo, pSyncData->szRecArr[2]);
    TrimRight(TrimLeft(pCustInfo->szConferenNo));
    if ( pCustInfo->szConferenNo[0] == 0 )
    {
        strcpy(pSyncData->szErrCode, ERROR_CODE_FIELDEMPTY);
        return MY_SUCCEED;
    }
    
    sprintf(pCustInfo->szBillingResource, "%s%s", pCustInfo->szConferenNo, (pCustInfo->szConferenNo)+strlen(pCustInfo->szConferenNo)-8);
    
    strcpy(pCustInfo->szContactName, pSyncData->szRecArr[3]);
    TrimRight(TrimLeft(pCustInfo->szContactName));

    strcpy(pCustInfo->szAddr, pSyncData->szRecArr[4]);
    TrimRight(TrimLeft(pCustInfo->szAddr));

    strcpy(pCustInfo->szPhone, pSyncData->szRecArr[5]);
    TrimRight(TrimLeft(pCustInfo->szPhone));
    
    strcpy(pCustInfo->szEmail, pSyncData->szRecArr[6]);
    TrimRight(TrimLeft(pCustInfo->szEmail));

    strcpy(pCustInfo->szPostCode, pSyncData->szRecArr[7]);
    TrimRight(TrimLeft(pCustInfo->szPostCode));
    
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    
    //Check if customer exists.
    if ( MY_SUCCEED != cidExists( pSyncData, &nExists ))
    {
        return MY_FAIL;

    }
    else if ( 0 == nExists )
    {
        //Create new customer
        if ( MY_SUCCEED != insertCust(pSyncData) )
        {
            return MY_FAIL;
        }
    }
    
    nExists = 0;
    if ( MY_SUCCEED != contactExists( pSyncData, &nExists ) )
    {
        return MY_FAIL;
    }
    else if ( 0 == nExists )
    {
        if ( MY_SUCCEED != insertContact(pSyncData) )
        {
            return MY_FAIL;
        }
    }
    
    //Check if user exists
    nExists = 0;
    if ( MY_SUCCEED != confNoExists( pSyncData, &nExists ) )
    {
        return MY_FAIL;
    }
    
    if ( 0 == nExists )
    {
        if ( MY_SUCCEED != insertUser(pSyncData) )
        {
            return MY_FAIL;
        }
    }
    
    ++pSyncData->lValidRec;
    return MY_SUCCEED;
}
*/
int sltConfine(const char *src, char *des1, char *des2)
{
  char srcTmp[1024] = {0};
  strcpy(srcTmp, src);
  char *p1 = strchr(srcTmp, '(');
  if (NULL == p1)
  {
    strcpy(des1, srcTmp);
    strcpy(des2, "string");
    return MY_SUCCEED;
  }
  *p1 = 0;

  char *p2 = strchr(p1+1, ')');
  if (p2 == NULL)
  return MY_FAIL;

  *p2 = 0;
  strcpy(des1, srcTmp);
  strcpy(des2, p1+1);
  return MY_SUCCEED;
}
int sltTerm(const char *src, char *des1, char *des2, char *des3)
{
  char srcTmp[1024] = {0};
  strcpy(srcTmp, src);
  char *p1 = strchr(srcTmp, ':');
  if (p1 == NULL) return 1;
  
  *p1 = 0;
  strcpy(des1, srcTmp);
  
  return sltConfine(p1+1, des2, des3);
}

int putRptRcd(int hand, const char *pTab, const char *key)
{
  char szLogInfo[MAX_RECORD_LEN + 1] = {0};
  sprintf(szLogInfo, "[repeat]:table:[%s] key:{%s}\n", pTab, key);
  if( strlen( szLogInfo ) != write( hand, szLogInfo, strlen( szLogInfo ) ) )
  {
    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "putRptRcd failed\n" );
    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
    return MY_FAIL;
  }
  return MY_SUCCEED;
}
int putRstRcd(int hand, typeTableInfo *pTable)
{
  char szLogInfo[MAX_RECORD_LEN + 1] = {0};
  while(pTable)
  {
    memset(szLogInfo, 0x00 ,sizeof(szLogInfo));
    sprintf(szLogInfo, "[result]:table:[%s] record:[%d]\n", pTable->tableName, pTable->istNum);
    if( strlen( szLogInfo ) != write( hand, szLogInfo, strlen( szLogInfo ) ) )
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "putRstRcd failed\n" );
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
      return MY_FAIL;
    }
    pTable = pTable->pNext;
  }
  return MY_SUCCEED;
}
int checkItem(PSYNC_DATA pSyncData, typeTableInfo *pTable, char **ppszRecArr)
{
  if (strlen(pTable->chk) == 0)
  {
    return 0;
  }

  DCI_SQL *pSelectSql = &pTable->stmtSelectSql;

  char chkKey[512] = {0};
  int itemCount = getstrnum(pTable->chk, ",") + 1;
  int i = 0;
  for (i=0; i<itemCount; ++i)
  {
    char term[64] = {0};
    getsubstr(pTable->chk, ',', i+1, term);
    char post[10] = {0};
    char type[22] = {0};
    sltConfine(term, post, type);

    if (0==strcmp(type,"int") || strcmp(type,"long")==0)
    {
      pSelectSql->param[i] = (char*)(ppszRecArr[atoi(post)]);
      pSelectSql->paramType[i] = DCI_TYPE_LONG;
      pSelectSql->paramLen[i]= strlen(ppszRecArr[atoi(post)]);
    }
    else
    {
      pSelectSql->param[i] = (char*)(ppszRecArr[atoi(post)]);
      pSelectSql->paramType[i] = DCI_TYPE_STRING;
      pSelectSql->paramLen[i]= strlen(ppszRecArr[atoi(post)]);
    }
    //strcat(chkKey,pSyncData->lTotalRec);
    sprintf(chkKey, "%s%d", chkKey, pSyncData->lTotalRec);
    strcat(chkKey,":");
    strcat(chkKey,post);
    strcat(chkKey, "[");
    strcat(chkKey, ppszRecArr[atoi(post)]);
    strcat(chkKey, "]");
  }

  pSelectSql->paramNum = itemCount;
  pSelectSql->bufferFlag = '1';

  pSelectSql->resFieldNum = 1;
  pSelectSql->resultNum = 0;

  if (MY_SUCCEED != dbStmtQuery(pSelectSql))
  {
    MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Query no failed, errno: %d, error: %s !\n\n", pSelectSql->nErrNo, pSelectSql->szErrInfo );
    return -1;
  }
  
  if (pSelectSql->resultNum > 0)
  {
    putRptRcd(pSyncData->nProcHandle, pTable->tableName, chkKey);
  }
  return pSelectSql->resultNum ;
}

int dealRecord (PSYNC_DATA pSyncData)
{
  int ret = record2Array(pSyncData->szRecord, \
                         pSyncData->szRecArr, pSyncData->syncCfg.szFieldSep);
  if (ret != MY_SUCCEED)
  {
    return MY_FAIL;
  }
  typeTableInfo *pTable = pSyncData->insertInfo.pHead;
  //DCI *pDci = &pSyncData->insertInfo.dci;
  while(pTable)
  {
    //check
    if (checkItem(pSyncData, pTable, pSyncData->szRecArr) > 0)
    {
      pTable = pTable->pNext;
      continue;
    }

    int k=0;
    int max = getstrnum(pTable->map, ",") + 1;
    for (k=0;k<max;++k)
    {
      char confine[512] = {0};
      char post[512] = {0};
      char type[10] = {0};
      getsubstr(pTable->map, ',', k+1, confine);
      sltConfine(confine, post, type);
      
      if (!isNumber(post))
      {
        mygetstr(pSyncData->szFiled[k], post, pSyncData->szRecArr);
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_DEBUG, "func:[%s], val:[%s]\n", post, pSyncData->szFiled[k]);
      }
      else
      {
        strcpy(pSyncData->szFiled[k], pSyncData->szRecArr[atoi(post)]);
      }

      if (0==strcmp("int", type) || strcmp(type,"long")==0)
      {
        pTable->stmtInsertInfo.param[k] = (char*)(pSyncData->szFiled[k]);
        pTable->stmtInsertInfo.paramType[k] = DCI_TYPE_LONG;
        pTable->stmtInsertInfo.paramLen[k] = sizeof(pSyncData->szFiled[k]);
      }
      else
      {
        pTable->stmtInsertInfo.param[k] = (char*)(pSyncData->szFiled[k]);
        pTable->stmtInsertInfo.paramType[k] = DCI_TYPE_STRING;
        pTable->stmtInsertInfo.paramLen[k]= strlen(pSyncData->szFiled[k]);
      }
    }
    pTable->stmtInsertInfo.paramNum = max;
    pTable->stmtInsertInfo.bufferFlag = '1';
    pTable->stmtInsertInfo.resultNum = 0;

    if (MY_SUCCEED != dbStmtInsert(&pTable->stmtInsertInfo))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Insert table[%s] failed, errno: %d, error: %s !\n\n", \
               pTable->tableName, pTable->stmtInsertInfo.nErrNo, pTable->stmtInsertInfo.szErrInfo );
        return MY_FAIL;
    }
    ++pTable->istNum;
    pTable = pTable->pNext;
  }
  ++pSyncData->lValidRec;
  return MY_SUCCEED;
}

int dealSrcFile( PSYNC_DATA pSyncData )
{
    DIR * dirp;
    struct dirent * entry;

    FILE * fpSrc;
    char szInFile[MAX_PATH_LEN + 1];
    struct timeval dwStart;  
    struct timeval dwEnd;  
    unsigned long dwTime=0;  

    dirp = ( opendir( (pSyncData->syncCfg).szInDir ) );
    if( NULL == dirp )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Opened input file directory [%s] failed!\n", (pSyncData->syncCfg).szInDir );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    while( NULL != ( entry = readdir( dirp ) ) )
    {
        if (0 == strcmp(entry->d_name, "..")  || 0 == strcmp(entry->d_name, "."))
        {
            continue;
        }

        gettimeofday(&dwStart,NULL);

        if( PROC_CTRL_RUN != getProcCtrlFlag( pSyncData ) )
        {
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "The system control flag was not set 1, it not allowed to run !\n\n" );
            closedir( dirp );
            freeMemoryData( pSyncData );
            dbDisconnect( pSyncData );
            exit(0);
        }
        tmEndTime = time( NULL );
        tmEndTime = tmEndTime - tmEndTime % (pSyncData->syncCfg).nInterval + (pSyncData->syncCfg).nInterval;

        if( tmEndTime > tmStartTime )
        {
            tmStartTime = tmEndTime;

            if( MY_SUCCEED != switchLogFile( pSyncData ) )
            {
                freeMemoryData( pSyncData );
                dbDisconnect( pSyncData );
                exit(0);
            }
        }

        memset( pSyncData->szStartTime, 0, sizeof( pSyncData->szStartTime ) );
        getMicroSecondTime( pSyncData->szStartTime );

        memset( szInFile, 0, sizeof( szInFile ) );
        memset( pSyncData->szFileName, 0, sizeof( pSyncData->szFileName ) );

        strcpy( pSyncData->szFileName, entry->d_name );
        sprintf( szInFile, "%s/%s", pSyncData->syncCfg.szInDir, pSyncData->szFileName );

        if( !isSyncFile( pSyncData, entry->d_name ) )
        {
            char szRejFile[MAX_PATH_LEN+1];
            szRejFile[0] = 0;
            sprintf( szRejFile, "%s/%s", pSyncData->syncCfg.szRejDir, pSyncData->szFileName );
            if( MY_SUCCEED != moveFile( szInFile, szRejFile ) )
            {
                MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Move file: %s to reject directory: %s failed!\n", szInFile, pSyncData->syncCfg.szRejDir );
                return MY_FAIL;
            }
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Reject file: %s !\n", szInFile );
            continue;
        }

        MyLog( LOG_SOURCE_APP, LOG_LEVEL_DEBUG, "Deal file [%s] ...\n", szInFile );

        pSyncData->nRecordLen = 0;

        fpSrc = fopen( szInFile, "r" );
        if( NULL == fpSrc )
        {
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Opened Input file [%s] failed!\n", szInFile );
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "ErrorInfo: %d : %s\n", errno, strerror( errno ) );
            closedir( dirp );
            return MY_FAIL;
        }

        memset( pSyncData->szRecord, 0, sizeof( pSyncData->szRecord ) );
        pSyncData->lTotalRec = 0;
        pSyncData->lValidRec = 0;
        pSyncData->lErrorRec = 0;
        while( NULL != fgets( pSyncData->szRecord, sizeof( pSyncData->szRecord ), fpSrc ) )
        {
            pSyncData->nRecordLen = strlen( pSyncData->szRecord );
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_DEBUG, "Process record: [%s]\n", pSyncData->szRecord );
            ++pSyncData->lTotalRec;
            if ( 1 == pSyncData->lTotalRec )
            {
                ++pSyncData->lErrorRec;
                continue;
            }
            TrimLeft(TrimRight(pSyncData->szRecord));
            if( 0 == pSyncData->szRecord[0] )
            {
                ++pSyncData->lErrorRec;
                continue;
            }
            memset( pSyncData->szErrCode, 0, sizeof( pSyncData->szErrCode ) );

            strcpy( pSyncData->szErrCode, ERROR_CODE_NORMAL );
            
            if( MY_SUCCEED != dealRecord( pSyncData ) )
            {
                closedir( dirp );
                return MY_FAIL;
            }

            if( 0 != strcmp( pSyncData->szErrCode, ERROR_CODE_NORMAL ) )
            {
                if( MY_SUCCEED != writeErrRecord( pSyncData ) )
                {
                    MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Write error record: %s failed!\n", pSyncData->szRecord);
                    return MY_FAIL;
                }
                
            }

            memset( pSyncData->szRecord, 0, sizeof( pSyncData->szRecord ) );
        }

        gettimeofday(&dwEnd,NULL);
        dwTime = 1000000*(dwEnd.tv_sec-dwStart.tv_sec)+(dwEnd.tv_usec-dwStart.tv_usec);  

        if( NULL != fpSrc )
        {
            fclose( fpSrc );
            fpSrc = NULL;
        }

        memset( pSyncData->szEndTime, 0, sizeof( pSyncData->szEndTime ) );
        getMicroSecondTime( pSyncData->szEndTime );

        if (MY_SUCCEED != putRstRcd(pSyncData->nProcHandle, pSyncData->insertInfo.pHead))
        {
          closedir( dirp );
          return MY_FAIL;
        }
        if( MY_SUCCEED != writeProcLog( pSyncData ) )
        {
            closedir( dirp );
            return MY_FAIL;
        }

        if( MY_SUCCEED != moveAndBakFile( pSyncData ) )
        {
            closedir( dirp );
            return MY_FAIL;
        }

    }
    closedir( dirp );
    return MY_SUCCEED;
}
