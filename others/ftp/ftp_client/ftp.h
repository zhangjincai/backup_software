#ifndef __FTP_H
#define __FTP_H

int Init_FTP(void);
int LogIn(char *user, char *password);
int LogOut(void);
int SendDataToFTP(char *name, char *pass, char *FileName, char *buf, int FileLen);

#endif
