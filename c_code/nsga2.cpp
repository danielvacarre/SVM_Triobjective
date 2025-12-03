#include <stdlib.h>                                  
#include <math.h>  
#include <time.h>
#include <stdio.h> 
#include "global.h"
#include "nsga2.h"

extern poblac *poblacion;
extern dat *datos;

//genera un numero aleatorio entre 0 y el parametro hasta-1
int genera_num_aleat_entero(int hasta)
{
    int random;
    
    random=rand();
    random=fmod(random,hasta);
    return(random);
}

float genera_num_aleatorio_real01()
{

    float random;
    
    random=rand();
    random=random/RAND_MAX;
    return(random);
}

float genera_num_aleatorio_real_menos1_1()
{

    float random1, random2;
    
    random1=rand();
    random1=random1/RAND_MAX;
    random2=rand();
    random2=random2/RAND_MAX;
    if (random2<0.5) random1=-random1;
    return(random1);
}

void genera_solucion_aleat(int num_sol)
{
    
    struct indice_lista
    {
        int indice;
        indice_lista *sigte;
    };
    
    int indice;
    int indices_features[NUM_DIM];
    int j, k;
    int tam_lista, desplaz;
    //float random;
    indice_lista *punt_lista, *punt_lista_aux, *punt_lista_aux2;
    
    
    if (NUM_FEATURES==NUM_DIM) //las caracteristicas son todas, desde 0 hasta NUM_FEATURES-1
    {
        for (j=0; j<NUM_FEATURES; j++)
            poblacion->solucion[num_sol].features[j]=j;
    }
    else //Las caracteristicas se heredan de forma aleatoria
    {
        //  En primer lugar vamos a generar con punteros una lista de indices de dimensiones, de la primera a la última, que después vamos a ir recorriendo para seleccionar aleatoriamente las features de esta solución
        //***************
        punt_lista=new(indice_lista);
        punt_lista->indice=0;
        //El primer elemento de la lista es la guia y no sirve
        punt_lista->sigte=NULL;
        punt_lista_aux=punt_lista;
    
        for (j=0; j<NUM_DIM; j++)
        {
            indices_features[j]=0;
        
            punt_lista_aux2=new(indice_lista);
            punt_lista_aux2->indice=j;
            punt_lista_aux2->sigte=NULL;
            punt_lista_aux->sigte=punt_lista_aux2;
            punt_lista_aux=punt_lista_aux2;
        }
        tam_lista=NUM_DIM;
        for (j=0; j<NUM_FEATURES; j++)
        {
            
            desplaz=genera_num_aleat_entero(tam_lista)+1;
            //desplaz es un entero entre 1 y el numero total de características
            punt_lista_aux=punt_lista;
            punt_lista_aux2=punt_lista_aux->sigte;
            for (k=1; k<desplaz; k++)
            {
                punt_lista_aux2=punt_lista_aux2->sigte;
                punt_lista_aux=punt_lista_aux->sigte;
                
            }
            indices_features[punt_lista_aux2->indice]=1;
            punt_lista_aux->sigte=punt_lista_aux2->sigte;
            delete(punt_lista_aux2);
            tam_lista--;
        }
        k=0;
        for (j=0;j<NUM_DIM;j++)
        {
            if (indices_features[j]==1)
            {
                poblacion->solucion[num_sol].features[k]=j;
                k++;
            }
        }
        
        for (j=1;j<=(NUM_DIM-NUM_FEATURES+1);j++)
        {
            punt_lista_aux=punt_lista->sigte;
            delete(punt_lista);
            punt_lista=punt_lista_aux;
        }
        punt_lista_aux2=NULL;
    }
    
    //***************

        
    indice=genera_num_aleat_entero(datos->tot_tipo[0]);
    poblacion->solucion[num_sol].indices[0]=datos->indices[0][indice];
    
    indice=genera_num_aleat_entero(datos->tot_tipo[1]);
    poblacion->solucion[num_sol].indices[1]=datos->indices[1][indice];
    
        
    
    //Inicializacion de valores
    //poblacion->solucion[num_sol].evaluada=0;
    for (int i=0; i<NUM_OBJETIVOS;i++)
    {
        poblacion->solucion[num_sol].obj[i]=0;
        poblacion->solucion[num_sol].obj2[i]=0;
    }
    for (int i=0; i<NUM_TIPOS;i++)
    {
        poblacion->solucion[num_sol].puntos_en_plano[i]=0;
        poblacion->solucion[num_sol].plano_termino[i]=0;
        poblacion->solucion[num_sol].puntos_mal_clasif[i]=0;
        poblacion->solucion[num_sol].epsilon[i]=0;
        
    }
    poblacion->solucion[num_sol].plano_termino[NUM_TIPOS]=0;
    //Se generan aleatoriamente las coordenadas de los planos
    for (int i=0; i<NUM_FEATURES;i++)
        poblacion->solucion[num_sol].plano_coord[i]=genera_num_aleatorio_real_menos1_1();
 
      printf("\n\tGenerada solucion aleatoria %d", num_sol);
}

void genera_poblacion_inicial()
{
    srand(123);
    
    int i=0, j=0;
    clock_t segundos_ini=0, segundos_fin=0;
    double segundos_trans=0, segundos_trans2=0;
    
    
    printf("\n\nGenerando poblacion inicial de tamaño %d....\n", TAM_POBLACION);
    
    //for (i=0; i<TAM_POBLACION; i++)
    while (i<TAM_POBLACION)
    {  
        genera_solucion_aleat(i);
        j++;
        segundos_ini=clock();
        if (evaluar_solucion(i)!=0)
        {
        
            if (CLONES==0) //No se permiten clones
            {
                if (buscar_clones(i)==0) //No hay clones
                {
                    i++;
                }
                else { } //es un clon
            }
            else //se permiten clones
            {
                i++;
            }
        }
        segundos_fin=clock();
        segundos_trans2=((double) (segundos_fin-segundos_ini))/CLOCKS_PER_SEC;
        segundos_trans+=segundos_trans2;
        printf(". Tiempo: %.4f segundos\n ", segundos_trans2);


    }
    
    printf("\n\nTotal soluciones generadas: %d\n", j);
    printf("\nTiempo total: %.4f segundos. %.6f segundos/sol", segundos_trans, segundos_trans/j);
    
    printf("\n\nTotal soluciones finales generadas: %d\n\n", TAM_POBLACION);

}

int construir_planos_solucion(unsigned int num_sol)
{
    
    
    int j;
    double indep;
    

    // Lo primero es construir la ecuación del plano dada por la parte principal de la solución, es decir, aquella que tiene tantos puntos como dimensiones
    //El plano tiene forma m x + n y - b = 0 Siendo b el termino independiente
    //Las coordenadas (m, n, ...) ya están generadas. Falta sólo calcular los términos independientes de ambos planos y del intermedio

    indep =0;
    for (j=0; j<NUM_FEATURES; j++)
    indep=indep+poblacion->solucion[num_sol].plano_coord[j]*datos->punto[    poblacion->solucion[num_sol].indices[0]].coord[poblacion->solucion[num_sol].features[j]];
    poblacion->solucion[num_sol].plano_termino[0]=-indep;
       
    indep =0;
    for (j=0; j<NUM_FEATURES; j++)
        indep=indep+poblacion->solucion[num_sol].plano_coord[j]*datos->punto[    poblacion->solucion[num_sol].indices[1]].coord[poblacion->solucion[num_sol].features[j]];
        poblacion->solucion[num_sol].plano_termino[1]=-indep;
//        printf("\n");
        

    poblacion->solucion[num_sol].plano_termino[2]=(poblacion->solucion[num_sol].plano_termino[0]+poblacion->solucion[num_sol].plano_termino[1])/2;
    
    //Aqui vamos a comprobar si todos los coeficientes del plano son 0, en ese caso, la función devuelve un 0 (error) para que se descarte dicha solución y se genere de nuevo
    int k=0;
    int seguir=1;
    while ((k<NUM_FEATURES)&&(seguir==1))
    {
        if (abs_2(poblacion->solucion[num_sol].plano_coord[k])>PRECISION_FLOAT)
            seguir=0;
        k++;
    }
    if (seguir==1)
        return(0); //Todos los terminos son 0. La solucion debe descartarse;
    else
        return(1); //El plano es valido. La solución no debe descartarse;
    
}

void calcular_objetivo_epsilon(unsigned int num_sol)
{

    //int distancia_punto_calculada[NUM_PUNTOS];
    int punto_en_plano[NUM_TIPOS][NUM_PUNTOS];
    int signo_distancia_plano[NUM_TIPOS];
    //signo_distancia_plano[0] tiene un -1 si los puntos tipo 0 que estan en el lado incorrecto del plano 0 estan a distancia negativa (sin valor absoluto) del plano 0
    //signo_distancia_plano[1] tiene un -1 si los puntos tipo 1 que estan en el lado incorrecto del plano 1 estan a distancia negativa (sin valor absoluto) del plano 0
    //double valores_objetivo[NUM_OBJETIVOS][NUM_TIPOS];
    // Para el cálculo del objetivo 1, nos basamos en cada uno de los planos. Debemos calcular la distancia de cada uno de los puntos al plano correspondiente
    // evidentemente todos los puntos que forman parte de la solucion tienen distancia 0 a su plano correspondiente.
    // Sin embargo, puede haber más puntos con distancia cero a su plano correspondiente aunque no estén en la solución. Estos puntos los vamos a guardar de alguna forma pues nos pueden ser útiles en operaciones posteriores
    double denominador=0;
    double distancia=0;
    double max_distancia;
    double distancia_aux=0;
    
    for (int j=0; j<NUM_TIPOS; j++)
        poblacion->solucion[num_sol].epsilon[j]=0;
    poblacion->solucion[num_sol].obj[1]=0;
    
    for (int i=0; i<NUM_PUNTOS;i++)
    {
        //distancia_punto_calculada[i]=0;
        for (int j=0; j<NUM_TIPOS; j++)
        {
            punto_en_plano[j][i]=0;
            
        }
    }
    for (int j=0; j<NUM_TIPOS; j++)
//        El contador de puntos que se apoyan en cada plano es cero
        poblacion->solucion[num_sol].puntos_en_plano[j]=0;
    
    
    
    max_distancia=0;
    for (int i=0; i<=1; i++)
    {
        //Calculamos la distancia de cada punto de la solucion a su correspondiente plano. Estas distancias deberían ser todas 0, pero como por precision algunas no lo son, calculamos la mayor de estas distancias, que va a ser la precisión para determinar si alguno de los otros puntos que no estan en la solución tienen esa distancia o menos
        denominador=0;
        distancia=0;
        //////if (distancia_punto_calculada[i]!=0)
        //////{
        
        for (int j=0; j<NUM_FEATURES; j++)
        {
            distancia+=poblacion->solucion[num_sol].plano_coord[j]*datos->punto[poblacion->solucion[num_sol].indices[i]].coord[poblacion->solucion[num_sol].features[j]];
            
            //denominador+=pow(datos->punto[poblacion->solucion[num_sol].indices[i]].coord[poblacion->solucion[num_sol].features[j]],2);
            denominador+=pow(poblacion->solucion[num_sol].plano_coord[j],2);
            
        }
        denominador=sqrt(denominador);
        distancia+=poblacion->solucion[num_sol].plano_termino[datos->punto[poblacion->solucion[num_sol].indices[i]].tipo];
        if (distancia<0)
        {
            distancia=-distancia;
        }
        distancia/=denominador;
        if (distancia>max_distancia)
            max_distancia=distancia;
        punto_en_plano[datos->punto[poblacion->solucion[num_sol].indices[i]].tipo][poblacion->solucion[num_sol].indices[i]]=1;
        poblacion->solucion[num_sol].puntos_en_plano[datos->punto[poblacion->solucion[num_sol].indices[i]].tipo]++;
        //distancia_punto_calculada[poblacion->solucion[num_sol].indices[i]]=1;
    
    }
    ///////NUEVO/////
    //VAMOS A EVALUAR EL SIGNO DE LA DISTANCIA DE UN PUNTO TIPO 0 AL PLANO TIPO 1 Y EL SIGNO DE LA DISTANCIA DE UN PUNTO TIPO 1 AL PLANO CERO
    
    
    //Evaluamos en el plano1 el punto en la posicion 0, que es siempre de tipo 0
    distancia=0;
    
    for (int j=0; j<NUM_FEATURES; j++)
    {
        distancia+=poblacion->solucion[num_sol].plano_coord[j]*datos->punto[poblacion->solucion[num_sol].indices[0]].coord[poblacion->solucion[num_sol].features[j]];
        
        
    }
    
    distancia+=poblacion->solucion[num_sol].plano_termino[1];
    if (distancia<0)
    {
        signo_distancia_plano[1]=-1;
        signo_distancia_plano[0]=1;
    }
    else
    {
        signo_distancia_plano[0]=-1;
        signo_distancia_plano[1]=1;
    }
    
    
    /////FIN NUEVO. TODO ESTO NUEVO DEBERÍA AÑADIRSE SI EN LUGAR DE USAR ESTA FUNCION USÁRAMOS LA FUNCIÓN  calcular_distancias_puntos_su_plano2/////
    
    //Ahora calculamos las distancias del resto de puntos, que no pertenecen a la solución, a su respectivo plano. En caso que la distancia sea menor o igual a distancia_max, la consideramos y ponemos a cero
    for (int i=0; i<NUM_PUNTOS; i++)
    {
        if (punto_en_plano[datos->punto[i].tipo][i]==0)
        {
            // Si no es un punto de la solucion
            //Calculamos la distancia de cada punto de la bbdd a su correspondiente plano. Objetivo 0
            denominador=0;
            distancia=0;
            int signo_distancia=0;;
                              
            for (int j=0; j<NUM_FEATURES; j++)
            {
            distancia+=poblacion->solucion[num_sol].plano_coord[j]*datos->punto[i].coord[poblacion->solucion[num_sol].features[j]];
            
                //denominador+=pow(datos->punto[i].coord[poblacion->solucion[num_sol].features[j]],2);
                denominador+=pow(poblacion->solucion[num_sol].plano_coord[j],2);
            }
            denominador=sqrt(denominador);
            distancia+=poblacion->solucion[num_sol].plano_termino[datos->punto[i].tipo];
            distancia_aux=distancia;
            if (distancia<0)
            {
                distancia=-distancia;
                signo_distancia=-1;
            }
            else if (distancia>=0) signo_distancia=1;
            
            distancia/=denominador;
            if (distancia<=(max_distancia+PRECISION_FLOAT))
            //if (distancia<=(max_distancia)) SVM_PLUS pensar si cambiar el anterior por este if
            {
                distancia=0;
                poblacion->solucion[num_sol].puntos_en_plano[datos->punto[i].tipo]++;
                punto_en_plano[datos->punto[i].tipo][i]=1;
            }
            else
            {
                if (signo_distancia*signo_distancia_plano[datos->punto[i].tipo]>0)
                    poblacion->solucion[num_sol].epsilon[datos->punto[i].tipo]+=distancia;
                    //valores_objetivo[0][datos->punto[i].tipo]+=distancia;
                    
            }
        //printf("");
        }
                              
    }
    poblacion->solucion[num_sol].obj[1]=poblacion->solucion[num_sol].epsilon[0]+poblacion->solucion[num_sol].epsilon[1];
    
    
    
    //OJO SI EL CALCULO ES NAN, SE PONE AL MÁXIMO VALOR POSIBLE PARA QUE LA SOLUCIÓN SE DESCARTE
//    printf("\n\tPlano h-: %d puntos. Plano h+: %d puntos", poblacion->solucion[num_sol].puntos_en_plano[0], poblacion->solucion[num_sol].puntos_en_plano[1]);
//    printf("\nObjetivo epsilon: %lf\n\tepsilon_0: %lf \n\tepsilon_1: %lf",poblacion->solucion[num_sol].obj[1], poblacion->solucion[num_sol].epsilon[0], poblacion->solucion[num_sol].epsilon[1]);
    if (isnan(poblacion->solucion[num_sol].obj[1]))
        poblacion->solucion[num_sol].obj[1]=MAX_DOUBLE;
    
}

void calcular_objetivo_distancia(unsigned int num_sol)
{
    //Esta funcion calcula la distancia entre ambos planos para una solución
    //Vamos a calcular directamente la distancia entre dos planos con la fórmula
    
    //Dicha distancia corresponde con el valor del objetivo 0
    
    
    double denominador=0;
    double distancia=0;
    
    poblacion->solucion[num_sol].obj[0]=0;
    for (int j=0; j<NUM_FEATURES; j++)
    {
        denominador+=+pow(poblacion->solucion[num_sol].plano_coord[j],2);
    }
    denominador=sqrt(denominador);
    distancia=poblacion->solucion[num_sol].plano_termino[0]-poblacion->solucion[num_sol].plano_termino[1];
    if (distancia<0)
    {
        distancia=-distancia;
    }
    distancia/=denominador;
    poblacion->solucion[num_sol].obj[0]=distancia;
    //printf("\n\tDistancia entre planos: %lf", distancia);
    if (isnan(distancia))
        poblacion->solucion[num_sol].obj[0]=-1;
    // OJOOOOOOO; La distancia se pone a -1, puesto que ha saluido NaN
    
}

void calcular_objetivo_costes(unsigned int num_sol){
    int j;
    int feature;
    float coste= 0.0;
    //printf("\n");
    for(j=0;j<NUM_FEATURES;j++){
        feature = poblacion->solucion[num_sol].features[j];
        //printf("FEATURE %d-%f\t", feature, datos->costes[feature]);
        coste = coste + datos->costes[feature];
    }
    //printf("COSTE_TOTAL=%f\n",coste);
    poblacion->solucion[num_sol].obj[2]=coste;
    poblacion->solucion[num_sol].obj2[2]=coste;

}

double calcular_distancia_plano_intermedio(unsigned int num_sol, int indice)
{
 
    
    double denominador=0;
    double distancia=0;
    
    for (int j=0; j<NUM_FEATURES; j++)
    {
    
        distancia+=poblacion->solucion[num_sol].plano_coord[j]*datos->punto[indice].coord[poblacion->solucion[num_sol].features[j]];
        denominador+=+pow(poblacion->solucion[num_sol].plano_coord[j],2);
    }
    denominador=sqrt(denominador);
    distancia+=poblacion->solucion[num_sol].plano_termino[NUM_TIPOS];
    if (distancia<0)
    {
        distancia=-distancia;
    }
    distancia/=denominador;
    return(distancia);
}
    
int evaluar_punto_plano_intermedio(unsigned int num_sol, int indice)
{
    //Esta función evalúa el punto cuyo indice se pasa como argumento en el plano intermedio, devolviendo un 0 si es menor que el termino independiente de éste, un 1 si es mayor y un 2 si es igual, es decir, car en el plano intermedio. LOS PUNTOS QUE CAEN EN EL PLANO INTERMEDIO SE CONSIDERAN MAL CLASIFICADOS
    
    double eval=0;
    
    for (int j=0; j<NUM_FEATURES; j++)
        eval=eval+(poblacion->solucion[num_sol].plano_coord[j]*datos->punto[indice].coord[poblacion->solucion[num_sol].features[j]]);

    if (eval>-(poblacion->solucion[num_sol].plano_termino[NUM_TIPOS]))
        return(1);
    else if (eval<-(poblacion->solucion[num_sol].plano_termino[NUM_TIPOS]))
        return(0);
    else
        return(2);
}

void calcular_puntos_bien_clasif(unsigned int num_sol)
{
    double eval=0;
    int k, resul_eval;
    

    poblacion->solucion[num_sol].puntos_mal_clasif[0]=0;
    poblacion->solucion[num_sol].puntos_mal_clasif[1]=0;

    
    k=poblacion->solucion[num_sol].indices[0];
    //Voy a evaluar siempre el punto en primera posición, que es de tipo - o tipo0
    for (int j=0; j<NUM_FEATURES; j++)
        eval=eval+(poblacion->solucion[num_sol].plano_coord[j]*datos->punto[k].coord[poblacion->solucion[num_sol].features[j]]);
    if (eval>-(poblacion->solucion[num_sol].plano_termino[NUM_TIPOS]))
    {
        poblacion->solucion[num_sol].signo_intermedio[0]=1;
        poblacion->solucion[num_sol].signo_intermedio[1]=0;
    }
    else
    {
        poblacion->solucion[num_sol].signo_intermedio[0]=0;
        poblacion->solucion[num_sol].signo_intermedio[1]=1;
    }
    //Ahora en signo[0] tengo un 0 si al evaluar un punto tipo0 me tiene que salir menor que el termino independiente y un 1 si me tiene que salir mayor
    //Ahora en signo[1] tengo un 0 si al evaluar un punto tipo1 me tiene que salir menor que el termino independiente y un 1 si me tiene que salir mayor
    //Ahora voy a evaluar cada uno de los puntos de la BBDD, sabiendo que los puntos tipo1 tendrán que dar un valor superior/inferior al termino independiente del plano intermedio
    //y los tipo 0 un valor inferior/superior. Si un punto no cumple esta condición, lo contabilizo como punto mal clasificado, calculo su distancia al plano intermedio, y la sumo al valor de epsilon;
    //Empezamos calculando los puntos tipo 0
    //printf("\nEvaluación punto: %d\n", num_sol);
    for (int m=0; m<NUM_TIPOS; m++)
    {
        //printf("\nTipo\n: %d", m);
        for (int i=0; i<datos->tot_tipo[m];i++)
        {
            
            resul_eval=evaluar_punto_plano_intermedio(num_sol, datos->indices[m][i]); //esta funcion devuelve un 0 si la evaluación es menor, un 1 si es mayor Y UN 2 SI ES IGUAL
            
            if (resul_eval!=poblacion->solucion[num_sol].signo_intermedio[m]) //El punto tipo m está mal clasificado
            {
                
                poblacion->solucion[num_sol].puntos_mal_clasif[m]++;
//   Quitamos de aqui el calculo de epsilon             poblacion->solucion[num_sol].epsilon[m]+=calcular_distancia_plano_intermedio(num_sol, datos->indices[m][i]);
            }
            
        }
    }

    poblacion->solucion[num_sol].obj2[0]=poblacion->solucion[num_sol].puntos_mal_clasif[0];
    poblacion->solucion[num_sol].obj2[1]=poblacion->solucion[num_sol].puntos_mal_clasif[1];
}
    
int evaluar_solucion(unsigned int num_sol)
//Añadido en dic 2020: si todos los coeficientes del plano son 0, la solución debe descartarse. Si ocurre esto, esta función va a generar de nuevo la solución a evaluar (aleatoriamente) y la vuelve a evaluar hasta que esto no ocurra
{
    //float obj0, obj1;
//    int i, j;
    
    
    //Añadido: Se hace un bucle hasta que la función construir_planos_solucion devuelva que la solución no debe descartarse
    //En esta versión, contruir_planos sólo calcula los terminos independientes, puesto que las componentes del plano intermedio están generadas
    if (construir_planos_solucion(num_sol)==0) return(0);
    
    // Ecuación de ambos planos construidas
    // Ambas ecuaciones se diferencian solo en el término independiente
    
    calcular_objetivo_distancia(num_sol);
    //calcular_objetivo_distancia2(num_sol);
   
    calcular_objetivo_epsilon(num_sol);//Esta funcion determina que puntos caen en su propio plano y los guarda. Puede ser necesario mas adelante. Además, los puntos que caen en la parte equivocada del plano, su distancia la acumula para el cálculo del epsilon EN EL CORRESPONDIENTE EPSILON[TIPO]
    
    calcular_puntos_bien_clasif(num_sol); //Se calculan los puntos bien/mal clasificados, es decir, caen en el lado correcto/incorrecto del plano intermedio. ESTA FUNCION PUEDE NO SER NECESARIA MAS ADELANTE
  
    if (NUM_OBJETIVOS==3){
        calcular_objetivo_costes(num_sol);
    }

    return(1);
    
}

unsigned int compara_padres(unsigned int padre1, unsigned int padre2)
//Devuelve cual de las dos soluciones apuntadas por los indices que se le pasa es mejor en funcion de: i) front; ii)menor nº sol que le dominan; iii)mayor nº sol a las que domina. ESTE CRITERIO SE ELIMINA YA QUE CUANDO SE HAGA EL CRUCE, DESPUES DE HABER HECHO LA REDUCCION DE POBLACION, SI SE HAN ELIMINADO SOLUCIONES ESTE VALOR CALCULADO ANTES PUEDE NO SER EL CORRECTO; iv) aleatorio
{
    
    int moneda;
    
    if (poblacion->solucion[padre1].front==poblacion->solucion[padre2].front)
    {
        if (poblacion->solucion[padre1].num_sol_me_dom==poblacion->solucion[padre2].num_sol_me_dom)
        
        {

                moneda=genera_num_aleat_entero(2);
                if (moneda==0)
                {
                    return (padre1);
                }
                else
                {
                    return(padre2);
                }

        }
        else
        {
            if (poblacion->solucion[padre1].num_sol_me_dom<=poblacion->solucion[padre2].num_sol_me_dom)
            {
                return(padre1);
            }
            else
            {
                return(padre2);
            }
        }
        
        
    }
    else
    {
        if (poblacion->solucion[padre1].front>poblacion->solucion[padre2].front)
        {
            return(padre1);
        }
        else
        {
            return(padre2);
        }
    }
}

int comprobar_caracteristicas_heredadas(int hijo)
{
    
    for (int i=0; i<NUM_FEATURES-1; i++)
        for (int j=i+1; j<NUM_FEATURES; j++)
            if (poblacion->solucion[hijo].features[i]==poblacion->solucion[hijo].features[j])
                return(1); //Hay features repetidas
    return(0);
}

int comprobar_double(double a, double b){
    
    double value = a-b;
    if (value>PRECISION_DOM){
        return 1;
    }else if(value<PRECISION_DOM && value>(-PRECISION_DOM)){
        return 0;
    }else{
        return -1;
    }
}

int domina(int p, int q)
{
    /* Devuelve:
       0 -> ninguna domina
       1 -> p domina a q
       2 -> q domina a p
    */

    int o[3];   // hasta 3 objetivos
    int i;

    // 1) Calcular las comparaciones objetivo a objetivo
    if (TIPO_OBJETIVOS == 1){
        for (i = 0; i < NUM_OBJETIVOS; i++){
            o[i] = comprobar_double(poblacion->solucion[p].obj[i],
                                    poblacion->solucion[q].obj[i]);
        }
    }
    else{
        for (i = 0; i < NUM_OBJETIVOS; i++){
                    o[i] = comprobar_double(poblacion->solucion[p].obj2[i],
                                    poblacion->solucion[q].obj2[i]);
        }
    }

    // 2) Caso: igualdad total → nadie domina
    int iguales = 1;
    for (i = 0; i < NUM_OBJETIVOS; i++)
        if (o[i] != 0)
            iguales = 0;

    if (iguales)
        return 0;

    // 3) Comprobar si p domina q
    int p_no_peor = 1;   // p es >= (max) o <= (min) según tu convención
    int p_mejor_en_algo = 0;

    if (TIPO_OBJETIVOS == 1){
        for (i = 0; i < NUM_OBJETIVOS; i++){
            if (i == 0) { // DIST (max)
                if (o[i] < 0) p_no_peor = 0;   // peor
                if (o[i] > 0) p_mejor_en_algo = 1; // mejor
            }
            else { // epsilon y coste (min)
                if (o[i] > 0) p_no_peor = 0;   // peor
                if (o[i] < 0) p_mejor_en_algo = 1; // mejor
            }
        }
    }
    else{
        for (i = 0; i < NUM_OBJETIVOS; i++){
            // FP-FN: todo es minimización
            if (o[i] > 0) p_no_peor = 0;     // peor
            if (o[i] < 0) p_mejor_en_algo = 1; // mejor
        }
    }

    if (p_no_peor && p_mejor_en_algo)
        return 1;   // p domina a q

    // 4) Comprobar si q domina p (inverso)
    int q_no_peor = 1;
    int q_mejor_en_algo = 0;


    if (TIPO_OBJETIVOS == 1){
        for (i = 0; i < NUM_OBJETIVOS; i++){
            int inv = -o[i];
            if (i == 0) { // DIST max (ahora q >= p)
                if (inv < 0) q_no_peor = 0;
                if (inv > 0) q_mejor_en_algo = 1;
            }
            else { // min
                if (inv > 0) q_no_peor = 0;
                if (inv < 0) q_mejor_en_algo = 1;
            }
        }
    }
    else{
        for (i = 0; i < NUM_OBJETIVOS; i++){
            int inv = -o[i];
            // todo min
            if (inv > 0) q_no_peor = 0;
            if (inv < 0) q_mejor_en_algo = 1;
        }
    }
        
    if (q_no_peor && q_mejor_en_algo)
        return 2;

    // Ninguna domina
    return 0;

}


void cruzar_soluciones(int num_sol1, int num_sol2, unsigned int hijo)
{
    int  j, dominador1, dominador2;
    unsigned int posicion1, posicion2;
    float random;
    
    //1º Hijo
    
    for (j=0; j<NUM_FEATURES; j++)
    {
        poblacion->solucion[hijo].features[j]=poblacion->solucion[num_sol1].features[j];
        poblacion->solucion[hijo+1].features[j]=poblacion->solucion[num_sol2].features[j];
        poblacion->solucion[hijo+2].features[j]=poblacion->solucion[num_sol1].features[j];
        poblacion->solucion[hijo+3].features[j]=poblacion->solucion[num_sol2].features[j];
        
        poblacion->solucion[hijo].plano_coord[j]=poblacion->solucion[num_sol1].plano_coord[j];
        poblacion->solucion[hijo+1].plano_coord[j]=poblacion->solucion[num_sol2].plano_coord[j];
        poblacion->solucion[hijo+2].plano_coord[j]=poblacion->solucion[num_sol1].plano_coord[j];
        poblacion->solucion[hijo+3].plano_coord[j]=poblacion->solucion[num_sol2].plano_coord[j];
    }
    
    poblacion->solucion[hijo].indices[0]=poblacion->solucion[num_sol1].indices[0];
    poblacion->solucion[hijo].indices[1]=poblacion->solucion[num_sol2].indices[1];
    
    poblacion->solucion[hijo+1].indices[0]=poblacion->solucion[num_sol1].indices[0];
    poblacion->solucion[hijo+1].indices[1]=poblacion->solucion[num_sol2].indices[1];
    
    poblacion->solucion[hijo+2].indices[0]=poblacion->solucion[num_sol2].indices[0];
    poblacion->solucion[hijo+2].indices[1]=poblacion->solucion[num_sol1].indices[1];
    
    poblacion->solucion[hijo+3].indices[0]=poblacion->solucion[num_sol2].indices[0];
    poblacion->solucion[hijo+3].indices[1]=poblacion->solucion[num_sol1].indices[1];
    
    //De las 4 soluciones generadas, nos vamos a quedar con las dos mejores, que se van a quedar en posiciones hijo e hijo+1
    //Evaluar soluciones
    //for(j=0; j<4; j++)
    //{
    //    evaluar_solucion(hijo+j);
    //}

    dominador1=domina(hijo, hijo+1);
    //Posicion nos da el lugar del perdedor entre hijo e hijo+1;
    posicion1=hijo;
    if (dominador1==1)
        posicion1=hijo+1;
    else
        if (dominador1==0)
        {
            random=genera_num_aleatorio_real01();
            if (random<0.5)
                posicion1=hijo+1;
        }
    
    
    dominador2=domina(hijo+2, hijo+3);
    //Posicion2 nos da el lugar del vencedor entre hijo+2 e hijo+3;
    posicion2=hijo+2;
    if (dominador2==2)
        posicion2=hijo+3;
    else
        if (dominador2==0)
        {
            random=genera_num_aleatorio_real01();
            if (random<0.5)
                posicion2=hijo+3;
        }
    
    for (j=0; j<NUM_FEATURES; j++)
    {
        poblacion->solucion[posicion1].features[j]=poblacion->solucion[posicion2].features[j];
        poblacion->solucion[posicion1].plano_coord[j]=poblacion->solucion[posicion2].plano_coord[j];
        
    }
    poblacion->solucion[posicion1].indices[0]=poblacion->solucion[posicion2].indices[0];
    poblacion->solucion[posicion1].indices[1]=poblacion->solucion[posicion2].indices[1];
    
    //printf("");
}

void fast_non_dominated_sort(unsigned int hasta){
   
    //hasta sirve para determinar si el FNDS se aplica solo a la poblacion P (TAM_POBLACION-1) o a la poblacion PUQ (TAM_POBLACION*3-1)
    //A las soluciones no dominadas se les asigna front=0 (En NSGA-II se lesd asigna front=1). En problacion->num_sol_front[i] se almacena el número de soluciones que tiene ese front
    int dom, front_cont;
    unsigned int p, q, k;
    int num_sol_me_dom_aux[2*TAM_POBLACION];
    
    printf("\n\nRealizando FNDS desde 0 hasta %d......",hasta);
    for (k=0; k<hasta; k++)
        poblacion->num_sol_front[k]=0;
    
    poblacion->num_fronts=0;
    
    for (p=0; p<=hasta; p++)
    {
        poblacion->solucion[p].num_sol_me_dom=0;
        poblacion->solucion[p].num_sol_domina=0;
        poblacion->solucion[p].front=-1;
        for (q=0; q<=hasta; q++)
        {
            if (p!=q)
            {
                dom=domina(p,q);
                if (dom==1) //si p domina a q
                {
                    poblacion->solucion[p].sol_domina[poblacion->solucion[p].num_sol_domina]=q;
                    poblacion->solucion[p].num_sol_domina++;
                    
                }
                else if (dom==2)
                    poblacion->solucion[p].num_sol_me_dom++;
            }
                
        }
        if (poblacion->solucion[p].num_sol_me_dom==0)
        {
            poblacion->solucion[p].front=0;
            poblacion->num_sol_front[0]++;
        }
    }
    front_cont=0;
    int seguir=1;
    
    for (p=0; p<=hasta; p++)
        num_sol_me_dom_aux[p]=poblacion->solucion[p].num_sol_me_dom; //se utiliza este array auxiliar para que al final de este procedimiento poblacion->solucion[p].num_sol_me_dom tenga el valor correcto
    
    while (seguir==1)
    {
        seguir=0;
        for (p=0; p<=hasta; p++)
        {
            if (poblacion->solucion[p].front==front_cont)
            {
                
                for (k=0; k<poblacion->solucion[p].num_sol_domina; k++)
                {
                    num_sol_me_dom_aux[poblacion->solucion[p].sol_domina[k]]--;
                    if (num_sol_me_dom_aux[poblacion->solucion[p].sol_domina[k]]==0)
                    {
                        poblacion->solucion[poblacion->solucion[p].sol_domina[k]].front=front_cont+1;
                        poblacion->num_sol_front[front_cont+1]++;
                            seguir=1;
                    }
                }
            }
        }
        front_cont++;
    }
    poblacion->num_fronts=front_cont;
    printf("\nNº fronts: %d",front_cont);
    
    printf("\nNº SND: %d", poblacion->num_sol_front[0]);
    
    
}

int torneo_elegir_padre(){
    unsigned int padre1, padre2;
    
    padre1=genera_num_aleat_entero(TAM_POBLACION);
    padre2=genera_num_aleat_entero(TAM_POBLACION);
    while (padre1==padre2)
        padre2=genera_num_aleat_entero(TAM_POBLACION);
    
    return(compara_padres(padre1, padre2)); //Devuelve cual de las dos soluciones apuntadas por los indices que se le pasa es mejor en funcion de: i) front; ii)menor nº sol que le dominan; iii)mayor nº sol a las que domina (ELIMINADO); iv) aleatorio
    
}



void mutar_solucion(int num_sol){
    
    int indice, i, j, volver_feat, dividir;
    float random, porcentaje_variar;
    int features_solucion[NUM_DIM];
    
    
    //Mutación de indices
    random=genera_num_aleatorio_real01();
    if (random<P_MUT_IND)
    {
        do {
            indice=genera_num_aleat_entero(datos->tot_tipo[0]);

        }while (indice==poblacion->solucion[num_sol].indices[0]);
        poblacion->solucion[num_sol].indices[0]=datos->indices[0][indice];

    }

    if (random<P_MUT_IND)
    {
        do {
            indice=genera_num_aleat_entero(datos->tot_tipo[1]);

        }while (indice==poblacion->solucion[num_sol].indices[1]);
        poblacion->solucion[num_sol].indices[1]=datos->indices[1][indice];

    }
    
    
    if (NUM_DIM>NUM_FEATURES) //Si el numero de dimensiones es igual al de características, la mutacion de features no debe llevarse a cabo;
    {
        //Mutación de features
        for (i=0; i<NUM_DIM; i++)
            features_solucion[i]=0;
        
        //printf("\n");
        for (i=0; i<NUM_FEATURES; i++) //OJO PONIA NUM_DIM Y LO CAMBIO POR NUM_FEATURES
            features_solucion[poblacion->solucion[num_sol].features[i]]=1;
        
        ////Cada característica muta con una probabilidad de mutación P_MUT_FEAT
        volver_feat=0;
        if (2*NUM_FEATURES>=NUM_DIM) volver_feat=1;
        for (i=0; i<NUM_FEATURES; i++)
        {
            random=genera_num_aleatorio_real01();
            if (random<=P_MUT_FEAT) //Debe mutar la feature en la posicion i de num_sol
            {
                indice=genera_num_aleat_entero(NUM_DIM);
                while (features_solucion[indice]==1)
                {
                    indice++;
                    if (indice==NUM_DIM)
                        indice=0;
                }
                if (volver_feat==1)
                    //features_solucion[i]=0; antes estaba asi
                    features_solucion[poblacion->solucion[num_sol].features[i]]=0;
                poblacion->solucion[num_sol].features[i]=indice;
                
                //OJOOOOOOO Si muta una feature le ponemos una componente aleatoria en el plano?? Quizás esto introduce más variabilidad en las soluciones. PENSARLO. Se hace en la siguiente linea
                poblacion->solucion[num_sol].plano_coord[i]=genera_num_aleatorio_real01();
                
                features_solucion[indice]=1;
            }
            
        }
    }
    //Mutación de los planos
    //Las componentes del plano que muten se van a ver incrementadas o decrementadas un porcentaje
    //Ojo, debemos hacer que los valores de las componentes siempre estén en el intervalo [0,1]
    for (i=0; i<NUM_FEATURES; i++)
    {
        random=genera_num_aleatorio_real01();
        if (random<=P_MUT_COORD) //Debe mutar la componente del plano en la posicion i de num_sol
        {
            if (TIPO_MUT_COORD==0)
            {
                //La coordenada en el plano seleccionada se va a incrementar o decrementar entre un 5 y un 25%
                porcentaje_variar=genera_num_aleat_entero(21); //toma valores entre 0 y 20;
                porcentaje_variar+=5; //Ahora toma valores entre 5 y 25;
                //Decidimos aleatoriamente si se incrementa o se decrementa ese porcentaje
                random=genera_num_aleatorio_real01();
                dividir=0;
                if (random<0.5) //se incrementa
                {
                    poblacion->solucion[num_sol].plano_coord[i]=poblacion->solucion[num_sol].plano_coord[i]*(1+porcentaje_variar/100);
                    if (poblacion->solucion[num_sol].plano_coord[i]>1)
                        dividir=1;
                }
                else  //se decrementa
                {
                    poblacion->solucion[num_sol].plano_coord[i]=poblacion->solucion[num_sol].plano_coord[i]*(1-porcentaje_variar/100);
                    if (poblacion->solucion[num_sol].plano_coord[i]<-1)
                        dividir=1;
                }
                if (dividir>0) //La coordenada que ha mutado es mayor que 1 o menor que -1. SE debe dividir todo por 10 (ya que nunca se incrementará por encima de 2 o debajo de -2
                    for (j=0; j<NUM_FEATURES; j++)
                        poblacion->solucion[num_sol].plano_coord[j]=poblacion->solucion[num_sol].plano_coord[j]/10;
            }
            else //TIPO_MUT_COORD=1 mutación aleatoria
            {
                poblacion->solucion[num_sol].plano_coord[i]=genera_num_aleatorio_real_menos1_1();
            }
            
        }
        
    }
}
////////////CODIGO COPIADO FUNCION QUICKSORT//////////////

void intercambiar(int *a, int *b) {
    int temporal = *a;
    *a = *b;
    *b = temporal;
}

int particion(int arreglo[], int izquierda, int derecha, int objetivo) {
    //Si objetivo==0 ó 1, los números son índices, se ordena de menor a mayor en base a objetivo. 0= distancia, 1 =epsilon
    //Si objetivo==2, los números son índices, se ordena de mayor a menor en base a i_distance
    //Si objetivo==3 se ordena en función de los propios números del array. Sirve para ordenar el array de features
    //Si objetivo==4 ó 5, los números son índices, se ordena de menor a mayor en base a objetivo. 4=mal_clasif_0, 5=mal_clasif_1
    //Si objetivo==6 ó 7, se ordena por coste

    // Elegimos el pivote, es el primero
    double pivote;
    
    if ((objetivo==0)||(objetivo==1))
        pivote=poblacion->solucion[arreglo[izquierda]].obj[objetivo];
    else if ((objetivo==4)||(objetivo==5))
        pivote=poblacion->solucion[arreglo[izquierda]].obj2[objetivo-4];
    else if ((objetivo==6)) 
        pivote=poblacion->solucion[arreglo[izquierda]].obj[objetivo-4];
    else if ((objetivo==7)) 
        pivote=poblacion->solucion[arreglo[izquierda]].obj2[objetivo-5];
    else if (objetivo==2)   
        pivote=poblacion->solucion[arreglo[izquierda]].i_distance;
    else //objetivo=3
        pivote=arreglo[izquierda];
    
    
    // Ciclo infinito
    while (1) {
        // Mientras cada elemento desde la izquierda esté en orden (sea menor que el
        // pivote) continúa avanzando el índice
        if ((objetivo==0)||(objetivo==1))
        {
            while (poblacion->solucion[arreglo[izquierda]].obj[objetivo] < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (poblacion->solucion[arreglo[derecha]].obj[objetivo] > pivote) {
                derecha--;
            }
            
        }
        else if ((objetivo==6))
        {
            while (poblacion->solucion[arreglo[izquierda]].obj[objetivo-4] < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (poblacion->solucion[arreglo[derecha]].obj[objetivo-4] > pivote) {
                derecha--;
            }
            
        }
        else if ((objetivo==4)||(objetivo==5))
        {
            while (poblacion->solucion[arreglo[izquierda]].obj2[objetivo-4] < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (poblacion->solucion[arreglo[derecha]].obj2[objetivo-4] > pivote) {
                derecha--;
            }
        }
        else if ((objetivo==7))
        {
            while (poblacion->solucion[arreglo[izquierda]].obj2[objetivo-5] < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (poblacion->solucion[arreglo[derecha]].obj2[objetivo-5] > pivote) {
                derecha--;
            }
        }
        else if (objetivo==2)
        {
            while (poblacion->solucion[arreglo[izquierda]].i_distance < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (poblacion->solucion[arreglo[derecha]].i_distance > pivote) {
                derecha--;
            }
        }
        else //(objetivo=3)
        {
            while (arreglo[izquierda] < pivote) {
                izquierda++;
            }
            // Mientras cada elemento desde la derecha esté en orden (sea mayor que el
            // pivote) continúa disminuyendo el índice
            while (arreglo[derecha] > pivote) {
                derecha--;}
        }
        
        
         //Si la izquierda es mayor o igual que la derecha significa que no
         //necesitamos hacer ningún intercambio
         //de variables, pues los elementos ya están en orden (al menos en esta
         //iteración)
         
        
        if (izquierda >= derecha)
        {
            // Indicar "en dónde nos quedamos" para poder dividir el arreglo de nuevo
            // y ordenar los demás elementos
            return derecha;
        }
        else
        {
            //Nota: yo sé que el else no hace falta por el return de arriba, pero así el algoritmo es más claro
            /*
             Si las variables quedaron "lejos" (es decir, la izquierda no superó ni
             alcanzó a la derecha)
             significa que se detuvieron porque encontraron un valor que no estaba
             en orden, así que lo intercambiamos
             */
            intercambiar(&arreglo[izquierda], &arreglo[derecha]);
            /*
             Ya intercambiamos, pero seguimos avanzando los índices
             */
            izquierda++;
            derecha--;
        }
        
        // El while se repite hasta que izquierda >= derecha
    }
}

void quicksort(int arreglo[], int izquierda, int derecha, int objetivo)
{
    if (izquierda < derecha)
    {
        int indiceParticion = particion(arreglo, izquierda, derecha, objetivo);
        quicksort(arreglo, izquierda, indiceParticion, objetivo);
        quicksort(arreglo, indiceParticion + 1, derecha, objetivo);
        
    }
}


int buscar_clones(int k)
// devuelve 0 si los VFo de la solución k no se repiten en ninguna
// solución desde 0 hasta k-1. Devuelve 1 si encuentra un clon (en vfo)
{
    int i = 0;
    int j;
    double prec[3] = {PRECISION_CLONES_F1, PRECISION_CLONES_F2, PRECISION_CLONES_F3};

    while (i <= k - 1)
    {
        int es_clon = 1;  // asumimos que es clon hasta que se demuestre lo contrario

        for (j = 0; j < NUM_OBJETIVOS && es_clon; j++)
        {
            double vi, vk;

            if (TIPO_OBJETIVOS == 1)
            {
                vi = poblacion->solucion[i].obj[j];
                vk = poblacion->solucion[k].obj[j];
            }
            else
            {
                vi = poblacion->solucion[i].obj2[j];
                vk = poblacion->solucion[k].obj2[j];
            }

            if (abs_2(vi - vk) >= prec[j])
                es_clon = 0;
        }

        if (es_clon)
            return 1;   // encontrado clon

        i++;
    }

    return 0; // no hay clones
}

void nueva_poblacion()
{
    //Esta funcion cruza los elementos de la población P para crear la poblacion de dobre tamaño PUQ=R
    int padre, madre, j, k;
    float aleatorio;
    
    printf("\n\nCreando nueva población por cruce desde %d hasta %d....", TAM_POBLACION, 2*TAM_POBLACION-1);
    k=TAM_POBLACION;
    while (k<(2*TAM_POBLACION)-1)
    {
        padre=torneo_elegir_padre();
        madre=torneo_elegir_padre();
        while (padre==madre)
            madre=torneo_elegir_padre();
        cruzar_soluciones(padre, madre, k); //cruza las soluciones padre y madre y los hijos (genera 4), los compara 2 a 2 y los dos mejores los pone en las posiciones k y k+1
        
        
        for (j=0; j<=1; j++)
        {
            aleatorio=genera_num_aleatorio_real01();
            
            if (aleatorio<P_MUT)
                mutar_solucion(k);
                //quicksort(poblacion->solucion[k].features,0, NUM_FEATURES-1,3);//ordena el array de features para que el cruce funcione bien ¿ES NECESARIO AHORA?
        
                while (evaluar_solucion(k)==0)
                    genera_solucion_aleat(k); //Antes si se generaba una solucion no válida por cruce, se volvía a generar por cruce. Ahora se genera aleatoriamente. OJOOOOO: PENSAR SI HACERLO ASÍ
            
            //NUEVO con CONSTANTE CLONES
            while ((CLONES==0)&&(buscar_clones(k)!=0))
                //No se permiten clones y es un clon
            {
                do {
                    mutar_solucion(k); //OJOOOO, Si hay un clon la generamos aleat. PENSAR SI DEJARLO ASÍ.
                } while (evaluar_solucion(k)==0);
            }
            k++;
                    
        }
                
    }
}

void reducir_poblacion() //esta función reduce el tamaño de la población de tamaño 2*TAM_POBLACION a una con la mitad. Se copian primero los elementos de los primeros fronts. Si llega un momento que no cabe un front entero, se seleccionan primero ...
{
    int contador_sol=0; //va contando las nuevas soluciones en la nueva poblacion
    int front;
    int indice, i, k;
    poblac *nueva_poblacion;
    double max[NUM_OBJETIVOS], min[NUM_OBJETIVOS]; //Para calcular el máximo y minimo si TIPO_OBJETIVOS=1 (distancia y epsilon)
    double diferencia;
    //unsigned int max2[NUM_OBJETIVOS], min2[NUM_OBJETIVOS];  //Para calcular el máximo y minimo si TIPO_OBJETIVOS=2 (mal clasificados cada tipo)
    //unsigned int diferencia2;
    int indices_front[2*TAM_POBLACION];
    int  l=0;
        
    
    printf("\n\nReduciendo de tamaño 2N a tamaño N.....");
    front=0;
    contador_sol=0;
    nueva_poblacion=new(poblac);
    
    nueva_poblacion->num_fronts=0;
    
    while (contador_sol+poblacion->num_sol_front[front]<=TAM_POBLACION) //Todas las soluciones del front "front" caben en la nueva_poblacion
    {
        nueva_poblacion->num_fronts++;
        //lanzamos un número aleatorio entre 0 y 2*TAM*POBLACION y a partir de ahí recorremos la poblacion triple y si el elemento que nos encontramos es del front actual, lo copiamos en la nueva
        indice=genera_num_aleat_entero(2*TAM_POBLACION);
        for (i=1; i<=poblacion->num_sol_front[front]; i++) //vamos a copiar tantas soluciones como tiene este front
        {
            while (poblacion->solucion[indice].front!=front)
            {
                indice++;
                if (indice==2*TAM_POBLACION) indice=0;
            }
            //Tenemos que copiar poblacion->solucion[indice] en nueva_poblacion->solucion[contador_sol]
            ///////PROCEDIMIENTO DE COPIA///////////////
            nueva_poblacion->solucion[contador_sol].modo= poblacion->solucion[indice].modo;
            for (k=0; k<NUM_OBJETIVOS; k++)
            {
                nueva_poblacion->solucion[contador_sol].obj[k]=poblacion->solucion[indice].obj[k];
                nueva_poblacion->solucion[contador_sol].obj2[k]=poblacion->solucion[indice].obj2[k];
            }
            
            for (k=0; k<NUM_TIPOS; k++)
            {
                nueva_poblacion->solucion[contador_sol].epsilon[k]= poblacion->solucion[indice].epsilon[k];
                nueva_poblacion->solucion[contador_sol].puntos_en_plano[k]= poblacion->solucion[indice].puntos_en_plano[k];
                nueva_poblacion->solucion[contador_sol].puntos_mal_clasif[k]= poblacion->solucion[indice].puntos_mal_clasif[k];
                nueva_poblacion->solucion[contador_sol].signo_intermedio[k]= poblacion->solucion[indice].signo_intermedio[k];
                nueva_poblacion->solucion[contador_sol].plano_termino[k]= poblacion->solucion[indice].plano_termino[k];
            }
            nueva_poblacion->solucion[contador_sol].plano_termino[k]= poblacion->solucion[indice].plano_termino[k];
            
            for (k=0; k<NUM_FEATURES; k++)
            {

                nueva_poblacion->solucion[contador_sol].features[k]= poblacion->solucion[indice].features[k];
                nueva_poblacion->solucion[contador_sol].plano_coord[k]= poblacion->solucion[indice].plano_coord[k];
            }


            nueva_poblacion->solucion[contador_sol].indices[0]= poblacion->solucion[indice].indices[0];
            nueva_poblacion->solucion[contador_sol].indices[1]= poblacion->solucion[indice].indices[1];


            nueva_poblacion->solucion[contador_sol].num_sol_me_dom= poblacion->solucion[indice].num_sol_me_dom;
            nueva_poblacion->solucion[contador_sol].num_sol_domina= poblacion->solucion[indice].num_sol_domina;
            nueva_poblacion->solucion[contador_sol].front= poblacion->solucion[indice].front;
            
            for (k=0; k<TAM_POBLACION; k++)
                nueva_poblacion->solucion[contador_sol].sol_domina[k]= poblacion->solucion[indice].sol_domina[k];

            
            contador_sol++;
            indice++;
            if (indice==2*TAM_POBLACION) indice=0;
        }
        front++;
    }
    if (front>0)//Todas las del front 0 han cabido
        nueva_poblacion->num_sol_front[0]=poblacion->num_sol_front[0];
    else nueva_poblacion->num_sol_front[0]=TAM_POBLACION;
    
    
    if (contador_sol<TAM_POBLACION)
        //La nueva población no está completa y las soluciones del Front "front" no caben todas. Se debe aplicar a este front la funcion de cálculo de "crowding distance"
    {

        nueva_poblacion->num_fronts++;
        k=0;
        l=0;
        
        while (k<poblacion->num_sol_front[front])
        {
            if (poblacion->solucion[l].front==front)
            {
                indices_front[k]=l;
                k++;
            }
            
            l++;
        }
        //En el array indices_front hemos guardado los indices de las soluciones del front "front". Ahora se debe ordenar y calcular su i_distance;
        //Ahora, para cada objetivo, tenemos que saber su valor máximo y mínimo entre los elementos del front, que se guarda en max[] y min[]
        
        if (TIPO_OBJETIVOS==1)
        {
            for (k=0; k<NUM_OBJETIVOS; k++)
            {
                max[k]=0;
                min[k]=MAX_DOUBLE;
            }
            for (l=0; l<poblacion->num_sol_front[front]; l++)
            {
                for (k=0; k<NUM_OBJETIVOS; k++)
                {
                    if (poblacion->solucion[indices_front[l]].obj[k]>max[k])
                        max[k]=poblacion->solucion[indices_front[l]].obj[k];
                    if (poblacion->solucion[indices_front[l]].obj[k]<min[k])
                        min[k]=poblacion->solucion[indices_front[l]].obj[k];
            
                }
           
            }
            //Ya esta calculado el max y min de cada objetivo
            //Nos va a servir para normalizar los valores de los objetivos al calcular las i_distance
        
            for (l=0; l<poblacion->num_sol_front[front]; l++) //Ponemos a cero todas las i_distance de los elementos del front
            {
                poblacion->solucion[indices_front[l]].i_distance=0;
            }
            //Ahora se debe ordenar el array de indices en funcion de cada objetivo
            /////FUNCION PARA ORDENAR ARRAY objetivo 0/////
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 0);
            //////Ahora suponemos indices_front esta ordenado en base al objetivo 0
        
            //Los extremos se ponen a infinito. No se usa el MAX_DOUBLE porque a su i_distance infinita quizás se le sume algo mas
            poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
            poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
            //el objetivo 0 es la distancia, que se maximiza
            
            diferencia=max[0]-min[0];
            if (diferencia==0)
                    diferencia=1;
            for (l=1; l<poblacion->num_sol_front[front]-1; l++)
            {
                poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj[0]- poblacion->solucion[indices_front[l-1]].obj[0])/diferencia;
            }
        
            /////FUNCION PARA ORDENAR ARRAY objetivo 1/////
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 1);
            ///////
            poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
            poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
            
            diferencia=max[1]-min[1];
            if (diferencia==0)
                    diferencia=1;
            
            for (l=1; l<poblacion->num_sol_front[front]-1; l++)
            {
                poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj[1]- poblacion->solucion[indices_front[l-1]].obj[1])/diferencia;
            
            }


            if (NUM_OBJETIVOS == 3){
                /////FUNCION PARA ORDENAR ARRAY objetivo 2 (costes) /////
                quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 6); ///*** ERROR, OBJETIVO NO ES 2, ES 6 ***/
                ///////
                poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
                poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
                
                diferencia=max[2]-min[2];
                if (diferencia==0)
                        diferencia=1;
                
                for (l=1; l<poblacion->num_sol_front[front]-1; l++)
                {
                    poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj[2]- poblacion->solucion[indices_front[l-1]].obj[2])/diferencia;
                
                }
            }

            //Una vez calculada la i_distance de todos, debemos ir seleccionando elementos según ésta hasta completar la nueva_poblacion
            //Antes habrá que ordenar de nuevo en función de i_distance
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 2); //ordena indices_front, de menor a mayor i_distance

            
        }

        else
        {
            for (k=0; k<NUM_OBJETIVOS; k++)
            {
                max[k]=0;
                min[k]=MAX_DOUBLE;
            }
            for (l=0; l<poblacion->num_sol_front[front]; l++)
            {
                for (k=0; k<NUM_OBJETIVOS; k++)
                {
                    if (poblacion->solucion[indices_front[l]].obj2[k]>max[k])
                        max[k]=poblacion->solucion[indices_front[l]].obj2[k];
                    if (poblacion->solucion[indices_front[l]].obj2[k]<min[k])
                        min[k]=poblacion->solucion[indices_front[l]].obj2[k];
            
                }
           
            }
            //Ya esta calculado el max y min de cada objetivo
            //Nos va a servir para normalizar los valores de los objetivos al calcular las i_distance
        
            for (l=0; l<poblacion->num_sol_front[front]; l++) //Ponemos a cero todas las i_distance de los elementos del front
            {
                poblacion->solucion[indices_front[l]].i_distance=0;
            }
            //Ahora se debe ordenar el array de indices en funcion de cada objetivo
            /////FUNCION PARA ORDENAR ARRAY objetivo 0/////
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 4);
            //////Ahora suponemos indices_front esta ordenado en base al objetivo 0
        
            //Los extremos se ponen a infinito. No se usa el MAX_DOUBLE porque a su i_distance infinita quizás se le sume algo mas
            poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
            poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
            //el objetivo 0 es la distancia, que se maximiza
            
            diferencia=max[0]-min[0];
            if (diferencia==0)
                    diferencia=1;
            
            for (l=1; l<poblacion->num_sol_front[front]-1; l++)
            {
                poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj2[0]- poblacion->solucion[indices_front[l-1]].obj2[0])/diferencia;
            
            
            }
        
            /////FUNCION PARA ORDENAR ARRAY objetivo 1/////
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 5);  
            ///////
            poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
            poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
            
            diferencia=max[1]-min[1];
            if (diferencia==0)
                    diferencia=1;
            
            for (l=1; l<poblacion->num_sol_front[front]-1; l++)
            {
                poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj2[1]- poblacion->solucion[indices_front[l-1]].obj2[1])/diferencia;
            
            }

            if (NUM_OBJETIVOS == 3){
                /////FUNCION PARA ORDENAR ARRAY objetivo 7 (costes)/////
                quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 7);  
                ///////
                poblacion->solucion[indices_front[poblacion->num_sol_front[front]-1]].i_distance= MAX_FLOAT;
                poblacion->solucion[indices_front[0]].i_distance=MAX_FLOAT;
                
                diferencia=max[2]-min[2];
                if (diferencia==0)
                        diferencia=1;
                
                for (l=1; l<poblacion->num_sol_front[front]-1; l++)
                {
                    poblacion->solucion[indices_front[l]].i_distance= poblacion->solucion[indices_front[l]].i_distance+(poblacion->solucion[indices_front[l+1]].obj2[2]- poblacion->solucion[indices_front[l-1]].obj2[2])/diferencia;
                
                }
            }
            //Una vez calculada la i_distance de todos, debemos ir seleccionando elementos según ésta hasta completar la nueva_poblacion
            //Antes habrá que ordenar de nuevo en función de i_distance
            quicksort(indices_front, 0, poblacion->num_sol_front[front]-1, 2); //ordena indices_front, ede menor a mayor i_distance
            //printf("\nFIN QUICKSORT");
        }

        ///////
        //Ahora falta completar los elementos de la nueva poblacion, uno a uno, segun el orden establecido en indices_front, desde el último al primero
        l=poblacion->num_sol_front[front]-1;
        for (;contador_sol<TAM_POBLACION; contador_sol++)
        {
           //Se debe copiar la solucion apuntada por el indice indices_front[l]
           // en la posicion contador_sol de la nueva poblacion
            
            //Tenemos que copiar poblacion->solucion[indice] en nueva_poblacion->solucion[contador_sol]
            ///////PROCEDIMIENTO DE COPIA///////////////
            nueva_poblacion->solucion[contador_sol].modo= poblacion->solucion[indices_front[l]].modo;
            for (k=0; k<NUM_OBJETIVOS; k++)
            {
                nueva_poblacion->solucion[contador_sol].obj[k]=poblacion->solucion[indices_front[l]].obj[k];
                nueva_poblacion->solucion[contador_sol].obj2[k]=poblacion->solucion[indices_front[l]].obj2[k];
            }
            
            for (k=0; k<NUM_TIPOS; k++)
            {
                nueva_poblacion->solucion[contador_sol].epsilon[k]= poblacion->solucion[indices_front[l]].epsilon[k];
                nueva_poblacion->solucion[contador_sol].puntos_en_plano[k]= poblacion->solucion[indices_front[l]].puntos_en_plano[k];
                nueva_poblacion->solucion[contador_sol].puntos_mal_clasif[k]= poblacion->solucion[indices_front[l]].puntos_mal_clasif[k];
                nueva_poblacion->solucion[contador_sol].signo_intermedio[k]= poblacion->solucion[indices_front[l]].signo_intermedio[k];
                nueva_poblacion->solucion[contador_sol].plano_termino[k]= poblacion->solucion[indices_front[l]].plano_termino[k];
            }
            nueva_poblacion->solucion[contador_sol].plano_termino[k]= poblacion->solucion[indices_front[l]].plano_termino[k];
            
            for (k=0; k<NUM_FEATURES; k++)
            {
                
                nueva_poblacion->solucion[contador_sol].features[k]= poblacion->solucion[indices_front[l]].features[k];
                nueva_poblacion->solucion[contador_sol].plano_coord[k]= poblacion->solucion[indices_front[l]].plano_coord[k];
            }
            nueva_poblacion->solucion[contador_sol].indices[0]= poblacion->solucion[indices_front[l]].indices[0];
            nueva_poblacion->solucion[contador_sol].indices[1]= poblacion->solucion[indices_front[l]].indices[1];
            //LO siguiente no es necesario copiarlo, ya que a continuacion se va a aplicar el non_dominated_sort
            //NO NECESARIO//
            //nueva_poblacion->solucion[contador_sol].evaluada= poblacion->solucion[indices_front[l]].evaluada;
            nueva_poblacion->solucion[contador_sol].num_sol_me_dom= poblacion->solucion[indices_front[l]].num_sol_me_dom;
            nueva_poblacion->solucion[contador_sol].num_sol_domina= poblacion->solucion[indices_front[l]].num_sol_domina;
            nueva_poblacion->solucion[contador_sol].front= poblacion->solucion[indices_front[l]].front;
            
            for (k=0; k<TAM_POBLACION; k++)
                nueva_poblacion->solucion[contador_sol].sol_domina[k]= poblacion->solucion[indices_front[l]].sol_domina[k];
            
            //FIN NO NECESARIO//
            ///////FIN PROCEDIMIENTO DE COPIA///////////
            l--;
        }
    }
    delete(poblacion);
    poblacion=nueva_poblacion;
    printf("\nNº fronts: %d\nNº SND: %d\n", poblacion->num_fronts, poblacion->num_sol_front[0]);
    
    
}

float abs_2(float value){
    if (value<0){
        return -value;
    }else{
        return value;
    }
}


