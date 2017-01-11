#include <stdlib.h>  
#include <stdio.h>  
#include "sqlite3.h"

#if 0 
static int _sql_callback(void *notused, int argc, char **argv, char **szColName)  
{  
    int i = 0;  
      
    printf("notused:0x%x, argc:%d\n", notused, argc);  
    for (i = 0; i < argc; i++)  
    {  
        printf("%s = %s\n", szColName[i], argv[i] == 0 ? "NULL" : argv[i]);  
    }  
    printf("\n");  
      
    return 0;  
}  
  
/* 
 *  
 */  
int main(int argc, char** argv)  
{  
    const char *sSQL1 = "create table users(userid varchar(20) PRIMARY KEY, age int, birthday datetime);";  
    const char *sSQL2 = "insert into users values('wang', 20, '1989-5-4');";  
    const char *sSQL3 = "select * from users;";  
      
    sqlite3 *db = 0;  
    char *pErrMsg = 0;  
    int ret = 0;  
      
    //连接数据库  
    ret = sqlite3_open("./test.db", &db);  
    if (ret != SQLITE_OK)  
    {  
        fprintf(stderr, "open database failed: %s\n", sqlite3_errmsg(db));  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("open database success!\n");  
      
    //执行建表SQL  
    ret = sqlite3_exec(db, sSQL1, _sql_callback, 0, &pErrMsg);  
    if (ret != SQLITE_OK)  
    {  
        fprintf(stderr, "SQL create error: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg); //这个要的哦，要不然会内存泄露的哦！！！  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("create table success!\n");  
      
    //执行插入数据  
    ret = sqlite3_exec(db, sSQL2, _sql_callback, 0, &pErrMsg);  
    if (ret != SQLITE_OK)  
    {  
        fprintf(stderr, "SQL insert error: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg); //这个要的哦，要不然会内存泄露的哦！！！  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("insert data success!\n");  
      
    //执行查询操作  
    ret = sqlite3_exec(db, sSQL3, _sql_callback, 0, &pErrMsg);  
    if (ret != SQLITE_OK)  
    {  
        fprintf(stderr, "SQL error: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg);  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("select data success!\n");  
      
    //关闭数据库  
    sqlite3_close(db);  
    db = 0;  
      
    return 0;  
}
#endif

#if 0  
static int _sql_callback(void *notused, int argc, char **argv, char **szColName)  
{  
    int i = 0;  
      
    printf("notused:0x%x, argc:%d\n", notused, argc);  
    for (i = 0; i < argc; i++)  
    {  
        printf("%s = %s\n", szColName[i], argv[i] == 0 ? "NULL" : argv[i]);  
    }  
    printf("\n");  
      
    return 0;  
}  
  
int main(int argc, char** argv)  
{  
    const char *sSQL1 = "create table test1 (id int, name varchar(10), age int);";  
    char sql[100] = {0};  
    sqlite3 *db = NULL;  
    char *pErrMsg = NULL;  
    int ret = 0, i = 0;  
    int is_success = 1;  
    const char *sSQL3 = "select * from test1;";
      
    ret = sqlite3_open("./test1.db", &db);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "open database failed: %s\n", sqlite3_errmsg(db));  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("open database success!\n");    
      
    ret = sqlite3_exec(db, sSQL1, NULL, 0, &pErrMsg);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "SQL create error: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg);  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("create table success!\n");    
      
    sqlite3_exec(db, "begin;", _sql_callback, 0, &pErrMsg); //开启事务  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "SQL begin error: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg);  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("begin success!\n");    
      
    for (i = 0; i < 10; i++)  
    {  
        sprintf(sql, "insert into test1(id, name, age) values(%d, \"%s\", %d);", i, "Carl", i);  
        ret = sqlite3_exec(db, sql, _sql_callback, 0, &pErrMsg);  
        if (SQLITE_OK != ret)  
        {  
            is_success = 0;  
            fprintf(stderr, "for %d time error: %s\n", i, pErrMsg);  
            sqlite3_free(pErrMsg);  
            break;  
        }  
    }  
      
    if (is_success)  
    {  
        sqlite3_exec(db, "commit;", 0, 0, 0);  
        printf("insert data success!\n");    
    }  
    else  
    {  
        sqlite3_exec(db, "rollback;", 0, 0, 0);  
        printf("insert data failed!\n");    
    }  
      
    ret = sqlite3_exec(db, sSQL3, _sql_callback, 0, &pErrMsg);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "SQL ERROR: %s\n", pErrMsg);  
        sqlite3_free(pErrMsg);  
        sqlite3_close(db);  
        return 1;  
    }  
    printf("select success!\n");    
      
    sqlite3_close(db);  
    db = 0;  
      
    return 0;  
}
#endif

static int _sql_callback(void *notused, int argc, char **argv, char **szColName)  
{  
    int i = 0;  
      
    printf("notused:0x%x, argc:%d\n", notused, argc);  
    for (i = 0; i < argc; i++)  
    {  
        printf("%s = %s\n", szColName[i], argv[i] == 0 ? "NULL" : argv[i]);  
    }  
    printf("\n");  
      
    return 0;  
}  
  
int main(int argc, char** argv)  
{  
    sqlite3 *conn = NULL;  
    sqlite3_stmt *stmt = NULL;  
    char *err_msg = NULL;  
    int ret = 0;  
      
    char col_types[][10] = {"", "Interger", "Float", "Text", "Blob", "NULL"};  
      
    ret = sqlite3_open("./test1.db", &conn);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "sqlite open err, %d\n", ret);  
        return 1;  
    }  
    printf("open database success!\n");  
      
    //ret = sqlite3_prepare_v2(conn, "SELECT * FROM [test1] WHERE [name]==:name", -1, &stmt, (const char **)&err_msg);  
    ret = sqlite3_prepare_v2(conn, "SELECT * FROM [test1] WHERE [name]==?2", -1, &stmt, (const char **)&err_msg);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "sqlite prepare error: %s\n", err_msg);  
        sqlite3_free(err_msg);  
        sqlite3_close(conn);  
        return 1;  
    }  
    //printf("prepare success! %d\n", sqlite3_bind_parameter_index(stmt, ":name"));  
    printf("prepare success!\n");  
      
    ret = sqlite3_bind_text(stmt, 2, "Carl", 4, SQLITE_STATIC);  
    if (SQLITE_OK != ret)  
    {  
        fprintf(stderr, "sqlite bind error: %d\n", ret);  
        sqlite3_close(conn);  
        return 1;  
    }  
    printf("bind success!\n");  
      
    while (ret = sqlite3_step(stmt), ret == SQLITE_ROW)  
    {  
        int col_count = sqlite3_column_count(stmt); //结果集中列的数量  
        printf("col_count:%d\t", col_count);  
        const char *col_0_name = sqlite3_column_name(stmt, 0); //获取列名  
        printf("col_0_name:%s\t", col_0_name);  
        int id = sqlite3_column_int(stmt, 0);  
        printf("id value:%d\t", id);  
        int id_type = sqlite3_column_type(stmt, 0); //获取列数据类型  
        printf("id type:%d\t", id_type);  
          
        const char *col_2_name = sqlite3_column_name(stmt, 2);  
        int age = sqlite3_column_int(stmt, 2);  
        int age_type = sqlite3_column_type(stmt, 2);  
          
        const char *col_1_name = sqlite3_column_name(stmt, 1);  
        char name[80];  
        strncpy(name, (const char *)sqlite3_column_text(stmt, 1), 80);  
        int name_type = sqlite3_column_type(stmt, 1);  
          
        //打印结果  
        printf("col_count: %d, %s = %d(%s), %s = %s(%s), %s = %d(%s)\n",   
               col_count, col_0_name, id, col_types[id_type], col_1_name, name,  
               col_types[name_type], col_2_name, age, col_types[age_type]);  
    }  
      
    fprintf(stderr, "sqlite step exit with %d\n", ret);  
    sqlite3_finalize(stmt);  
    sqlite3_close(conn);  
      
    return 0;  
} 
