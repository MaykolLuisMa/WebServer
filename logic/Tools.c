#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <curl/curl.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <time.h>
#include <asm-generic/errno.h>
#include <errno.h>


#define True 1
#define False 0
#define BufferSize 10240

char *GetSize(unsigned long size);
void GetDirection(char *path);
char **Split(char *sentence,char *caracters);

char *GetSize(unsigned long size)
{
    char *output = malloc(200); 
    char *unity = " bytes";

    if(size > 1024)
    {
        size /=1024;
        unity = " kb";
        if (size > 1024) 
        {
            size /= 1024;
            unity = " mb";

            if (size > 1024) 
            {
                size /= 1024;
                unity = " gb";
            }
        }
    } 
    sprintf(output,"%ld",size);
    strcat(output,unity);

}

void GetDirection(char *path) 
{
    int index = (int) 
        strlen(path) - 1;
    while (path[index] != '/') 
    {
        index--;
        if (index == -1) 
            break;
    }
    path[index + 1] = 0;
}

char **Split(char *sentence,char *caracters)
{
     
    //Separa el contenido de sentence en palabras donde los caracteres de caracters actuan como
    //delimitadores
    
    char **Words = malloc(1024 * sizeof(char *));
    
    char *word;   
    word = strtok(sentence,caracters);
    for (int i = 0;; i++)
    {
        Words[i] = word;
        if(word == NULL)
            break;
        word = strtok(NULL,caracters);
    }
    return Words;
    
}