/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 98384      Nome: Mário André Chen Cao
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: É responsável pela atribuicao de um enfermeiro
 ** para admnistrar as vacinas aos cidadaos que chegam aos Centros de Saúde
 **
 ******************************************************************************/
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

//Declara as variaveis globais
Cidadao a;
Vaga vagas[NUM_VAGAS];
Enfermeiro *enfermeiros;
long size;
int counter;


/*Inicia o index_enfermeiro das vagas a -1
Ou seja, limpa a lista*/
void init_vagas(){
    /*Percorre as vagas*/
    for(int i = 0; i < NUM_VAGAS; i++){
        /*Preenche a -1*/
        vagas[i].index_enfermeiro = -1;
    }
    /*Mensagem de sucesso*/
    sucesso("S3) Iniciei a lista de <%d> vagas", NUM_VAGAS);
}


/*Regista o PID do servidor.c no ficheiro servidor.pid*/
void register_pid(){
    //Inicializa as variaveis
    FILE *fp1;

    //Abre o ficheiro servidor.pid em modo leitura
    fp1 = fopen(FILE_PID_SERVIDOR , "w");

    //Se o apontador for NULL
    if(fp1 == NULL){
        //Emite o erro e acaba o processo
        erro("S1) Não consegui registar o servidor!");
        exit(-1);
    }

    //Escreve no ficheiro servidor.pid o pid do processo servidor.c
    fprintf(fp1, "%d", getpid());

    //Output de sucesso
    sucesso("S1) Escrevi no ficheiro FILE_PID_SERVIDOR o PID: <%d>", getpid());

    //Fecha o ficheiro
    fclose(fp1);
}

//Lê o ficheiro pedidovacina.txt
void read_request(){

        /*Declara as variaveis*/
        FILE *fp;
        char *p;
        char linha[100];
        int i = 0;

        /*Se o ficheiro pedidovacina.txt não existir, logo não recebeu pedido de vacinacao*/
        if( access( FILE_PEDIDO_VACINA , F_OK ) != 0 ) {
            erro("S5.1) Não foi possível abrir o ficheiro FILE_PEDIDO_VACINA");
        }

        //Abre o ficheiro pedidovacina.txt em modo leitura
        fp = fopen(FILE_PEDIDO_VACINA, "r");

        //Se houver erro em abrir o ficheiro, emitir 
        if(fp == NULL){
            erro("S5.1) Não foi possível ler o ficheiro FILE_PEDIDO_VACINA");
            exit(-1);
        }

        //Começa a ler o ficheiro pedidovacina.txt
        fgets(linha,100,fp);

        //Define o delimitador como :
        p = strtok(linha,":");

        //Guarda o numero de utente na variavel global Cidadao a
        if ( p != NULL ) a.num_utente= atoi(p);

        p = strtok( NULL, ":");

        //Guarda o nome na variavel global Cidadao a
        if ( p != NULL ) strcpy(a.nome, p );

        p = strtok( NULL, ":");

        //Guarda a idade na variavel global Cidadao a
        if ( p != NULL ) a.idade= atoi(p);

        p = strtok( NULL, ":");

        //Guarda a localidade na variavel global Cidadao a
        if ( p != NULL ) strcpy(a.localidade, p );

        p = strtok( NULL, ":");

        //Guarda o numero de telemovel na variavel global Cidadao a
        if ( p != NULL ) strcpy(a.nr_telemovel, p );

        p = strtok( NULL, ":");

        //Guarda o estado de vacinacao na variavel global Cidadao a
        if ( p != NULL ) a.estado_vacinacao = atoi(p);

        p = strtok( NULL, ":");

        //Guarda o PID do processo cidadao na variavel global Cidadao a
        if ( p != NULL ) a.PID_cidadao = atoi(p);

        
        printf("Chegou o cidadão com o pedido nº <%d>, com nº utente %d, para ser vacinado no Centro de Saúde <CS%s>\n", 
                                        a.PID_cidadao, a.num_utente, a.localidade);

        //Output sucesso
        sucesso("S5.1) Dados Cidadão: <%d>; <%s>; <%d>; <%s>; <%s>; 0", a.num_utente, a.nome, a.idade, a.localidade, a.nr_telemovel);

        //Fecha o ficheiro
        fclose(fp);

}

//Escreve no enfermeiros.dat, recebendo o indice da linha para escrever
void write_dat(int index){

    //Inicializa as variaveis
    FILE *fb;

    //Se o ficheiro enfermeiros.dat nao existir
    if( access( FILE_ENFERMEIROS , F_OK ) != 0 ) {
        erro("enfermeiros.dat não encontrado");
        exit(-1);
    }

    //Abre o ficheiro em leitura e escrita
    fb = fopen(FILE_ENFERMEIROS, "r+"); 

    //Se ocorrer um erro emite mensagem de erro
    if(fb == NULL) erro("S2) Não consegui escrever no ficheiro FILE_ENFERMEIROS!");
    
    //Coloca o ponteiro na linha dada
    fseek(fb, index*sizeof(Enfermeiro), SEEK_SET); 

    //Escrever na linha dada com os dados atualizados do enfermeiro index
    fwrite(&enfermeiros[index], sizeof(Enfermeiro), 1, fb); 

    //Output de sucesso
    sucesso("S5.5.3.4) Ficheiro FILE_ENFERMEIROS <%d> atualizado para <%d> vacinas dadas", index, enfermeiros[index].num_vac_dadas);

    //Fecha o ficheiro
    fclose(fb);
}


//Processo que liberta a vaga apos a vacinacao acabar
void clean_vaga(int pid){

    //Define variavel
    int index_enfermeiro;

    //Percorre as vagas
    for(int i = 0; i < NUM_VAGAS; i++){

        //Se o o pid forem iguais
        if(vagas[i].PID_filho==pid){
            
            //Guarda o indice do enfermeiro no "array" de enfermeiros
            index_enfermeiro = vagas[i].index_enfermeiro;

            //Liberta a vaga
            vagas[i].index_enfermeiro = -1;

            //Output de sucesso
            sucesso("S5.5.3.1) Vaga <%d> que era do servidor dedicado <%d> libertada", i, pid);

            //Coloca a disponibilidade do enfermeiro como disponivel = 1
            enfermeiros[index_enfermeiro].disponibilidade=1;

            //Output de sucesso
            sucesso("S5.5.3.2) Enfermeiro <%d> atualizado para disponível", index_enfermeiro);

            //Aumenta o numero de vacinas dadas pelo enfermeiro
            enfermeiros[index_enfermeiro].num_vac_dadas+=1;
            
            //Output de sucesso
            sucesso("S5.5.3.3) Enfermeiro <%d> atualizado para <%d> vacinas dadas", index_enfermeiro,  
                                                                                enfermeiros[index_enfermeiro].num_vac_dadas );
            //Escreve no ficheiro enfermeiros.dat
            write_dat(index_enfermeiro);

            //Output de sucesso
            sucesso("S5.5.3.5) Retorna");
        }
    }   
}

//Handler de sinais
void signal_handler(int sig){

    //Se o pai receber o sinal de SIGCHLD
    if(sig == SIGCHLD){
        //Guarda o pid do processo filho que morreu
        int pid = waitpid(-1, NULL, WNOHANG);
        clean_vaga(pid);
    }
    //Se o sinal for SIGTERM
    if(sig == SIGTERM){
        //Envia ao PID_cidadao o sinal SIGTERM
        kill(a.PID_cidadao, SIGTERM);
        //Output sucesso
        sucesso("S5.6.1) SIGTERM recebido, servidor dedicado termina Cidadão");
    }
}


//Funcao que vacina os cidadaos
void vaccinate(int i){
    // O i é utilizado para as vagas
    pid_t child = fork();

    //Se houver erro em criar o filho
    if(child < 0){
        erro("S5.4) Não foi possível criar o servidor dedicado"); 
        exit(-1);
    }

    //Processo Filho
    if(child == 0){

        /*Output sucesso*/
        sucesso("S5.4) Servidor dedicado <%d> criado para o pedido <%d>", child, vagas[i].cidadao.PID_cidadao);

        /*Arma o sinal SIGTERM*/
        signal(SIGTERM, signal_handler);

        /*Ignora o sinal SIGINT*/
        signal(SIGINT, SIG_IGN);

        /*Envia o sinal SIGUSR1*/
        kill(vagas[i].cidadao.PID_cidadao, SIGUSR1);

        /*Output sucesso*/
        sucesso("S5.6.2) Servidor dedicado inicia consulta de vacinação");

        /*Tempo de vacinacao*/
        sleep(TEMPO_CONSULTA);

        /*Output sucesso*/
        sucesso("S5.6.3) Vacinação terminada para o cidadão com o pedido nº <%d>", vagas[i].cidadao.PID_cidadao);

        /*Envia ao processo cidadao o sinal SIGUSR2*/
        kill(vagas[i].cidadao.PID_cidadao,SIGUSR2);

        /*Acaba o processo filho*/
        exit(0);

    //Processo Pai
    }else{

        /*Guarda o PID do processo filho*/
        vagas[i].PID_filho = child;

        /*Output sucesso*/
        sucesso("S5.5.1) Servidor dedicado <%d> na vaga <%d>", vagas[i].PID_filho, i);

        /*Arma o sinal SIGCHLD*/
        signal(SIGCHLD, signal_handler);

        /*Output sucesso*/
        sucesso("S5.5.2) Servidor aguarda fim do servidor dedicado <%d>", vagas[i].PID_filho);
    }
}

/*Verifica se há enfermeiros disponiveis*/
void verify_avail(){

    //Inicia as variaveis
    char *tmp = strdup(a.localidade);
    int index_enfermeiro;

    //Concatena a localidade do cidadao com CS, ou seja CS"<a.localidade>"
    strcpy(a.localidade, "CS"); 
    strcat(a.localidade, tmp);  

    //Percorre a lista de enfermeiros
    for(int i=0; i<counter; i++){
        //Compara a localidade do enfermeiro com a do cidadao, se forem iguais
        if(strcmp(enfermeiros[i].CS_enfermeiro, a.localidade) == 0){
            
            //Guarda o indice do enfermeiro
            index_enfermeiro = i;

            //Se a disponibilidade do enfermeiro for disponivel = 1
            if(enfermeiros[i].disponibilidade == 1){

                sucesso("S5.2.1) Enfermeiro <%d> disponível para o pedido <%d>", index_enfermeiro, a.PID_cidadao);

                //Se não houver vagas
                if(vagas[NUM_VAGAS-1].index_enfermeiro != -1){

                    erro("S5.2.2) Não há vaga para vacinação para o pedido <%d>", a.PID_cidadao);


                //Se houver vagas e enfermeiro disponivel
                }else{
                    
                    sucesso("S5.2.2) Há vaga para vacinação para o pedido <%d>", a.PID_cidadao);

                    //Percorre as vagas
                    for(int j = 0; j < NUM_VAGAS; j++){
                        //Se nao houver ninguem a ocupar a vaga
                        if(vagas[j].index_enfermeiro == -1){
                            //Guarda o indice do enfermeiro e o cidadao e disponibilidade do enfermeiro é 0
                            vagas[j].index_enfermeiro = index_enfermeiro;
                            vagas[j].cidadao = a;
                            enfermeiros[index_enfermeiro].disponibilidade = 0;

                            sucesso("S5.3) Vaga nº <%d> preenchida para o pedido <%d>", j, a.PID_cidadao);

                            //Inicia o processo de vacinacao
                            vaccinate(j);

                            break;
                        }
                    }
                }

            //Se não houver enfermeiro disponivel
            }else{
                //Envia ao processo cidadao o sinal SIGTERM
                kill(a.PID_cidadao, SIGTERM);
                erro("S5.2.1) Enfermeiro <%d> indisponível para o pedido <%d> para o Centro de Saúde <%s>", i, a.PID_cidadao, enfermeiros[i].CS_enfermeiro);
            }
        }
    }
    //Liberta a memoria
    free(tmp); 

 }


/*Devolve o tamanho do ficheiro*/
int fsize(FILE* file) {
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    return size;
}


/*Lê o ficheiro enfermeiros.dat*/
void read_file(){

    //Inicializa as variaveis
    FILE *fp;
    Enfermeiro a;

    //Se o ficheiro existir
    if( access( FILE_ENFERMEIROS , F_OK ) != 0 ) {
        erro("enfermeiros.dat não encontrado");
        exit(-1);
    }

    //Abre o ficheiro enfermeiros.dat em modo leitura
    fp = fopen(FILE_ENFERMEIROS, "r"); 

    //Se ocorrer erro em ler
    if(fp == NULL) erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!");
    
    //Regista o tamanho do ficheiro
    size = fsize(fp);

    //O numero de linhas corresponde ao bytes do ficheiro / tamanho da estrutura
    counter = size / sizeof(Enfermeiro);

    //Cria uma estrutura dinamica na memoria
    enfermeiros = (Enfermeiro*)malloc(counter*sizeof(Enfermeiro));

    //Se houver um erro na alocacao de memoria
    if (enfermeiros == NULL){
        erro("Malloc failed!\n");
        exit(-1);
    }

    //Lê o ficheiro até ao fim
    fread(enfermeiros, sizeof(Enfermeiro), counter, fp); 

    sucesso("S2) Ficheiro FILE_ENFERMEIROS tem <%ld> bytes, ou seja, <%d> enfermeiros", size, counter);

    //Fecha o ficheiro
    fclose(fp);

}

void sig_handler(int sig){
    if(sig == SIGUSR1){
        read_request();
        verify_avail();
    }
    if(sig == SIGINT){
        printf("\n");
        sucesso("S6) Servidor terminado");
        remove(FILE_PID_SERVIDOR);
        killpg(getpid(), SIGTERM);
        exit(0);
    }
}


int main(){

    /*Processos que invocam outros
    Quando recebe SIGUSR1 --> read_request()
                          --> verify_avail() --> vaccinate(index)

    Quando pai recebe SIGCHLD --> clean_vaga() --> write_dat()
    */

    /*Ignora o sinal SIGTERM*/
    signal(SIGTERM, SIG_IGN);

    /*Regista o PID do servidor.c no ficheiro servidor.pid*/
    register_pid();

    /*Lê e guarda os enfermeiros numa estrutura dinamica de memoria do ficheiro enfermeiros.dat*/
    read_file();

    /*Preenche os index_enfermeiro com -1, essencialmente, limpa a lista*/
    init_vagas();

    /*Arma os sinais SIGUSR1, SIGINT*/
    signal(SIGUSR1,sig_handler);
    signal(SIGINT, sig_handler);

    sucesso("S4) Servidor espera pedidos");

    /*Fica num loop em espera ativa à espera de receber um sinal*/
    while(1) pause();

    /*Devolve 0*/
    return 0;
    
}