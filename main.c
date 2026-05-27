#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#define CAPACIDAD_MAX 100
#define MAX_ID_VISITANTES 100000 /*sintaxis para la generacion de id: id = rand() % (MAX_ID_VISITANTES + 1)*/


char fechaActual[10];
int recaudacionEntradas[4]; /*En cada elemento del array se guarda la recaudacion en su respectivo estado
Ej: un visitante compra una entrada general, entonces se realiza recaudacionEntradas[entrada->tipo] += entrada->valor*/

struct Entrada{
    int idEntrada;
    int tipo; /*0-> Entrada general, 1-> Pase infantil, 2-> Entrada familiar, 3 -> Pase VIP*/
    int valor;
    int estado; /*0-> activa, 1-> utilizada, 2->anulada, 3-> vencida  */
    char *fechaUsada;
};

/*Lista simplemente enlazada con nodo fantasma*/
struct NodoEntrada{
    struct Entrada *datos;
    struct NodoEntrada *sig;
};

struct Visitante{
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
    char *nombre;
    int estado; /*0 -> Operativa, 1 -> En mantenimiento, 2 -> Fuera de servicio, 3 -> Cerrada por horario */
    int capacidad;
    struct Visitante visitantesEnAtraccion[CAPACIDAD_MAX]; /*La capacidad real de la atracción es la misma que se
    encuentra dentro del struct y es la que se va a tomar en cuenta para la lógica de las funciones*/
    int duracion; /*duración en minutos*/
    float alturaMinima;/*altura en metros*/
    int edadMinima;
    struct NodoFila *headFila; /*head a la lista de la fila de la atracción*/
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

char *pasarAMinus(char *cadena){
    int i;
    int largo = strlen(cadena) + 1;
    char *NuevaCadena = (char *)malloc(sizeof(char) * largo);

    for(i = 0; cadena[i] != '\0'; i++){
        NuevaCadena[i] = tolower((unsigned char)cadena[i]);
    }
    return NuevaCadena;


struct Visitante *buscarVisitantePorID(struct NodoVisitante *raiz, int idVisitanteBuscar) {
    if (raiz == NULL) return NULL;
    if (raiz->datos->idVisitante==idVisitanteBuscar) {
        return raiz->datos;
    }
    if (raiz->datos->idVisitante < idVisitanteBuscar) {
        return buscarVisitantePorID(raiz->der,idVisitanteBuscar);
    } else {
        return buscarVisitantePorID(raiz->izq, idVisitanteBuscar);
    }
}

int crearIdNuevoVisitante(struct NodoVisitante *raiz) {
    int idNuevo = 0, esUnica = 0;
    while (esUnica != 1) {
        idNuevo = rand() % (MAX_ID_VISITANTES + 1);
        if (buscarVisitantePorID(raiz, idNuevo) == NULL) {
            esUnica = 1;
        }
    }
    return idNuevo;
}

struct Visitante *crearVisitante(struct NodoVisitante *raiz) {
    int idVisitanteNuevo;
    struct Visitante *visitanteNuevo = malloc(sizeof(struct Visitante));

    idVisitanteNuevo = crearIdNuevoVisitante(raiz);
    visitanteNuevo ->idVisitante = idVisitanteNuevo;

    visitanteNuevo->nombre = malloc(sizeof(char)*50);
    visitanteNuevo->rut = malloc(sizeof(char)*12);


    printf("Ingresar nombre completo del visitante: ");
    fgets(visitanteNuevo->nombre,50, stdin);

    printf("Ingresar rut del visitante: ");
    fgets(visitanteNuevo->rut,12, stdin);

    visitanteNuevo->boolEstaEnParque = 0;
    visitanteNuevo->zonaActual = NULL;
    visitanteNuevo->headEntradas = NULL;

    return visitanteNuevo;
}
struct NodoVisitante *buscarNodoParaVisitanteNuevo(struct NodoVisitante *raiz, int idVisitanteNuevo) {
    struct NodoVisitante *rec;
    if (raiz != NULL) {
        rec = raiz;
        while (rec!= NULL) {
            if (rec->datos->idVisitante < idVisitanteNuevo) {
                rec = rec->der;
            }else {
                rec = rec->izq;
            }
        }
    }
}

struct NodoVisitante *crearYAgregarNodoVisitante(struct NodoVisitante *raiz) {
    struct NodoVisitante *nodoNuevo = malloc(sizeof(struct NodoVisitante));
    nodoNuevo->datos=crearVisitante(raiz);

    return nodoNuevo;
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

struct Entrada *buscarEntradaPorId(*struct NodoVisitante raiz, int idEntrada) {

}

struct Entrada *crearEntrada(void) {
    int idEntradaNueva;
}

void comprarEntradaVisitante(struct Visitante *visitante) {
    printf("Ingrese entrada que desea comprar");
}

int validarEntradaVisitante(struct Visitante *visitante, int idEntrada) {

}

void mostrarMenuVisitantes(void) {

}




//Retorna un arreglo dinamico de punteros a las atracciones NO OPERATIVAS//



int contarAtraccionesNoOperativasEnZona (struct NodoAtraccion *headAtracciones) {
    int contador = 0;
    struct NodoAtraccion *rec = headAtracciones->sig;

    while (rec != NULL) {
        if (rec->datos->estado != 0) {
            contador++;
        }
        rec = rec->sig;
    }

    return contador;
}

int contarAtraccionesNoOperativas (struct Zona ** zonas, int plibre) {
    int contador = 0;
    int i;
    for (i = 0; i < plibre; i++) {
        if(zonas[i] != NULL) {
            contador+= contarAtraccionesNoOperativasEnZona(zonas[i]->headAtracciones);
        }
    }

    return contador;
}


struct Atraccion ** obtenerArregloAtraccionesNoOperativas (struct Zona ** zonas, int plibre) {
    struct Atraccion ** ArregloNoOperativas;
    int cantidadNoOperativas;
    int i;
    int posicion = 0;
    struct NodoAtraccion *rec = NULL;

    cantidadNoOperativas = contarAtraccionesNoOperativas (zonas, plibre);
    if (cantidadNoOperativas == 0) return NULL;

    ArregloNoOperativas = (struct Atraccion **) malloc(cantidadNoOperativas * sizeof (struct Atraccion *));

    for (i = 0;  i <plibre; i++) {
        if(zonas[i] != NULL) {
            rec = zonas[i]->headAtracciones->sig;
            while (rec != NULL) {
                if (rec->datos->estado != 0) {
                    ArregloNoOperativas[posicion] = rec->datos;
                    posicion++;
                }
                rec = rec->sig;
            }
        }
    }

    return ArregloNoOperativas;
}

void RecorrerArbolAnadiendo (struct NodoVisitante *headVisitantes, struct Visitante ** arreglo, int *posicion) {
    struct Visitante *datos;
    if (headVisitantes == NULL) return;
    datos = headVisitantes->datos;

    RecorrerArbolAñadiendo(headVisitantes->izq,arreglo,posicion);


    if (datos->boolEstaEnParque == 1) {
        arreglo[*posicion] = datos;
        (*posicion)++;
    }


    RecorrerArbolAñadiendo(headVisitantes->der,arreglo,posicion);

}

struct Visitante **DentroDelParque (struct Parque *IbcLandia) {
    struct Visitante ** VisitantesEnElParque;
    int contador;
    int posicion = 0;

    if (IbcLandia == NULL) return NULL;

    contador = contarVisitantesEnParque(IbcLandia->headVisitantes);

    if (contador == 0) return NULL;

    VisitantesEnElParque = (struct Visitante **) malloc (contador * sizeof (struct Visitante *));

    RecorrerArbolAñadiendo(IbcLandia->headVisitantes,VisitantesEnElParque,&posicion);

    return VisitantesEnElParque;


}


// atraccion con mas visitantes en espera //

int cantidadEnFila (struct  NodoFila *fila) {
    int contador = 0;
    struct NodoFila *rec;
    if (fila->sig == NULL) return 0;

    rec = fila->sig;

    while (rec != NULL) {
        contador++;
        rec = rec->sig;

    }
    return contador;

}

struct Atraccion *atraccionConMasEspera (struct NodoAtraccion *headAtracciones, int *cantFila) {
    struct NodoAtraccion *rec;
    struct Atraccion *MayorFila = NULL;
    int cantidadEnEsaFila;
    if (headAtracciones->sig == NULL) return NULL;
    rec = headAtracciones->sig;
    while (rec != NULL) {
        cantidadEnEsaFila = cantidadEnFila(rec->datos->headFila);
        if (cantidadEnEsaFila > *cantFila) {
            MayorFila = rec->datos
            *cantFila = cantidadEnEsaFila;
        }
        rec = rec->sig;


    }

    return MayorFila;
}

struct Atraccion *AtraccionMayorFilaEntreZonas(struct Zona ** zonas, int plibre ) {
    int i;
    struct Atraccion *atraccionCampeonaGlobal = NULL;
    struct Atraccion *atraccionCandidataZona = NULL;
    int cantidadEnFilaMayor = 0;
    struct NodoAtraccion *atraccionesActuales = NULL;

    if (zonas == NULL) return NULL;

    for (i = 0; i < plibre; i++) {
        atraccionesActuales = zonas[i]->headAtracciones;
        atraccionCandidataZona = atraccionConMasEspera(zonas[i]->headAtracciones, &cantidadEnFilaMayor);

        if (atraccionCandidataZona != NULL) {
            atraccionCampeonaGlobal = atraccionCandidataZona;
        }
    }

    return atraccionCampeonaGlobal;
}



// cuantas entradas se utilizaron ese dia comparar con fecha actual global//

int cantidadDeEntradasDiaria (struct NodoEntrada *nodoEntrada) {
    int contador = 0;
    struct NodoEntrada *rec;
    if (nodoEntrada->sig == NULL) return 0;
    rec = nodoEntrada->sig;
    while (rec != NULL) {
        if (rec->datos->estado == 0 && strcmp(rec->datos->fecha, fechaActual) == 0) {
            contador++;
        }
        rec = rec->sig;
    }
    return contador;
}

int cantidadDeEntradasEnArbol (struct NodoVisitante *headVisitantes){
    int contador = 0;
    struct Visitante *actual;

    if (headVisitantes == NULL) return 0;

    actual = headVisitantes->datos;

    contador += cantidadDeEntradasDiarias(actual->headEntradas);

    contador += cantidadDeEntradasEnArbol(headVisitantes->izq);
    contador += cantidadDeEntradasEnArbol(headVisitantes->der);

    return contador;

}



struct NodoAtraccion *crearAtraccion(){
    /*Asignacion de memoria a los nuevos struct e inicialización de variables*/
    struct Atraccion *atraccionNueva = malloc(sizeof(struct Atraccion));
    struct NodoAtraccion *nuevoNodo = malloc(sizeof(struct NodoAtraccion));
    char bufferNombre[100];

    /* fase de llenado por parte del usuario */
    printf("Ingrese el nombre de la atracción\n");
    fgets(bufferNombre, 100, stdin);

    bufferNombre[strcspn(bufferNombre, "\n")] = '\0';
    atraccionNueva -> nombre = (char *)malloc(sizeof(char) * (strlen(bufferNombre) + 1) );
    strcpy(atraccionNueva -> nombre, bufferNombre);

    printf("Ingrese la capacidad de la atraccion\n");
    scanf("%d", &(atraccionNueva -> capacidad));

    printf("Ingrese (en minutos) cuanto tiempo dura el recorrido de la atracción\n");
    scanf("%d", &(atraccionNueva -> duracion));

    printf("Ingrese (en metros, formato x.xx) la altura minima para subir a la atracción\n");
    scanf("%f", &(atraccionNueva -> alturaMinima));

    printf("Ingrese la edad minima para subir a la atracción\n");
    scanf("%d", &(atraccionNueva -> edadMinima));

    /* valores default de la atracción */
    atraccionNueva -> visitantesTotales = 0;
    atraccionNueva -> estado = 2; /* fuera de servicio*/
    atraccionNueva -> headFila = NULL;

    nuevoNodo -> datos = atraccionNueva;
    nuevoNodo -> sig = NULL;
    nuevoNodo -> ant = NULL;

    return nuevoNodo;
}

void agregarAtraccion(struct Zona **zonas, int *pLibreZonas){
    struct NodoAtraccion *atraccionNueva = NULL;
    struct NodoAtraccion *rec;
    struct Zona *zonaElegida;
    int i, zona;

    atraccionNueva = crearAtraccion();

    /*Selección de zona*/
    printf("==============================================\n");
    printf("Lista de Zonas\n");
    printf("==============================================\n");

    for(i = 0; i < *pLibreZonas; i++){
        printf("%d: %s\n", i, zonas[i] -> nombre);
    }
    printf("\n");

    scanf("Ingrese el numero de la zona a la que desea agregar la nueva atracción %d\n", &zona);
    if(zona > *pLibreZonas){
        do{
            scanf("Ingrese un numero válido %d\n", &zona);
        }while(zona > *pLibreZonas);
    }

    /*Agregado de la atraccion nueva a la lista de atracciones*/
    zonaElegida = zonas[i];
    rec = zonaElegida -> headAtracciones -> sig;

    while(rec -> sig != NULL){
        rec = rec -> sig;
    }

    rec -> sig = atraccionNueva;
    rec -> sig -> ant = rec;
    rec -> sig -> sig = NULL;
}

void cerrarAtraccion(struct NodoAtraccion *atraccionACerrar, int razon){

    if(razon != 3){
        printf("Ingrese la razon de cierre\n");
        printf("1 = En Mantenimiento\n");
        printf("2 = Fuera de servicio\n");
        scanf("%d", &razon);
    }

    /* cambia el estado y vacia la fila de espera */
    atraccionACerrar -> datos -> estado = razon;
    atraccionACerrar -> datos -> headFila -> sig = atraccionACerrar -> datos -> headFila;
}

int main(void) {
    int opcionMenu, c;
    opcionMenu = 1;
    srand(time(NULL)); /*Establece la semilla para la funcion rand(), para que cambien sus resultados en cada ejecucion del programa*/

    printf("Bienvenido al menu de IBCLandia\n");
    printf("1.- Menu de visitantes\n");
    printf("2.- Menu de entradas \n");
    printf("3.- Menu de atracciones\n");
    printf("4.- Menu de zonas\n");
    printf("5.- Menu de datos\n");
    printf("0.- Cerrar programa");
    printf("\n");

    while (opcionMenu != 0){
        printf("Ingrese operacion deseada: ");
        scanf("%d",&opcionMenu);
        printf("\n");
        while ((c=getchar()) != '\n' && c != EOF); /*Esta línea limpia el buffer del teclado*/

        switch (opcionMenu) {
            case 1:
                mostrarMenuVisitantes();
            case 2:
                /*mostrarMenuEntradas();*/
            default:
                printf("Ingrese una opcion valida. \n");
        }

        }
    printf("Cerrando programa. ¡Que tengas un dia IBCtastico!");
    return 0;
}
  
  
