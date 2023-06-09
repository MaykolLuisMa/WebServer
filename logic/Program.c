#include "Execution.c"
#define DefaultPort 8888

char *DefaultServingDirectory()
{
    //Obtiene el directorio de trabajo actual
    
    char *CurrentDirectory = malloc(300);
    
    if(getcwd(CurrentDirectory,300) == NULL)
    {
        printf("No de puede obtener el direcorio\n");
        exit(EXIT_FAILURE);
    }
    return CurrentDirectory;
}

int main(int argc,char *argv[])
{
    //ignorar SIGPIPE(cuando envias un socket no conectado o una ruptura en un pipe)
    signal(SIGPIPE,SIG_IGN);

    //Puerto de escucha
    int ListeningPort;
    //desde donde se va a mostrar informacion
    char *ServingDirectory;

    if(argc >= 2)
    {
        //El segundo argumento correspondera al puerto
        ListeningPort = atoi(argv[1]);
    }
    else
    {
        //Establecemos un puerto de escucha por defecto en caso que no se pase el 
        //argumento pertinente
        ListeningPort = DefaultPort;
    }
    if(argc >= 3)
    {
        //El tercer argumento sera el directorio cuya informacion sera mostrada
        ServingDirectory = argv[2];   
    }
    else
    {
        //Se establece un directorio por defecto
        ServingDirectory = DefaultServingDirectory();
    }

    printf("Lisening in port: %d\n",ListeningPort);
    printf("Serving Directory: %s\n", ServingDirectory);

    //Comenzamos la ejecucion del programa con los datos obtenidos
    Execute(ListeningPort,ServingDirectory);
        
}
