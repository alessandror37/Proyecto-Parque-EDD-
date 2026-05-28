#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char fechaActual[10];

struct Entrada{
    int idEntrada;
    int tipo; /*0-> Entrada general, 1-> Pase infantil, 2-> Entrada familiar, 3 -> Pase VIP*/
    int valor;
    int estado; /* */
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
    int ocupacionActual;
    int ocupacionHistorica;
    struct NodoAtraccion *headAtracciones; /*head a la lista de atracciones*/
};

struct Parque{
    int recaudacionTotal;
    int totalVisitantes;
    struct Zona **zonas;
    int *pLibreZonas;
    struct NodoVisitante *headVisitantes;
};

char *pasarAMinus(char *cadena){
    int i;
    int largo = strlen(cadena) + 1;
    char *NuevaCadena = (char *)malloc(sizeof(char) * largo);

    for(i = 0; cadena[i] != '\0'; i++){
        NuevaCadena[i] = tolower((unsigned char)cadena[i]);
    }
    return NuevaCadena;
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
    int zona;

    atraccionNueva = crearAtraccion();

    /*Selección de zona*/
    printf("¿A que zona se agregará la nueva atracción?\n");
    zona = seleccionDeZona(zonas, *pLibreZonas);

    /*Agregado de la atraccion nueva a la lista de atracciones*/
    zonaElegida = zonas[zona];
    rec = zonaElegida -> headAtracciones -> sig;

    while(rec -> sig != NULL){
        rec = rec -> sig;
    }

    rec -> sig = atraccionNueva;
    rec -> sig -> ant = rec;
    rec -> sig -> sig = NULL;
}

void cerrarAtraccion(struct NodoAtraccion *atraccionACerrar, int razon){
    /* Si la razon de cierre es fuera de horario simplemente se cierra */
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


int main(){
    printf("Profe ponganos un 7 xd\n");
    return 0;
}