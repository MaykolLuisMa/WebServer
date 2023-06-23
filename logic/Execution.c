#include "SO.c"
void Execute( int ListeningPort,char *ServingDirectory);
void *ClientHandler(void *args);

void *ClientHandler(void *args)
{
    
    //Asigno los datos recibidos a variables mas comodas 
    struct ThreadArgs MyArgs = *(struct ThreadArgs *)args;
    int ConnectedSocked = MyArgs.ConnectedSocket;
    char *ServingDirectory = MyArgs.ServingDirectory;

    //Recibo el mensaje mensaje del cliente o user agent segun el protocolo HTTP y lo descompongo en
    //componentes manejables
    char UserAgentMessege[1024];
    int messege = recv(ConnectedSocked,UserAgentMessege,1024,0);

    if(messege<0)
    {
        printf("Recuperacion de datos del cliente fallida\n");
    }
    char **MessegeComponents = Split(UserAgentMessege," \t\r\n\a");


    //Del mensaje recibido solo nos interesa la linea inicial que contiene el metodo de peticion,
    //el cual debe ser GET, ya que solo nos interesa la recuperacion de recursos y
    //el URL del recurso



    if(MessegeComponents[0] != NULL && strcmp(MessegeComponents[0], "GET") == 0 && MessegeComponents[1] != NULL)
    {
        //Une la direccion del directorio y el URL para obtener la direccion a utilizar
        char *ServerPath = GetServerPath(MessegeComponents[1], ServingDirectory);
        
        int open = OpenPage(ServerPath, ServingDirectory, ConnectedSocked);

        if(open == 0)
        { 
            int down = Download(ServerPath,ConnectedSocked);
            if(down == 0)
            {
               printf("No se ha encontrado el directorio deseado\n");
            }
           
        }
       
    }    
    else
    {
        printf("Ha habido problemas con el mensaje HTTP del cliente\n");
    }
    close(ConnectedSocked);
    return NULL;
}

void Execute( int ListeningPort,char *ServingDirectory)
{

    //Obtengo un socket a la escucha para actuar como servidor
    
    int ListenSocket = GetListenSocket(ListeningPort);

    while (True)
    {
    
       //Acepto la coneccion pendiente. No nos interesa ningun dato de la pareja por lo que los 
       //otros argumentos seran NULL
       int ConnectedSocket = accept(ListenSocket,NULL,NULL);
        
       //Creo una estructura para almacenar los datos de interes para el cliente actual: El socket 
       //conectado y el directorio cuya informacion es requerida
       struct ThreadArgs args;
       args.ConnectedSocket = ConnectedSocket;
       
       args.ServingDirectory = (char *) malloc(1024);
       
       strcpy(args.ServingDirectory,ServingDirectory);

       //Creo un hilo que se encargara de atender al cliente actual
       pthread_t thread;
       pthread_create(&thread,NULL,ClientHandler,&args);

    }
    
}