
#include "syncdef.h"
#include "syncutil.h"
#include <string.h>


/**
 * Connect to a database instance.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */


int dbConnect(PSYNC_DATA pSyncData)
{

    if (MY_SUCCEED != dbStmtConnect( &(pSyncData->insertInfo.dci )) )
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Create connection to database failed!\n");
        return MY_FAIL;
    }
    return MY_SUCCEED;
}



/**
 * Prepare sql statements used in the system.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */
/*
int dbPrepare(PSYNC_DATA pSyncData)
{

    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);
    PDCI pDci = &(pSyncData->dciInfo.dci);

    if (MY_SUCCEED != dbStmtPrepare( pDci, &( pDciInfo->stmtGetConfNo)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtGetConfNo, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &( pDciInfo->stmtNewUser)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtNewUser, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtGetContact)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtGetContact, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }

    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtImpContact)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtImpContact, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtGetCid)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtGetCid, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtImpCid)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtImpCid, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtGetLastId)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtGetLastId, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }
    
    if (MY_SUCCEED != dbStmtPrepare( pDci, &(pDciInfo->stmtGetCtrlFlag)))
    {

        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SQL statement prepare failed for stmtGetCtrlFlag, errno: %d, error: %s !\n",  pDci->nErrNo, pDci->szErrInfo );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}
*/
/**
 *重写dbPrepare方法。
 *@autor:vince_lee at 20131111
 */
int dbPrepare(PSYNC_DATA pSyncData)
{
  PDCI pDci = &pSyncData->insertInfo.dci;
  typeTableInfo *pTabTmp = pSyncData->insertInfo.pHead;
  while(pTabTmp)
  {
    if (MY_SUCCEED != dbStmtPrepare(pDci, &pTabTmp->stmtSelectSql))
    {
      MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "SELECT SQL statement prepare failed for tab[%s], errno: %d, error: %s !\n", pTabTmp->tableName, pDci->nErrNo, pDci->szErrInfo );
      return MY_FAIL;
    }
    if (MY_SUCCEED != dbStmtPrepare(pDci, &pTabTmp->stmtInsertInfo))
    {
      MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "INSERT SQL statement prepare failed for tab[%s], errno: %d, error: %s !\n", pTabTmp->tableName, pDci->nErrNo, pDci->szErrInfo );
      return MY_FAIL;
    }
    pTabTmp = pTabTmp->pNext;
  }
  return MY_SUCCEED;
}


/**
 * Bind the input parameters, bind result buffer,  execute the query and get affected rows.
 *
 * @param MYSQL_STMT* stmt	The statement handle.
 * @param MYSQL_BIND* pParamBind The input bind parameter.
 * @param MYSQL_BIND* pResultBind The output bind.
 * @param char bufferFlag The flag determines whether the result should be buffered in the client end.
 * @param my_ulonglong* rowNum The affected rows number.
 * @return MY_SUCCEED|MY_FAIL
 */

int dbQuery( DCI_SQL* pDciSql)
{

    if ( MY_SUCCEED != dbStmtQuery( pDciSql ) )
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Execute sql: %s failed, errno: %d, error: %s !\n", pDciSql->szSql, pDciSql->nErrNo, pDciSql->szErrInfo);
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Fetch the result.
 *
 * @param MYSQL_STMT *stmt The statement that fetch 
 * @return MY_SUCCEED|MY_FAIL
 */

int dbFetch( DCI_SQL* pDciSql)
{
    if ( MY_SUCCEED != dbStmtFetch( pDciSql ) )
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Fetch result: %s failed, errno: %d, error: %s !\n", pDciSql->szSql, pDciSql->nErrNo, pDciSql->szErrInfo);
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Bind the input parameters, bind result buffer,  execute the query and get affected rows.
 *
 * @param PSYNC_DATA pSyncData The global billing data.
 * @return MY_SUCCEED|MY_FAIL
 */

/*
int dbDisconnect(PSYNC_DATA pSyncData)
{
    PDCI_INFO pDciInfo = &(pSyncData->dciInfo);

    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtGetConfNo)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetConfNo, errno: %d, error: %s !\n",  pDciInfo->stmtGetConfNo.nErrNo, pDciInfo->stmtGetConfNo.szErrInfo );
    }
    
    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtNewUser)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetUser, errno: %d, error: %s !\n",  pDciInfo->stmtNewUser.nErrNo, pDciInfo->stmtNewUser.szErrInfo );
    }

    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtGetContact)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetPriceInst, errno: %d, error: %s !\n", pDciInfo->stmtGetContact.nErrNo, pDciInfo->stmtGetContact.szErrInfo );
    }

    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtImpContact)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtImpContact, errno: %d, error: %s !\n", pDciInfo->stmtImpContact.nErrNo, pDciInfo->stmtImpContact.szErrInfo );
    }

    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtGetCid)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetCid, errno: %d, error: %s !\n", pDciInfo->stmtGetCid.nErrNo, pDciInfo->stmtGetCid.szErrInfo );
    }
    
    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtImpCid)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtImpCid, errno: %d, error: %s !\n", pDciInfo->stmtImpCid.nErrNo, pDciInfo->stmtImpCid.szErrInfo );
    }
    
    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtGetLastId)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetLastId, errno: %d, error: %s !\n", pDciInfo->stmtGetLastId.nErrNo, pDciInfo->stmtGetLastId.szErrInfo );
    }

    if (MY_SUCCEED != dbStmtFree(&(pDciInfo->stmtGetCtrlFlag)))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the stmtGetCtrlFlag, errno: %d, error: %s !\n",  pDciInfo->stmtGetCtrlFlag.nErrNo, pDciInfo->stmtGetCtrlFlag.szErrInfo );
    }
    
    return MY_SUCCEED;
}
*/
/**
 *重写dbDisconnect
 *@autor:vince_lee at 20131111
 */
int dbDisconnect(PSYNC_DATA pSyncData)
{
  typeTableInfo *pTabTmp = pSyncData->insertInfo.pHead;
  while(pTabTmp)
  {
    if (MY_SUCCEED != dbStmtFree(&pTabTmp->stmtSelectSql))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the select tab[%s], errno: %d, error: %s !\n", pTabTmp->tableName, pTabTmp->stmtSelectSql.nErrNo, pTabTmp->stmtSelectSql.szErrInfo );
    }
    if (MY_SUCCEED != dbStmtFree(&pTabTmp->stmtInsertInfo))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_WARN, "failed while closing the insert tab[%s], errno: %d, error: %s !\n", pTabTmp->tableName, pTabTmp->stmtInsertInfo.nErrNo, pTabTmp->stmtInsertInfo.szErrInfo );
    }
    pTabTmp = pTabTmp->pNext;
  }
  return MY_SUCCEED;
}

/**
 * Get process running control flag.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

/*
int getProcCtrlFlag( PSYNC_DATA pSyncData)
{

    char  operator_id[10];
    memset(operator_id,0, sizeof(operator_id));

    PDCI_SQL pDciSql = &(pSyncData->dciInfo.stmtGetCtrlFlag);
    pDciSql->bufferFlag = '1';
    pDciSql->paramType[0] = DCI_TYPE_STRING;

    pDciSql->param[0] = pSyncData->syncCfg.szCtrlFlag;
    pDciSql->paramNum = 1;
    pDciSql->paramLen[0] = strlen(pSyncData->syncCfg.szCtrlFlag);

    pDciSql->resFieldNum = 1;

    pDciSql->resultNum = 0;

    pDciSql->resultType[0] = DCI_TYPE_STRING;

    if (MY_SUCCEED != dbStmtQuery( pDciSql ))
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_ERROR, "Get process control flag failed, errno: %d, error: %s !\n\n", pDciSql->nErrNo, pDciSql->szErrInfo );
        return MY_FAIL;
    }

    if ( MY_SUCCEED != dbStmtFetch(pDciSql))
    {
        return MY_FAIL;
    }

    strcpy(operator_id, pDciSql->resultBuff[0]);

    if( NULL == operator_id )
    {
        MyLog( LOG_SOURCE_DB, LOG_LEVEL_CRITICAL, "Get process control flag: [%s] failed!\n\n", pSyncData->syncCfg.szCtrlFlag );
        return PROC_CTRL_EXIT;
    }
    else
    {
        return atoi(  operator_id );
    }
}
*/
int getProcCtrlFlag( PSYNC_DATA pSyncData)
{
  return PROC_CTRL_RUN;
}