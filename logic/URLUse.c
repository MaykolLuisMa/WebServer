#include "Tools.c"

char *ToURL(char *path);
char *GetURL(char *ServerPath,char *ServingDirectory);
char *URLDecode(char *path);
char *GetServerPath(char *path, char *ServingDirectory);


char *ToURL(char *path)
{
    char **components = Split(path,"/");
     
    char *code = (char *) malloc(1000);
     
    code[0] = 0; 

    if (components[0] == NULL) 
        strcat(code, "/");
    for (int i = 0; components[i] != NULL; i++) {
        char *codified;

        CURL *curl = curl_easy_init();

        codified = curl_easy_escape(curl, components[i], (int) strlen(components[i]));

        strcat(code, "/");
        strcat(code, codified);
        curl_easy_cleanup(curl);
    }
    return code;   
}

char *URLDecode(char *URL)
{
    //Este metodo decodifica el URL (direccion de recurso de internet) en un string plano 
    
    CURL *curl = curl_easy_init();
    
    char *output = curl_easy_unescape(curl,URL,(int)strlen(URL),NULL);
    
    curl_easy_cleanup(curl);
    
    return output;
}

char *GetURL(char *ServerPath,char *ServingDirectory)
{
    size_t ServerPathLength = strlen(ServerPath);
    size_t ServingDirectoryLength = strlen(ServingDirectory);
     
    char *browser = (char *) malloc((int)ServerPathLength + 1);
     
    for (size_t i = 0; i < ServerPathLength-ServingDirectoryLength; i++)
    {
        browser[i] = ServerPath[ServingDirectoryLength+i];
    }
    browser[(int)(ServerPathLength-ServingDirectoryLength)] = 0;
    return browser;

}




char *GetServerPath(char *ResourceURL, char *ServingDirectory)
{
    //decodifica el URL
    char *DecodedURL  = URLDecode(ResourceURL);
    

    //Obtengo la direccion de la carpeta deseada uniendo la direccion del directorio y el URL
    int DecodedURLLength = (int)strlen(DecodedURL);
    int ServingDirectoryLength = (int)strlen(ServingDirectory);
    char *serverpath = (char *)malloc(DecodedURLLength+ServingDirectoryLength+1);
    
    strcpy(serverpath,ServingDirectory);
    strcat(serverpath,DecodedURL);

    if(serverpath[DecodedURLLength+ServingDirectoryLength-1]=='/')
        serverpath[DecodedURLLength+ServingDirectoryLength-1] = 0;

    return serverpath;
}






