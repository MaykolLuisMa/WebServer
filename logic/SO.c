#include "HTMLConstruction.c"
int GetListenSocket(int ListeningPort);

struct sockaddr_in ServerDates(char *ServerIP, int ServerPort)
{
   struct sockaddr_in server = {0};
   server.sin_family = AF_INET;            
    server.sin_port = htons(ServerPort);   
    inet_aton(ServerIP, &server.sin_addr);
    return server;
}
struct ThreadArgs
{
    int ConnectedSocket;//Socket resultado de aceptar la coneccion de un cliente
    char *ServingDirectory;//Directorio cuya informacion es requerida
};

int GetListenSocket(int ListeningPort)
{
    //Creo un socket
    int socketfd = socket(AF_INET,SOCK_STREAM,0);
    if(socketfd < 0)
    {
        printf("Error al crear el socket del servidor\n");
        exit(EXIT_FAILURE);
    }

    //Establece las opciones del comportamiento del socket: Si este tiene valor 1 se establece
    //a nivel de socket(SOL_SOCKET) que las reglas para validar las direcciones proporcionadas por
    //bind deben permitir la reutilizacion de direccciones locales
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    //Creo la estructura sockaddr_in
    struct sockaddr_in serverdates = ServerDates("localhost",ListeningPort);

    //Vinculo el soccket creado con el puerto ListeningPort
    int BindSuccess = bind(socketfd,(struct sockaddr *)&serverdates,sizeof(serverdates));
    if(BindSuccess < 0)
    {
        printf("El socket no se puede vincular con el puerto indicado\n");
        exit(EXIT_FAILURE);
    }

    //Establece el socket a la escucha con solo un elemento a la espera por conectarse como maximo    
    int ListenSuccess = listen(socketfd,1);
    if(ListeningPort < 0)
    {
        printf("El socket no se pudo poner a la escucha\n");
        exit(EXIT_FAILURE);
    }
    return socketfd;
}