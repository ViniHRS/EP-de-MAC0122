#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "html_tags.h"
#include "extrator_txt.h"

// Protótipos
void style_css(FILE *fp);
void main_page_html(FILE *fp);
void gerar_todas_as_paginas_indices(ColecaoDados *colecao);

#define MAX_ITEMS 100

// Função auxiliar para ler uma pasta específica
void processar_pasta(const char *nome_pasta, ColecaoDados *colecao) {
    DIR *dir = opendir(nome_pasta);
    if (dir != NULL) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".txt") != NULL) {
                char caminho_completo[512];
                snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", nome_pasta, entry->d_name);

                FILE *txt_file = fopen(caminho_completo, "r");
                if (txt_file != NULL) {
                    DadosTXT dados = {0};
                    char linha[1024];

                    while (fgets(linha, sizeof(linha), txt_file) != NULL) {
                        processar_linha_txt(linha, &dados);
                    }
                    fclose(txt_file);

                    // ---> ADICIONA ESTAS DUAS LINHAS AQUI <---
                    // Transforma o nome do txt (ex: selection_sort.txt) em html (ex: selection_sort.html)
                    snprintf(dados.nome_arquivo_html, sizeof(dados.nome_arquivo_html), "item_%d.html", colecao->total);
                    // Ou, se preferires usar o nome exato do ficheiro txt trocando a extensão:
                    // snprintf(dados.nome_arquivo_html, sizeof(dados.nome_arquivo_html), "%.*s.html", (int)(strlen(entry->d_name) - 4), entry->d_name);

                    if (colecao->total < MAX_ITEMS) {
                        colecao->itens[colecao->total++] = dados;
                        printf("Lido com sucesso [%s]: %s -> gerará %s\n", nome_pasta, entry->d_name, dados.nome_arquivo_html);
                    }
                }
            }
        }
        closedir(dir);
    } else {
        printf("Aviso: Não foi possível abrir a pasta '%s'\n", nome_pasta);
    }
}

int main(void) {
    // 1. Criar a pasta de saída gerada
    if (mkdir("generated", 0777) == -1) {}

    // 2. Gerar o CSS global
    FILE *css_fp = fopen("generated/style.css", "w");
    if (css_fp != NULL) {
        style_css(css_fp);
        fclose(css_fp);
    }

    // 3. Inicializar a coleção
    ColecaoDados colecao = {0};

    // 4. Ler os ficheiros de ambas as pastas
    processar_pasta("algoritmos", &colecao);
    processar_pasta("conceitos", &colecao);

    // 5. Gerar o index.html principal
    FILE *html_fp = fopen("generated/index.html", "w");
    if (html_fp != NULL) {
        main_page_html(html_fp);
        fclose(html_fp);
    }

    // 6. Gerar as 6 páginas de índices dinâmicos
    gerar_todas_as_paginas_indices(&colecao);

    printf("\nSite gerado com sucesso na pasta 'generated/' com base nas pastas 'algoritmos/' e 'conceitos/'!\n");

    return 0;
}