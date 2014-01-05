#include "syncutil.h"
#include "syncdef.h"
#include <string.h>



/**
 * Get field value by field number.
 *
 * @param char* szSource 	The log source.
 * @param char* szLevel 	The log level including LOG_LEVEL_DEBUG, LOG_LEVEL_WARN, LOG_LEVEL_ERROR, LOG_LEVEL_CRITICAL.
 * @param char* szFmt 	The log format.
 * @return MY_SUCCEED|MY_FAIL
 */


int MyLog( char * szSource, char * szLevel, char * szFmt, ... )
{
    int nLevel = 0;

    if (0 == strcmp(szLevel, LOG_LEVEL_DEBUG)) 
    {
        nLevel = 1;
    }
    else if (0 == strcmp(szLevel, LOG_LEVEL_WARN))
    {
        nLevel = 2;
    }
    else if (0 == strcmp(szLevel, LOG_LEVEL_ERROR))
    {
        nLevel = 3;
    }
    else if (0 == strcmp(szLevel, LOG_LEVEL_CRITICAL))
    {
        nLevel = 4;
    }


    va_list vaList;
    char szBuf[MAX_RECORD_LEN + 1];	
    char szTime[14 + 1];
    FILE * fp;
    memset( szTime, 0, sizeof( szTime ) );
    getCurrTime( szTime );

    if( 0 == *(szSrcDir) )
    {
        sprintf( szBuf, "%s/%s%8.8s.runlog", szLogPath, szLogPrefix, szTime );
    }
    else
    {
        sprintf( szBuf, "%s/%s%8.8s.%s.runlog", szLogPath, szLogPrefix, szTime, szSrcDir );
    }

    if ( nLevel < gLogLevel )
    {
        return MY_SUCCEED;
    }

    #ifdef _vince_
      printf("logFile:%s\n", szBuf);
    #endif //_vince_
    fp = fopen( szBuf, "a+" );
    if( NULL == fp )
    {
        fprintf( stderr, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t%s\n", 
                szTime,
                szTime + 4,
                szTime + 6,
                szTime + 8, 
                szTime + 10, 
                szTime + 12,
                "sys",
                "critical",
                "Can't open log file" );
        exit(1);
    }

    sprintf( szBuf, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t", 
            szTime,
            szTime + 4,
            szTime + 6,
            szTime + 8, 
            szTime + 10, 
            szTime + 12,
            szSource,
            szLevel );

    fprintf( fp, "%s", szBuf );
    printf( szBuf );

    va_start( vaList, szFmt );

    vfprintf( fp, szFmt, vaList );
    va_end( vaList );


    /*
     *We need this because va_list vaList will have an undefined value after the call to vfprintf or vsprintf, 
     *so we use va_start before every related function.
     */

    va_start( vaList, szFmt );
    vsprintf( szBuf, szFmt, vaList );

    printf("%s", szBuf );

    va_end( vaList );

    if( 0 == strcmp( szLevel, LOG_LEVEL_CRITICAL ) )
    {
        sprintf( szBuf, "%4.4s/%2.2s/%2.2s %2.2s:%2.2s:%2.2s\t%s\t%s\t%s\n", 
                szTime,
                szTime + 4,
                szTime + 6,
                szTime + 8, 
                szTime + 10, 
                szTime + 12,
                "sys",
                "critical",
                "Exception encoutered, stop running and exit!" );

        fprintf( fp, "%s", szBuf );
        printf( szBuf );
    }

    fclose( fp );

    return MY_SUCCEED;
}

/**
 * Trim the spaces on the string's left.
 *
 * @param char* szTrim 	The source string.
 * @return char* The string with no left space.
 */


char * TrimLeft( char * szTrim )
{
    char * pLoc;

    pLoc = szTrim;

    while( isspace( (int)*pLoc ) ) pLoc++;
    //strcpy( szTrim, pLoc);
    memmove(szTrim, pLoc, strlen(pLoc));
    szTrim[strlen(pLoc)]=0;
    return szTrim;
}

/**
 * Trim the spaces on the string's right.
 *
 * @param char* szTrim 	The source string.
 * @return char* The string with no right space.
 */

char * TrimRight( char * szTrim )
{
    int nLen;

    nLen = strlen( szTrim );
    if ( nLen <= 0 )
    {
        return szTrim;
    }

    while( nLen > 0 && ( isspace( (int)szTrim[ nLen - 1 ] ) ||  iscntrl( (int)szTrim[ nLen - 1 ] ) ) ) nLen--;

    szTrim[nLen] = '\0';

    return szTrim;
}

/**
 * Trim the 0 on the string's right.
 *
 * @param char* szTrim 	The source string.
 * @return char* The string with no right 0.
 */

char * TrimLeftZero( char * szTrim )
{
    char * pLoc;

    pLoc = szTrim;

    while( (*pLoc) == '0' ) pLoc++;

    if( 0 == *(pLoc) || 0 == strlen( pLoc ) )
    {
        strcpy( szTrim, "0" );
    }
    else
    {
        strcpy( szTrim, pLoc );
    }

    return szTrim;
}

/**
 * Get configure parameter value.
 *
 * @param char* szCfgFile 	The configure file name.
 * @param char* szLabel 	The configure parameter type.
 * @param char* szLeftValue 	The configure parameter name.
 * @param char* szRightValue 	The parameter value.
 * @return MY_SUCCEED|MY_FAIL
 */

int getConfigParaValue( char * szCfgFile, char * szLabel, char * szLeftValue, char * szRightValue )
{
    FILE * fpConfig;
    char szString[MAX_RECORD_LEN + 1];
    char szRecord[MAX_RECORD_LEN + 1];
    char szValue[MAX_RECORD_LEN + 1];
    char sztemp[254+1];
    memset(sztemp,0, sizeof(sztemp));
    char * szPos;
    char bStart;
    memset(szRightValue, 0, sizeof(szRightValue));
    fpConfig = fopen( szCfgFile, "r" );
    if( NULL == fpConfig )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Open Config File %s Failed!\n\n", szCfgFile );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Errorinfo(%d): %s\n", errno, strerror( errno ) );

        return MY_FAIL;
    }

    memset( szRecord, 0, sizeof( szRecord ) );

    bStart = MY_FALSE;

    while( NULL != fgets( szRecord, sizeof( szRecord ), fpConfig ) )
    {
        if( isRecordLine( szRecord ) )
        {
            if( !bStart )
            {
                if( '[' == szRecord[0] && ']' == szRecord[strlen( szRecord ) - 1] )
                {
                    sprintf( szString, "%s", szRecord + 1 );
                    szString[strlen( szString ) - 1] = 0;

                    TrimLeft( TrimRight( szString ) );
                }
                if( 0 == strcmp( szString, szLabel ) )
                {
                    bStart = MY_TRUE;
                    memset( szRecord, 0, sizeof( szRecord ) );
                    continue;
                }
            }
            else
            {
                if( '[' == szRecord[0] && ']' == szRecord[strlen( szRecord ) - 1] )
                {
                    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Get Config [%s] Value [%s] Failed!\n\n", szLabel, szLeftValue );
                    fclose( fpConfig );
                    return MY_FAIL;
                }

                szPos = NULL;
                if( NULL != ( szPos = ( char * ) strchr( szRecord, '=' ) ) )
                {
                    memcpy( szValue, szRecord, szPos - szRecord );
                    szValue[szPos - szRecord] = 0;
                    TrimLeft( TrimRight( szValue ) );
                    if( strcmp( szValue, szLeftValue ) == 0 )
                    {
                        memset(szRightValue, 0, sizeof(szRightValue));
                        strcpy( szRightValue, szPos + 1 );
                        TrimLeft( TrimRight( szRightValue ) );
                        fclose( fpConfig );
                        return MY_SUCCEED;
                    }
                }
            }
        }

        memset( szRecord, 0, sizeof( szRecord ) );
    }

    MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Get Config [%s] Value [%s] Failed!\n\n", szLabel, szLeftValue );

    fclose( fpConfig );

    return MY_FAIL;
}

/**
 * Determine whether the input string is float.
 *
 * @param char* szNum 	The input string.
 * @return MY_FAIL|MY_SUCCEED
 */
int isFloat( char * szNum )
{
    int nFlag;

    nFlag = 0;

    if ( 0 == *(szNum) || 0 == strlen( szNum ) )
    {
        return MY_FALSE;
    }

    while ( *szNum )
    {
        if ( !isdigit( (int)(*szNum) ) )
        {
            if( '.' != (*szNum) || nFlag || 0 == *(szNum + 1) )
            {
                return MY_FALSE;
            }

            nFlag++;
        }

        szNum++;
    }

    return MY_TRUE;
}

/**
 * Determine whether the input string is number.
 *
 * @param char* szNum 	The input string.
 * @return MY_FAIL|MY_SUCCEED
 */

int isNum( char * szNum )
{
    if ( 0 == *(szNum) || 0 == strlen( szNum ) )
    {
        return MY_SUCCEED;
    }

    while ( *szNum )
    {
        if ( isdigit( (int)(*szNum) ) )
        {
            szNum++;
        }
        else
        {
            return MY_FALSE;
        }
    }

    return MY_TRUE;
}

/**
 * Determine whether the input is string.
 *
 * @param char* szNum 	The input parameter.
 * @return MY_FAIL|MY_SUCCEED
 */

int IsStr( char * szStr )
{
    int nOffset;

    nOffset = 0;

    if( 0 == strlen( szStr ) || 0 == *(szStr) )
    {
        return MY_FALSE;
    }

    while( nOffset < strlen( szStr ) )
    {
        if( ( szStr[nOffset] >= 'a' &&  szStr[nOffset] <= 'z' )
                || ( szStr[nOffset] >= 'A' &&  szStr[nOffset] <= 'Z' )
                || ( szStr[nOffset] >= '0' &&  szStr[nOffset] <= '9' )
                || '_' == szStr[nOffset]
          )	
        {
            nOffset++;
        }
        else
        {
            return MY_FALSE;
        }
    }

    return MY_TRUE;
}

/**
 * Determine whether the input number leap year.
 *
 * @param char* szNum 	The year.
 * @return MY_FAIL|MY_SUCCEED
 */

int IsLeapYear( int nYear )
{
    if( ( nYear % 4 == 0 && nYear % 100 != 0 ) || nYear % 400 == 0 )
    {
        return MY_TRUE;
    }
    else
    {
        return MY_FALSE;
    }
}


/**
 * Get file lines number.
 *
 * @param char* szPath 	The file name.
 * @return MY_FAIL|MY_SUCCEED
 */

long getFileCount( char * szPath )
{
    FILE * fp;
    int lCount;
    char szBuf[MAX_RECORD_LEN + 1];

    fp = fopen( szPath, "r" );

    if( NULL == fp )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Open file %s failed!\n", szPath );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
        return -1;
    }

    lCount = 0;

    while( NULL != fgets( szBuf, sizeof( szBuf ), fp ) )
    {
        if( 0 != strncmp( szBuf, FILE_NOTE_FLAG, strlen( FILE_NOTE_FLAG ) ) )
        {
            lCount++;
        }
    }

    fclose( fp );

    return lCount;
}

/**
 * Get current time.
 *
 * @param char* szStr 	The result contains the current time.
 * @return MY_FAIL|MY_SUCCEED
 */

int getCurrTime( char * szStr )
{
    time_t tTime;
    struct tm * tmTime;

    time(&tTime);
    tmTime = localtime(&tTime);

    sprintf(szStr, "%04d%02d%02d%02d%02d%02d",
            tmTime->tm_year + 1900, tmTime->tm_mon + 1, 
            tmTime->tm_mday, tmTime->tm_hour, 
            tmTime->tm_min, tmTime->tm_sec);

    return MY_SUCCEED;
}

/**
 * Get the time to an accuracy of microsecond.
 *
 * @param char* szMicroTime 	The result contains the time.
 * @return MY_FAIL|MY_SUCCEED
 */

int getMicroSecondTime( char * szMicroTime )
{
    struct tm * tmTime;
    struct timeval tvTime;

    memset( &tvTime, 0, sizeof( tvTime ) );
    gettimeofday( &tvTime, NULL );

    tmTime = localtime( &(tvTime.tv_sec) );

    sprintf( szMicroTime, "%04d%02d%02d%02d%02d%02d%06d", 
            tmTime->tm_year + 1900, tmTime->tm_mon + 1, tmTime->tm_mday,
            tmTime->tm_hour, tmTime->tm_min, tmTime->tm_sec,
            (int)tvTime.tv_usec );

    return MY_SUCCEED;
}

/**
 * Get the time interval.
 *
 * @param char* szStartTime	The start time string.
 * @param char* szEndTime The end time string.
 * @return time_t The interval.
 */

time_t getTimeInterval( char * szStartTime, char * szEndTime )
{
    time_t stTime, etTime;
    struct tm stmTime, etmTime;
    char szBuf[14 + 1];

    memset( &stmTime, 0, sizeof( struct tm ) );
    memset( &etmTime, 0, sizeof( struct tm ) );

    strncpy( szBuf, szStartTime + 12, 2 ); szBuf[2] = 0;
    stmTime.tm_sec = atoi( szBuf );

    strncpy( szBuf, szStartTime + 10, 2 ); szBuf[2] = 0;
    stmTime.tm_min = atoi( szBuf );

    strncpy( szBuf, szStartTime +  8, 2 ); szBuf[2] = 0;
    stmTime.tm_hour = atoi( szBuf );

    strncpy( szBuf, szStartTime +  6, 2 ); szBuf[2] = 0;
    stmTime.tm_mday = atoi( szBuf );

    strncpy( szBuf, szStartTime +  4, 2 ); szBuf[2] = 0;
    stmTime.tm_mon = atoi( szBuf ) - 1;

    strncpy( szBuf, szStartTime,   4    ); szBuf[4] = 0;
    stmTime.tm_year = atoi( szBuf ) - 1900;

    stTime =  mktime( &stmTime );

    strncpy( szBuf, szEndTime + 12, 2 ); szBuf[2] = 0;
    etmTime.tm_sec = atoi( szBuf );

    strncpy( szBuf, szEndTime + 10, 2 ); szBuf[2] = 0;
    etmTime.tm_min = atoi( szBuf );

    strncpy( szBuf, szEndTime +  8, 2 ); szBuf[2] = 0;
    etmTime.tm_hour = atoi( szBuf );

    strncpy( szBuf, szEndTime +  6, 2 ); szBuf[2] = 0;
    etmTime.tm_mday = atoi( szBuf );

    strncpy( szBuf, szEndTime +  4, 2 ); szBuf[2] = 0;
    etmTime.tm_mon = atoi( szBuf ) - 1;

    strncpy( szBuf, szEndTime,   4    ); szBuf[4] = 0;
    etmTime.tm_year = atoi( szBuf ) - 1900;

    etTime =  mktime( &etmTime );

    return ( etTime - stTime );	
}

/**
 * Get the time to an accuracy of second.
 *
 * @param char* szTime	The time string.
 * @return time_t The interval.
 */

time_t getSecondTime( char * szTime )
{
    struct tm tmTime;
    char szBuf[14 + 1];

    memset( &tmTime, 0, sizeof( struct tm ) );

    strncpy( szBuf, szTime + 12, 2 ); szBuf[2] = 0;
    tmTime.tm_sec = atoi( szBuf );

    strncpy( szBuf, szTime + 10, 2 ); szBuf[2] = 0;
    tmTime.tm_min = atoi( szBuf );

    strncpy( szBuf, szTime +  8, 2 ); szBuf[2] = 0;
    tmTime.tm_hour = atoi( szBuf );

    strncpy( szBuf, szTime +  6, 2 ); szBuf[2] = 0;
    tmTime.tm_mday = atoi( szBuf );

    strncpy( szBuf, szTime +  4, 2 ); szBuf[2] = 0;
    tmTime.tm_mon = atoi( szBuf ) - 1;

    strncpy( szBuf, szTime,   4    ); szBuf[4] = 0;
    tmTime.tm_year = atoi( szBuf ) - 1900;

    return ( mktime( &tmTime ) );
}

/**
 * Get the last day reference to the given time.
 *
 * @param char* szLastTime	The last day time.
 * @param char* szCurrTime	The given reference time.
 * @return MY_SUCCEED|MY_FAIL
 */

int getLastDay( char * szLastTime, char * szCurrTime )
{
    time_t tTime;
    struct tm * tmTime;
    struct tm otmTime;
    char szBuf[128 + 1];
    char szTime[14 + 1];
    int nLen;
    int nYear, nMon, nDay, nHour, nMin, nSec;

    memset( &otmTime, 0, sizeof( struct tm ) );	

    nLen = strlen( szCurrTime );

    if( nLen > 6 )
    {
        memcpy( szBuf, szCurrTime + 6, 2 ); szBuf[2] = 0;
        otmTime.tm_mday = atoi( szBuf );
    }
    else
    {
        otmTime.tm_mday = 1;
    }

    if( nLen > 4 )
    {
        memcpy( szBuf, szCurrTime + 4, 2 ); szBuf[2] = 0;
        otmTime.tm_mon = atoi( szBuf ) - 1;
    }

    memcpy( szBuf, szCurrTime, 4 ); szBuf[4] = 0;
    otmTime.tm_year = atoi( szBuf ) - 1900;

    tTime =  mktime( &otmTime );
    tTime -= 86400;
    tmTime = localtime( &tTime );

    sprintf( szTime, "%04d%02d%02d%02d%02d%02d",
            tmTime->tm_year + 1900, 
            tmTime->tm_mon + 1, 
            tmTime->tm_mday, 
            tmTime->tm_hour, 
            tmTime->tm_min, 
            tmTime->tm_sec);

    strcpy( szLastTime, szTime );

    return MY_SUCCEED;
}

/**
 * Get the next day reference to the given time.
 *
 * @param char* szNextDay	The next day time.
 * @param char* szCurrDate	The given reference time.
 * @return MY_SUCCEED|MY_FAIL
 */

int getNextDay( char * szNextDay, char * szCurrDate )
{
    time_t tTime;
    struct tm * tmTime;
    struct tm otmTime;
    char szBuf[14 + 1];
    /*
       otmTime.tm_sec  = 0;
       otmTime.tm_min  = 0;
       otmTime.tm_hour = 1;
       */	
    memset( &otmTime, 0, sizeof( struct tm ) );

    strncpy( szBuf, szCurrDate + 6, 2 ); szBuf[2] = 0;
    otmTime.tm_mday = atoi( szBuf );

    strncpy( szBuf, szCurrDate + 4, 2 ); szBuf[2] = 0;
    otmTime.tm_mon = atoi( szBuf ) - 1;

    strncpy( szBuf, szCurrDate, 4 ); szBuf[4] = 0;
    otmTime.tm_year = atoi( szBuf ) - 1900;

    tTime =  mktime( &otmTime );
    tTime += 86400;
    tmTime = localtime( &tTime );

    sprintf( szNextDay, "%04d%02d%02d",
            tmTime->tm_year + 1900, 
            tmTime->tm_mon + 1, 
            tmTime->tm_mday );

    return MY_SUCCEED;
}

/**
 * Get the next month reference to the given date.
 *
 * @param char* szNextMonth	The next month.
 * @param char* szCurrDate	The given reference date.
 * @return MY_SUCCEED|MY_FAIL
 */

int getNextMonth( char * szNextMonth, char * szCurrDate )
{
    int nYear, nMon, nLastDay;
    char szYear[DATE_YEAR_LEN + 1];
    char szMonth[DATE_MON_LEN + 1];

    strncpy( szYear, szCurrDate, DATE_YEAR_LEN );
    szYear[DATE_YEAR_LEN] = 0;
    nYear = atoi( szYear );
    strncpy( szMonth, szCurrDate + DATE_YEAR_LEN, DATE_MON_LEN );
    szMonth[DATE_MON_LEN] = 0;
    nMon = atoi( szMonth );

    nMon++;

    if( nMon > 12 )
    {
        nMon = 1;
        nYear++;
    }

    sprintf( szNextMonth, "%04d%02d", nYear, nMon );

    return MY_SUCCEED;
}

/**
 * Get the last day reference to the given date.
 *
 * @param char* szLastDay	The last day of the given datetime.
 * @param char* szTime	The given reference datetime.
 * @return MY_SUCCEED|MY_FAIL
 */

int getMonthLastDay( char * szLastDay, char * szTime )
{
    int nYear, nMon, nLastDay;
    char szYear[DATE_YEAR_LEN + 1];
    char szMonth[DATE_MON_LEN + 1];

    strncpy( szYear, szTime, DATE_YEAR_LEN );
    szYear[DATE_YEAR_LEN] = 0;
    nYear = atoi( szYear );
    strncpy( szMonth, szTime + DATE_YEAR_LEN, DATE_MON_LEN );
    szMonth[DATE_MON_LEN] = 0;
    nMon = atoi( szMonth );

    if( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 )
    {
        nLastDay = 30;
    }
    else if( nMon == 2 )
    {
        if( IsLeapYear( nYear ) )
        {
            nLastDay = 29;
        }
        else
        {
            nLastDay = 28;
        }
    }
    else
    {
        nLastDay = 31;
    }

    sprintf( szLastDay, "%04d%02d%02d", nYear, nMon, nLastDay );

    return MY_SUCCEED;
}


/**
 * Determine if the specified file is existed.
 *
 * @param char* szFile	The file name.
 * @return MY_TRUE|MY_FALSE
 */


int fileExists( char * szFile )
{
    if( 0 == access( szFile, F_OK ) )
    {
        return MY_TRUE;
    }
    else
    {
        return MY_FALSE;
    }
}

/**
 * Get the file size.
 *
 * @param char* szFile	The file name.
 * @return long The file size.
 */


long getFileSize( char * szFile )
{
    struct stat statBuf;

    lstat( szFile, &statBuf );

    return ( statBuf.st_size );
}

/**
 * Determine if the given string is a directory.
 *
 * @param char* szDirName	The directory name.
 * @return long The file size.
 */


int isDir( char * szDirName )
{
    struct stat statMode;

    if ( (lstat( szDirName, &statMode ) == 0) && (statMode.st_mode & S_IFDIR) )
    {
        return (MY_TRUE);
    }
    else
    {
        return (MY_FALSE);
    }
}

/**
 * Determine if the specified file is existed.
 *
 * @param char* szFileName	The file name.
 * @return MY_TRUE|MY_FALSE
 */

int file_exists( char * szFileName )
{
    struct stat statMode;

    if ( (lstat( szDirName, &statMode ) == 0) && (statMode.st_mode & S_IFDIR) )
    {
        return (MY_TRUE);
    }
    else
    {
        return (MY_FALSE);
    }
}

/**
 * Make sure log path is opened.
 *
 * @param char* szPath	The file path.
 * @return MY_SUCCEED|MY_FAIL
 */

int chkLogPath( char * szPath )
{
    FILE * fp;
    char szBuf[MAX_RECORD_LEN + 1];
    char szTime[14 + 1];

    if( !isDir( szPath ) )
    {
        return MY_FAIL;
    }

    memset( szTime, 0, sizeof( szTime ) );
    getCurrTime( szTime );

    if( 0 == *(szSrcDir) )
    {
        sprintf( szBuf, "%s/%s%8.8s.runlog", szLogPath, szLogPrefix, szTime );
    }
    else
    {
        sprintf( szBuf, "%s/%s%8.8s.%s.runlog", szLogPath, szLogPrefix, szTime, szSrcDir );
    }

    fp = fopen( szBuf, "a" );
    if( NULL == fp ) 
    {
        return MY_FAIL;
    }

    fclose( fp );

    return MY_SUCCEED;
}

/**
 * Turn the program to background.
 *
 * @param void
 * @return MY_SUCCEED|MY_FAIL
 */

int backGround( void )
{
    pid_t pid;

    pid = fork();

    if ( 0 > pid )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_CRITICAL, "Switch to background failed!\n\n" );
        exit( 1 );
    }
    else if ( pid > 0 )
    {
        exit( 0 );
    }
    else
    {
        setsid();
        chdir( "/" );
        umask( 0 );
    }

    return MY_SUCCEED;
}

/**
 * Determine if the string is date time.
 *
 * @param szDate The date time.
 * @return MY_FALSE|MY_TRUE
 */

int isDate( char * szDate )
{
    char szBuf[128 + 1];
    int nYear, nMon, nDay, nHour, nMin, nSec;
    int nLen;

    if( !isNum( szDate ) )
    {
        return MY_FALSE;
    }

    nLen = strlen( szDate );

    if( DATE_STD_LEN != nLen )
    {
        return MY_FALSE;
    }

    memcpy( szBuf, szDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
    memcpy( szBuf, szDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
    memcpy( szBuf, szDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );
    memcpy( szBuf, szDate +  8, 2 ); szBuf[2] = 0;	nHour = atoi( szBuf );
    memcpy( szBuf, szDate + 10, 2 ); szBuf[2] = 0;	nMin  = atoi( szBuf );
    memcpy( szBuf, szDate + 12, 2 ); szBuf[2] = 0;	nSec  = atoi( szBuf );

    if( nMon > 12 || nDay > 31 || nHour > 23 || nMin > 59 || nSec > 59 )
    {
        return MY_FALSE;
    }

    if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
    {
        return MY_FALSE;
    }

    if( nMon == 2 )
    {
        if( IsLeapYear( nYear ) )
        {
            return (nDay > 29) ? MY_FALSE : MY_TRUE;   
        }	
        else 
        {
            return (nDay > 28) ? MY_FALSE : MY_TRUE;
        }	
    }

    return MY_TRUE;
}

/**
 * Determine if the string is a time to an accuracy of hour.
 *
 * @param szDate The date time.
 * @return MY_FALSE|MY_TRUE
 */

int isHourDate( char * szDate )
{
    char szBuf[128 + 1];
    int nYear, nMon, nDay, nHour, nMin, nSec;
    int nLen;

    if( !isNum( szDate ) )
    {
        return MY_FALSE;
    }

    nLen = strlen( szDate );

    if( ( DATE_YEAR_LEN + DATE_MON_LEN + DATE_DAY_LEN + DATE_HOUR_LEN ) != nLen )
    {
        return MY_FALSE;
    }

    memcpy( szBuf, szDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
    memcpy( szBuf, szDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
    memcpy( szBuf, szDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );
    memcpy( szBuf, szDate +  8, 2 ); szBuf[2] = 0;	nHour = atoi( szBuf );

    if( nMon > 12 || nDay > 31 || nHour > 23 )
    {
        return MY_FALSE;
    }

    if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
    {
        return MY_FALSE;
    }

    if( nMon == 2 )
    {
        if( IsLeapYear( nYear ) )
        {
            return (nDay > 29) ? MY_FALSE : MY_TRUE;   
        }	
        else 
        {
            return (nDay > 28) ? MY_FALSE : MY_TRUE;
        }	
    }

    return MY_TRUE;
}

/**
 * Determine if the string is a time to an accuracy of date.
 *
 * @param szDayDate The date time.
 * @return MY_FALSE|MY_TRUE
 */

int isDayDate( char * szDayDate )
{
    char szBuf[128 + 1];
    int nYear, nMon, nDay;
    int nLen;

    if( !isNum( szDayDate ) )
    {
        return MY_FALSE;
    }

    nLen = strlen( szDayDate );

    if( ( DATE_YEAR_LEN + DATE_MON_LEN + DATE_DAY_LEN ) != nLen )
    {
        return MY_FALSE;
    }

    memcpy( szBuf, szDayDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
    memcpy( szBuf, szDayDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );
    memcpy( szBuf, szDayDate +  6, 2 ); szBuf[2] = 0;	nDay  = atoi( szBuf );

    if( nMon > 12 || nDay > 31 )
    {
        return MY_FALSE;
    }

    if( ( nMon == 4 || nMon == 6 || nMon == 9 || nMon == 11 ) && nDay > 30 )
    {
        return MY_FALSE;
    }

    if( nMon == 2 )
    {
        if( IsLeapYear( nYear ) )
        {
            return (nDay > 29) ? MY_FALSE : MY_TRUE;   
        }	
        else 
        {
            return (nDay > 28) ? MY_FALSE : MY_TRUE;
        }	
    }

    return MY_TRUE;
}

/**
 * Determine if the string is a time to an accuracy of month.
 *
 * @param IsMonthDate The date time.
 * @return MY_FALSE|MY_TRUE
 */

int isMonthDate( char * IsMonthDate )
{
    char szBuf[128 + 1];
    int nYear, nMon;
    int nLen;

    if( !isNum( IsMonthDate ) )
    {
        return MY_FALSE;
    }

    nLen = strlen( IsMonthDate );

    if( ( DATE_YEAR_LEN + DATE_MON_LEN ) != nLen )
    {
        return MY_FALSE;
    }

    memcpy( szBuf, IsMonthDate     , 4 ); szBuf[4] = 0;	nYear = atoi( szBuf );
    memcpy( szBuf, IsMonthDate +  4, 2 ); szBuf[2] = 0;	nMon  = atoi( szBuf );

    if( nMon > 12 )
    {
        return MY_FALSE;
    }

    return MY_TRUE;
}

/**
 * Get the executable file's root name and base name.
 *
 * @param char* szRootName The result contains root name.
 * @param char* szBaseName The result contains base name.	
 * @param char* szFullDir The full executable file name.
 * @return MY_SUCCEED
 */

int getIniInfo( char * szRootName, char * szBaseName, char * szFullDir )
{
    char szDir[MAX_PATH_LEN + 1];

    char * szPos;

    int nLen;

    strcpy( szDir, szFullDir );
    nLen = strlen( szDir );

    while( nLen )
    {
        if( '/' == szDir[nLen - 1] )
        {
            szDir[nLen - 1] = 0;
        }
        else
        {
            break;
        }

        nLen--;
    }

    szPos = strrchr( szDir, '/' );

    if( NULL == szPos )
    {
        strcpy( szRootName, "./" );
        strcpy( szBaseName, szDir );
    }
    else
    {
        strncpy( szRootName, szDir, szPos - szDir );
        szRootName[szPos - szDir] = 0;

        strcpy( szBaseName, szPos + 1 );
    }

    return MY_SUCCEED ;
}

/**
 * Get the user name and password crypted in the login file.
 *
 * @param char* szLoginFile The login file.
 * @param char* szUser The result contains the username.
 * @param char* szPwd The result contains the password.
 * @return MY_SUCCEED|MY_FAIL
 */

int getLoginInfo( char * szLoginFile, char * szUser, char * szPwd )
{
    FILE * fpLogin;
    char szKey[MAX_RECORD_LEN + 1];
    char szStr[MAX_RECORD_LEN + 1];
    int nLoop;

    if( NULL == ( fpLogin = fopen( szLoginFile, "r" ) ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Get password file %s failed!\n", szLoginFile );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );

        return MY_FAIL;
    }

    strcpy( szKey, ENCRYPT_KEYWORD );

    fgets( szUser, MAX_RECORD_LEN, fpLogin );
    fgets( szUser, MAX_RECORD_LEN, fpLogin );
    fgets( szPwd , MAX_RECORD_LEN, fpLogin );
    fgets( szPwd , MAX_RECORD_LEN, fpLogin );

    szUser[strlen(szUser) - 1] = 0;
    szPwd[strlen(szPwd) - 1] = 0;

    fclose( fpLogin );

    strcpy( szStr, szUser );

    for( nLoop = 0; nLoop < strlen( szStr ); nLoop++ )
    {
        szStr[nLoop] = szStr[nLoop] ^ szKey[nLoop%strlen(szKey)];
    }

    strcpy( szUser, szStr );

    strcpy( szStr, szPwd );

    for( nLoop = 0; nLoop < strlen( szStr ); nLoop++ )
    {
        szStr[nLoop] = szStr[nLoop] ^ szKey[nLoop%strlen(szKey)];
    }

    strcpy( szPwd, szStr );
    /*
       printf( "[%s] / [%s]\n", szUser, szPwd ); 
       */

    return MY_SUCCEED;
}


/**
 * Move the source file to the destination.
 *
 * @param char* szSrcFile The source file.
 * @param char* szDstFile The destination name.
 * @return MY_SUCCEED|MY_FAIL
 */

int moveFile( char * szSrcFile, char * szDstFile )
{
    if( -1 == rename( szSrcFile, szDstFile ) )
    {
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "Move file from  %s to %s failed!\n", szSrcFile, szDstFile );
        MyLog( LOG_SOURCE_APP, LOG_LEVEL_WARN, "ErrorInfo: %d : %s !\n", errno, strerror( errno ) );
        return MY_FAIL;
    }

    return MY_SUCCEED;
}

/**
 * Determine if the given directory are in the same file system.
 *
 * @param char* szDir1 The first directory name.
 * @param char* szDir2 The second directory name.
 * @return MY_TRUE|MY_FALSE
 */

int isSameFileSystem( char * szDir1, char * szDir2 )
{
    struct stat statMode1;
    struct stat statMode2;

    if( 0 != lstat( szDir1, &statMode1 ) || 0 != lstat( szDir2, &statMode2 ) )
    {
        return MY_FALSE;
    }


    if( statMode1.st_dev != statMode2.st_dev )
    {
        return MY_FALSE;
    }

    return MY_TRUE;
}

/**
 * Determine if the file line is a record line.
 *
 * @param char* szRecord The record string.
 * @return MY_TRUE|MY_FALSE
 */

int isRecordLine( char * szRecord )
{
    if( 0 == strncmp( szRecord, FILE_NOTE_FLAG, strlen( FILE_NOTE_FLAG ) ) )
    {
        return MY_FALSE;;
    }

    TrimLeft( TrimRight( szRecord ) );

    if( 0 == *( szRecord ) )
    {
        return MY_FALSE;;
    }

    return MY_TRUE;
}



/**
 * Determine if the string is a mobile header.
 *
 * @param char* szHead The string head.
 * @return MY_TRUE|MY_FALSE
 */


int isMobileHead( char * szHead )
{
    if( !isNum( szHead ) )
    {
        return MY_FALSE;
    }

    if( MOBILE_HEAD_LEN != strlen( szHead ) )
    {
        return MY_FALSE;
    }

    return MY_TRUE;
}


/**
 * Determine if the string is a mobile user.
 *
 * @param char* szHead The string user.
 * @return MY_TRUE|MY_FALSE
 */

int isMobileUser( char * szUser )
{
    if( !isNum( szUser ) )
    {
        return MY_FALSE;
    }

    if( MOBILE_NUMBER_LEN != strlen( szUser ) )
    {
        return MY_FALSE;
    }

    return MY_TRUE;
}

/**
 * Get two precision of the value. 0.175000 will be set to 0.17 if calculate by %.2f, but it should be 0.18.
 *
 * @param char* szResult The result
 * @param double d The origin value.
 * @return MY_TRUE|MY_FALSE
 */

    
int get2Precision( char * szResult, double d )
{
    char szOrigin[20];
    char szTemp[20];
    char thirdPrecision;
    char* p;
    
    float fTmp;
    
    szOrigin[0]=0;
    szTemp[0]=0;
    sprintf(szOrigin, "%f", d);

    p = strchr(szOrigin, '.');
    strncpy(szTemp, szOrigin, p-szOrigin+3);
    szTemp[p-szOrigin+3]=0;
    
    fTmp = atof(szTemp);
    thirdPrecision = p[3]-'0';

    if (thirdPrecision >= 5)
    {
        fTmp += 0.01;
    }

    sprintf(szResult, "%.2f", fTmp);
    return MY_SUCCEED;
}
