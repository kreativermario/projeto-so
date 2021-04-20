/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 98384      Nome: Mário André Chen Cao
 ** Nome do Módulo: cidadao.c
 ** Descrição/Explicação do Módulo: Simula a chegada do cidadao ao centro de saude
 **
 **
 ******************************************************************************/
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdbool.h>

#define TIME 5

/*Signal handler*/
void sig_handler(int sig){

  //Guarda o pid do processo cidadao.c
  int pid = getpid();

  /*Se o sinal for o SIGINT*/
  if(sig == SIGINT){
    printf("\n");
    sucesso("C5) O cidadão cancelou a vacinação, o pedido nº <%d> foi cancelado", pid);
    remove(FILE_PEDIDO_VACINA);
    exit(0);
  }
  /*Se o sinal for o SIGUSR1*/
  if(sig == SIGUSR1){
    sucesso("C7) Vacinação do cidadão com o pedido nº <%d> em curso", pid);
    remove(FILE_PEDIDO_VACINA);
  }
  /*Se o sinal for o SIGUSR2*/
  if(sig == SIGUSR2){
    sucesso("C8) Vacinação do cidadão com o pedido nº <%d> concluída", pid);
    exit(0);
  }
  /*Se o sinal for o SIGTERM*/
  if(sig == SIGTERM){
    sucesso("C9) Não é possível vacinar o cidadão no pedido nº <%d>", pid);
    remove(FILE_PEDIDO_VACINA);
    exit(0);
  }
  /*Se o sinal for o SIGALRM*/
  if(sig == SIGALRM){
    //Espera 5 segundos 
    alarm(TIME);
  }
}

/*Pede ao utilizador os dados a inserir de modo a efeturar o pedido de vacinação*/
void get_data(){

  /*Declara as variaveis*/
  FILE *fp1;
  Cidadao a;

  //Pede ao utilizador a informacao
  printf("Número Utente: ");
  scanf("\n%d", &a.num_utente);
  scanf("%*[^\n]"); // Limpa o stdin buffer

  printf("Nome: ");
  my_gets(a.nome, 100);

  printf("Idade: ");
  scanf("\n%3d", &a.idade);
  scanf("%*[^\n]"); // Limpa o stdin buffer

  printf("Localidade: ");
  my_gets(a.localidade,100);

  printf("Nrº Telemóvel: ");
  my_gets(a.nr_telemovel,10);

  //Coloca o estado de vacinação do cidadão a 0
  a.estado_vacinacao=0;
  
  //Output de sucesso
  sucesso("C1) Dados Cidadão: <%d>; <%s>; <%d>; <%s>; <%s>; 0", a.num_utente, a.nome, a.idade, a.localidade, a.nr_telemovel );
  
  //arranja o PID do processo cidadao e coloca o valor
  a.PID_cidadao=getpid();

  //Output de sucesso
  sucesso("C2) PID Cidadão: <%d>", a.PID_cidadao);

  /*Aqui não preciso de verificar se o ficheiro FILE_PEDIDO_VACINA já existe ou não,
  pois já verifiquei no main*/

  //inicializa o ficheiro em modo escrita
  fp1 = fopen(FILE_PEDIDO_VACINA, "w");

  //se houver um erro em inicializar o ficheiro
  if(fp1 == NULL){
    erro("C4) Não é possível criar o ficheiro FILE_PEDIDO_VACINA");
    exit(-1);
  }

  //regista as informacoes no ficheiro pedidovacina.txt
  fprintf(fp1, "%d:%s:%d:%s:%s:%d:%d\n", a.num_utente, a.nome, a.idade, a.localidade, a.nr_telemovel, a.estado_vacinacao, a.PID_cidadao);

  //Output de sucesso
  sucesso("C4) Ficheiro FILE_PEDIDO_VACINA criado e preenchido");

  //fecha o ficheiro
  fclose(fp1);

  //Arma o sinal SIGINT
  signal(SIGINT, sig_handler);

}

/*Trata dos pedidos com o servidor.c*/
void servidor(){
  
  /*Declara as variavies*/
  FILE *fp2;
  int pid_num;

  /*Se o ficheiro servidor.pid não existir*/
  if( access(FILE_PID_SERVIDOR, F_OK ) != 0 ){
    erro("C6) Não existe ficheiro FILE_PID_SERVIDOR!");
    exit(-1);
  }

  /*Se o ficheiro existir, abre o ficheiro em modo leitura*/
  fp2 = fopen(FILE_PID_SERVIDOR , "r");

  /*Se houver um erro*/
  if(fp2 == NULL){
    printf("Erro a ler o ficheiro. \n");
    exit(-1);
  }

  /*Dá scan ao ficheiro*/
  fscanf (fp2, "%d", &pid_num);

  //Enquanto não for vazio    
  while (!feof (fp2)){  
    /*Guarda o valor do PID do processo servidor.pid
    na variável pid_num*/
    fscanf (fp2, "%d", &pid_num);      
  }

  //Envia o sinal SIGUSR1 ao servidor
  kill(pid_num, SIGUSR1);

  //Mensagem de sucesso
  sucesso("C6) Sinal enviado ao Servidor: <%d>", pid_num);

  //fecha o ficheiro
  fclose(fp2);

}


/*Verifica se o ficheiro pedidovacina.txt existe ou nao*/
bool exists_request(){
  //Se existir, devolve verdadeiro
  if( access(  FILE_PEDIDO_VACINA , F_OK ) == 0 ) {
    erro("C3) Não é possível iniciar o processo de vacinação neste momento");
    return 1;
  }

  //Se não existir, devolve falso
  if( access(  FILE_PEDIDO_VACINA , F_OK) != 0)
    sucesso("C3) Ficheiro FILE_PEDIDO_VACINA pode ser criado");
    return 0;
}

//Funcao que corre tudo
int main(){

  //Arma o sinal SIGALRM
  signal(SIGALRM, sig_handler);

  //Se o ficheiro pedidovacina.txt já existir, emite o sinal alarme e fica num loop até o ficheiro deixar de existir
  while((exists_request() == 1)){
    //emite o SIGALRM que corresponde ao int 14
    sig_handler(14);
    //Espera passiva
    pause();
  }

  //Invoca a função que pede ao utilizador as informações
  get_data();

  //Comunica com o servidor
  servidor();

  /*Arma os sinais SIGUSR1 e SIGUSR2*/
  signal(SIGUSR1, sig_handler);
  signal(SIGUSR2, sig_handler);
  //Arma o SIGTERM
  signal(SIGTERM, sig_handler);

  //Fica em espera passiva ate receber SIGINT, SIGUSR2, SIGTERM
  while(1) pause();
  
  //Devolve 0 e acaba o programa
  return 0;
}
