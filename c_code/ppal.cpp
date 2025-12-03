#include "global.h"
#include "inicio.h"
#include "nsga2.h"


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/****************************************************************************/
/*							DECLARACI�N DE VARIABLES GLOBALES               */
/****************************************************************************/
dat *datos;
poblac *poblacion;

/****************************************************************************/


int main(int argc, char *argv[])
{

    char nom_fich[] = "train.txt";
    unsigned int num_gen_max=0;
    unsigned int num_gen=1;
    FILE *fichero_sol_SND;
    int opcion_menu;
    int tipo_fichero, lectura;
    //Comentar
    tipo_fichero = 1;

    clock_t segundos_ini=0, segundos_fin=0;
    double segundos_trans=0;
    double  segundos_max=0;
    //////
    //Inicializacion variables globales;
    datos=new(dat);
    poblacion=new(poblac);


    if  (tipo_fichero==1)
        lectura=leer_fichero_txt(nom_fich, NUM_PUNTOS, NUM_DIM, NUM_TIPOS ); //Tipo UCI
    else
        lectura=leer_fichero_txt2(nom_fich, NUM_PUNTOS, NUM_DIM, NUM_TIPOS ); //Tipo SVMLIB
    if (lectura==0) return(0);

    opcion_menu = 2;
    segundos_max = 3600;

    fichero_sol_SND=fopen("evol_num_SND.txt", "w");
    fprintf(fichero_sol_SND, "TAM POB: %d\nP. MUT: %f\n\tP. MUT. IND: %f\n\tP. MUT. FEAT: %f\n\tP. MUT. COORD: %f", TAM_POBLACION, P_MUT, P_MUT_IND, P_MUT_FEAT, P_MUT_COORD);

    fprintf(fichero_sol_SND, "\n#Gen\t#Fronts\t#SND\t#t\n");

    //transformar_datos();
    //printf("\n\nOJO: TRANSFORMACION DE DATOS QUITADA. COMPROBAR SI ES NECESARIA\n\n");
    //
    segundos_ini=clock();

    //
    genera_poblacion_inicial();
    //evaluar_poblacion(0, TAM_POBLACION-1); //Eval�a los TAM_POBLACION primeros elementos No hace falta hacerlo aqui, ya que se ha cambiado y, ahora, eval�a cada soluci�n despu�s de generarla
    fast_non_dominated_sort(TAM_POBLACION-1);
    segundos_fin=clock();
    segundos_trans= ((double) (segundos_fin-segundos_ini))/CLOCKS_PER_SEC;

    while (((opcion_menu==1)&&(num_gen<=num_gen_max))||((opcion_menu==2)&&(segundos_trans<=segundos_max)))
    //for (int k=1; k<=num_gen_max; k++)
    {

        nueva_poblacion(); //A la poblacion P (desde 0 a TAM_POBLACION-1) se le aplica el cruce para crear la poblaciob PUQ=R (desde 0 a 2*TAM_POBLACION-1). Cada nueva solucion que se crea se comprueba si es un clon y si es as�, se crea otra. Es decir, no se permiten clones en las nuevas soluciones creadas
        //evaluar_poblacion(TAM_POBLACION, 2*TAM_POBLACION-1); //Eval�a la segunda parte de la poblaci�n. No hace falta, ya se hace en nueva_poblacion
        fast_non_dominated_sort(2*TAM_POBLACION-1);
        reducir_poblacion();
        //fast_non_dominated_sort(TAM_POBLACION-1); //aunque se deber�a hacer para que al hacer la nueva poblacion se usen correctamente los criterios establecidos (front (ok), num_sol_me_dom (ok), num_sol_domino (puede no ok), aleatorio (ok)), se elimina para reducir tiempos. SE HA ELIMINADO EL CRITERIO num_sol_domino, POR TANTO NO ES NECESARIO HACER DE NUEVO EL FNDS AQUI PARA QUE LOS VALORES QUE SE USAN EN LOS CRITERIOS SEAN CORRECTOS. SE PUEDE ELIMINAR DE AQUI


        segundos_fin=clock();
        segundos_trans= ((double) (segundos_fin-segundos_ini))/CLOCKS_PER_SEC;

        printf("\n\nFin generacion %d\n\n\n", num_gen);

        fprintf(fichero_sol_SND, "%d\t%d\t%d\t\%f\n", num_gen, poblacion->num_fronts, poblacion->num_sol_front[0], segundos_trans);
        num_gen++;

    }

    fast_non_dominated_sort(TAM_POBLACION-1); //OJO, ES NECESARIO HACERLO AQUI??
    printf("\n");

    fclose(fichero_sol_SND);
    printf("\n\nLa poblacion final tiene %d soluciones no dominadas", poblacion->num_sol_front[0]);
    generar_fichero_solucion(num_gen_max, segundos_max, num_gen-1, segundos_trans);
    //scanf("%d", &no_vale);

    //Liberacion memoria;
    delete(datos);
    delete(poblacion);

//    leer_determinante();


    printf("\n\nFin programa\n");
}
