#include "syncutil.h"
#include "syncdef.h"
#include "public.h"   //add by vince_lee


void helpInfo(  )
{
    printf( "\n\n" );

    printf( "    [usage]: %s [-i Inifile]\n\n", szBinName );

    printf( "             Optional parameter: \n\n" );

    printf( "             нч\n" );

    printf( "\n" );

    printf( "             Optional parameter: \n\n" );

    printf( "             If specified -i , the dedicate parameter will act as the system configuration file.\n" );
    printf( "             If -i is not specified, the config.ini file in the same directory as  executable file will act as the default system configuration file.\n" );

    printf( "\n\n" );
}

int getCmd( PSYNC_DATA pSyncData, int argc, char * argv[] )
{
    PSYNC_CFG pSyncCfg;

    extern char * optarg;
    int opt;

    pSyncCfg = &(pSyncData->syncCfg);

    memset( szDirName, 0, sizeof( szDirName ) );
    memset( szBinName, 0, sizeof( szBinName ) );

    getIniInfo( szDirName, szBinName, argv[0] );

    sprintf( pSyncCfg->szIniFile, "%s/%s.ini", szDirName, szBinName );

    while( -1 != ( opt = getopt( argc, argv, "I:i:HhVv" ) ) )
    {
        switch( opt )
        {
            case 'I':
            case 'i':
                strcpy( pSyncCfg->szIniFile, optarg );
                break;

            case 'H':
            case 'h':
                helpInfo();
                exit(0);

                break;

            case 'V':
            case 'v':
                fprintf( stderr, "\n\t Database Customer information Sync %s\n\n", szBinName );
                exit(0);

            default:
                printf( "Not valid parameter %c !\n", opt );
                exit(0);
                break;
        }
    }

    return MY_SUCCEED;
}

/**
 * Get the sql statement from syncsql.ini
 *
 * @param PSYNC_DATA pSyncData The global sync data.
 * @return MY_SUCCEED|MY_FAIL
 */
/*
int getSqlStmt(PSYNC_DATA pSyncData)
{
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "GetConfNo", pSyncData->dciInfo.stmtGetConfNo.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT GetConfNo is not configured !\n\n" );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "ImportNewUser", pSyncData->dciInfo.stmtNewUser.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT ImportNewUser is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "GetContact", pSyncData->dciInfo.stmtGetContact.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT GetContact is not configured !\n\n" );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "ImportNewContact", pSyncData->dciInfo.stmtImpContact.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT ImportNewContact is not configured !\n\n" );
        return MY_FAIL;
    }
    
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "GetCid", pSyncData->dciInfo.stmtGetCid.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT GetCid is not configured !\n\n" );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "ImportNewCid", pSyncData->dciInfo.stmtImpCid.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT ImportNewCid is not configured !\n\n" );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "GetLastId", pSyncData->dciInfo.stmtGetLastId.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT GetLastId is not configured !\n\n" );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "SQL", "GetCtlrFlag", pSyncData->dciInfo.stmtGetCtrlFlag.szSql ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[SQL] SQLSTMT GetCtlrFlag is not configured !\n\n" );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}
*/
/**
 *get insert table info list from syncsql.ini
 *add by liwq at 20131111
 */
int getInsertInfo(PSYNC_DATA pSyncData)
{
  if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, "insertlist", "list", pSyncData->insertInfo.tableList) )
  {
    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[insertlist] list is not configured !\n\n" );
    return MY_FAIL;
  }
  pSyncData->insertInfo.pHead = NULL;
  pSyncData->insertInfo.tableNum = getstrnum(pSyncData->insertInfo.tableList, "|") + 1;
  #ifdef _vince_
    printf("table list:%s\n", pSyncData->insertInfo.tableList);
    printf("tab list count:%d\n", pSyncData->insertInfo.tableNum);
  #endif //_vince_
  int i = 0;
  for (i=0; i<pSyncData->insertInfo.tableNum; ++i)
  {
    typeTableInfo *pTableInfo = malloc(sizeof(typeTableInfo));
    if (NULL == pTableInfo)
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN,"malloc typeTableInfo return err\n\n");
      return MY_FAIL;
    }
    memset(pTableInfo, 0x00, sizeof(typeTableInfo));

    pTableInfo->istNum = 0;
    getsubstr(pSyncData->insertInfo.tableList, '|', i+1, pTableInfo->tableName);
    #ifdef _vince_
      printf("table:%s\n", pTableInfo->tableName);
    #endif
    
    char tmpTableName[128] = {0};
    sprintf(tmpTableName, "%d:%s", i, pTableInfo->tableName);
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, tmpTableName, "chk", pTableInfo->chk) )
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[%s] chk is not configured !\n\n",  tmpTableName);
      return MY_FAIL;
    }
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, tmpTableName, "chksql", pTableInfo->chkSql) )
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[%s] chksql is not configured !\n\n",  tmpTableName);
      return MY_FAIL;
    }
    strcpy(pTableInfo->stmtSelectSql.szSql, pTableInfo->chkSql);

    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, tmpTableName, "sqlstr", pTableInfo->sqlStr) )
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[%s] sqlstr is not configured !\n\n",  tmpTableName);
      return MY_FAIL;
    }
    strcpy(pTableInfo->stmtInsertInfo.szSql ,pTableInfo->sqlStr);
    if( MY_SUCCEED != getConfigParaValue( pSyncData->syncCfg.szSqlFileName, tmpTableName, "map", pTableInfo->map) )
    {
      MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[%s] map is not configured !\n\n",  tmpTableName);
      return MY_FAIL;
    }
    pTableInfo->pNext = pSyncData->insertInfo.pHead;
    pSyncData->insertInfo.pHead = pTableInfo;
  }
  return MY_SUCCEED;
}

/**
 * Initialize the synchronization global data.
 *
 * @param PSYNC_DATA pSyncData The global synchronization data.
 * @return MY_SUCCEED|MY_FAIL
 */

int initSyncData(PSYNC_DATA pSyncData)
{
    memset(pSyncData, 0, sizeof(SYNC_DATA));
    memset(&(pSyncData->syncCfg), 0, sizeof(SYNC_CFG));

    pSyncData->szRecArr = (char**) malloc(MAX_RECORD_NUM * sizeof(char*));
    int loop;
    for (loop = 0; loop< MAX_RECORD_NUM; ++loop)
    {
        pSyncData->szRecArr[loop] = (char*) malloc(MAX_FIELD_LEN * sizeof(char));
        memset(pSyncData->szRecArr[loop], 0, MAX_FIELD_LEN);
    }
    
    pSyncData->szFiled = (char**) malloc(MAX_RECORD_NUM * sizeof(char*));

    for (loop = 0; loop< MAX_RECORD_NUM; ++loop)
    {
        pSyncData->szFiled[loop] = (char*) malloc(MAX_FIELD_LEN * sizeof(char));
        memset(pSyncData->szFiled[loop], 0, MAX_FIELD_LEN);
    }
    
    //memset(&(pSyncData->custInfo), 0, sizeof(pSyncData->custInfo));
    memset(&(pSyncData->insertInfo), 0, sizeof(pSyncData->insertInfo));
    return 0;
}
/**
 * Get configure information.
 *
 * @param char*  szFormatFileName The control file name.
 * @return MY_SUCCEED|MY_FAIL
 */


int getConfig( PSYNC_DATA pSyncData )
{
    char szValue[128 + 1];
    PSYNC_CFG pSyncCfg =  &(pSyncData->syncCfg);
    //PDCI pDci = &(pSyncData->dciInfo.dci);
    PDCI pDci = &(pSyncData->insertInfo.dci);

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "RUNLOG", pSyncCfg->szRunLog ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] RUNLOG is not configured !\n\n" );
        return MY_FAIL;
    }

    //Verify run log directory
    if( ! isDir( pSyncCfg->szRunLog ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Invalid run log directory: %s !\n", pSyncCfg->szRunLog );
        return MY_FAIL;
    }

    strcpy( szLogPath, pSyncCfg->szRunLog );
    strcpy( szLogPrefix, "billing" );

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "LOGDIR", pSyncCfg->szProcLog ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] LOGDIR is not configured !\n\n" );
        return MY_FAIL;
    }

    // Verify process log directory
    if( ! isDir( pSyncCfg->szProcLog ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Invalid process log directory: %s !\n", pSyncCfg->szProcLog );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "LOGTMP", pSyncCfg->szLogTmp ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] LOGTMP is not configured !\n\n" );
        return MY_FAIL;
    }

    //Verify temporary log directory
    if( ! isDir( pSyncCfg->szLogTmp ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Invlid temporary log directory: %s !\n", pSyncCfg->szLogTmp);
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "INDIR", pSyncCfg->szInDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] INDIR is not configured !\n\n" );
        return MY_FAIL;
    }

    //Verify source directory
    if( ! isDir( pSyncCfg->szInDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Invalid source directory!\n" );
        return MY_FAIL;
    }

    memset( szSrcDir, 0, sizeof( szSrcDir ) );

    getIniInfo( szValue, szSrcDir, pSyncCfg->szInDir );

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "REJDIR", pSyncCfg->szRejDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] REJDIR is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DIRECTORY", "BAKDIR", pSyncCfg->szBakDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DIRECTORY] BAKDIR is not configured !\n\n" );
        return MY_FAIL;
    }

    //Verify backup directory
    if( ! isDir( pSyncCfg->szBakDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Invalid source file backup directory!\n" );
        return MY_FAIL;
    }


    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "CONFIG", "SQLSTMT", pSyncCfg->szSqlFileName ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONFIG] SQLSTMT is not configured !\n\n" );
        return MY_FAIL;
    }


    if( ! fileExists( pSyncCfg->szSqlFileName ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Sql file name: %s doesn't exist!\n\n", pSyncCfg->szSqlFileName );
        return MY_FAIL;
    }
    
    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "CONFIG", "FIELDSEP", pSyncCfg->szFieldSep ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONFIG] FIELDSEP is not configured !\n\n" );
        return MY_FAIL;
    }


//-----modify by vince_lee at 20131111 b ------
//    if ( MY_SUCCEED !=getSqlStmt(pSyncData))
//    {
//        return MY_FAIL;
//    }

    if (MY_SUCCEED != getInsertInfo(pSyncData))
    {
      return MY_FAIL;
    }
//-----modify by vince_lee at 20131111 e ------

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DATABASE", "DBTYPE", pDci->szDbType ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] DBTYPE is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DATABASE", "DBSERVER", pDci->szDbServer ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] DBSERVER is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DATABASE", "DBNAME", pDci->szDbName ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] DBNAME is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DATABASE", "DBPORT", szValue ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] DBPORT is not configured !\n\n" );
        return MY_FAIL;
    }
    if ( !isNum( szValue ))
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] DBPORT is invalid !\n\n" );
        return MY_FAIL;
    }

    pDci->nPort = atoi( szValue );	

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "DATABASE", "LOGINFILE", pSyncCfg->szLoginFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[DATABASE] LOGINFILE is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getLoginInfo( pSyncCfg->szLoginFile, pDci->szUserName, pDci->szPassWord ) )
    {
        return MY_FAIL;
    }

    memset(szValue, 0, sizeof(szValue));

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "CONTROL", "LOGLEVEL", szValue ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONTROL] CTRLFLAG is not configured !\n\n" );
        return MY_FAIL;
    }

    if (0 == strcasecmp(szValue, LOG_LEVEL_DEBUG)) 
    {
        gLogLevel = 1;
    }
    else if (0 == strcasecmp(szValue, LOG_LEVEL_WARN))
    {
        gLogLevel = 2;
    }
    else if (0 == strcasecmp(szValue, LOG_LEVEL_ERROR))
    {
        gLogLevel = 3;
    }
    else if (0 == strcasecmp(szValue, LOG_LEVEL_CRITICAL))
    {
        gLogLevel = 4;
    }	

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "CONTROL", "CTRLFLAG", pSyncCfg->szCtrlFlag ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONTROL] CTRLFLAG is not configured !\n\n" );
        return MY_FAIL;
    }

    if( MY_SUCCEED != getConfigParaValue( pSyncCfg->szIniFile, "CONTROL", "LOGBATCH", szValue ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONTROL] CTRLFLAG is not configured !\n\n" );
        return MY_FAIL;
    }

    if( ! isNum( szValue ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "[CONTROL] LOGBATCH %s illegal !\n\n", szValue );
        return MY_FAIL;
    }

    pSyncCfg->nInterval = atoi( szValue );

    if( 0 >= pSyncCfg->nInterval )
    {
        pSyncCfg->nInterval = 900;
    }
    else
    {
        pSyncCfg->nInterval = ( pSyncCfg->nInterval + 60 - 1 ) / 60 * 60;
    }

    //Verify process log directory and temporary log directory are in the same file system.
    if( ! isSameFileSystem( pSyncCfg->szProcLog, pSyncCfg->szLogTmp ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Process log directory and temporary log directory are not in the same file system.!\n\n" );
        return MY_FAIL;
    }

    //Verify running log directory and temporary log directory are in the same file system.
    if( ! isSameFileSystem( pSyncCfg->szRunLog, pSyncCfg->szLogTmp ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Running log directory and temporary log directory are not in the same file system!\n\n" );
        return MY_FAIL;
    }

    //Verify source directory and backup directory are in the same file system.
    if( ! isSameFileSystem( pSyncCfg->szInDir, pSyncCfg->szBakDir ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Source directory and backup directory are not in the same file system!\n\n" );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}


int main(int argc,char *argv[])
{
    SYNC_DATA syncData;
    initSyncData(&syncData);
    
    if ( MY_SUCCEED != getCmd(&syncData, argc, argv))
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "Get system parameters failed!\n\n" );
        freeMemoryData( &syncData );
        exit(0);
    }
    
    if ( MY_SUCCEED != getConfig(&syncData))
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "Get system configuration parameters failed!\n\n" );
        freeMemoryData( &syncData );
        exit(0);
    }
    
    if( MY_SUCCEED != dbConnect(&syncData ))
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "Create connection to database failed!\n\n" );
        freeMemoryData( &syncData );
        exit(0);
    }

    if( MY_SUCCEED != dbPrepare(&syncData ))
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "Database prepare statement failed!\n\n" );
        freeMemoryData( &syncData );
        dbDisconnect( &syncData);
        exit(0);
    }
    

    if( PROC_CTRL_RUN != getProcCtrlFlag( &syncData ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "The system running control flag is set to 1, forbbiden to run!\n\n" );
        freeMemoryData( &syncData );
        dbDisconnect( &syncData);

        exit(0);
    }


    tmStartTime = time( NULL );
    tmStartTime = tmStartTime - tmStartTime % (syncData.syncCfg).nInterval + (syncData.syncCfg).nInterval;

    getCurrBatch( &syncData );

    if( MY_SUCCEED != openLogFile( &syncData ) )
    {
        freeMemoryData( &syncData );
        dbDisconnect( &syncData );
        exit(0);
    }

    MyLog( LOG_SOURCE_APP, LOG_LEVEL_DEBUG, "Billing the cdr in the direcotry: %s !\n\n", syncData.syncCfg.szInDir );
    
    int runTimes = 1;
    while( runTimes-- )
    {
        // Deal the file under the input direcotry
        if( MY_SUCCEED != dealSrcFile( &syncData ) )
        {
            if (MY_SUCCEED != moveErrFile( &syncData ))
            {
                freeMemoryData( &syncData );
                dbDisconnect( &syncData );
                exit(0);
            }
            MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Billing file %s  failed!\n\n", syncData.szFileName );
        }

        tmEndTime = time( NULL );
        tmEndTime = tmEndTime - tmEndTime % (syncData.syncCfg).nInterval + (syncData.syncCfg).nInterval;

        if( tmEndTime > tmStartTime )
        {
            tmStartTime = tmEndTime;
            // Switch process and error log files. 
            if( MY_SUCCEED != switchLogFile( &syncData ) )
            {
                MyLog( LOG_SOURCE_APP, LOG_LEVEL_ERROR, "Switch log file failed!\n\n" );
                freeMemoryData( &syncData );
                dbDisconnect( &syncData );
                exit(0);
            }
        }

        sleep( 1 );
    }
    //putRstRcd(syncData.nProcHandle, syncData.insertInfo.pHead);
    freeMemoryData( &syncData );
    dbDisconnect(&syncData);
    printf("====success\n");

}



/**
 * Free system memory.
 *
 * @param PSYNC_DATA pSyncData Global data.
 * @return MY_SUCCEED|MY_FAIL
 */

int freeMemoryData( PSYNC_DATA pSyncData )
{

    if( NULL != pSyncData->fpTmpOut )
    {
        fclose( pSyncData->fpTmpOut );
        pSyncData->fpTmpOut = NULL;
    }
    if( NULL != pSyncData->fpErrLog )
    {
        fclose( pSyncData->fpErrLog );
        pSyncData->fpErrLog = NULL;
    }
    
    if( -1 != pSyncData->nProcHandle )
    {
        close( pSyncData->nProcHandle );
        pSyncData->nProcHandle = -1;
    }

    if( -1 != pSyncData->nErrHandle )
    {
        close( pSyncData->nErrHandle );
        pSyncData->nErrHandle = -1;
    }

    int loop;
    for (loop = 0; loop< MAX_RECORD_NUM; ++loop)
    {
        free(pSyncData->szRecArr[loop]);
        pSyncData->szRecArr[loop] = NULL;
        free(pSyncData->szFiled[loop]);
        pSyncData->szFiled[loop] = NULL;
    }
    free(pSyncData->szRecArr);
    pSyncData->szRecArr = NULL;
    
    return MY_SUCCEED;
}