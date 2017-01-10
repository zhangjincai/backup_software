/****************************************** 
  本文件学习mysql的database使用 
  学习mysql的C接口 
  包括初始化db，检查数据库是否存在，不存在就创建。 
  检查表是否存在，不存在则创建 
 *****************************************/  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
  
//需要包含mysql的头文件，/usr/include/mysql/  
//编译的时候需要加上-L/usr/lib/ -lmysqlclient -I/usr/include/mysql选项  
  
#include <mysql.h> //基本的头文件，一些重要结构体的声明  
#include <errmsg.h>//错误信息的存放  
#include <mysql_version.h>  //包含当前mysql的版本信息  
//////////////////全部变量，宏定义//////////////////////  
#define DEBUG  
#define SERVER_HOST "localhost"  //mysql的远程地址  
#define SERVER_USER "root"      //数据库登录名  
#define SERVER_PWD  "314159"  //数据库登录密码  
  
#define DB_NAME     "tmp_db"    //新建数据库的名字  
#define TABLE_NAME  "mytables"  //库中的表  
  
int check_tbl(MYSQL* mysql,char *name);  
int check_db(MYSQL *mysql,char *db_name);  
  
int init_db()  
{  
  
    int err=0;  
    MYSQL mysql;  
  
    if(!mysql_init(&mysql)){  
        perror("mysql_init:");  
        exit(1);  
    }     
  
    if(!mysql_real_connect(&mysql,SERVER_HOST,SERVER_USER,SERVER_PWD,NULL,0,NULL,0))  
    {     
        perror("mysql_real_connect");  
        exit(1);  
    }     
    printf("connected.....\n");  
  
    err = check_db(&mysql,DB_NAME);  
    if(err != 0)  
    {     
        printf("create db is err!\n");  
        mysql_close(&mysql);  
        exit(1);  
    }     
    //select which db  
        if(mysql_select_db(&mysql,DB_NAME)) //return 0 is success ,!0 is err  
    {  
        perror("mysql_select_db:");  
        mysql_close(&mysql);  
        exit(1);  
    }  
    //chuangjianbiao  
    if((err=check_tbl(&mysql,TABLE_NAME))!=0)  
    {  
        printf("check_tbl is err!\n");  
        mysql_close(&mysql);  
        exit(1);  
    }  
    mysql_close(&mysql);  
    return 0;  
}  
  
int check_db(MYSQL *mysql,char *db_name)  
{  
    MYSQL_ROW row = NULL;  
    MYSQL_RES *res = NULL;  
  
    res = mysql_list_dbs(mysql,NULL);  
    if(res)  
    {  
        while((row = mysql_fetch_row(res))!=NULL)  
        {  
            printf("db is %s\n",row[0]);  
            if(strcmp(row[0],db_name)==0)  
            {  
                printf("find db %s\n",db_name);  
                break;  
            }  
        }  
        //mysql_list_dbs会分配内存，需要使用mysql_free_result释放  
        mysql_free_result(res);  
    }  
    if(!row)  //没有这个数据库，则建立  
    {  
        char buf[128]={0};  
        strcpy(buf,"CREATE DATABASE ");  
        strcat(buf,db_name);  
#ifdef DEBUG  
        printf("%s\n",buf);  
#endif  
        if(mysql_query(mysql,buf)){  
            fprintf(stderr,"Query failed (%s)\n",mysql_error(mysql));  
            exit(1);  
        }  
    }  
    return 0;  
}  
  
int check_tbl(MYSQL* mysql,char *name)  
{  
    if(name == NULL)  
        return 0;  
    MYSQL_ROW row=NULL;  
    MYSQL_RES *res = NULL;  
    res = mysql_list_tables(mysql,NULL);  
    if(res)  
    {  
        while((row = mysql_fetch_row(res))!=NULL)  
        {  
            printf("tables is %s\n",row[0]);  
            if(strcmp(row[0],name) == 0)  
            {  
                printf("find the table!\n");  
                break;  
            }  
        }  
        mysql_free_result(res);  
    }  
    if(!row) //create table  
    {  
        char buf[128]={0};  
        char qbuf[128]={0};  
        snprintf(buf,sizeof(buf),"%s (name VARCHAR(20),sex char(1),score int(3));",TABLE_NAME);  
        strcpy(qbuf,"CREATE TABLE ");  
        strcat(qbuf,buf);  
//#ifdef DEBUG  
        printf("%s\n",qbuf);  
//#endif  
        if(mysql_query(mysql,qbuf)){  
            fprintf(stderr,"Query failed (%s)\n",mysql_error(mysql));  
            exit(1);  
        }  
    }  
    return 0;  
}  
  
int main()  
{  
    int err = 0;
  
    err = init_db();  
  
    return err;  
}
