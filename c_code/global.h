//Declaraci�n de constantes sobre la BBDD
const int NUM_PUNTOS = 200;
const int NUM_DIM = 13;
const int NUM_FEATURES= 5;
const int NUM_TIPOS = 2;

//Declaraci�n de constantes sobre el NSGA2
const int TIPO_OBJETIVOS=1; //Si vale 1, los objetivos a considerar son max_distancia y min_suma_epsilon. Si vale 2, los objetivos son min_mal_clasif de ambos tipos

const int NUM_OBJETIVOS = 3;
const int TAM_POBLACION=50;


const float MAX_FLOAT=3.4e+15f;
const float MIN_FLOAT=-3.4e+15f;
const float PRECISION_FLOAT=0.000000001;
const float PRECISION_DOM = 0.000001; /*OJO, SI SE CAMBIA ESTA PRECISIÓN, SE DEBERÍA CAMBIAR LA PRECISIÓN CON LA QUE SE IMPRIMEN LOS VFO EN EL FICHERO DE RESULTADOS*/

const int MAX_INT= 32767;
const unsigned int MAX_UNSIG_INT=65534;



const double MAX_DOUBLE= 1.7E+308;

//const float P_CRUCE1=0.8; //Indica la probabilidad con la que el hijo hereda una caracter�stica no com�n del padre mejor. La probabilidad contraria es la de heredar del padre peor.



const float P_MUT=0.7; //Probabilidad de que a cada individuo se le aplique el proceso de mutaci�n
const float P_MUT_IND=0.4; //Indica la probabilidad de que mute cada indice de la solucion a mutar. UNA RAZONABLE ES 1/NUM_FEAT
const float P_MUT_FEAT=0.4; //Indica la probabilidad de que mute cada feature de la solucion a mutar. OJO, si no hay seleccion de caracter�sticas (se eligen todas), esta probabilidad debe ser 0, ya que no se puede intercambiar una feature por otra que no est�. UNA RAZONABLE ES 1/NUM_FEAT
const float P_MUT_COORD= 0.2;
const int TIPO_MUT_COORD=0; //Dos tipos de mutaci�n de coordenadas: 1, la coordenada muta de forma totalmente aleatoria; 0 la coordenada sufre una variaci�n ligera respecto a la anterior. Se ha probado y la no aleatoria funciona mejor


const int CLONES=0; //Determina si se permiten clones en los valores objetivo (CLONES=1) o no (CLONES=0)
const float PRECISION_CLONES_F1=0.1; //Diferencia en Objetivo1 (distancia) para considerarlos iguales. Cuando TIPO_OBJETIVOS=2, al ser los objetivos enteros, no se usa precisi�n, son clones cuando coinciden los valores
const float PRECISION_CLONES_F2=0.1; //Diferencia en objetivo2 (suma epsilon) para considerarlos iguales.Cuando TIPO_OBJETIVOS=2, al ser los objetivos enteros, no se usa precisi�n, son clones cuando coinciden los valores
const float PRECISION_CLONES_F3=0.001;

//Declaraci�n de estructuras globales
struct punt {
    float coord[NUM_DIM];
    int tipo;

};

struct dat{
    punt punto[NUM_PUNTOS];
    int tot_tipo[NUM_TIPOS]; //Indica el n�mero total de puntos de cada tipo;
    int indices [NUM_TIPOS][NUM_PUNTOS]; //Lista de �nices para cada tipo
    float costes[NUM_DIM]; // Lista con los costes de cada característica;
    };

struct soluc{
    int modo;//El modo 0 tiene las NUM_FEATURES primeras posiciones con puntos de tipo -1 (en la bbdd, en nuestra memoria 0) y el �ltimo punto tipo 1 (tanto en la bbdd como en memoria)
    //El modo 1 tiene la primera posicion con puntos de tipo -1 (en la bbdd, en nuestra memoria 0) y el resto de puntos tipo 1 (tanto en la bbdd como en memoria)
    double obj[NUM_OBJETIVOS]; //VFO objetivo especificado. El objetivo 0 es la distancia (se maximiza) y el 1 es el epsilon (se minimiza)
    double obj2[NUM_OBJETIVOS]; //El objetivo 0 es min el numero de mal clasificados tipo 0 y el objetivo 1 el numero de mal clasificados tipo 1.
    // la variable global TIPO_OBJETIVOS indicar� si se consideran los objetivos en obj (TIPO_OBJETIVOS=1)o en obj2 (TIPO_OBJETIVOS=2)
    double epsilon[NUM_TIPOS]; //El objetivo epsilon se calcula como la suma de los epsilon de los puntos tipo 0 y los epsilon de los puntos tipo1

    int puntos_en_plano[NUM_TIPOS];
    int puntos_mal_clasif[NUM_TIPOS]; //contador para el numero de puntos mal clasificados de cada tipo
    //int indices[NUM_FEATURES+1]; //INDICES_SOLUCION vale, o bien NUM_FEATURES+1, con una posici�n adicional para el punto del otro tipo por el que pasar� el plano paralelo, o bien 2, si solo son dos puntos.
    int indices[2]; //Las soluciones ahora s�lo tienen 2 �ndices
    int features[NUM_FEATURES]; //indica los �ndices de las features elegidas en esta soluci�n, ordenadas, siendo la primera columna del fichero la feature 0
    double plano_coord[NUM_FEATURES];
    double plano_termino[NUM_TIPOS+1];//La posici�n adicional es para el t�rmino independiente del plano solucion (el intermedio)
    int signo_intermedio[NUM_TIPOS]; //en la posicion signo_intermedio[0] tendremos un 0 si al evaluar los puntos tipo 0 en el plano intermedio nos tiene que salir un valor menor que el t�rmino independiente y un 1 si nos tiene que salir un valor mayor que el t�rmino independiente, en la posicion signo[1] habr� siempre lo contrario a signo[0]

    //int evaluada; //0 no ha sido evaluada; 1 s� lo ha sido

    int num_sol_me_dom; //numero de soluciones que dominan a �sta
    int sol_domina[2*TAM_POBLACION]; //Conjunto de soluciones a las que �sta domina
    int num_sol_domina; //numero de soluciones a las que �sta domina
    int front; //front en el que queda colocada la solucion
    double i_distance;
};


struct poblac{
    soluc solucion[2*TAM_POBLACION+2];//Al generar por cruce 4 hijos, es necesario tener 2 espacios adicionales
    //int indice_mejor_eps, indice_mejor_distancia, indice_mejor_mal_clasif;
    int num_fronts;
    int num_sol_front[2*TAM_POBLACION]; //En la posicion [i] indica el n�mero de soluciones que pertenecen al front_i. El front 0 es al que pertenecen las soluciones no dominadas

};


