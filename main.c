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
int valorEntradas[4] = {22990,10990,0,44990}; /*En cada elemento del array se guarda el valor de la entrada con su respectivo tipo
Ej: para escribir el valor de una entrada en una funcion se usaria: entrada->valor = valorEntradas[tipo]*/

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
    float altura; /*altura en metros*/
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

char *pasarAMinus(char *cadena) {
    int i;
    int largo = strlen(cadena) + 1;
    char *NuevaCadena = (char *)malloc(sizeof(char) * largo);

    for(i = 0; cadena[i] != '\0'; i++){
        NuevaCadena[i] = tolower((unsigned char)cadena[i]);
    }
    return NuevaCadena;
}


/*Busca a un visitante dentro del arbol por su ID
 * Utiliza recursividad
 */
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

/*Funcion que recorre el arbol de visitantes recursivamente para encontrar visitante por rut*/
struct Visitante *buscarVisitantePorRut(struct NodoVisitante *raiz, char *rut) {
    struct Visitante *visitanteBuscar;

    if (raiz == NULL) return NULL;
    visitanteBuscar = raiz->datos;

    if (strcmp(visitanteBuscar->rut,rut) == 0) return visitanteBuscar;

    visitanteBuscar = buscarVisitantePorRut(raiz->izq,rut);
    if (visitanteBuscar != NULL) return visitanteBuscar;
    visitanteBuscar = buscarVisitantePorRut(raiz->der,rut);
    if (visitanteBuscar != NULL) return visitanteBuscar;
    return NULL;
}

/*Funcion para crear un id para un visitante nuevo.
 * Esta función tiene en cuenta que el id del visitante tiene que ser unico
 * todo: falta agregarle un caso especial en el que estén todas las ids usadas
 */
int crearIdNuevoVisitante(struct NodoVisitante *raiz) {
    int idNuevo = 0, esUnica = 0;
    if (raiz==NULL) return 50000;
    while (esUnica != 1) {
        idNuevo = (rand() % MAX_ID_VISITANTES) + 1; /*rango de los ids: 1-100000*/
        if (buscarVisitantePorID(raiz, idNuevo) == NULL) {
            esUnica = 1;
        }
    }
    return idNuevo;
}


/*Funcion que crea el struct de un visitante, inicializa datos y pide al usuario algunos.
 * La funcion asume que antes de ser llamada se limpia el buffer de entrada para no generar problemas
 */
struct Visitante *crearVisitante(struct NodoVisitante *raiz) {
    int idVisitanteNuevo;
    struct Visitante *visitanteNuevo = malloc(sizeof(struct Visitante));

    idVisitanteNuevo = crearIdNuevoVisitante(raiz);
    visitanteNuevo ->idVisitante = idVisitanteNuevo;

    /*Se le asigna memoria a los chars, el programa asume que no se ingresará un nombre de más de 50 caracteres*/
    visitanteNuevo->nombre = malloc(sizeof(char)*50);
    visitanteNuevo->rut = malloc(sizeof(char)*12);

    /*Se pide el input al usuario de los datos*/
    printf("Ingresar nombre completo del visitante: ");
    fgets(visitanteNuevo->nombre,50, stdin);

    printf("Ingresar rut del visitante: ");
    fgets(visitanteNuevo->rut,12, stdin);


    /*Inicialización de datos fijos que no requieren input del usuario*/
    visitanteNuevo->boolEstaEnParque = 0;
    visitanteNuevo->zonaActual = NULL;
    visitanteNuevo->headEntradas = malloc(sizeof(struct NodoEntrada));
    visitanteNuevo->headEntradas->sig = NULL;

    return visitanteNuevo;
}


/*Funcion que busca el nodo al que debería estar conectado el nodo que va a contener un visitante nuevo*/
struct NodoVisitante *buscarNodoParaVisitanteNuevo(struct NodoVisitante *raiz, int idVisitanteNuevo) {
    struct NodoVisitante *rec;
    if (raiz != NULL) {
        rec = raiz;
        while (rec!= NULL) {
            if (rec->datos->idVisitante < idVisitanteNuevo) {
                if (rec->der == NULL) return rec;
                rec = rec->der;
            }else {
                if (rec->izq == NULL) return rec;
                rec = rec->izq;
            }
        }
    }
    return NULL;
}


/*Función que crea y anida un nuevo visitante al arbol
 * Esta función maneja el caso especial en el que el visitante nuevo sea el primero en ser ingresado al arbol
 */
void crearYAgregarVisitanteAArbol(struct NodoVisitante **raiz) {
    struct NodoVisitante *nodoNuevo, *nodoAnterior;
    /*Se asigna memoria para el nodo nuevo y se le entrega los datos del visitante*/
    nodoNuevo = malloc(sizeof(struct NodoVisitante));
    nodoNuevo->datos=crearVisitante(*raiz);

    nodoNuevo->izq = NULL;
    nodoNuevo->der = NULL;

    nodoAnterior = buscarNodoParaVisitanteNuevo(*raiz,nodoNuevo->datos->idVisitante);

    /*Estas lineas se encargan de asignar a que lado del nodo anterior va conectado el nuevo.
     * En el caso de que no haya nodo anterior se asume que el arbol esta vacio y se deja el nodo recien creado
     * como raiz
     */
    if (nodoAnterior == NULL) *raiz = nodoNuevo;
    else if (nodoAnterior->datos->idVisitante<nodoNuevo->datos->idVisitante) nodoAnterior->der = nodoNuevo;
    else nodoAnterior->izq = nodoNuevo;
}

/*Funcion recursiva que cuenta la cantidad de visitantes en el parque al momento de ejecutarse*/
int contarVisitantesEnParque(struct NodoVisitante *raiz) {
    int cont = 0;

    if (raiz == NULL) {
        return 0;
    }
    cont += contarVisitantesEnParque(raiz->izq);
    cont += contarVisitantesEnParque(raiz->der);

    return cont + raiz->datos->boolEstaEnParque;
}

/*Esta función recorre la lista simplemente enlazada del visitante y retorna puntero a la entrada si se encontró por id
 * o retorna NULL si no se encontró
 */
struct Entrada *buscarEntradaPorIdEnVisitante(struct NodoEntrada *head, int idEntrada) {
    struct NodoEntrada *rec;
    if (head != NULL) {
        rec = head ->sig;
        while (rec!=NULL) {
            if (rec->datos->idEntrada == idEntrada) return rec->datos;
            rec = rec->sig;
        }
    }
    return NULL;
}

/*Esta funcion recorre el arbol de visitantes y para cada visitante recorre su lista para encontrar la entrada
 * solicitada por id. En caso de no ser encontrada retorna NULL.
 */
struct Entrada *buscarEntradaPorId(struct NodoVisitante *raiz, int idEntrada) {
    struct Entrada *entradaBuscar;
    if (raiz == NULL) {
        return NULL;
    }
    entradaBuscar = buscarEntradaPorIdEnVisitante(raiz->datos->headEntradas,idEntrada);
    if (entradaBuscar != NULL) {
        return entradaBuscar;
    }
    entradaBuscar = buscarEntradaPorId(raiz->izq,idEntrada);
    if (entradaBuscar != NULL) return entradaBuscar;
    entradaBuscar = buscarEntradaPorId(raiz->der,idEntrada);
    if (entradaBuscar != NULL) return entradaBuscar;
    return NULL;

}


/*Funcion que genera un id para una entrada nueva, teniendo en cuenta de no repetir ninguna
 * todo: Revisar caso especial en el que esten todos los ids ocupados
 */
int generarIdEntradaNueva(struct NodoVisitante *raiz) {
    int idNuevo = 0,esUnico = 0;
    while (esUnico != 1) {
        idNuevo = rand() % (MAX_ID_VISITANTES + 1);
        if (buscarEntradaPorId(raiz,idNuevo) == NULL)  esUnico = 1;
    }
    return idNuevo;
}

/*Funcion que crea una entrada, recibiendo como uno de sus parametros el tipo de entrada, asi que se tiene que preguntar
 * al usuario que tipo de entrada se va a asignar antes de ser llamada.
 */
struct Entrada *crearEntrada(struct NodoVisitante *raiz, int tipoEntrada) {
    struct Entrada *entradaNueva = malloc(sizeof(struct Entrada));
    entradaNueva->idEntrada = generarIdEntradaNueva(raiz);
    entradaNueva->tipo = tipoEntrada;
    entradaNueva->estado = 0;
    entradaNueva->valor = valorEntradas[tipoEntrada];
    entradaNueva->fechaUsada = fechaActual;
    return entradaNueva;
}

/*Funcion que obtiene el ultimo nodo de la lista de entradas de un visitante para despues poder anidar el nodo nuevo
 * de una entrada recien creada
 */
struct NodoEntrada *obtenerNodoAnteriorParaEntradaNueva(struct NodoEntrada *headEntradas) {
    struct NodoEntrada *rec;
    if (headEntradas != NULL) {
        rec = headEntradas->sig;
        while (rec->sig !=NULL) {
            rec=rec->sig;
        }
        return rec;
    }
    return NULL;
}

/*Esta funcion crea y agrega una entrada a la lista del visitante. Se debe tener de antes un puntero al visitante para
 * poder entregarle el head de su lista de entradas a esta funcion
 */
void crearYAgregarEntradaALista(struct NodoVisitante *raiz,struct NodoEntrada *head,int tipoEntrada) {
    struct NodoEntrada *nodoNuevo,*nodoAnterior;
    nodoNuevo = malloc(sizeof(struct NodoEntrada));
    nodoNuevo->datos=crearEntrada(raiz,tipoEntrada);
    nodoAnterior = obtenerNodoAnteriorParaEntradaNueva(head);
    if (nodoAnterior==NULL) head->sig = nodoNuevo;
    else nodoAnterior->sig = nodoNuevo;
}

/*funcion inutil quizas la borre despues XD*/
void cambiarEstadoEntrada(struct Entrada *entrada,int estadoNuevo) {
    entrada->estado = estadoNuevo;
}

/*funcion que compra entrada y la añade a la lista de entradas del visitante
 * La función asume que el buffer de entrada se limpió antes de ser llamada
 */
void comprarEntradaVisitante(struct NodoVisitante *raiz,struct Visitante *visitante) {
    int tipoEntradaComprar = 0,opcionValida = 0,opcionDuenoCompra,idVisitanteReceptor;
    struct Visitante *visitanteReceptorEntrada;
    printf("Tipos de entrada: \n");
    printf("0.- Entrada general. \n");
    printf("1.- Pase infantil. \n");
    printf("2.- Entrada familiar. \n");
    printf("3.- Entrada VIP. \n");
    do {
        printf("\n");
        printf("Ingrese el tipo de entrada que el visitante desea comprar: ");
        scanf("%d",&tipoEntradaComprar);
        if (tipoEntradaComprar >= 0 && tipoEntradaComprar <= 3) opcionValida = 1;
        else {
            printf("\n");
            printf("Ingrese una opcion valida.");
        }
    }while (opcionValida!=1);
    opcionValida = 0;

    printf("\n");
    printf("¿La entrada es para quien hace la compra o para otro visitante? \n");
    printf("1.- Visitante que realiza la compra. \n");
    printf("2.- Otro visitante. \n");
    do {
        printf("\n");
        printf("Ingrese opcion deseada: ");
        scanf("%d", &opcionDuenoCompra);
        if (opcionDuenoCompra>0 && opcionDuenoCompra<3) opcionValida = 1;
        else {
            printf("\n");
            printf("Ingrese una opcion valida.");
        }
    }while (opcionValida!=1);

    if (opcionDuenoCompra==1) {
        visitanteReceptorEntrada = visitante;
    }else {
        do {
            opcionValida = 0;
            printf("\n");
            printf("Ingrese id del visitante al que se le asignara la entrada: ");
            scanf("%d",&idVisitanteReceptor);
            visitanteReceptorEntrada = buscarVisitantePorID(raiz,idVisitanteReceptor);
            if (visitanteReceptorEntrada != NULL) opcionValida = 1;
            else {
                printf("\n");
                printf("ERROR: No se encontro un visitante con el id %d , intentelo de nuevo.",idVisitanteReceptor);
            }
        }while (opcionValida!= 1);
    }

    crearYAgregarEntradaALista(raiz,visitanteReceptorEntrada->headEntradas,tipoEntradaComprar);

}

/*Funcion que valida la entrada del visitante.
 * esta funcion no tendra interaccion con el usuario
 */
int validarEntradaVisitante(struct Visitante *visitante, int idEntrada) {
    int resultadoFuncion;

}

void mostrarMenuVisitantes(void) {
    int c;



}




/*Retorna un arreglo dinamico de punteros a las atracciones NO OPERATIVAS*/



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

void recorrerArbolAnadiendo (struct NodoVisitante *headVisitantes, struct Visitante ** arreglo, int *posicion) {
    struct Visitante *datos;
    if (headVisitantes == NULL) return;
    datos = headVisitantes->datos;

    recorrerArbolAnadiendo(headVisitantes->izq,arreglo,posicion);


    if (datos->boolEstaEnParque == 1) {
        arreglo[*posicion] = datos;
        (*posicion)++;
    }


    recorrerArbolAnadiendo(headVisitantes->der,arreglo,posicion);

}

struct Visitante **DentroDelParque (struct Parque *IbcLandia) {
    struct Visitante ** VisitantesEnElParque;
    int contador;
    int posicion = 0;

    if (IbcLandia == NULL) return NULL;

    contador = contarVisitantesEnParque(IbcLandia->headVisitantes);

    if (contador == 0) return NULL;

    VisitantesEnElParque = (struct Visitante **) malloc (contador * sizeof (struct Visitante *));

    recorrerArbolAnadiendo(IbcLandia->headVisitantes,VisitantesEnElParque,&posicion);

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
        if (rec->datos->estado == 0 && strcmp(rec->datos->fechaUsada, fechaActual) == 0) {
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

    contador += cantidadDeEntradasDiaria(actual->headEntradas);

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
  
  
