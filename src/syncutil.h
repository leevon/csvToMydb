
#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

#define	MY_TRUE						1
#define	MY_FALSE					0
#define	MY_SUCCEED					0	
#define	MY_FAIL						1

// standard year length
#define	DATE_YEAR_LEN				4
// standard month length
#define	DATE_MON_LEN				2
// standard date length
#define	DATE_DAY_LEN				2
// standard hour length
#define	DATE_HOUR_LEN				2
// standard minutes length
#define	DATE_MIN_LEN				2
// standard second length
#define	DATE_SEC_LEN				2
//stand datetime length
#define	DATE_STD_LEN				14

//Log generated by application itself
#define	LOG_SOURCE_APP			 "app"
//Log generated by database error
#define	LOG_SOURCE_DB				"db"
//Log generated except application and database, like system errors.
#define	LOG_SOURCE_SYS				"sys"

//Log level: DEBUG
#define	LOG_LEVEL_DEBUG						"debug"
//Log level: WARN
#define	LOG_LEVEL_WARN						"warn"
//Log level: ERROR
#define	LOG_LEVEL_ERROR		     			"error"
//Log level: CRITICAL, in this case exit.
#define	LOG_LEVEL_CRITICAL					"critical"

#define	FILE_NAME							__FILE__
#define	LINE_NUMBER						__LINE__

//Temporary log file extend name
#define	TMP_FILE_TAIL				".tmp"
//Log file extend name
#define	LOG_FILE_TAIL				".log"

//The comment character in the configuration file.
#define	FILE_NOTE_FLAG				"#"
//The new line seperator
#define	CHAR_NEW_LINE				"\n"
//The seperator in the record
#define FILE_FIELD_SEP				"\t "

#define	MOBILE_HEAD_FLAG			"13"
#define	MOBILE_HEAD_LEN				7
#define	MOBILE_NUMBER_LEN			11

#define	MAX_MONTH_DAY				31

#define	MAX_RECORD_LEN			 1024
#define	MAX_PATH_LEN				1024

#define	ENCRYPT_KEYWORD				"DFJLT"

#ifndef	max
#define	max( a, b )					( a < b ) ? b : a
#endif
#ifndef	min
#define	min( a, b )					( a > b ) ? b : a
#endif

#ifndef	ulong
#define	ulong						unsigned long
#endif

typedef	struct _TIME_DATA
{
	//Datetime
	char szTime[DATE_STD_LEN + 1];


	char szYear[DATE_YEAR_LEN + 1];
	char szMon[DATE_MON_LEN + 1];	
	char szDay[DATE_DAY_LEN + 1];
	char szHour[DATE_HOUR_LEN + 1];
	char szMin[DATE_MIN_LEN + 1];
	char szSec[DATE_SEC_LEN + 1];
} TIME_DATA;
typedef TIME_DATA * PTIME_DATA;

char szLogPath[MAX_PATH_LEN + 1];	
char szLogPrefix[128 + 1];
char szSrcDir[MAX_PATH_LEN];

char szDirName[128 + 1];
char szBinName[128 + 1];

int	gLogLevel;

time_t tmStartTime;
time_t tmEndTime;

char * TrimLeft( char * szTrim );
char * TrimRight( char * szTrim );
char * TrimLeftZero( char * szTrim );
int getIniInfo( char * szFileName, char * szLValue, char * szRValue );
int getConfigParaValue( char * szCfgFile, char * szLabel, char * szLeftValue, char * szRightValue );
int isFloat( char * szNum );
int isNum( char * szNum );
int isStr( char * szStr );
int isLeapYear( int nYear );
int getCurrTime( char * szStr );
int getMicroSecondTime( char * szMicroTime );
time_t getSecondTime( char * szTime );
int getLastDay( char * szLastTime, char * szCurrTime );
int getNextDay( char * szNextDay, char * szCurrDate );
int getNextMonth( char * szNextMonth, char * szCurrDate );
int getMonthLastDay( char * szLastDay, char * szTime );
int fileIsExist( char * szFile );
long getFileSize( char * szFile );
int isDir( char * szDirName );
int chkLogPath( char * szPath );
int BackGround( void );
int isDate( char * szDate );
int isHourDate( char * szDate );
int isDayDate( char * szDayDate );
int isMonthDate( char * IsMonthDate );
int getInDirInfo( char * szRootName, char * szBaseName, char * szFullDir );
int getLoginInfo( char * szLoginFile, char * szUser, char * szPwd );
int moveFile( char * szSrcFile, char * szDstFile );
int isSameFileSystem( char * szDir1, char * szDir2 );
int isRecordLine( char * szRecord );
int getTimeData( char * szTime, PTIME_DATA ptData );
int isMobileHead( char * szHead );
int isMobileUser( char * szUser );
int incUnFlock(int fd, off_t offset, int whence, off_t len);
int  incFlock(int fd, off_t offset, int whence, off_t len);
int MyLog( char * szSource, char * szLevel, char * szFmt, ... );
long getFileCount( char * szPath );
time_t getTimeInterval( char * szStartTime, char * szEndTime );


#endif
