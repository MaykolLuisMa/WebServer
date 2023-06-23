#include "URLUse.c"

int OpenPage(char *ServerPath,char *ServingDirectory, int ConnectedSocked);
char **LoadPage();
char *GetHTMLPage( DIR *directory,char *ServerPath,char *ServingDirectory);
void BuildBackFolder(char *CompletePage,char *ServerPath,char *ServingDirectory);
int Download(char *ServerPath, int ConnectedSocket);
void CreateDateColumn(char *CompletePage, struct stat propieties);
void CreateSizeColumn(char *CompletePage, struct stat propieties);
void CreateNameColumn(char *CompletePage,char *ServerPath,char *ServingDirectory,struct stat propieties,char *dd_name);
void CreateRow(char *CompletePage,char *RowName, char *ServerPath, char *ServingDirectory, struct stat propieties);


int OpenPage(char *ServerPath,char *ServingDirectory, int ConnectedSocked)
{
    //abro la carpeta a mostrar
    DIR *directory = opendir(ServerPath);
    //devolvera 0 si falla

    if(directory)
    {
        //Contruyo la pagina HTML con el contenido de la carpeta
        char *page = GetHTMLPage(directory, ServerPath, ServingDirectory);

        //Muestro la pagina en el navegador
        int SendOut = send(ConnectedSocked, page, strlen(page), 0);
        
        if (SendOut < 0) 
        {
            printf("La pagina HTML no se pudo mostrar correctamente\n");
            return 1;
        }
        closedir(directory);
        return 1;
    }
    else
    {
        if(errno == EACCES)
        {
            printf("Acceso denegado al directorio: %s\n",ServerPath);
            closedir(directory);
            return 1;
        }
        printf("El directorio a mostrar no se pudo abrir correctamente\n");
        printf("Direccion indicada: %s\n",ServerPath);
        closedir(directory);
        return 0;
    }
}

char *GetHTMLPage( DIR *directory,char *ServerPath,char *ServingDirectory)
{

    char *CompletePage= (char *) malloc(BufferSize*2);

    //Obtengo la pagina original dividida en dos mitades. Entre ellas se pueden
    //definir nuevos elementos de la tabla
    char **page = LoadPage();

    //le indico al string que contendra codigo HTML y le copio la primera mitad
    //de la pagina original
    strcpy(CompletePage,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    strcat(CompletePage,page[0]);

    //Creo la fila que indica la carpeta anterior
    BuildBackFolder(CompletePage,ServerPath,ServingDirectory);

    //Creo una estructura para almacenar al archivo al que se esta apuntando actualmente y otra para
    //almacenar sus propiedades
    struct dirent *d;
    struct stat propieties;
    
    //Ahora paso a crear el resto de las filas de la tabla
    int mult = 2;
    while ((d = readdir(directory)) != NULL)
    {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name,"..")==0)
            continue;

        //Obtengo las propiedades de la estrucura de la carpeta a la que se apunta actualmente
        char ActualPoint[(int)(strlen(ServerPath)+strlen(d->d_name))+1];
        strcpy(ActualPoint, ServerPath);
        strcat(ActualPoint, "/");
        strcat(ActualPoint, d->d_name);
        stat(ActualPoint,&propieties);
        
        //Creo la fila correspondiente a esa estructura
        CreateRow(CompletePage,d->d_name,ServerPath,ServingDirectory,propieties);
        
        //aumentamos el espacio asignado a la pagnina en caso de ser necesario
        if(BufferSize * mult - strlen(CompletePage) < BufferSize)
        {
            mult *= 2;
            CompletePage = (char *) realloc(CompletePage, BufferSize * mult);
        }
    }
    strcat(CompletePage,page[1]);

    return CompletePage;
    
}

int Download(char *ServerPath, int ConnectedSocket) 
{
    //abrimos la carpeta deseada solo para leer su contenido
    int fd = open(ServerPath, O_RDONLY);
    if (fd == -1) 
    {
        printf("No se pude abrir la carpeta: %s\n",ServerPath);
        return 0;
    }
    off_t Myoff = 0;

    //Obtenemos las probpiedades de la carpeta 
    struct stat propieties;
    if (fstat(fd, &propieties) == -1) 
    {
        fprintf(stderr, "No se pueden acceder a las propiedades del directorio: %s\n",ServerPath);
        return 1;
    }

    char DocHead[1024];
    snprintf(DocHead, sizeof(DocHead), "HTTP/1.1 200 OK\r\n" "Content-Type: application/octet-stream\r\n" "Content-Disposition: attachment; filename=\"%s\"\r\n" "Content-Length: %ld\r\n" "\r\n", ServerPath, propieties.st_size);

    if (send(ConnectedSocket, DocHead, strlen(DocHead), 0) == -1) 
    {
        printf("Envio del codigo fallido\n");
    }
    else if(sendfile(ConnectedSocket, fd, &Myoff, propieties.st_size) == -1)
    {
        printf("Envio del directorio fallido\n");
    }
    close(fd);
    return 1;
}

void CreateRow(char *CompletePage,char *RowName, char *ServerPath, char *ServingDirectory, struct stat propieties)
{
    strcat(CompletePage, "<tr>");

    CreateNameColumn(CompletePage,ServerPath,ServingDirectory,propieties,RowName);
        
    CreateSizeColumn(CompletePage,propieties);
        
    CreateDateColumn(CompletePage,propieties);

    strcat(CompletePage, "</tr>");

}

void CreateDateColumn(char *CompletePage, struct stat propieties) 
{
    //Muestra la fecha de la ultima modificacion
    strcat(CompletePage, "<td class=\"center\">");
    
    struct tm *MyTime = localtime(&propieties.st_mtime);

    char temp[64];
    sprintf(temp, "%d-%02d-%02d %02d:%02d:%02d", MyTime->tm_year + 1900, MyTime->tm_mon + 1, MyTime->tm_mday,MyTime->tm_hour, MyTime->tm_min, MyTime->tm_sec);
    
    strcat(CompletePage, temp);
    strcat(CompletePage, "</td>");
}
void CreateSizeColumn(char *CompletePage, struct stat propieties) 
{
    //Creo el encabezado
    strcat(CompletePage,"<td class=\"center\">");
    

    //Imprimo el size en caso de que sea un archivo
    if (!S_ISDIR(propieties.st_mode)) 
    {
        strcat(CompletePage, GetSize(propieties.st_size));
    } 
    else 
    {   
        strcat(CompletePage," ");
    }
    strcat(CompletePage, "</td>");
}

void CreateNameColumn(char *CompletePage,char *ServerPath,char *ServingDirectory,struct stat propieties,char *dd_name)
{
    //Encabezo la fila
    strcat(CompletePage, "<td>");
    strcat(CompletePage, "<a ");
    strcat(CompletePage, "onmouseover=\"this.style.color='blue'\" onmouseout=\"this.style.color='black'\" style=\"color: black; text-decoration: none;\"");
    strcat(CompletePage, "href=\"");

    //Obtengo el URL y el nombre del archvio o directorio y los enlazo a la pagina
    char *URL = GetURL(ServerPath,ServingDirectory);
    char *temp = (char *) malloc(strlen(URL) + strlen(dd_name) + 2); 
    strcpy(temp,URL );
    strcat(temp, "/");
    strcat(temp, dd_name);
    char *code = ToURL(temp);
    strcat(CompletePage,code);

    //Lo separo segun file o folder
    if(S_ISDIR(propieties.st_mode))
    {
        strcat(CompletePage, "\"><span><div class=\"folder\"></div> ");
    } 
    else 
    {
        strcat(CompletePage, "\"><span><div class=\"file\"></div> ");
    }

    strcat(CompletePage, dd_name);
    //cierro la celda
    strcat(CompletePage, "</span></a>");
    strcat(CompletePage, "</td>");
}

char **LoadPage()
{
    //Abro la pagina html creada en modo solo lectura
    FILE *page = fopen("../visual/Page.html","r");

    //Copio el texto de la pagina 
    char text[BufferSize];
    fread(text,BufferSize,1,page);

   
    //Divido la pagina en dos mitades, quedando entre ellas zona para crear nuevas
    //filas en la tabla a mostrar en pantalla     
    char **output = (char **) malloc(2 * sizeof(char *));

    char *segment = strstr(text, "<!--Marc-->");
    output[1] = (char *) malloc(strlen(segment) + 1);
    strcpy(output[1], segment);

    text[segment - text] = 0;
    output[0] = (char *) malloc(strlen(text) + 1);
     
    strcpy(output[0], text);

    fclose(page);

    return output;

}

void BuildBackFolder(char *CompletePage,char *ServerPath,char *ServingDirectory)
{

    //Indico la creacion de una nueva fila en la pagina
    strcat(CompletePage, "<td>");
    strcat(CompletePage, "<a href=\"");

    //Obtengo el URL
    
    char *browser = GetURL(ServerPath,ServingDirectory);

    GetDirection(browser);

    //Codifico nuevamente el URL y lo copio a la pagina haciendo que la fila este asociada a este 
    char *codified = ToURL(browser);
    
    strcat(CompletePage,codified);
    strcat(CompletePage, "\">");

    //completo la creacion de la fila
    strcat(CompletePage, "<div class=\"gg-arrow-left\"></div></a></td>");
    strcat(CompletePage, "<td class=\"center\"></td><td class=\"center\"></td></tr>");

}