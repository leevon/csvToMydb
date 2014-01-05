csvToMydb
=========

CSV file into mysql db

=========
1. 功能描述：
特定分割符分割的文本文件按照配置文件规则入库。其中，文本中的一条数据可以入几张不同或相同的表。入库逻辑通过配置文件配置实现。
2. 配置数据：
公共配置文件
dbsync.ini
[DIRECTORY]
LOGDIR = /sbilling/work/vince/work/file2db/log
RUNLOG = /sbilling/work/vince/work/file2db/log/runlog
LOGTMP = /sbilling/work/vince/work/file2db/log/temp
REJDIR = /sbilling/work/vince/work/file2db/data/reject
INDIR = /sbilling/work/vince/work/file2db/data/in
BAKDIR = /sbilling/work/vince/work/file2db/data/bak

[CONFIG]
SQLSTMT = /sbilling/work/vince/work/file2db/cfg/file2db.ini
FIELDSEP = ,

[DATABASE]
DBTYPE = mysql
DBSERVER = XXX.XXX.XXX.XXX
DBNAME = test
DBPORT = 3306
LOGINFILE = /sbilling/work/vince/work/file2db/cfg/login

[CONTROL]
#option: DEBUG,WARN,ERROR,CRITICAL
LOGLEVEL = DEBUG
CTRLFLAG = sync
LOGBATCH = 120
插入逻辑配置文件
file2db.ini
[insertlist]
#list:文本中一条记录要插入的表，英文竖线（’|’）分割。（配置在此处的就会执行插入操作）
list=vince_lee_test_user|vince_lee_test_acct|vince_lee_test_phone
#[table_name]对应上述list中竖线分割的表名
[vince_lee_test_user]
#chksql:插入之前数据的重复验证，select 主键 from 表名 where 字段名=？ and …
chksql=select user_id from vince_lee_test_user where user_id=?
#chk:上述验证sql语句条件对应文本每条记录的位置（位置从0开始），多个条件英文逗号（’,’）分隔
chk=0
#sqlstr:插入SQL
sqlstr=insert into vince_lee_test_user(user_id,user_name,user_age,user_phone,user_acct) values(?,?,?,?,?)
#map:上述插入SQL的values值对应文本每条记录的位置（位置从0开始），英文逗号（’,’）分隔
map=0,1,2,6,3

[vince_lee_test_acct]
chksql=select acct_id from vince_lee_test_acct where acct_id=?
chk=3
sqlstr=insert into vince_lee_test_acct(acct_id,acct_name,acct_amount,user_id) values(?,?,?,?)
map=3,4,5,0

[vince_lee_test_phone]
chksql=select phone_id from vince_lee_test_phone where phone_id=?
chk=6
sqlstr=insert into vince_lee_test_phone(phone_id,phone_pin,user_id,acct_id) values(?,?,?,?)
map=6,7,0,3
3. 运行方法：
1.检查配置文件，根据入库逻辑修改配置文件。
2.把需要入库的文本文件，put到配置文件对应的INDIR对应的路径下。
3.启动应用：
./startup.sh 或者 ./file2db –i 配置文件（全路径）
4.检查日志结果。

[sbilling@A1 bin]$ cat startup.sh 
#!/bin/bash 

b=""
for ((i=0;i<=6;i+=1))
do
   printf "begin start up the file2db, please wait some minute %s\r" $b
   sleep 0.3
   b+="."
done

printf "\n"
./file2db -i /sbilling/work/vince/work/file2db/cfg/dbsync.ini &
4. 结果查看：
在配置文件LOGTMP路径下会生成文本入库记录。
[sbilling@A1 temp]$ more dbsync.in.log
[repeat]:table:[vince_lee_test_user] key:{2:0[1001]}
[repeat]:table:[vince_lee_test_acct] key:{3:3[2001]}
[repeat]:table:[vince_lee_test_user] key:{3:0[1001]}
[repeat]:table:[vince_lee_test_user] key:{5:0[1002]}
[result]:table:[vince_lee_test_phone] record:[4]
[result]:table:[vince_lee_test_acct] record:[3]
[result]:table:[vince_lee_test_user] record:[1]
[user_info.txt]:20131113141241375401:20131113141241379605:T 5:V 4:E 1
说明：
[repeat]:根据配置文件中chksql和chk检查的重复记录。其中[table_name]是入库的表名。Key是关键词，对应文本某条某个位置的值。
[result]:表插入记录数。
***.txt:该文件入库的起始时间结束时间T：文本总记录数，V：有效记录，E：错误记录
例如：
[repeat]:table:[vince_lee_test_user] key:{2:0[1001]}
说明：根据配置文件chksql检查，该表(vince_lee_test_user)已经存在key为1001的记录。1001对应文本的第2条的第0个位置
[result]:table:[vince_lee_test_phone] record:[4]
说明：该表(vince_lee_test_phone)本次入库记录为4条。
[user_info.txt]:20131113141241375401:20131113141241379605:T 5:V 4:E 1
说明：文件user_info.txt入库开始时间为20131113141241375401结束时间为20131113141241379605.文本总共5条记录，其中4条有效，1条无效。

5. 实现逻辑：
1.根据配置文件初始化运行环境。包括日志（路径、等级），入库文件（入库路径和备份路径），数据库（数据库类型、用户和口令）等。
2.内存加载入库配置逻辑。在内存中初始化为一个单链表。
结构体定义：
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

3.根据配置文件数据库的用户名和口令，连接数据库。
4.根据配置文件入库逻辑初始化数据库运行环境。
5.循环读取配置文件路径下的文件。
6.打开文件，循环读取每条记录。
7.处理每条记录：
7.1把记录通过分隔符分割放到数组中。
  7.2循环读取配置文件中入库的表的单链表，根据配置文件逻辑入库。
8.处理后再读取文本中的记录，结束后记录结果日志。
9.再读取目录下的文件。
10.结束，程序退出。

附录：
入库文件样例：
user_id,user_name,age,acct_id,acct_name,acct_amount,phone_id,pin
1001,vince,27,2001,vince,200,1334,123
1001,vince,27,2001,vince,200,1335,456
1002,lee,27,2002,lee,500,1336,789
1002,lee,27,2003,chou,3000,1337,789

注：入库文件中的文件头是必须的，没有也要空出一行，否则会少入库一条记录。
    联系：
Autor: vince_lee@bizconf.com
