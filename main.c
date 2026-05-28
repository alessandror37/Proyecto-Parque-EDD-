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
    int mayorFilaRegistrada;
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
        idNuevo = rand() % (MAX_ID_VISITANTES + 1);
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
    visitanteNuevo->headEntradas = NULL;

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

void comprarEntradaVisitante(struct Visitante *visitante) {
    printf("Ingrese entrada que desea comprar");
}

int validarEntradaVisitante(struct Visitante *visitante, int idEntrada) {

}

void mostrarMenuVisitantes(void) {
    int c;



}




/*Retorna un arreglo dinamico de punteros a las atracciones NO OPERATIVAS*/

 /*esta funcion recorre las atracciones de una zona y cuenta cuantas estan NO operativas*/
int NoOperativasEnZona (struct NodoAtraccion *headAtracciones) {
    int contador = 0;
    struct NodoAtraccion *rec = headAtracciones->sig;

    /*recorre las atracciones buscando cualquiera que NO esté en estado "Operativa"*/
    while (rec != NULL) {
        if (rec->datos->estado != 0) {
            contador++;
        }
        rec = rec->sig;
    }

    return contador;
}
/*esta funcion cuenta las NO operativas por zona*/
int contarNoOperativas (struct Zona ** zonas, int plibre) {
    int contador = 0;
    int i;

    /*recore el arreglo de zonas*/
    for (i = 0; i < plibre; i++) {
        if(zonas[i] != NULL) {
            /*Cuenta las NO operativas en la zona actual del recorrido y las suma al contador*/
            contador+= NoOperativasEnZona(zonas[i]->headAtracciones);
        }
    }

    return contador;
}

//*retorna un arreglo dinamico de punteros con las atracciones NO operatiivas*//
struct Atraccion ** NoOperativas (struct Zona ** zonas, int plibre) {
    struct Atraccion ** ArregloNoOperativas;
    int cantidadNoOperativas; /*contador para el malloc*/
    int i;
    int posicion = 0;
    struct NodoAtraccion *rec = NULL;

    cantidadNoOperativas = contarAtraccionesNoOperativas (zonas, plibre);
    if (cantidadNoOperativas == 0) return NULL;

    ArregloNoOperativas = (struct Atraccion **) malloc(cantidadNoOperativas * sizeof (struct Atraccion *));
    /*recorro las zonas*/
    for (i = 0;  i <plibre; i++) {
        /**por seguridad verifico que la zona no sea NULL antes de asignarla al rec*//
        if(zonas[i] != NULL) {
            rec = zonas[i]->headAtracciones->sig;
            while (rec != NULL) {
                /*verifico que sea NO operativa*/
                if (rec->datos->estado != 0) {
                    /*agregar al arreglo que se retornara*/
                    ArregloNoOperativas[posicion] = rec->datos;
                    posicion++;
                }
                rec = rec->sig;
            }
        }
    }

    return ArregloNoOperativas;
}

/*Qué visitantes siguen dentro del parque*/

/*recorre el arbol de visitantes y agrega al arreglo las que tengan el boolEstaEnParque en 1*/
void RecorrerArbolAnadiendo (struct NodoVisitante *headVisitantes, struct Visitante ** arreglo, int *posicion) {
    struct Visitante *datos;
    if (headVisitantes == NULL) return;
    datos = headVisitantes->datos;
    /*de forma recursiva agrega las de la izquerda primero para mantener un orden (si es que el arbol llega a estar ordenado no se)*/
    RecorrerArbolAnadiendo(headVisitantes->izq,arreglo,posicion);


    if (datos->boolEstaEnParque == 1) {
        arreglo[*posicion] = datos;
        (*posicion)++;
    }

    /*de forma recursiva agrega las de la deerecha*/
    RecorrerArbolAnadiendo(headVisitantes->der,arreglo,posicion);

}

struct Visitante **DentroDelParque (struct Parque *IbcLandia) {
    struct Visitante ** VisitantesEnElParque;
    int contador; /*contador para el malloc*/
    int posicion = 0;/*contador de posicion para el arreglo, se le pasa a la función que recorre el arbol para que sepa en que posición poner al visitante*/

    if (IbcLandia == NULL) return NULL;

    contador = contarVisitantesEnParque (IbcLandia->headVisitantes);

    if (contador == 0) return NULL;

    VisitantesEnElParque = (struct Visitante **) malloc (contador * sizeof (struct Visitante *));
    /*agrega mientras modifica la posicion en la que debe ser agregado*/
    RecorrerArbolAnadiendo(IbcLandia->headVisitantes,VisitantesEnElParque,&posicion);

    return VisitantesEnElParque;


}


// atraccion con mas visitantes en espera //
/*cuenta las personas dentro de cada fila en una atracción*/
int cantidadEnFila (struct  NodoFila *fila) {
    int contador = 0;
    struct NodoFila *rec;
    if (fila->sig == NULL) return 0;

    rec = fila->sig;
    /*recorre la fila*/
    while (rec != NULL) {
        contador++;
        rec = rec->sig;

    }
    return contador;

}
/*retorna la atraccion con mas fila dentro de una zona con el proposito de comparar globalmente con la campeona del momento entre el resto de zonas*/
struct Atraccion *atraccionConMasEspera (struct NodoAtraccion *headAtracciones, int *cantFila) {
    struct NodoAtraccion *rec;
    struct Atraccion *MayorFila = NULL;
    int cantidadEnEsaFila;
    if (headAtracciones->sig == NULL) return NULL;
    rec = headAtracciones->sig;
    /*recorre las atracciones */
    while (rec != NULL) {
        /*comprueba si la cantidad en esa fila supera a la mayor fila entre las zonas anteriores*/
        cantidadEnEsaFila = cantidadEnFila(rec->datos->headFila);
        if (cantidadEnEsaFila > *cantFila) {
            MayorFila = rec->datos;
            *cantFila = cantidadEnEsaFila;
        }
        rec = rec->sig;


    }

    return MayorFila;
}
/*retorna la atraccion con mayor fila en ese momento*/
struct Atraccion *AtraccionMayorFilaEntreZonas(struct Zona ** zonas, int plibre ) {
    struct Atraccion *atraccionCampeonaGlobal = NULL; /*la atraccion con mayor fila entre todas las zonas, se actualiza si es superada*/
    struct Atraccion *atraccionCandidataZona = NULL; /*atraccion con mas fila dee una zona, see compara con la campeonaMundial y se remplaza si la supera*/
    int cantidadEnFilaMayor = 0;

    if (zonas == NULL) return NULL;
    /*recorre las zonas*/
    for (int i = 0; i < plibre; i++) {
        /*busca la que tenga mayor fila en esa zona y la propone como candidata, retorna null si ninguna supera a la actual*/
        atraccionCandidataZona = atraccionConMasEspera(zonas[i]->headAtracciones, &cantidadEnFilaMayor);
        /*si la funcion anterior retornó null significa que ninguna supero a la campeona actual, si lo hizo, se cambia la campeona actual*/
        if (atraccionCandidataZona != NULL) {
            atraccionCampeonaGlobal = atraccionCandidataZona;
        }
    }

    return atraccionCampeonaGlobal;
}



/*cuantas entradas se utilizaron ese dia comparar con fecha actual global*/

/*cuenta la cantidad de entradas utilizadas ese dia en las entradas de un visitante*/
int cantidadDeEntradasDiaria (struct NodoEntrada *nodoEntrada) {
    int contador = 0;
    struct NodoEntrada *rec;

    if (nodoEntrada->sig == NULL) return 0;

    rec = nodoEntrada->sig;
    /*recorre las entradas*/
    while (rec != NULL) {
        /*si la entrada esta utilizada y la fecha de la entrada es igual a la fecha del dia suma 1 al contador*/
        if (rec->datos->estado == 0 && strcmp(rec->datos->fecha, fechaActual) == 0) {
            contador++;
        }
        rec = rec->sig;
    }
    return contador;
}
/*cuenta todas las entradas del dia entre todos los visitantes, recursivamente recorre el arbol*/
int cantidadDeEntradasEnArbol (struct NodoVisitante *headVisitantes){
    int contador = 0;
    struct Visitante *actual;

    if (headVisitantes == NULL) return 0;

    actual = headVisitantes->datos;
    /*suma la cantidad de entradas del dia pertenecientes a ese visitante*/
    contador += cantidadDeEntradasDiaria(actual->headEntradas);
    /*recorre la izquerda y la deerecha sumando al contador las entradas que cumplen*/
    contador += cantidadDeEntradasEnArbol(headVisitantes->izq);
    contador += cantidadDeEntradasEnArbol(headVisitantes->der);

    return contador;

}
    // atraccion mas visitada, comparar variable de visitantes totales entre atracciiones y despues entre filas//

/*retorna la atraccion con mas visitantes totales usando la variable VisitantesTotales dentro de cada atraccion*/
    /*la gracia de que retorne la atraccion es ir comparando la mejor de esa zona con la mejor de otras zonas*/
struct Atraccion *MasVisitantesTotales (struct NodoAtraccion *headAtracciones) {
    struct NodoAtraccion *rec;
    struct Atraccion * masVisitantes = NULL;
    int visitantesAnterior = 0;

    if (headAtracciones->sig == NULL) return NULL;

    rec = headAtracciones->sig;
    /*recorrer las atracciones de una zona*/
    while (rec != NULL) {
        /*si la atraccion tiene mas visitantesTotales que la anterior se cambia el record de la zona*/
        if (rec->datos->visitantesTotales > visitantesAnterior) {
            visitantesAnterior = rec->datos->visitantesTotales;
            /*significa que esa atraccion tiene mas visitantes y ocupa el record de la zona*/
            masVisitantes = rec->datos;
        }
        rec = rec->sig;
    }

    return masVisitantes;
}

/*recorre las zonas y retorna la atraccion mas visitada entre todas las zonas*/
struct Atraccion *AtracccionMasVisitadaEntreZonas (struct Zona ** zonas, int plibre ) {
    struct Atraccion *AtraccionConMasVisitantesTotales = NULL; /*aca se almacena el record entre todas las zonas*/
    struct Atraccion *candidato= NULL;/*candidato corresponde a la atraccion mas visitada de una zona, si es mayor al record se reemplaza*/
    int i;

    if (zonas == NULL) return NULL;
    /*recorre las zonas*/
    for (i = 0; i < plibre; i++) {
        /*retorna la atraccion mas visitada de la zona*/
        candidato = MasVisitantesTotales(zonas[i]->headAtracciones);
        /*si retorna null significa que la zona no tiene atracciones, si las tiene no retornara null*/
        if (candidato != NULL) {
            /*si no hay record o si el candidato supera el record actual se remplaza*/
            if (AtraccionConMasVisitantesTotales == NULL || AtraccionConMasVisitantesTotales->visitantesTotales < candidato->visitantesTotales) {
                AtraccionConMasVisitantesTotales = candidato;
            }
        }
    }

    return AtraccionConMasVisitantesTotales;

}

/*top 3 zonas mas concurridas, el struct de abajo sera un auxiliar*/

/*struct auxiliar con puntero a la zona y la cantidad de personas dentro de esa zona*/
    struct ReporteZona {
    struct Zona * datosZona;
    int TotalPersonas;
};
/*cuenta la cantidad de personas en la zona perteneciente al codigo ingresado, recorre el arbol recursivamente buscando ese codigo*/
int TotalPersonasEnZona (struct NodoVisitante * headVisitantes, int codigoBuscado) {
    int contador = 0;
    struct Visitante *actual;

    /*final de essa rama del arbol*/
    if (headVisitantes == NULL) return 0;

    actual = headVisitantes->datos;
    /*si el codigo del puntero de zona en el visitante coincide con el buscado se le suma uno al contador*/
    if (actual->zonaActual->codigo ==  codigoBuscado) {
        contador++;
    }
    /*de forma recursiva se le suma al contador analizando las personas de la izquerda y derecha*/
    contador+= TotalPersonasEnZona(headVisitantes->izq,codigoBuscado);
    contador += TotalPersonasEnZona(headVisitantes->der, codigoBuscado);

    return contador;
}
    /*ordena el arreglo dinamico de tipo ReporteZona de mayor a menor con un bubble sort*/
    void OrdenarZonasMasOcupadas (struct ReporteZona ** reporte, int tam) {
    int i;
    int j;

    struct ReporteZona *temporal; /*guarda temporalmente para ordenar */

    if (reporte == NULL || tam == 0) return;
    /*bubble sort*/
    for (i = 0; i < tam - 1; i++) {
        for (j = 0; j < tam - i - 1; j++) {
            /*compara el total de personas con el siguiente*/
            if (reporte[j]->TotalPersonas < reporte[j + 1]->TotalPersonas) {
                temporal = reporte[j]; /*guarda temporalmente ese reporte*/
                reporte[j] = reporte[j + 1];
                reporte[j + 1] = temporal;
            }
        }
    }
}
/*crea un arreglo dinamico de punteros de tipo ReporteZona, este guarda todas las zonas y el total de personas en esa zona*/
struct ReporteZona ** ArregloReporteZona(struct parque *IBCLandia) {
    struct ReporteZona **reporte;
    int tam = IBCLandia->pLibreZonas;
    int i;

    reporte = (struct ReporteZona **) malloc(tam * sizeof(struct ReporteZona *));
    /*recorre las zonas*/
    for (i = 0; i < tam; i++) {
        /*se guarda memoria para ese esspacio*/
        reporte[i] = (struct ReporteZona *) malloc(sizeof(struct ReporteZona));
        /*se rellena el struct ReporteZona para cada uno dentro del arreglo*/
        reporte[i]->datosZona = IBCLandia->zonas[i];
        reporte[i]->TotalPersonas =TotalPersonasEnZona(IBCLandia->headVisitantes,IBCLandia->zonas[i]->codigo);
    }
    /*ordena el arreglo de mas a menos visitantes*/
    OrdenarZonasMasOcupadas (reporte, tam);

    return reporte;
}




/*cuanto dinero se ha recaudado con las entradas */

/*recorre las entradas y suma el valor de las entradas usadas que tienen fechaActual*/
    /*retorna el valor de todas las entradas del dia de hoy del visitante*/
int recorrerEntradas (struct NodoEntrada *headEntradas) {
    struct NodoEntrada *rec;
    int recaudado = 0;
    struct Entrada *enUso;

    if (headEntradas == NULL) return 0;

    rec = headEntradas->sig;

    /*recorro las entradas*/
    while (rec != NULL) {
        enUso = rec->datos;
        /*asumi que solo las entradas usadas y de la fecha de hoy cuentan como lo recaudado diario*/
        if (strcmp(enUso->fecha, fechaActual) == 0 && enUso -> estado == 1) {
            recaudado += enUso->valor;
        }
        rec = rec->sig;
    }
    return recaudado;
}

/*recorre recursivamente los visitantes sumando el valor dee sus entradas usadas el mismo dia*/
int DineroRecaudadoDiario (struct NodoVisitante *headVisitantes) {
    struct Visitante *actual;
    int recaudado = 0;

    if (headVisitantes == NULL) return 0;

    actual = headVisitantes->datos;
    /*suma el valor de las entradas usadas del dia*/
    if (actual != NULL) {
        recaudado += recorrerEntradas(actual->headEntradas);
    }
    /*recorrer las ramas recursivamente*/
    recaudado += DineroRecaudadoDiario(headVisitantes->izq);
    recaudado += DineroRecaudadoDiario(headVisitantes->der);

    return recaudado;

}

/*cuantos integrantes visitantes ingresaron ese dia teniendo en cuenta de que un visitante puede entrar mas de una vez el mismo dia */
int VisitantesDelDia (struct NodoVisitante *headVisitantes) {
    int contador = 0;
    struct Visitante *datos;

    if (headVisitantes == NULL) return 0;

    datos = headVisitantes->datos;
    if (datos != NULL) {
        /*"recorrerentradas" retorna el monto de todas las entradas del dia de la persona, si pisó aunque sea una vez el parque ese dia su monto sera distinto de 0*/
        if (recorrerEntradas(datos->headEntradas) != 0) {
            contador++;
        }
    }
    /*recorre el arbol de visitantes recursivamente sumando al contador si cumple*/
    contador += VisitantesDelDia(headVisitantes->izq);
    contador += VisitantesDelDia(headVisitantes->der);

    return contador;
}


/* top 3 atracciones con las fila mas larga */

/*struct auxiliiar para almacenar la atraccion y la fila mas larga registrada dentro de esa atraccion*/
struct ReporteFilas {
    struct Atraccion *datos;
    int tamFila;
};
/* se usa en el malloc, cuenta las atracciones de una zona*/
int contarAtracciones (struct NodoAtraccion *headAtraccion) {
    int contador = 0;

    struct NodoAtraccion *rec;

    if (headAtraccion->sig == NULL) return 0;
    /*recorre las atracciones*/
    rec = headAtraccion->sig;
    while (rec != NULL) {
        contador++;
        rec = rec->sig;
    }
    return contador;
}
/*cuenta las atracciones del parque*/
int TotalAtracciones (struct Zona **Zonas,int plibre) {
    int contador = 0;
    int i;
    for (i = 0; i < plibre; i++) {
        contador += contarAtracciones(Zonas[i]->headAtracciones);
    }
    return contador;

}
/*rellena el arreglo dinamico con las atracciones de la zona dada y a la vez el contador de posicion*/
void CopiarAtracciones (struct ReporteFilas **repFilas, struct Zona *ZonaActual, int * pos) {
    struct NodoAtraccion *rec;

    struct Atraccion *atraccionActual;

    if (ZonaActual->headAtracciones == NULL) return;

    rec = ZonaActual->headAtracciones->sig;
    /*recorre las atracciones de esa zona*/
    while (rec != NULL) {
        atraccionActual = rec->datos;
        if (atraccionActual != NULL) {
            /*guarda espacio para esa posicion del arreglo dinamico*/
            repFilas[*pos] = (struct ReporteFilas *) malloc(sizeof(struct ReporteFilas));

            // verificar que no haya fallado el malloc x seguridad nomas//
            if (repFilas[*pos] != NULL) {
                /*rellena el Struct ReporteFilas*/
                repFilas[*pos]->datos = atraccionActual;
                repFilas[*pos]->tamFila = atraccionActual->mayorFilaRegistrada;
                (*pos)++;
            }
        }
        rec = rec->sig;
    }
    return;
}
/*ordena el arreglo dinamico de structs ReporteFilas de mayor a menor*/
void OrdenarAtracciones(struct ReporteFilas **reporte,int tam) {
    int i, j;
    struct ReporteFilas *temporal;/*guarda temporalmente un struct*/

    if (reporte == NULL || tam <= 1) return;

    /*buble sort*/
    for (i = 0; i < tam - 1; i++) {
        for (j = 0; j < tam - 1 - i; j++) {
            /*si el tam del siguiente es mayor*/
            if (reporte[j]->tamFila < reporte[j + 1]->tamFila) {
                temporal = reporte[j];
                reporte[j] = reporte[j + 1];
                reporte[j + 1] = temporal;
            }
        }
    }
}
/*crea el arreglo dinamico de atraccionees ordenado de mayor a menor*/
struct ReporteFilas ** ArregloAtracciones (struct parque *IBCLandia) {
    struct ReporteFilas **TodasLasAtracciones;
    int tam;
    int pos = 0;
    int i;

    if (IBCLandia == NULL) return NULL;

    tam = TotalAtracciones(IBCLandia->zonas,IBCLandia->pLibreZonas);
    TodasLasAtracciones = (struct ReporteFilas **) malloc(tam * sizeof(struct ReporteFilas *));
    /*recorre las zonas y guarda en el arreglo dinamico las atracciones de la zona junto con la fila mas larga registrada de cada una*/
    for (i = 0; i < IBCLandia->pLibreZonas; i++) {
        CopiarAtracciones(TodasLasAtracciones, IBCLandia->zonas[i], &pos);
    }
    /*ordena comparando los reegistros de filas de mayor a menor*/
    OrdenarAtracciones(TodasLasAtracciones, tam);

    return TodasLasAtracciones;

}
/*muestra todas las atracciones ordenadas de mayor a menor dependiendo de la mayor fila registrada*/
void MostrarAtraccionesConMayorFilaDeEspera(struct ReporteFilas **reporte, struct parque *IBCLandia) {
    struct Atraccion *Atraccion;
    int cantAtracciones;
    int i;
    int limite;

    if (reporte == NULL || IBCLandia == NULL) return;

    cantAtracciones = TotalAtracciones(IBCLandia->zonas,IBCLandia->pLibreZonas);


    printf("NOMBRE -> TAMAÑO FILA\n");
    /*recorre hasta que muestre todas las atracciones y su tamFila, todas estan ordenadas*/
    for (i = 0; i < cantAtracciones; i++) {
        if (reporte[i] != NULL) {
            printf("%i.%s -> %d \n",i + 1, reporte[i]->datos->nombre, reporte[i]->tamFila);
        }
    }
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


void mostrarAtraccionesMasVisitadasEnZona(struct NodoAtraccion *original) {
    struct NodoAtraccion *actual;
    int max = 0;
    actual = original;
    do {
        actual = actual->sig;
        if (max == 0) {
            max = actual->datos->visitantesTotales;
        }else if (actual->datos->visitantesTotales > max) {
            max = actual->datos->visitantesTotales;
        }

    }while (actual->sig != NULL);

    do {
        actual = actual->sig;
        if (max == actual->datos->visitantesTotales) {
            printf(actual->datos->nombre);
        }
    }while (actual->sig != NULL);
}

void listarAtraccionesNoDisponiblesEnZona(struct NodoAtraccion *original) {
    struct NodoAtraccion *actual;
    actual = original;
    do {
        actual = actual->sig;
        if (actual->datos->estado != 0) {
            printf(actual->datos->nombre);
        }
    }while(actual->sig != NULL);
}


/*En minutos*/
int tiempoEsperaEstimado(struct Atraccion *atraccion) {
    int fila;
    fila = cantidadEnFila(atraccion->headFila);
    return fila/atraccion->capacidad*atraccion->duracion;
}

int seleccionDeZona(struct Zona **zonas, int pLibreZonas){
    int zona, i;

    printf("==============================================\n");
    printf("Lista de Zonas\n");
    printf("==============================================\n");

    for(i = 0; i < pLibreZonas; i++){
        printf("%d: %s\n", i, zonas[i] -> nombre);
    }
    printf("\n");

    printf("Ingrese el numero de la zona a utilizar\n");
    scanf("%d", &zona);
    if(zona > pLibreZonas){
        do{
            printf("Ingrese un numero válido\n");
            scanf("%d", &zona);
        }while(zona > pLibreZonas);
    }

    return zona;
}

void consultarCantidadVisitantesPorZona(struct Zona **zonas, int pLibreZonas){
    int zona;
    struct Zona *zonaElegida;

    /* fase seleccion de zona */
    printf("CANTIDAD DE VISITANTES POR ZONA\n\n");
    zona = seleccionDeZona(zonas, pLibreZonas);

    /* recorrido de arbol */
    zonaElegida = zonas[zona];

    printf("La cantidad de personas en la zona %s es de: %d \n", zonaElegida -> nombre, zonaElegida -> ocupacionActual);
}

void consultarZonaConMasVisitantes(struct Zona **zonas, int pLibreZonas){
    int i, maxVisitantes, actual;
    struct Zona *zonaElegida;

    zonaElegida = zonas[0];
    maxVisitantes = zonas[0] -> ocupacionActual;

    for(i = 1; i < pLibreZonas; i++){
        actual = zonas[i] -> ocupacionActual;
        if(actual > maxVisitantes){
            zonaElegida = zonas[i];
            maxVisitantes = actual;
        }
    }

    printf("La zona con mas personas es: %s con: %d personas actualmente\n", zonaElegida -> nombre, maxVisitantes);
}

struct Zona *buscarZonaConMenosVisitantes(struct Zona **zonas, int pLibreZonas){
    struct Zona *zonaElegida;
    int menorCantVisitantes, i;

    zonaElegida = zonas[0];
    menorCantVisitantes = zonas[0] -> ocupacionActual;

    for(i = 1; i < pLibreZonas; i++) {
        if(zonas[i] -> ocupacionActual < menorCantVisitantes) {
            zonaElegida = zonas[i];
            menorCantVisitantes = zonas[i] -> ocupacionActual;
        }
    }
    return zonaElegida;
}

struct Zona *buscarZonaConMenosPersonal(struct Zona **zonas, int pLibreZonas){
    struct Zona *zonaElegida;
    int menorCantPersonal, i;

    zonaElegida = zonas[0];
    menorCantPersonal = zonas[0] -> personalEncargado;

    for(i = 1; i < pLibreZonas; i++) {
        if(zonas[i] -> personalEncargado < menorCantPersonal) {
            zonaElegida = zonas[i];
            menorCantPersonal = zonas[i] -> personalEncargado;
        }
    }
    return zonaElegida;
}

void verificarYAsignarPersonal(struct Zona **zonas, int pLibreZonas, int idZona){
    int i, capacidad, ocupacion;
    struct Zona *zonaConMenosVisitantes,*zonaConMenosPersonal, *zonaARevisar;

    for(i = 0; i < pLibreZonas; i++){
        if(zonas[i] -> codigo == idZona){
            zonaARevisar = zonas[i];
        }
    }

    capacidad = zonaARevisar -> capacidad;
    ocupacion = zonaARevisar -> ocupacionActual;

    /* si la zona se encuentra a mas de un 90% de su ocupacion y su personal no se ha aumentado nunca, se trae 5 personas de la zona con menos visitantes en ese momento*/
    if(ocupacion > capacidad * 0.9){
        if(zonaARevisar -> personalEncargado == 30){
            printf("LA ZONA ACTUAL TIENE UNA OCUPACION ALTA, SE ENVIARÁ PERSONAL DESDE OTRAS ZONAS\n");
            zonaARevisar -> personalEncargado += 5;
            zonaConMenosVisitantes = buscarZonaConMenosVisitantes(zonas, pLibreZonas);
            zonaConMenosVisitantes -> personalEncargado -= 5;
        }
    }else{
        /* si la zona no está sobre el 90% de ocupacion pero tiene personal de mas, este personal se mueve a la zona con menos personal en ese momento */
        if(zonaARevisar -> personalEncargado > 30){
            printf("LA ZONA ACTUAL TIENE UN EXCESO DE PERSONAL, SE ENVIARA PARTE DE SU PERSONAL A OTRA ZONA");
            zonaARevisar -> personalEncargado -= 5;
            zonaConMenosPersonal = buscarZonaConMenosPersonal(zonas, pLibreZonas);
            zonaConMenosPersonal -> personalEncargado += 5;
        }
    }
}

int calcularTiempoEsperaMedio(const struct Zona *zona){
    /* esta es la funcion auxiliar que usa la funcion del qsort para comparar */
    struct NodoAtraccion *rec;
    int contadorAtracciones = 0;
    int acumuladorTiempos = 0;
    rec = zona -> headAtracciones;

    while(rec -> sig != NULL) {
        acumuladorTiempos += tiempoEsperaEstimado(rec -> datos);
        contadorAtracciones ++;
        rec = rec -> sig;
    }

    return acumuladorTiempos/contadorAtracciones;
}

int ordenarZonasPorTiempoDeEspera(const void *a, const void *b){
    const struct Zona *zonaA = *(const struct Zona **)a;
    const struct Zona *zonaB = *(const struct Zona **)b;

    int tiempoA = calcularTiempoEsperaMedio(zonaA);
    int tiempoB = calcularTiempoEsperaMedio(zonaB);

    if (tiempoA < tiempoB) return -1;
    if (tiempoA > tiempoB) return 1;
    return 0;
}

void recomendarZonas(struct Zona **zonas, int pLibreZonas){
    struct Zona **zonasEnOrden = (struct Zona **)malloc(pLibreZonas * sizeof(struct Zona *));
    int i, limite;

    memcpy(zonasEnOrden, zonas, pLibreZonas * sizeof(struct Zona *));
    qsort(zonasEnOrden, (size_t)pLibreZonas, sizeof(struct Zona *), ordenarZonasPorTiempoDeEspera);

    printf("LA ZONA ACTUAL TIENE UN TIEMPO DE ESPERA MUY ALTO\n");
    printf("recomendación de zonas con menos tiempos de espera:\n");

    if(pLibreZonas >= 3) {
        limite = 3;
    }else{
        limite = pLibreZonas - 1;
    }

    for(i = 0; i < limite; i++){
        printf("%s \n", zonasEnOrden[i] -> nombre);
    }
}

void menuVisitantesEnZona(struct Zona **zonas, int pLibreZonas){
    int opcion;

    /* Menu de Visitantes */
    printf("==============================================\n");
    printf("MENU DE VISITANTES\n");
    printf("==============================================\n \n");
    printf("OPCIONES\n");
    printf("1. Consultar cantidad de visitantes por zona\n");
    printf("2. Consultar zona con mas visitantes\n");
    scanf("%d", &opcion);

    if(opcion < 1 || opcion > 2){
        do{
            printf("Ingrese un numero válido\n");
            scanf("%d", &opcion);
        }while(opcion < 1 || opcion > 2);
    }

    if(opcion == 1){
        consultarCantidadVisitantesPorZona(zonas, pLibreZonas);
    }else if(opcion == 2){
        consultarZonaConMasVisitantes(zonas, pLibreZonas);
    }
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
  
  
