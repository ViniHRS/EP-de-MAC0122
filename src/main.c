#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "extrator_txt.h"
#include "indexador.h"

static int criar_diretorio(const char *caminho)
{
    return (mkdir(caminho, 0777) == 0 || errno == EEXIST) ? 0 : -1;
}

/* Remove os .html de execuções anteriores para não sobrar página órfã. */
static void limpar_paginas_antigas(const char *diretorio)
{
    DIR *pasta = opendir(diretorio);
    struct dirent *entrada;

    if (pasta == NULL) return;

    while ((entrada = readdir(pasta)) != NULL) {
        size_t t = strlen(entrada->d_name);
        char caminho[1200];

        if (t < 5 || strcmp(entrada->d_name + t - 5, ".html") != 0) continue;
        snprintf(caminho, sizeof(caminho), "%s/%s", diretorio, entrada->d_name);
        remove(caminho);
    }
    closedir(pasta);
}

int main(int argc, char **argv)
{
    const char *dir_alg = argc > 1 ? argv[1] : "algoritmos";
    const char *dir_con = argc > 2 ? argv[2] : "conceitos";
    DadosTXT *algoritmos = NULL;
    DadosConceito *conceitos = NULL;
    size_t qa = 0, qc = 0;
    int erro = 0;
    FILE *fp;

    if (argc > 3) {
        fprintf(stderr, "Uso: %s [pasta_algoritmos] [pasta_conceitos]\n", argv[0]);
        return 1;
    }

    /* 1. Ler os TXT */
    if (carregar_algoritmos(dir_alg, &algoritmos, &qa) != 0) {
        fprintf(stderr, "Não foi possível abrir a pasta de algoritmos: %s\n", dir_alg);
        return 1;
    }
    if (carregar_conceitos(dir_con, &conceitos, &qc) != 0) {
        fprintf(stderr, "Não foi possível abrir a pasta de conceitos: %s\n", dir_con);
        liberar_algoritmos(algoritmos, qa);
        return 1;
    }

    /* 2. Pasta de saída */
    if (criar_diretorio(DIR_SAIDA) != 0) {
        perror("Não foi possível criar a pasta de saída");
        liberar_algoritmos(algoritmos, qa);
        liberar_conceitos(conceitos, qc);
        return 1;
    }
    limpar_paginas_antigas(DIR_SAIDA);

    /* 3. CSS global */
    fp = fopen(DIR_SAIDA "/style.css", "w");
    if (fp != NULL) {
        style_css(fp);
        fclose(fp);
    } else {
        perror(DIR_SAIDA "/style.css");
        erro = 1;
    }

    /* 4. index.html */
    fp = fopen(DIR_SAIDA "/index.html", "w");
    if (fp != NULL) {
        main_page_html(fp, qa, qc);
        fclose(fp);
    } else {
        perror(DIR_SAIDA "/index.html");
        erro = 1;
    }

    /* 5. Categorias, tags, algoritmos, glossário e conceitos */
    if (!erro && gerar_paginas_indices(algoritmos, qa, conceitos, qc) != 0) erro = 1;

    if (!erro) {
        printf("Site gerado em '%s/'.\n", DIR_SAIDA);
        printf("Algoritmos carregados: %zu\n", qa);
        printf("Conceitos carregados: %zu\n", qc);
        if (qa == 0 || qc == 0) {
            printf("Aviso: confira se os arquivos TXT foram colocados nas pastas de entrada.\n");
        }
    }

    liberar_algoritmos(algoritmos, qa);
    liberar_conceitos(conceitos, qc);
    return erro ? 1 : 0;
}