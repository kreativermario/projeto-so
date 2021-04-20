#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
Cidadao a;
Vaga vagas[NUM_VAGAS];
Enfermeiro *enfermeiros;
long size;
int counter;

int fsize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}

void read_file(){
    FILE *fb;
    Enfermeiro a;

    if( access( "enfermeiros.dat" , F_OK ) != 0 ) {
        erro("enfermeiros.dat não encontrado");
        exit(-1);
    }
    fb = fopen("enfermeiros.dat", "r"); 

    if(fb == NULL) erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!\n");
 

    size = fsize(fb);
    counter = size / sizeof(Enfermeiro);
    enfermeiros = (Enfermeiro*)malloc(counter*sizeof(Enfermeiro));

    if (enfermeiros == NULL){
        erro("Malloc failed!\n");
        exit(-1);
    }

    fread(enfermeiros, sizeof(Enfermeiro), counter, fb); 

    for(int i = 0; i<counter; i++){
        printf("Nºcedula: %d, nome: %s, CS_enfermeiro: %s, Vacinas: %d, Disp: %d\n", enfermeiros[i].ced_profissional, 
                                                              enfermeiros[i].nome, enfermeiros[i].CS_enfermeiro, 
                                                              enfermeiros[i].num_vac_dadas, enfermeiros[i].disponibilidade);
    }
    sucesso("S2) Ficheiro FILE_ENFERMEIROS tem <%d> bytes, ou seja, <%d> enfermeiros\n", size, counter);
    fclose(fb);

}

int main(){
    read_file();
    return 0;
}