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
	//1.�������ݿ�testSqlite3  
	sqlite3 *testSqlite3 = NULL;  
	//sqlite3_open��������������ݿ⣬û�о��Զ�����һ��  
	int resultOK = sqlite3_open("testSqlite9.db", &testSqlite3);  
	//��������0��˵�������ɹ�  
	if (resultOK != SQLITE_OK) {  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	CCLOG("resultOK  %d",resultOK);  

	//2.ʹ��sql��䴴����testTable���������ֶ�  
	const char *createTableSQL = "create table testTable (int_col INTERGER ,float_col REAL ,string_col TEXT,name_col TEXT)";  
	//stmt������state ment�����������sql���  
	sqlite3_stmt *stmt = NULL;  
	//sql���ĳ���  
	int length = strlen(createTableSQL);  
	CCLOG("length %d",length);  

	//sqlite3_prepare_v2��׼���������ݱ������ֱ������ݿ����ƣ������ƣ���䳤�ȣ���������д��������ͷ�stmt���󣬹ر����ݿ�  
	if (sqlite3_prepare_v2(testSqlite3, createTableSQL, length, &stmt, NULL)!= SQLITE_OK) {  
		if (stmt) {  
			sqlite3_finalize(stmt);  
			sqlite3_close(testSqlite3);  
			return ;  
		}  
	}  
	CCLOG("%d ��׼��ִ�����ɹ�",sqlite3_prepare_v2(testSqlite3, createTableSQL, length, &stmt, NULL));  
	//sqlite3_stepִ�д�����䣬��������������ͷ�stmt���󣬹ر����ݿ�  
	if (sqlite3_step(stmt)!=SQLITE_DONE) {  
		sqlite3_finalize(stmt);  
		sqlite3_close(testSqlite3);  
		return ;  
	}  
	//CCLOG("sqlite3_step(stmt) %d",sqlite3_step(stmt));  
	//�ͷŴ��������Ķ����ڴ�  
	sqlite3_finalize(stmt);  
	CCLOG("create the table successed!");  

	//3.insert ������  
	const char *insertDatabase = "insert into testTable values(100,100,'����һ������','�ҵ����ֽ�kevin')";  
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
	CCLOG("�������ݳɹ�");  
	//�ɹ�������������  
	sqlite3_finalize(stmt4);  

	//4.ʹ��sql��ѯ���  
	const char *selectSQL = "select * from testTable ";  
	//stmt������state ment�����������sql���  
	sqlite3_stmt *stmt2 = NULL;  
	//sql���ĳ���  
	int length2 = strlen(selectSQL);  
	CCLOG("length2 %d",length2);  

	//sqlite3_prepare_v2��׼���������ݱ������ֱ������ݿ����ƣ������ƣ���䳤�ȣ���������д��������ͷ�stmt���󣬹ر����ݿ�  
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
		CCLOG("��ѯ�Ѿ����");  
	}  
	//�ͷŴ��������Ķ����ڴ�  
	sqlite3_finalize(stmt2);  

	//5.ʹ��drop table ģ��  
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
