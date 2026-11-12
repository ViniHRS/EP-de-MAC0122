// indexador.h
#ifndef INDEXADOR_H_
#define INDEXADOR_H_

#include <stdio.h>
#include "extrator_txt.h"

#define MAX_ITEMS 100

typedef struct {
    DadosTXT itens[MAX_ITEMS];
    int total;
} ColecaoDados;

void gerar_paginas_indices(ColecaoDados *colecao);

#endif