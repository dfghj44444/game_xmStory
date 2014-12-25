#include "dataOperatorLite.h"
#include "sqllite/sqlite3.h"
#include "cocos2d.h"
//#include "base/CCPlatformMacros.h"
dataOperatorLite::dataOperatorLite(void)
{
}


dataOperatorLite::~dataOperatorLite(void)
{
}

void dataOperatorLite::createTable()
{
	//1.创建数据库testSqlite3  
	sqlite3 *testSqlite3 = NULL;  
	//sqlite3_open（）方法会打开数据库，没有就自动创建一个  
	int resultOK = sqlite3_open("testSqlite9.db", &testSqlite3);  
	//返回数字0，说明创建成功  
	if (resultOK != SQLITE_OK) {  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	CCLOG("resultOK  %d",resultOK);  

	//2.使用sql语句创建表testTable，并设置字段  
	const char *createTableSQL = "create table testTable (int_col INTERGER ,float_col REAL ,string_col TEXT,name_col TEXT)";  
	//stmt，即是state ment句柄，承载着sql语句  
	sqlite3_stmt *stmt = NULL;  
	//sql语句的长度  
	int length = strlen(createTableSQL);  
	CCLOG("length %d",length);  

	//sqlite3_prepare_v2，准备创建数据表，参数分别是数据库名称，表名称，语句长度，句柄。如果写入错误，则释放stmt对象，关闭数据库  
	if (sqlite3_prepare_v2(testSqlite3, createTableSQL, length, &stmt, NULL)!= SQLITE_OK) {  
		if (stmt) {  
			sqlite3_finalize(stmt);  
			sqlite3_close(testSqlite3);  
			return ;  
		}  
	}  
	CCLOG("%d ：准备执行语句成功",sqlite3_prepare_v2(testSqlite3, createTableSQL, length, &stmt, NULL));  
	//sqlite3_step执行创建语句，如果创建错误，则释放stmt对象，关闭数据库  
	if (sqlite3_step(stmt)!=SQLITE_DONE) {  
		sqlite3_finalize(stmt);  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	//CCLOG("sqlite3_step(stmt) %d",sqlite3_step(stmt));  
	//释放创建表语句的对象内存  
	sqlite3_finalize(stmt);  
	CCLOG("create the table successed!");  

	//3.insert 表数据  
	const char *insertDatabase = "insert into testTable values(100,100,'这是一个测试','我的名字叫kevin')";  
	sqlite3_stmt *stmt4;  
	if (sqlite3_prepare_v2(testSqlite3, insertDatabase, strlen(insertDatabase), &stmt4,NULL)!=SQLITE_OK) {  
		if (stmt4) {  
			sqlite3_finalize(stmt4);  
		}  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	int res1 = sqlite3_step(stmt4);  
	if (res1 != SQLITE_DONE) {  
		sqlite3_finalize(stmt4);  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	CCLOG("插入数据成功");  
	//成功后清除句柄对象  
	sqlite3_finalize(stmt4);  

	//4.使用sql查询语句  
	const char *selectSQL = "select * from testTable ";  
	//stmt，即是state ment句柄，承载着sql语句  
	sqlite3_stmt *stmt2 = NULL;  
	//sql语句的长度  
	int length2 = strlen(selectSQL);  
	CCLOG("length2 %d",length2);  

	//sqlite3_prepare_v2，准备创建数据表，参数分别是数据库名称，表名称，语句长度，句柄。如果写入错误，则释放stmt对象，关闭数据库  
	if (sqlite3_prepare_v2(testSqlite3, selectSQL, length2, &stmt2, NULL)!= SQLITE_OK) {  
		if (stmt2) {  
			sqlite3_finalize(stmt2);  
			sqlite3_close(testSqlite3);  
			return ;  
		}  
	}  

	int res = sqlite3_step(stmt2);  
	CCLOG("res  %d",res);  
	int fieldCount = sqlite3_column_count(stmt2);  
	CCLOG("sqlite3_column_count(stmt2) fieldCount :%d",fieldCount);  
	if (res ==SQLITE_ROW ) {  
		for (int count = 0; count < fieldCount; count ++) {  
			int stype = sqlite3_column_type(stmt2, count);  
			if (stype == SQLITE_INTEGER) {  
				int temp1 = sqlite3_column_int(stmt2, count);  
				CCLOG("temp1  %d",temp1);  
			}  
			if (stype == SQLITE_FLOAT) {  
				double temp2 = sqlite3_column_double(stmt2, count);  
				CCLOG("temp2  %.2f",temp2);  
			}  
			if (stype == SQLITE_TEXT) {  
				const char *temp3 = (const char*)sqlite3_column_text(stmt2, count);  
				CCLOG("temp3  %s",temp3);  
			}  
			if (stype == SQLITE_NULL) {  
				CCLOG("NULL");  
			}  
		}  
	}
	else if (res == SQLITE_DONE){  
		CCLOG("查询已经完成");  
	}  
	//释放创建表语句的对象内存  
	sqlite3_finalize(stmt2);  

	//5.使用drop table 模块  
	const char *deleteDatabase = "drop table testTable";  
	sqlite3_stmt *stmt3;  
	if (sqlite3_prepare_v2(testSqlite3, deleteDatabase, strlen(deleteDatabase), &stmt3, NULL)!= SQLITE_OK) {  
		if (stmt3) {  
			sqlite3_finalize(stmt3);  
		}  
		sqlite3_close(testSqlite3);  
		return;  
	}  
	if (sqlite3_step(stmt3)==SQLITE_DONE) {  
		CCLOG("drop the table succeed");  
	}  
	sqlite3_finalize(stmt3);  
	sqlite3_close(testSqlite3);  

	CCLOG("Hello Sqlite!");  
}
