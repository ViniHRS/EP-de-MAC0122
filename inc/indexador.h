// indexador.h
#ifndef INDEXADOR_H_
#define INDEXADOR_H_

#include <stdio.h>
#include "extrator_txt.h"

#define DIR_SAIDA "generated"
#define TOTAL_CATEGORIAS 6

typedef struct {
    const char *arquivo;   /* categoria.html */
    const char *prefixo;   /* prefixo das páginas de tag */
    const char *titulo;
    const char *descricao; /* frase curta mostrada no índice */
} Categoria;

extern const Categoria categorias[TOTAL_CATEGORIAS];

/* gerador_html.c */
void escrever_html_escapado(FILE *fp, const char *texto);
void escrever_texto_com_quebras(FILE *fp, const char *texto);
void style_css(FILE *fp);
void render_navegacao(FILE *fp);   /* barra escura do topo (<nav>) */
void main_page_html(FILE *fp, size_t qtd_algoritmos, size_t qtd_conceitos);

/* gerador_indices.c: categorias, tags, algoritmos, glossário e conceitos.
   Retorna 0 em sucesso. */
int gerar_paginas_indices(DadosTXT *algoritmos, size_t qtd_algoritmos,
                          DadosConceito *conceitos, size_t qtd_conceitos);

void escrever_codigo_colorido(FILE *fp, const char *codigo);

#endif