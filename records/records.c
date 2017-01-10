#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "sqlite3.h"
#include "lib_records.h"



#define REC_VERSION						"lib records version 1.1.1 2016/04/13  10:15"


#define REC_CONSUME					"tbl_consume"
#define REC_EXCEPTION					"tbl_exception"
#define REC_RECONSUME					"tbl_reconsume"
#define REC_ONCARD_ACK					"tbl_oncard_ack"
#define REC_OFFCARD_ACK				"tbl_offcard_ack"
#define REC_RECHARGE					"tbl_recharge"


union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};   

struct records_file
{
	sqlite3 *db;
	char pathname[64];
	int sem_id;
	union semun arg;
};

void rec_sleep(const unsigned int s)
{
	struct timeval tv;

	tv.tv_sec = s;
	tv.tv_usec = 0;
	select(0, NULL, NULL, NULL, &tv);
}

void rec_msleep(const unsigned int ms)
{
	struct timeval tv;

	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms * 1000 % 1000000;

	select(0, NULL, NULL, NULL, &tv);
}

void rec_usleep(const unsigned int us)
{
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = us;
	select(0, NULL, NULL, NULL, &tv);
}

static int __sqlite_busy_callback(void *ptr, int count)
{
	if(count > 10)
		return 0;

	fprintf(stderr, "sqlite busy callback!\n");
	
	rec_usleep(1000); 	// 1ms
	return 1;
}

static int __sem_p(struct records_file *rec)
{
	struct sembuf P;	
	
	P.sem_num = 0;
	P.sem_op = -1; 	//表示为减1操作 
	P.sem_flg = SEM_UNDO;

	return semop(rec->sem_id, &P, 1);	
}

static int __sem_v(struct records_file *rec)
{
	struct sembuf V;
	V.sem_num = 0;
	V.sem_op = 1; //表示为加1操作 
	V.sem_flg = SEM_UNDO;

	return semop(rec->sem_id, &V, 1);
}

static int __sqlite3_create(struct records_file *rec)
{
	if(rec == NULL)
		return LIB_REC_ERROR;
	
	int rc = SQLITE_ERROR;
	char *errmsg = NULL;	
	char sql_cmd[512] = {0};

	rc = sqlite3_open(rec->pathname, &(rec->db));
	if(rc != SQLITE_OK)
	{
		sqlite3_close(rec->db);
		fprintf(stderr, "Can't open sqlite3 %s: %s\n", rec->pathname, sqlite3_errmsg(rec->db));
		return LIB_REC_ERROR;
	}

	/* sem mutex */
	int id = time(NULL) % 100;
	key_t key = ftok(rec->pathname, id);
	fprintf(stderr, "sem key: %d\n", key);

	rec->sem_id = -1;
	rec->sem_id = semget(key, 1, 0666 | IPC_CREAT);
	rec->arg.val = 1;
	semctl(rec->sem_id, 0, SETVAL, rec->arg);

	sqlite3_busy_handler(rec->db, __sqlite_busy_callback, (void*)rec->db);

	/* journal_mode */
	strcpy(sql_cmd, "PRAGMA journal_mode=OFF");
	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA journal_mode=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/* synchronous */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA synchronous=OFF");
	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA synchronous=OFF, errmsg = %s\n", errmsg);	
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/* case_sensitive_like */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	strcpy(sql_cmd, "PRAGMA case_sensitive_like=1");
	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "can't set PRAGMA case_sensitive_like=1, errmsg = %s\n", errmsg);
		
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}	
		goto ERR;
	}


	/*
	 * tbl_consume
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					pkt_sn INTEGER DEFAULT 0, pid CHAR(8), lid CHAR(8),  trade_record BLOB, \
					[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_CONSUME); 

	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_CONSUME, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}


	/*
	 * tbl_exception
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					sn INTEGER DEFAULT 0, pid CHAR(8), trade_record BLOB, \
					[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_EXCEPTION); 

	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_EXCEPTION, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}


	/*
	 * tbl_reconsume
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					pkt_sn INTEGER DEFAULT 0, pid CHAR(8), macaddr CHAR(6), pkt_len INTEGER DEFAULT 0, pkt_data BLOB, \
					[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_RECONSUME); 

	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_RECONSUME, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/*
	 * tbl_oncard_ack
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
						pkt_sn INTEGER DEFAULT 0, pid CHAR(8), macaddr CHAR(6), pkt_len INTEGER DEFAULT 0, pkt_data BLOB,  \
						[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_ONCARD_ACK); 

	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_ONCARD_ACK, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/*
	 * tbl_offcard_ack
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					pkt_sn INTEGER DEFAULT 0, pid CHAR(8), macaddr CHAR(6), pkt_len INTEGER DEFAULT 0, pkt_data BLOB,  \
					[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_OFFCARD_ACK); 

	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_OFFCARD_ACK, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}

	/*
	* tbl_recharge
	 */
	memset(sql_cmd, 0, sizeof(sql_cmd));
	sprintf(sql_cmd, "CREATE TABLE IF NOT EXISTS %s(idx INTEGER PRIMARY KEY AUTOINCREMENT, \
					pkt_sn INTEGER DEFAULT 0, pid CHAR(8), macaddr CHAR(6), pkt_len INTEGER DEFAULT 0, pkt_data BLOB,  \
					[last_time] TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))", REC_RECHARGE); 
	
	rc = sqlite3_exec(rec->db, sql_cmd, 0, 0, &errmsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "Can't create table {%s}, errmsg = %s\n", REC_RECHARGE, errmsg);
		if(errmsg != NULL)
		{
			sqlite3_free(errmsg);
			errmsg = NULL;
		}
		goto ERR;
	}
	






	if(errmsg != NULL)
	{
		sqlite3_free(errmsg);
		errmsg = NULL;
	}
	
	return LIB_REC_OK;

ERR:
	if(errmsg != NULL)
	{
		sqlite3_free(errmsg);
		errmsg = NULL;
	}

	if(rec->sem_id != -1)
	{
		semctl(rec->sem_id , 0, IPC_RMID, rec->arg);
		rec->sem_id = -1;
	}
	
	if(rec->db != NULL)
	{
		sqlite3_close(rec->db);
		rec->db = NULL;
	}
	
	return LIB_REC_ERROR;
}






char *lib_records_get_version(void)
{
	return REC_VERSION;
}


lib_records_file_t *lib_records_create(const char *pathname)
{
	if(pathname == NULL)
		return LIB_REC_NULL;

	int err = -1;
	struct records_file *rec = NULL;

	rec = (struct records_file *)malloc(sizeof(struct records_file));
	if(rec == NULL)
		goto ERR;

	memset(rec, 0, sizeof(struct records_file));

	strcpy(rec->pathname, pathname);
	rec->db = NULL;

	err = __sqlite3_create(rec);
	if(err != LIB_REC_OK)
		goto ERR;


	fprintf(stderr, "lib record create ok\n");
	return rec;

ERR:
	if(rec->sem_id != -1)
	{
		semctl(rec->sem_id , 0, IPC_RMID, rec->arg);
		rec->sem_id = -1;
	}
	
	if(rec != NULL)
	{
		free(rec);
		rec = NULL;
	}

	fprintf(stderr, "lib records create failed!\n");

	return LIB_REC_NULL;	
}

void lib_records_destroy(lib_records_file_t *rec)
{
	if(rec == NULL)
		return;

	if(rec->sem_id != -1)
	{
		semctl(rec->sem_id , 0, IPC_RMID, rec->arg);
		rec->sem_id = -1;
	}
	
	if(rec->db != NULL)
	{
		sqlite3_close(rec->db);
		rec->db = NULL;
	}

	free(rec);
	rec = NULL;	
}


int lib_records_consume_insert_data(lib_records_file_t *rec, const int pkt_sn, const unsigned char pid[8], const unsigned char lid[8], const unsigned char trade_record[90])
{
	if((rec == NULL) || (pid == NULL) || (lid == NULL) ||(trade_record == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(pkt_sn, pid, lid, trade_record) VALUES (?,?,?,?);", REC_CONSUME);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, pkt_sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, lid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 4, trade_record, 90, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;
}


int lib_records_exception_insert_data(lib_records_file_t *rec, const int sn, const unsigned char pid[8], const unsigned char trade_record[90])
{
	if((rec == NULL) || (pid == NULL) ||(trade_record == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(sn, pid, trade_record) VALUES (?,?,?);", REC_EXCEPTION);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, trade_record, 90, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;
}


int lib_records_reconsume_insert_data(lib_records_file_t *rec, const int pkt_sn, const unsigned char pid[8], const unsigned char macaddr[6], const int pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pid == NULL) || (macaddr == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(pkt_sn, pid, macaddr, pkt_len, pkt_data) VALUES (?,?,?,?,?);", REC_RECONSUME);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, pkt_sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, macaddr, 6, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, pkt_len);
	sqlite3_bind_blob(stmt, 5, pkt_data, pkt_len, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;	
}


int lib_records_reconsume_select_data(lib_records_file_t *rec, const int pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_len = 0;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE pkt_sn = %u;", REC_RECONSUME, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;		
}

int lib_records_reconsume_select_all_data(lib_records_file_t *rec, int *pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_sn == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_sn, p_pkt_len;

	sprintf(sql_cmd, "SELECT * FROM %s LIMIT 1;", REC_RECONSUME);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_sn = sqlite3_column_int(stmt, 1);
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_sn = p_pkt_sn;
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_OK;		
}

int lib_records_reconsume_delete_data(lib_records_file_t *rec, const int pkt_sn)
{
	if(rec == NULL)
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "DELETE FROM %s WHERE pkt_sn = %u;", REC_RECONSUME, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}


	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;		
}

int lib_records_reconsume_count(lib_records_file_t *rec, int *count)
{
	if((rec == NULL) || (count == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", REC_RECONSUME);
	
	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		*count = sqlite3_column_int(stmt, 0);
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(rec);
		return LIB_REC_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_ERROR;	//查询失败
}

int lib_records_oncard_ack_insert_data(lib_records_file_t *rec, const int pkt_sn, const unsigned char pid[8], const unsigned char macaddr[6], const int pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pid == NULL) || (macaddr == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(pkt_sn, pid, macaddr, pkt_len, pkt_data) VALUES (?,?,?,?,?);", REC_ONCARD_ACK);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, pkt_sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, macaddr, 6, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, pkt_len);
	sqlite3_bind_blob(stmt, 5, pkt_data, pkt_len, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;
}

int lib_records_oncard_ack_select_data(lib_records_file_t *rec, const int pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_len = 0;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE pkt_sn = %u;", REC_ONCARD_ACK, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;
}

int lib_records_oncard_ack_select_all_data(lib_records_file_t *rec, int *pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_sn == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_sn, p_pkt_len;

	sprintf(sql_cmd, "SELECT * FROM %s LIMIT 1;", REC_ONCARD_ACK);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_sn = sqlite3_column_int(stmt, 1);
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_sn = p_pkt_sn;
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;	
}


int lib_records_oncard_ack_delete_data(lib_records_file_t *rec, const int pkt_sn)
{
	if(rec == NULL)
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "DELETE FROM %s WHERE pkt_sn = %u;", REC_ONCARD_ACK, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}


	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;	
}

int lib_records_oncard_ack_count(lib_records_file_t *rec, int *count)
{
	if((rec == NULL) || (count == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", REC_ONCARD_ACK);
	
	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		*count = sqlite3_column_int(stmt, 0);
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(rec);
		return LIB_REC_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_ERROR;	//查询失败	
}

int lib_records_offcard_ack_insert_data(lib_records_file_t *rec, const int pkt_sn, const unsigned char pid[8], const unsigned char macaddr[6], const int pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pid == NULL) || (macaddr == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(pkt_sn, pid, macaddr, pkt_len, pkt_data) VALUES (?,?,?,?,?);", REC_OFFCARD_ACK);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, pkt_sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, macaddr, 6, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, pkt_len);
	sqlite3_bind_blob(stmt, 5, pkt_data, pkt_len, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;
}

int lib_records_offcard_ack_select_data(lib_records_file_t *rec, const int pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_len = 0;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE pkt_sn = %u;", REC_OFFCARD_ACK, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;	
}

int lib_records_offcard_ack_select_all_data(lib_records_file_t *rec, int *pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_sn == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_sn, p_pkt_len;

	sprintf(sql_cmd, "SELECT * FROM %s LIMIT 1;", REC_OFFCARD_ACK);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_sn = sqlite3_column_int(stmt, 1);
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_sn = p_pkt_sn;
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;		
}

int lib_records_offcard_ack_delete_data(lib_records_file_t *rec, const int pkt_sn)
{
	if(rec == NULL)
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "DELETE FROM %s WHERE pkt_sn = %u;", REC_OFFCARD_ACK, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}


	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;	
}

int lib_records_offcard_ack_count(lib_records_file_t *rec, int *count)
{
	if((rec == NULL) || (count == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", REC_OFFCARD_ACK);
	
	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		*count = sqlite3_column_int(stmt, 0);
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(rec);
		return LIB_REC_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_ERROR;	//查询失败		
}

int lib_records_recharge_insert_data(lib_records_file_t *rec, const int pkt_sn, const unsigned char pid[8], const unsigned char macaddr[6], const int pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pid == NULL) || (macaddr == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "INSERT INTO %s(pkt_sn, pid, macaddr, pkt_len, pkt_data) VALUES (?,?,?,?,?);", REC_RECHARGE);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{	
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}	
	
	sqlite3_bind_int(stmt, 1, pkt_sn);
	sqlite3_bind_blob(stmt, 2, pid, 8, SQLITE_STATIC);
	sqlite3_bind_blob(stmt, 3, macaddr, 6, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, pkt_len);
	sqlite3_bind_blob(stmt, 5, pkt_data, pkt_len, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	

	return LIB_REC_OK;	
}

int lib_records_recharge_select_data(lib_records_file_t *rec, const int pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_len = 0;

	sprintf(sql_cmd, "SELECT * FROM %s WHERE pkt_sn = %u;", REC_RECHARGE, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_OK;		
}

int lib_records_recharge_select_all_data(lib_records_file_t *rec, int *pkt_sn, int *pkt_len, void *pkt_data)
{
	if((rec == NULL) || (pkt_sn == NULL) || (pkt_len == NULL) ||(pkt_data == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	
	const void *p_pkt_data = NULL;
	int p_pkt_sn, p_pkt_len;

	sprintf(sql_cmd, "SELECT * FROM %s LIMIT 1;", REC_RECHARGE);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		p_pkt_sn = sqlite3_column_int(stmt, 1);
		p_pkt_len = sqlite3_column_int(stmt, 4);
		p_pkt_data = sqlite3_column_blob(stmt, 5);

		if(p_pkt_len > 512)
			return LIB_REC_ERROR;
	
		if(p_pkt_data != NULL)
		{
			*pkt_sn = p_pkt_sn;
			*pkt_len = p_pkt_len;
			memcpy(pkt_data, p_pkt_data, p_pkt_len);
			
			sqlite3_finalize(stmt);
			
			__sem_v(rec);
			return LIB_REC_OK;		
		}
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;		
}

int lib_records_recharge_delete_data(lib_records_file_t *rec, const int pkt_sn)
{
	if(rec == NULL)
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "DELETE FROM %s WHERE pkt_sn = %u;", REC_RECHARGE, pkt_sn);

	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_DONE)  //执行成功返回SQLITE_DONE
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}


	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	
	return LIB_REC_OK;		
}

int lib_records_recharge_count(lib_records_file_t *rec, int *count)
{
	if((rec == NULL) || (count == NULL))
		return LIB_REC_ERROR;

	if(rec->db == NULL)
		return LIB_REC_ERROR;	

	int rc = SQLITE_ERROR;
	char sql_cmd[128] = {0};
	sqlite3_stmt *stmt = NULL;	

	sprintf(sql_cmd, "SELECT COUNT(*) FROM %s;", REC_RECHARGE);
	
	__sem_p(rec);
	rc = sqlite3_prepare_v2(rec->db, sql_cmd, strlen(sql_cmd), &stmt, NULL);
	if(rc != SQLITE_OK)
	{
		if(stmt != NULL)
			sqlite3_finalize(stmt);
		
		__sem_v(rec);
		return LIB_REC_ERROR;
	}

	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW)  //如果rc==SQLITE_DONE，说明查询失败
	{
		*count = sqlite3_column_int(stmt, 0);
		if(stmt != NULL)
			sqlite3_finalize(stmt);
			
		__sem_v(rec);
		return LIB_REC_OK;		
	}

	if(stmt != NULL)
		sqlite3_finalize(stmt);
	__sem_v(rec);
	
	return LIB_REC_ERROR;	//查询失败	
}










