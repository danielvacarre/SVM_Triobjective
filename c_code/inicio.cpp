#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <math.h>    
#include <time.h>       
#include "global.h"


//#include "genet_mm.h"
//#include "pprl_mm.h"


extern dat *datos;
extern poblac *poblacion;


/****************************************************************************/
/*                             TRANSFORMAR DATOS                            */
/****************************************************************************/
/* Par�metros:    void                                                      */
/* Devuelve:    void                                                        */
/* Acci�n: Cambia la bbdd en base a la f�rmula de Chia  [2xij-(xmaxj+xminj)]/ (xmaxj+xminj) */
/****************************************************************************/
void transformar_datos(void)
{
    int j,i;
    float max[NUM_DIM], min[NUM_DIM];
    
    FILE *fichero;

    printf("\nTransformando datos...");
    for (i=0; i<NUM_DIM; i++)
    {
        max[i]=MIN_FLOAT;
        min[i]=MAX_FLOAT;
        for (j=0; j<NUM_PUNTOS; j++)
        {
            if (datos->punto[j].coord[i]>max[i])
                max[i]=datos->punto[j].coord[i];
            if (datos->punto[j].coord[i]<min[i])
                min[i]=datos->punto[j].coord[i];
        }
    }
    
    fichero=fopen("datos_transformados.txt","w");
    
    //Ya hemos calculado el m�ximo y el m�nimo por columna
    //Ahora transformamos los datos leidos en funci�n de la f�rmula:
    //[2xij-(xmaxj+xminj)]/ (xmaxj-xminj)
    fprintf(fichero, "%d\n%d\n", NUM_PUNTOS, NUM_DIM+2);;
    for (j=0; j<NUM_PUNTOS; j++)
    {
       fprintf(fichero, "%d\t", j+1);
       if (datos->punto[j].tipo==0)
            fprintf(fichero, "%d", -1);
       else
            fprintf(fichero, "%d", 1);
        for (i=0; i<NUM_DIM; i++)
        {
            ////CAMBIO HECHO DESPUES DE LAS PRUEBAS
            if (max[i]!=min[i]) //Si todos los elementos de la columna son iguales, la transformaci�n dar�a error. Por ello, dejamos la columna sin transformar
            ////FIN CAMBIO
            {
                datos->punto[j].coord[i]= ((2*datos->punto[j].coord[i]-(max[i]+min[i])))/(max[i]-min[i]);
            }
          fprintf(fichero, "\t%f", datos->punto[j].coord[i]);
            
            
        }
      fprintf(fichero,"\n");
    }
    
}


	
   
/****************************************************************************/
/*						     LEER FICHERO                               */  
/****************************************************************************/
/* Par�metros:	void														*/
/* Devuelve:	Un entero. Si es negativo indica error                      */
/* Acci�n :		Lee el fichero de datos y rellena la estructura proyecto    */
/****************************************************************************/							

			
int leer_fichero_txt(char *nom_fich, int num_puntos, int num_dim, int num_tipos) //Lee fichero tipo UCI
{   
	       
	
    
    FILE *fichero;
    int num_dim_aux, num_puntos_aux, aux1, aux2;
     int i, j;
    
    
    fichero=fopen(nom_fich,"r");
    
    
   
    
    for (i=0; i<num_tipos; i++)
        datos->tot_tipo[i]=0;
    
    
    printf("\nComienza la lectura de %s, que tiene %d puntos de %d tipos y %d dimensiones", nom_fich, NUM_PUNTOS, NUM_TIPOS, NUM_DIM);
    
    fscanf(fichero,"%d",&num_puntos_aux);
    fscanf(fichero,"%d",&num_dim_aux);
    if (((num_dim_aux-2)!=num_dim)||(num_puntos_aux!=num_puntos))
        printf("\nLa base de datos no se corresponde con las estructuras de datos definidas");

    //Lectura de costes
    //fscanf(fichero,"%f %f", &aux1, &aux2);
    for (i=0; i<num_dim; i++){
       fscanf(fichero,"%f,",&datos->costes[i]);
       //printf("%f\t",datos->costes[i]);
    }
//    OJO esta lectura de datos solo est� preparada para bbdd con puntos de dos tipos;
    for (j=0; j<num_puntos; j++)
    {
        fscanf(fichero,"%d %d", &aux1, &aux2);
        for (i=0; i<num_dim; i++)
        {
            
            fscanf(fichero,"%f,",&datos->punto[j].coord[i]);
            
        }
//        fscanf(fichero,"%d",&aux2);
        if (aux2==-1) aux2=0;
               datos->punto[j].tipo=aux2;
    datos->indices[datos->punto[j].tipo][datos->tot_tipo[datos->punto[j].tipo]]=j;
        datos->tot_tipo[datos->punto[j].tipo]++;
        
    
    }
    printf("\nLectura de fichero terminada");
    fclose(fichero);
    printf("\nPuntos - : %d\nPuntos + : %d", datos->tot_tipo[0] , datos->tot_tipo[1]);
    return(1);
        
}

int leer_fichero_txt2(char *nom_fich, int num_puntos, int num_dim, int num_tipos) //Lee fichero tipo UCI
{
           
    
    
    FILE *fichero;
    
    int aux1, aux2;
    int i, j;
    
    
    fichero=fopen(nom_fich,"r");
    
    
   
    
    for (i=0; i<num_tipos; i++)
        datos->tot_tipo[i]=0;
    
    

    printf("\nComienza la lectura de %s, que tiene %d puntos de %d tipos y %d dimensiones", nom_fich, NUM_PUNTOS, NUM_TIPOS, NUM_DIM);
    
    //Lectura de costes
    fscanf(fichero,"%f %f", &aux1, &aux2);
    for (i=0; i<num_dim; i++){
       fscanf(fichero,"%f,",&datos->costes[i]);
       //printf("%f\t",datos->costes[i]);
    }
    
//    OJO esta lectura de datos solo est� preparada para bbdd con puntos de dos tipos;
    for (j=0; j<num_puntos; j++)
    {
        fscanf(fichero,"%d", &aux2);
        for (i=0; i<num_dim; i++)
        {
            
            fscanf(fichero," %d:%f", &aux1, &datos->punto[j].coord[i]);
            
            
            //printf("\n%f",datos->punto[j].coord[i]);
        }
//        fscanf(fichero,"%d",&aux2);
        if (aux2==-1) aux2=0;
               datos->punto[j].tipo=aux2;
    datos->indices[datos->punto[j].tipo][datos->tot_tipo[datos->punto[j].tipo]]=j;
        datos->tot_tipo[datos->punto[j].tipo]++;
        
    
    }
    printf("\nLectura de fichero terminada");
    fclose(fichero);
    printf("\nPuntos - : %d\nPuntos + : %d", datos->tot_tipo[0] , datos->tot_tipo[1]);

    return(1);
        
}

float get_costes(unsigned int num_sol){
    int j;
    int feature;
    float coste= 0.0;
    for(j=0;j<NUM_FEATURES;j++){
        feature = poblacion->solucion[num_sol].features[j];
        coste = coste + datos->costes[feature];
    }
    return coste;
}

void generar_fichero_solucion(unsigned int num_gen_max, unsigned int segundos_max, unsigned int num_gen, double segundos_trans)
{
    
    FILE *fichero, *fichero2, *fichero3, *fichero4;
    
    int* array_pos;
    int num_sol_dom = poblacion->num_sol_front[0];

    float costes = 0.0;

    array_pos = (int*)malloc(num_sol_dom*sizeof(int));
    
    int i, j;  
    
    fichero=fopen("SOLyVFO.txt","w");
    fichero2=fopen("VFO.txt","w");
    fichero3=fopen("CARAC.txt","w");
    fichero4=fopen("PLANOS.txt","w");
    
    fprintf(fichero, "TIPO OBJETIVOS: %d\nTAM POB: %d\nP. MUT: %f\n\tP. MUT. IND: %f\n\tP. MUT. FEAT: %f\n P_MUT_COORD: %f\n", TIPO_OBJETIVOS, TAM_POBLACION,  P_MUT, P_MUT_IND, P_MUT_FEAT, P_MUT_COORD);
    //fprintf(fichero2, "TIPO OBJETIVOS: %d\nTAM POB: %d\nP. MUT: %f\n\tP. MUT. IND: %f\n\tP. MUT. FEAT: %f\n P_MUT_COORD: %f\n", TIPO_OBJETIVOS, TAM_POBLACION,  P_MUT, P_MUT_IND, P_MUT_FEAT, P_MUT_COORD);
    
    fprintf(fichero, "\nNUM. GENER. MAX: %d \t\tNUM. GENER.: %d \nTIEMPO MAX.: %d \t\t\tTIEMPO: %lf\n\nN� SND: %d \n", num_gen_max, num_gen, segundos_max, segundos_trans, poblacion->num_sol_front[0]);
    
    //fprintf(fichero2, "\nNUM. GENER. MAX: %d \t\tNUM. GENER.: %d \nTIEMPO MAX.: %d \t\t\tTIEMPO: %lf\n\nN� SND: %d \n", num_gen_max, num_gen, segundos_max, segundos_trans, poblacion->num_sol_front[0]);
       
    fprintf(fichero2,"DIST\tEPSILON\tCOSTES\tMC-\tMC+\tPP-\tPP+");
    j=0;
    for (i=0; i<poblacion->num_sol_front[0]; i++)
    {
        while (poblacion->solucion[j].front!=0)
            j++;
        array_pos[i]=j;

        if (NUM_OBJETIVOS == 2){
            costes = get_costes(j);
        }
        else {
            costes = poblacion->solucion[j].obj[2];
        }

        fprintf(fichero, "\n\nModo: %d",poblacion->solucion[j].modo );
        fprintf(fichero, "\nPuntos: " );
        for (int k=0; k<=1; k++)
            fprintf(fichero, " %d\t", poblacion->solucion[j].indices[k]);
        fprintf(fichero, "\nCaracteristicas: " );
        for (int k=0; k<NUM_FEATURES; k++){
            fprintf(fichero, " %d\t", poblacion->solucion[j].features[k]);
            fprintf(fichero3, "%d\t", poblacion->solucion[j].features[k]);
        }
        fprintf(fichero3,"\n");
        fprintf(fichero, "\nDistancia: %lf\tEpsilon: %lf\tCostes: %lf",poblacion->solucion[j].obj[0],poblacion->solucion[j].obj[1],costes);
        fprintf(fichero, "\nMC-: %f\tMC+: %f \tCostes: %lf",poblacion->solucion[j].obj2[0],poblacion->solucion[j].obj2[1], costes);

        fprintf(fichero,  "\nPlano intermedio: " );
        for (int k=0; k<NUM_FEATURES; k++){
            fprintf(fichero, "%lf\t", poblacion->solucion[j].plano_coord[k]);
            fprintf(fichero4, "%lf\t", poblacion->solucion[j].plano_coord[k]);
        }
        fprintf(fichero, "\"%lf\"\t", poblacion->solucion[j].plano_termino[2]);
        // Si imprime 0 significa que los puntos tipo "-1" deben dar negativo.
        //Si imprime 1 significa que los puntos tipo "-1" deben dar positivo.
        fprintf(fichero, "%d",poblacion->solucion[j].signo_intermedio[0]);
      
        fprintf(fichero4, "%lf    %d", poblacion->solucion[j].plano_termino[2], poblacion->solucion[j].signo_intermedio[0]);
        fprintf(fichero4,"\n");
        fprintf(fichero2, "\n%lf\t%lf\t%lf",poblacion->solucion[j].obj[0],poblacion->solucion[j].obj[1],costes);
        fprintf(fichero2, "\t%f\t%lf",poblacion->solucion[j].obj2[0],poblacion->solucion[j].obj2[1]);
        fprintf(fichero2, "\t%d\t%d", poblacion->solucion[j].puntos_en_plano[0], poblacion->solucion[j].puntos_en_plano[1]);

        //fprintf(fichero,  "\nP. mal clasificados -: %d", poblacion->solucion[j].puntos_mal_clasif[0]);
        //fprintf(fichero,  "\nP. mal clasificados +: %d", poblacion->solucion[j].puntos_mal_clasif[1] );
        fprintf(fichero, "\nPuntos en plano -: %d", poblacion->solucion[j].puntos_en_plano[0]);
        fprintf(fichero, "\nPuntos en plano +: %d", poblacion->solucion[j].puntos_en_plano[1]);
        j++;
    }

    fclose(fichero);
    fclose(fichero2);
    fclose(fichero3);
    fclose(fichero4);
    free(array_pos);
}
