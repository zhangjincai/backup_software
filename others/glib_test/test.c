#include  <errno.h>
#include  <math.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <glib.h>
        
int main ( int argc, char *argv[] )
{
        GString * p_gstr = g_string_new(NULL);
        char* desStr = (char*)malloc(10);
        memset(desStr, '1', 9);                                                                                                                  desStr[9] = '\0';

        g_string_sprintfa(p_gstr, "\"post_summary\":\"%s\",", desStr);
        free(desStr);

        char* buf = (char*)malloc(p_gstr->len + 1);
        strncpy(buf, p_gstr->str, p_gstr->len);
        g_string_free(p_gstr, TRUE);                                        
        buf[p_gstr->len] = '\0';
        printf ("\nProgram %s\n\n", buf );
        free(buf);

        return EXIT_SUCCESS;
}             
