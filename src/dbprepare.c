#include "dbprepare.h"

/**
 * Connect to a database instance.
 *
 * @param PBILL_DATA pBillData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int dbStmtConnect(DCI* pDci)
{
    if (0 == strcmp(pDci->szDbType, "mysql"))
    {
        pDci->dbHandle = mysql_init((MYSQL* )pDci->dbHandle);
        if (NULL == pDci->dbHandle )
        {
            strcpy(pDci->szErrInfo, "mysql initializtion failed!");
            return MY_FAIL;
        }
        pDci->dbHandle = mysql_real_connect(pDci->dbHandle, pDci->szDbServer, 
                pDci->szUserName, pDci->szPassWord, 
                pDci->szDbName, pDci->nPort , NULL, 0);
        if (NULL == pDci->dbHandle )
        {
            strcpy(pDci->szErrInfo, "mysql connect failed!");
            return MY_FAIL;
        }

        my_bool reconnect = 1;
        mysql_options(pDci->dbHandle, MYSQL_OPT_RECONNECT, &reconnect);

        if (!mysql_set_character_set(pDci->dbHandle, "utf8"))
        {
            mysql_character_set_name(pDci->dbHandle);
        }
    }
    else
    {
        strcpy(pDci->szErrInfo, "database type is not supported!");
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

int dbStmtInit( DCI* pDci, DCI_SQL* pDciSql )
{
    pDciSql->hStmt = mysql_stmt_init((MYSQL*)pDci->dbHandle);
    if (NULL == pDciSql )
    {
        strcpy(pDci->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDci->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }
    
    return MY_SUCCEED;
}

/**
 * Checks whether the connection to the server is working. 
 * If the connection has gone down and auto-reconnect is enabled an attempt to reconnect is made.
 *
 * @param DCI* pDci
 * @return MY_SUCCEED|MY_FAIL
 */
int dbPing( DCI* pDci )
{
    mysql_ping( (MYSQL*)pDci->dbHandle );
    return MY_SUCCEED;
}

/**
 * Prepare sql statements used in the system.
 *
 * @param PBILL_DATA pBillData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int dbStmtPrepare(DCI* pDci, DCI_SQL* pDciSql)
{
    //memset(pDciSql, 0, sizeof(pDciSql));
    memset(pDciSql->param,0, sizeof(pDciSql->param));
    memset(pDciSql->resultBuff, 0, sizeof(pDciSql->resultBuff));
    pDciSql->hStmt = mysql_stmt_init((MYSQL*)pDci->dbHandle);
    if (NULL == pDciSql )
    {
        strcpy(pDci->szErrInfo, mysql_stmt_error((MYSQL_STMT *)pDciSql->hStmt ));
        pDci->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    int err = -1;
    err = mysql_stmt_prepare(pDciSql->hStmt , pDciSql->szSql, strlen(pDciSql->szSql));
    if (0 != err)
    {
        strcpy(pDci->szErrInfo, mysql_stmt_error(pDciSql->hStmt) );
        pDci->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);

        if (0 == pDci->nErrNo)
        {
            pDci->nErrNo = err;
        }

        return MY_FAIL;
    }
    return MY_SUCCEED;

}

/**
 * Bind the input parameters, execute the insert statement.
 *
 * @param DCI_SQL* pDciSql	The sql statement structure.
 * @return MY_SUCCEED|MY_FAIL
 */

int dbStmtInsert( DCI_SQL* pDciSql )
{
    int nCount=0;
    memset(pDciSql->paramBind, 0, sizeof(pDciSql->paramBind));
    memset(pDciSql->resultBind, 0, sizeof(pDciSql->resultBind));
    /*
       MYSQL_BIND** pBind;
       pBind = (MYSQL_BIND**)malloc(sizeof(MYSQL_BIND*)*(pDciSql->paramNum));
       */
    for ( nCount=0; nCount<pDciSql->paramNum; nCount++ )
    {
        //pBind[nCount] = (MYSQL_BIND*) malloc(sizeof(MYSQL_BIND));
        switch ( pDciSql->paramType[nCount] ) 
        {
            case DCI_TYPE_STRING:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_STRING;
                pDciSql->paramBind[nCount].length= &(pDciSql->paramLen[nCount]);
                break;
            case DCI_TYPE_LONG:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_LONG;
                pDciSql->paramBind[nCount].length = &(pDciSql->paramLen[nCount]);
                break;
            case DCI_TYPE_FLOAT:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_FLOAT;
                pDciSql->paramBind[nCount].length = &(pDciSql->paramLen[nCount]);
                break;
            default:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_STRING;
                pDciSql->paramBind[nCount].length= &(pDciSql->paramLen[nCount]);
                break;
        }

        pDciSql->paramBind[nCount].buffer= (char*) pDciSql->param[nCount];
        //strcpy(pDciSql->paramBind[nCount].buffer, pDciSql->param[nCount]);
        pDciSql->paramBind[nCount].is_null= 0;

    }

    char bErr;
    if (NULL != pDciSql->paramBind)
    {
        bErr = mysql_stmt_bind_param( (MYSQL_STMT *)pDciSql->hStmt, pDciSql->paramBind );
        if (0 != bErr)
        {
            strcpy(pDciSql->szErrInfo, mysql_stmt_error((MYSQL_STMT *)pDciSql->hStmt ));
            pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
            return MY_FAIL;
        }
    }

    int err = mysql_stmt_execute( pDciSql->hStmt );
    if (0 != err)
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }
    pDciSql->resultNum = mysql_stmt_affected_rows( pDciSql->hStmt );

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

int dbStmtQuery( DCI_SQL* pDciSql )
{
    int nCount=0;
    memset(pDciSql->paramBind, 0, sizeof(pDciSql->paramBind));
    memset(pDciSql->resultBind, 0, sizeof(pDciSql->resultBind));
    /*
       MYSQL_BIND** pBind;
       pBind = (MYSQL_BIND**)malloc(sizeof(MYSQL_BIND*)*(pDciSql->paramNum));
       */
    for ( nCount=0; nCount<pDciSql->paramNum; nCount++ )
    {
        //pBind[nCount] = (MYSQL_BIND*) malloc(sizeof(MYSQL_BIND));
        switch ( pDciSql->paramType[nCount] ) 
        {
            case DCI_TYPE_STRING:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_STRING;
                pDciSql->paramBind[nCount].length= &(pDciSql->paramLen[nCount]);
                break;
            case DCI_TYPE_LONG:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_LONG;
                pDciSql->paramBind[nCount].length = &(pDciSql->paramLen[nCount]);
                break;
            default:
                pDciSql->paramBind[nCount].buffer_type = MYSQL_TYPE_STRING;
                pDciSql->paramBind[nCount].length= &(pDciSql->paramLen[nCount]);
                break;
        }

        pDciSql->paramBind[nCount].buffer= (char*) pDciSql->param[nCount];
        //strcpy(pDciSql->paramBind[nCount].buffer, pDciSql->param[nCount]);
        pDciSql->paramBind[nCount].is_null= 0;

    }

    char bErr;
    if (NULL != pDciSql->paramBind)
    {
        bErr = mysql_stmt_bind_param( (MYSQL_STMT *)pDciSql->hStmt, pDciSql->paramBind );
        if (0 != bErr)
        {
            strcpy(pDciSql->szErrInfo, mysql_stmt_error((MYSQL_STMT *)pDciSql->hStmt ));
            pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
            return MY_FAIL;
        }
    }

    int err = mysql_stmt_execute( pDciSql->hStmt );
    if (0 != err)
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    for ( nCount=0; nCount<pDciSql->resFieldNum; nCount++ )
    {
        switch ( pDciSql->resultType[nCount] )
        {
            case DCI_TYPE_STRING:
                pDciSql->resultBind[nCount].buffer_type = MYSQL_TYPE_STRING;
                break;
            case DCI_TYPE_LONG:
                pDciSql->resultBind[nCount].buffer_type = MYSQL_TYPE_LONG;
                break;
            default:
                pDciSql->resultBind[nCount].buffer_type = pDciSql->resultType[nCount];
                break;
        }

        pDciSql->resultBind[nCount].buffer= &(pDciSql->resultBuff[nCount]);
        pDciSql->resultBind[nCount].buffer_length= sizeof(pDciSql->resultBuff[nCount]);
        pDciSql->resultBind[nCount].length= &(pDciSql->real_length[nCount]);

    }

    err = -1;
    err = mysql_stmt_bind_result( pDciSql->hStmt, pDciSql->resultBind );
    if (0 != err)
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    //Buffer all the result in mysql client end.
    err = -1;
    if (pDciSql->bufferFlag == '1')
    {
        err = mysql_stmt_store_result( pDciSql->hStmt );
        if (0 != err)
        {
            strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
            pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
            return MY_FAIL;
        }
    }

    pDciSql->resultNum = mysql_stmt_num_rows( pDciSql->hStmt );
    return MY_SUCCEED;
}


/**
 * Fetch the result.
 *
 * @param MYSQL_STMT *stmt The statement that fetch 
 * @return MY_SUCCEED|MY_FAIL
 */

int dbStmtFetch( DCI_SQL* pDciSql )
{
    int err = -1;
    err = mysql_stmt_fetch( pDciSql->hStmt );
    if ( 0 != err && 101 != err )
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    return MY_SUCCEED;
}



int dbAutoCommit( DCI* pDci, int nMode )
{
    int err = -1;
    my_bool mode = nMode;
    err = mysql_autocommit( (MYSQL* )pDci->dbHandle, mode );
    if ( 0 != err )
    {   
        pDci->nErrNo =  mysql_errno( (MYSQL* )pDci->dbHandle );
        pDci->szErrInfo, mysql_error( (MYSQL* )pDci->dbHandle );
        
        return MY_FAIL;
    }   

    return MY_SUCCEED;
}

/**
 * Bind the input parameters, bind result buffer,  execute the query and get affected rows.
 *
 * @param PBILL_DATA pBillData The global billing data.
 * @return MY_SUCCEED|MY_FAIL
 */


int dbStmtFree( DCI_SQL* pDciSql )
{
    if (mysql_stmt_free_result(pDciSql->hStmt))
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    if (mysql_stmt_close(pDciSql->hStmt))
    {
        strcpy(pDciSql->szErrInfo, mysql_stmt_error(pDciSql->hStmt ));
        pDciSql->nErrNo =  mysql_stmt_errno(pDciSql->hStmt);
        return MY_FAIL;
    }

    pDciSql->hStmt= NULL;

    return MY_SUCCEED;
}


int dbStmtClose(DCI* pDci)
{
    mysql_close(pDci->dbHandle);
    pDci->dbHandle = NULL;
    return MY_SUCCEED;
}
