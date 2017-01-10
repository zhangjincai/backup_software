#include <my_global.h>
#include <mysql.h>

int main(int argc, char **argv)
{
#if 1
  //write image
  MYSQL *conn;
  int len, size;
  char data[1000*1024];
  char chunk[2*1000*1024+1];
  char query[1024*5000];
  FILE *fp;
  
  conn = mysql_init(NULL);
  
  mysql_real_connect(conn, "localhost", "root", "314159", "test", 0, NULL, 0);
  
  //create table
  mysql_query(conn, "CREATE TABLE images(id int not null primary key, data mediumblob)");

  fp = fopen("./image.png", "rb");
  size = fread(data, 1, 1024*1000, fp);
  mysql_real_escape_string(conn, chunk, data, size);
  
  char *stat = "INSERT INTO images(id, data) VALUES('1', '%s')";
  len = snprintf(query, sizeof(stat)+sizeof(chunk) , stat, chunk);
  mysql_real_query(conn, query, len);
  
  fclose(fp);
  mysql_close(conn);
#endif

#if 1
  //read image
  //MYSQL *conn;
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned long *lengths;
  //FILE *fp;
  
  conn = mysql_init(NULL);
  mysql_real_connect(conn, "localhost", "root", "314159", "test", 0, NULL, 0);
  
  fp = fopen("image.png", "wb");
  mysql_query(conn, "SELECT data FROM images WHERE id=1");
  result = mysql_store_result(conn);
  row = mysql_fetch_row(result);
  lengths = mysql_fetch_lengths(result);
  printf("image lengths:%dbytes\n",*lengths);
  
  fwrite(row[0], lengths[0], 1, fp);
  
  mysql_free_result(result);
  fclose(fp);
  mysql_close(conn);
#endif
  return 0;
}
