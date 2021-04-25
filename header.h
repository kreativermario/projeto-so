#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*Define as funções*/
long fsize(FILE* file);
void dad_handler(int sig);
void child_handler(int sig);
void vaccinate(int i);
void write_dat(int index);


#endif


