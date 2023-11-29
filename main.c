#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    //Threads
/*
Pautas:
> Leer los elementos de un archivo al ejecutar el programa en paralelo mientras el usuario puede recorrer el menu.
> Poder mostrar los elementos de la lista.
*/
//      Constantes | Globales
#define archivoUsuarios "datos\\datoUsuario\\usuariosData.bin"

//      ESTRUCTURAS

typedef struct
{
    int dni;
    char nombre[40];
    int edadActual;

} stUsuario;

typedef struct
{
    stUsuario usuario;
    struct nodoUsuario * sig;
} nodoUsuario;

//      PROTOTIPADOS

void imprimirMenu();     //Imprime el texto del menu
void menu();             //El menu del ejercicio

//Nodos
nodoUsuario * inicLista();  //Retorna NULL
nodoUsuario * crearNodoUsuario(stUsuario usuarioNodeable);     //Creacion del nodo Usuario con el parametro recibido
nodoUsuario * agregarNodoUsuarioALista(nodoUsuario * lista, nodoUsuario * nuevoNodo);   //Se agrega el nodo recibido al final de la lista
nodoUsuario * cargarListaDesdeArchivo(nodoUsuario * lista);  //Carga la lista con todos los elementos del archivo
nodoUsuario * vaciarListaUsuario(nodoUsuario * lista);       //Va hasta el ultimo nodo de la lista y los libera hasta el principal, retorna null

//Archivos
void cargarUsuarioArchivo();                //Carga un stUsuario al archivo
void imprimirErrorAperturaArchivo();        //Imprime por pantalla un mensaje de error
void verArchivo();                          //Muestra todos los stUsuarios del archivo por pantalla

//Estructuras
stUsuario cargarstUsuario();                             //Carga los campos del stUsuario | Podria recibir el puntero tambien.
void mostrarstUsuario(stUsuario usuarioAMostrar);        //Muestra los campos de un stUsuario recibido por pantalla




//      FUNCIONES

int main()
{
    menu();
    return 0;
}


void menu()             //El menu del ejercicio
{
    nodoUsuario * lista = inicLista();

    pthread_t hiloCargarLista;
    pthread_t hiloVaciarLista;

    int opcion = 0;

    do
    {
        if(opcion == 0)
        {
            pthread_create(&hiloCargarLista, NULL, &cargarListaDesdeArchivo, lista);
        }
        imprimirMenu(); //Imprime las opciones del menu
        fflush(stdin);
        scanf("%i", &opcion);

        switch(opcion)
        {
        case 1:

            pthread_create(&hiloVaciarLista, NULL, &vaciarListaUsuario, lista); //Vacia la lista
            cargarUsuarioArchivo();
            pthread_join(hiloVaciarLista, &lista);  //Espera el regreso del hilo si no termino
            pthread_create(&hiloCargarLista, NULL, &cargarListaDesdeArchivo, lista);

            break;
        case 2:
            pthread_join(hiloCargarLista, &lista);
            if(lista != NULL)
            {
                imprimirListaUsuario(lista);
            }
            else
            {
                system("cls");
                printf("No hay usuarios cargados para mostrar\n");
            }
            system("pause");
            break;
        default:
            if(opcion != 0)
            {
                system("cls");
                printf("Valor Invalido de Opcion\n\n");
                system("pause");
            }
            break;
        }
    }
    while(opcion != 0);
}

void imprimirMenu()     //Imprime el texto del menu
{
    system("cls");
    printf("TITULO GENERICO\n");
    printf("1- Agregar Usuario\n");
    printf("2- Ver lista de usuarios\n");
    printf("0- Salir\n");
    printf("* Ingrese su eleccion \n>: ");

}
//---------------------------------------
//              NODOS USUARIO
//---------------------------------------

nodoUsuario * inicLista()       //  Retorna NULL para un nodoUsuario
{
    return NULL;
}

nodoUsuario * cargarListaDesdeArchivo(nodoUsuario * lista)  //Carga la lista con todos los elementos del archivo
{

    FILE * archivo = fopen(archivoUsuarios, "rb");
    stUsuario usuarioAux;
    if(archivo != NULL)
    {
        fseek(archivo, 0,SEEK_SET);
        while(fread(&usuarioAux, sizeof(stUsuario), 1, archivo) != 0)
        {
            lista = agregarNodoUsuarioALista(lista, crearNodoUsuario(usuarioAux));
        }
    }

    return lista;
}

nodoUsuario * agregarNodoUsuarioALista(nodoUsuario * lista, nodoUsuario * nuevoNodo)   //Se agrega el nodo recibido al final de la lista
{

    if(lista != NULL)
    {
        lista->sig = agregarNodoUsuarioALista(lista->sig, nuevoNodo);
    }
    else
    {
        lista = nuevoNodo;
    }
    return lista;
}


nodoUsuario * crearNodoUsuario(stUsuario usuarioNodeable)     //Creacion del nodo Usuario con el parametro recibido
{
    nodoUsuario * nuevoNodo = malloc(sizeof(nodoUsuario));

    nuevoNodo->usuario = usuarioNodeable;
    nuevoNodo->sig = inicLista();

    return nuevoNodo;
}


void imprimirListaUsuario(nodoUsuario * lista)
{
    if(lista != NULL)
    {

        imprimirNodoUsuario(lista);
        imprimirListaUsuario(lista->sig);
    }


}
void imprimirNodoUsuario(nodoUsuario * nodo)
{

    printf("------------------------------------------------\n");
    mostrarstUsuario(nodo->usuario);
    printf("| Nodo Actual:        < %p >\n", nodo);
    printf("| Nodo Siguiente:     < %p >\n", nodo->sig);
    printf("------------------------------------------------\n");
}


//VaciarLista
nodoUsuario * vaciarListaUsuario(nodoUsuario * lista)       //Va hasta el ultimo nodo de la lista y los libera hasta el principal, retorna null
{
    if(lista != NULL)
    {
        vaciarListaUsuario(lista->sig);
        free(lista);
    }
    return NULL;
}


//---------------------------------------
//          ARCHIVOS USUARIO
//---------------------------------------

void cargarUsuarioArchivo()
{
    mkdir("datos");                     //  Crea la carpeta "datos" si no existe
    mkdir("datos\\datoUsuario");        //  Crea la carpeta "datoUsuario" dentro de "datos" si no existe

    FILE * archivo = fopen(archivoUsuarios, "ab");
    stUsuario usuarioAux;


    if(archivo != NULL)
    {
        usuarioAux = cargarstUsuario();  // Si existe el archivo carga el auxiliar
        fseek(archivo, 0, SEEK_END);    //  Va al final del archivo
        fwrite(&usuarioAux, sizeof(stUsuario), 1, archivo);    //Lo carga

        fclose(archivo);
    }

}

void verArchivo()       //Muestra todos los stUsuarios del archivo por pantalla
{

    FILE * archivo = fopen(archivoUsuarios, "rb");
    stUsuario usuarioAux;
    if(archivo != NULL)
    {

        while(fread(&usuarioAux, sizeof(stUsuario), 1, archivo) != 0)
        {
            printf("-------------------------------\n");
            mostrarstUsuario(usuarioAux);
            printf("-------------------------------\n");

        }
        fclose(archivo);

    }
    else
    {
        imprimirErrorAperturaArchivo();
    }

}

void imprimirErrorAperturaArchivo()     //Imprime por pantalla un mensaje de error
{
    system("cls");
    printf("Error al abrir el archivo\n\n");
    system("pause");

}


//---------------------------------------
//              stUsuario
//---------------------------------------

stUsuario cargarstUsuario()     //Carga los campos del stUsuario | Podria recibir el puntero tambien.
{

    stUsuario nuevoUsuario;

    printf("> Ingrese el DNI del usuario\n>: ");
    fflush(stdin);
    scanf("%i", &nuevoUsuario.dni);

    printf("> Ingrese el Nombre del usuario\n>: ");
    fflush(stdin);
    gets(nuevoUsuario.nombre);

    printf("> Ingrese la EDAD del usuario\n>: ");
    fflush(stdin);
    scanf("%i", &nuevoUsuario.edadActual);

    return nuevoUsuario;
}

void mostrarstUsuario(stUsuario usuarioAMostrar)        //Muestra los campos de un stUsuario recibido por pantalla
{

    printf("| DNI:        %i  \n", usuarioAMostrar.dni);
    printf("| Nombre:     %s  \n", usuarioAMostrar.nombre);
    printf("| Edad:       %i  \n", usuarioAMostrar.edadActual);

}
