
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
 * dbPrepare
 * vince_lee at 20131111
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
 * dbDisconnect
 * vince_lee at 20131111
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

int getProcCtrlFlag( PSYNC_DATA pSyncData)
{
  return PROC_CTRL_RUN;
}