 #include <stdio.h>  
 #include <stdlib.h>  
 #include <stddef.h>  
 #include <string.h>  
 #include <sys/stat.h>
 #include <curl/curl.h>
 #include <time.h>
 #include <ctype.h>

 #include "json.h"  
 #include "lib_crypto.h"
 
 #define MAX_BUF    65536
 char wr_buf[MAX_BUF+1];
 int wr_index;

 //#define REMOTE_URL 	"http://14.23.145.210:7020/yctI/APPLogin"//岭南通自行车充值前生成订单
 #define REMOTE_URL 	"http://192.168.1.118:10086"
 #define LOCAL_FILE     "/tmp/upload.txt"

 #define CONNECTTIMEOUT 5
 #define TIMEOUT 		10

 #define CURL_TEST	1
 
 size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp );
 size_t read_data(void *ptr, size_t size, size_t nmemb, void *stream);

//加密流程：明文->base64->ascii 转换->3des 加密->密文
//解密流程：密文->3des 解密->ascii 转换->base64->明文

 int test_jsonc()	
 {	
	 time_t timestamp;
	 unsigned char key[128] = {0};

	 //获取时间截
	 time(&timestamp);
	 //printf("timestamp:%ld\n",timestamp);
	 
	 #if 0
     //时间截做MD5获取密钥
	 lib_md5_checksum((unsigned char *)&timestamp,sizeof(timestamp),(unsigned char *)&key);
	
	 int i;
	 printf("--------------------key--------------------\n");
	 for(i = 0; key[i] != '\0'; i++)
	 {
		printf("%x",key[i]);
	 }
	 printf("\n");
     #endif
	
	 #if CURL_TEST
	 //---------------------------------------------------
	 CURL *curl;
	 CURLcode ret;
	 FILE *hd_src;
 	 struct stat file_info;
 	 curl_off_t fsize;
	 int  wr_error = 0,wr_index = 0;
	 
	 const char *data = "data to send";
	 
	 curl = curl_easy_init();//初始化libcurl库
	 if (!curl) {
		printf("couldn't init curl!\n");
		return -1;
	 }
	 #endif

	 #if 0
	 //////////组数据方式1:由字符串构造为json对象///////////////
 	 struct json_object *newobj = NULL;

	 //const char *mystr="{\"timestamp\":1428042275, \"platform\":1, \"userinfo\":\"userid:gzzxc001\", \"type\": 1}";
	 const unsigned char *mystr="{\"platform\":1, \"userinfo\":\"userid:gzzxc001\", \"type\": 1}";

	 //将符合json格式的字符串构造为一个json对象
	 newobj = json_tokener_parse(mystr);
	 if(is_error(newobj)) 
	 {
		 printf("json_tokener_parse err!\n");
	 }
	 //添加json名称和值到json对象集合中
	 json_object_object_add(newobj, "timestamp", json_object_new_int(timestamp));  
	 

	 //明文做base64编码
	 char base64[512] = {0};
	 
	 lib_base64_encode(mystr,base64,strlen(mystr));
	 printf("----------------base64_encode-----------------\n%s\n",base64);
	
	 //base64编码后的字符串用16进制的ascii码表示
	 unsigned char ascii[512] = {0};
	 int len = 0;
	 
	 printf("------------------toascii--------------------\n");
	 for(i = 0; base64[i] != '\0'; i++)
	 {
	 	len++;
		ascii[i] = toascii(base64[i]);
		printf("%x",ascii[i]);
	 }
	 printf("\n");
	 //printf("len = %d\n",len);
	 
	 //对需要加密的字段做3DES加密
	 unsigned char desout[512] = {0};
 
	 printf("------------------des3_encrypt--------------------\n");
	 lib_des3_encrypt(desout,ascii,len,key);
	 for(i = 0; desout[i]; i++)
	 	printf("%x",desout[i]);
	 printf("\n");

     //把加密后的信息填入 data 字段
     
	 //MD5生成mac
	#endif
	
 	 //////////组数据方式2:直接构造json对象/////////////////
#if 0
	  struct json_object *infor_object = NULL;	
	  infor_object = json_object_new_object();	
	  if (NULL == infor_object)  
	  {  
		  printf("new infor_object failed.\n");	
		  return;  
	  }  
	  
	  struct json_object *para_object = NULL;  
	  para_object = json_object_new_object();  
	  if (NULL == para_object)	
	  {  
		  json_object_put(infor_object);//free	
		  json_object_put(newobj);//free	 
		  printf("new para_object failed.\n");	
		  return;  
	  }  
  
	  json_object_object_add(para_object, "timestamp", json_object_new_int(timestamp));  
	  json_object_object_add(para_object, "platform", json_object_new_int(1));  
	  json_object_object_add(para_object, "userinfo", json_object_new_string("userid:gzzxc001"));  
	  json_object_object_add(para_object, "type", json_object_new_int(1));  
#if 0
      //----------------测试-------------------
	  struct json_object *object1 = NULL;  
	  object1 = json_object_new_object();
	  json_object_object_del(para_object, "timestamp");
	  object1 = json_object_object_get(para_object, "timestamp"); 
	  printf("%s\n", json_object_to_json_string(object1));
#endif
	  
	  /*添加json名称和值到json对象集合中*/	
	  //json_object_object_add(newobj, "param", para_object);	  
	  json_object_object_add(infor_object, "param", para_object);	  

	  //将json对象以符合json格式的字符串打印
	  printf("----------------------json infor--------------------\n");	
	  printf("%s\n", json_object_to_json_string(newobj));  
	  printf("%s\n", json_object_to_json_string(infor_object)); 
	 
#endif	 

#if CURL_TEST
	 //----------------URL相关设置----------------//
	 ret = curl_easy_setopt(curl, CURLOPT_URL, REMOTE_URL);//指定url
	 ret = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, CONNECTTIMEOUT);//设置连接超时 s
	 ret = curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIMEOUT);//设置传输时间 s
     ret = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);//屏蔽其它信号
	 ret = curl_easy_setopt(curl, CURLOPT_HEADER, 1);//下载数据包括HTTP头部
	 ret = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);//CURL报告每一件意外的事情
	 ret = curl_easy_setopt(curl, CURLOPT_POST, 1);//正规的HTTP POST

	
	 //设置http发送的内容类型为JSON 
	 struct curl_slist *plist = NULL;
	 plist = curl_slist_append(plist,"Content-Type:application/json;charset=UTF-8");  
	 //plist = curl_slist_append(plist,"Content-Type:application/x-www-from-urlencoded;charset=UTF-8");  
	 ret = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);//设置消息头

	 //设置要POST的JSON数据
     //ret = curl_easy_setopt( curl, CURLOPT_POSTFIELDS, data);
     
#if 1
	 //--------------------download---------------------//
	 //ret = curl_easy_setopt( curl, CURLOPT_WRITEDATA, (void *)&wr_buf);
     //ret = curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );//下载
#else
	 //--------------------upload---------------------//
	 /* get the file size of the local file */ 
     if(stat(LOCAL_FILE, &file_info)) {
	     printf("can't open '%s'\n", LOCAL_FILE);
	     return 1;
     }
  	 fsize = (curl_off_t)file_info.st_size;
     printf("Local file size: %d bytes.\n", fsize);
 
     /* get a FILE * of the same file */ 
     hd_src = fopen(LOCAL_FILE, "rb");//只读方式打开一个二进制文件

	 ret = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);//上传
	 ret = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);//让libcurl做好上传的准备

	 /* now specify which file to upload */ 
     ret = curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
	 
	 /* Set the size of the file to upload (optional)*/
	 ret = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)fsize);
#endif  

   	 ret = curl_easy_perform(curl); //transfer http   	 
     if(ret != CURLE_OK){      
	 	fprintf(stderr,"curl_easy_perform() failed: %s, ret = %d\n",curl_easy_strerror(ret), ret);
     }

#endif

     //退出清理
     #if CURL_TEST
	 //fclose(hd_src); /* close the local file */   
     #endif
	 
	 //json_object_put(infor_object);//free	
	 //json_object_put(newobj);//free

	#if CURL_TEST
	 curl_slist_free_all(plist);	
     curl_easy_cleanup(curl);
     curl_global_cleanup();
	#endif
	
	 return 0;
 }	
   
   
int main(int argc, char *argv[])  
{	
	int ret = -1;

	ret = test_jsonc();	

	return 0;	
}

 //download时触发
 size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp )
 {
	printf("in write func.......\n");
	
	int segsize = size * nmemb;
	printf("write size:%d\n",segsize);
	
	if ( wr_index + segsize > MAX_BUF ) {
			*(int *)userp = 1;
			return 0;
	}
   
	memcpy( (void *)&wr_buf[wr_index], buffer, (size_t)segsize );
   
	wr_index += segsize;
   
	wr_buf[wr_index] = 0;
   
	return segsize;
 }

//upload时触发
size_t read_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  curl_off_t nread;

  printf("in read func.......\n");
  
  size_t retcode = fread(ptr, size, nmemb, stream);
  nread = (curl_off_t)retcode;
  fprintf(stderr, "*** We read %d bytes from file\n", nread);
  
  return retcode;
}

