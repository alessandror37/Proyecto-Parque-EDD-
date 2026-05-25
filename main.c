#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define CAPACIDAD_MAX 100
#define MAX_ID_VISITANTES 100000 /*sintaxis para la generacion de id: id = rand() % (MAX_ID_VISITANTES + 1)*/


struct Entrada{
    int idEntrada;
    int tipo; /*0-> Entrada general, 1-> Pase infantil, 2-> Entrada familiar, 3 -> Pase VIP*/
    int valor;
    char *estado;
};


/*Lista simplemente enlazada con nodo fantasma*/
struct NodoEntrada{
    struct Entrada *datos;
    struct NodoEntrada *sig;
};

struct	Visitante{
    int idVisitante; /*Al ser manejados con árboles, vamos a tener que investigar sobre como manejar los ids para
    obtener un arbol binario*/
    int boolEstaEnParque; /*0-> No esta en parque, 1 -> Si esta en parque*/
    char *rut;
    char *nombre;
    struct NodoEntrada *headEntradas; /*head lista entradas de cada visitante*/
    struct Zona *zonaActual;
};

/*ABB*/
struct NodoVisitante{
    struct Visitante *datos;
    struct NodoVisitante *izq,*der;
};

/*Lista simple circular con nodo fantasma*/
struct NodoFila{
    struct Visitante *datos;
    struct NodoFila *sig;
};

struct Atraccion{
    int estado; /*0 -> Operativa, 1 -> En mantenimiento, 2 ->  Fuera de servicio, 3 -> Cerrdada por horario*/
    int capacidad;
    struct Visitante visitantesEnAtraccion[CAPACIDAD_MAX]; /*La capacidad real de la atracción es la misma que se encuentra
    dentro del struct y es la que se va a tomar en cuenta para la lógica de las funciones*/
    int duracion; /*duración en minutos*/
    float alturaMinima;/*altura en metros*/
    int edadMinima;
    struct Nodofila *headFila; /*head a la lista de la fila de la atracción*/
    int visitantesTotales;
};

/*Lista doblemente enlazada con nodo fantasma*/
struct NodoAtraccion{
    struct Atraccion *datos;
    struct NodoAtraccion *ant,*sig;
};


struct Zona{
    char *nombre;
    int codigo;
    char *tematica;
    char *horaInicio;
    char *horaCierre;
    int capacidad;
    int personalEncargado;
    struct NodoAtraccion *headAtracciones; /*head a la lista de atracciones*/
};


struct Parque {
    int recaudacionTotal;
    int totalVisitantes;
    struct Zona **zonas;
    int pLibreZonas; /*pLibre para array de zonas*/
    struct NodoVisitante *headVisitantes; /*head a la raiz de arbol visitantes*/
};

int agregarVisitante() {

}

int contarVisitantesEnParque(struct NodoVisitante *raiz) {
    int cont = 0;

    if (raiz == NULL) {
        return 0;
    }
    cont += contarVisitantesEnParque(raiz->izq);
    cont += contarVisitantesEnParque(raiz->der);

    return cont + raiz->datos->boolEstaEnParque;
}
void mostrarMenuVisitantes(void) {

}



int main(void) {
    int opcionMenu;

    printf("Bienvenido al menu de IBCLandia\n");
    printf("1.- Menu de visitantes\n");
    printf("2.- Menu de entradas");
    printf("3.- Menu de atracciones");
    printf("4.- Menu de zonas");
    printf("5.- Menu de datos");

    printf("Ingrese operacion deseada:");
    scanf("%d",&opcionMenu);
    switch (opcionMenu) {
        case 1:
            mostrarMenuVisitantes();
        default:
            printf("Ingrese una opcion valida.");


    }
    return 0;
}
