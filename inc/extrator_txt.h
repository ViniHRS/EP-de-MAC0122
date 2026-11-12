#ifndef EXTRATOR_TXT_H
#define EXTRATOR_TXT_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef struct {
    char *tecnica;
    char *algoritmo;
    char *autores;
    char *data;
    char *conceitos;
    char *problema;
    char *solucao;
    char *codigo;
    int estado_leitura;
    char *arquivo;   /* nome do .txt sem extensão */
    char *slug;      /* usado no nome do .html */
} DadosTXT;

typedef struct {
    char *nome;
    char *explicacao;
    char *arquivo;
    char *slug;
} DadosConceito;

char *limpar_string(char *str);
void anexar_solucao(char **solucao_atual, const char *nova_linha);
void processar_linha_txt(const char *linha, DadosTXT *dados);
void processar_linha_conceito(const char *linha, DadosConceito *dados);

int carregar_algoritmos(const char *diretorio, DadosTXT **algoritmos, size_t *quantidade);
int carregar_conceitos(const char *diretorio, DadosConceito **conceitos, size_t *quantidade);
void liberar_algoritmos(DadosTXT *algoritmos, size_t quantidade);
void liberar_conceitos(DadosConceito *conceitos, size_t quantidade);

#endif
