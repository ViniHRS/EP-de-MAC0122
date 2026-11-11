#ifndef EXTRATOR_TXT_H
#define EXTRATOR_TXT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tecnica;
    char *algoritmo;
    char *autores;
    char *data;
    char *conceitos;
    char *problema;
    char *solucao;
} DadosTXT;

//Declarações de função

char* limpar_string(char *str);

void anexar_solucao(char **solucao_atual, const char *nova_linha);

void processar_linha_txt(const char *linha, DadosTXT *dados);

#endif