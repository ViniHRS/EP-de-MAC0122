#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "html_tags.h"
#include "extrator_txt.h"

// Função auxiliar para sanitizar nomes de arquivos (ex: "Divisão e conquista" -> "tag_divisao_e_conquista.html")
void sanitizar_nome_tag(const char *tag, char *saida, size_t max_len) {
    size_t j = 0;
    snprintf(saida, max_len, "tag_");
    j = strlen(saida);

    for (size_t i = 0; tag[i] != '\0' && j < max_len - 6; i++) {
        unsigned char c = (unsigned char)tag[i];
        if (isalnum(c)) {
            saida[j++] = tolower(c);
        } else if (c == ' ' || c == '-' || c == '_') {
            if (j > 0 && saida[j-1] != '_') saida[j++] = '_';
        }
    }
    if (j > 0 && saida[j-1] == '_') j--;
    saida[j] = '\0';
    strcat(saida, ".html");
}

// 1. Gera a página individual de UMA TAG (ex: tag_recursaes.html contendo links para Merge Sort e Quick Sort)
void gerar_pagina_da_tag(const char *nome_tag, ColecaoDados *colecao, int tipo_campo) {
    char nome_html[128];
    sanitizar_nome_tag(nome_tag, nome_html, sizeof(nome_html));

    char caminho[256];
    snprintf(caminho, sizeof(caminho), "generated/%s", nome_html);

    FILE *fp = fopen(caminho, "w");
    if (fp == NULL) return;

    DOCTYPE;
    HTML("en") {
        HEAD() {
            META("charset='utf-8'");
            META("name='viewport' content='width=device-width, initial-scale=1'");
            fprintf(fp, "<title>MAC0122 - Tag: %s</title>", nome_tag);
            LINK("rel='stylesheet' href='style.css'");
        }
        BODY("") {
            MAIN() {
                DIV("class='nav-back top'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
                HR();

                H1("class='main-title'") { _("Tag: %s", nome_tag); }
                H2("class='main-subtitle'") { _("Algoritmos / Conteúdos associados a este termo:"); }

                UL("class='sub-tree'") {
                    for (int i = 0; i < colecao->total; i++) {
                        char *valor_campo = NULL;
                        switch (tipo_campo) {
                            case 1: valor_campo = colecao->itens[i].tecnica; break;
                            case 2: valor_campo = colecao->itens[i].algoritmo; break;
                            case 3: valor_campo = colecao->itens[i].autores; break;
                            case 4: valor_campo = colecao->itens[i].data; break;
                            case 5: valor_campo = colecao->itens[i].conceitos; break;
                            case 6: valor_campo = colecao->itens[i].problema; break;
                        }

                        // Se a tag pesquisada estiver dentro do campo do item
                        if (valor_campo != NULL && strstr(valor_campo, nome_tag) != NULL) {
                            LI("") {
                                const char *nome_alg = colecao->itens[i].algoritmo ? 
                                                       colecao->itens[i].algoritmo : 
                                                       "Ver Detalhes";
                                // Link direto para a página final do algoritmo
                                A(colecao->itens[i].nome_arquivo_html, nome_alg);
                            }
                        }
                    }
                }

                HR();
                DIV("class='nav-back bottom'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
            }
        }
    }
    fclose(fp);
}

// 2. Gera a página da categoria (ex: tecnica.html) listando APENAS as tags únicas sem repetições
void gerar_pagina_categoria(const char *nome_arquivo, const char *titulo_categoria, int tipo_campo, ColecaoDados *colecao) {
    char caminho[256];
    snprintf(caminho, sizeof(caminho), "generated/%s", nome_arquivo);

    FILE *fp = fopen(caminho, "w");
    if (fp == NULL) return;

    // Estrutura para controlar tags já impressas (evita duplicar "Recursão")
    char tags_processadas[100][64];
    int total_tags = 0;

    DOCTYPE;
    HTML("en") {
        HEAD() {
            META("charset='utf-8'");
            META("name='viewport' content='width=device-width, initial-scale=1'");
            fprintf(fp, "<title>MAC0122 - %s</title>", titulo_categoria);
            LINK("rel='stylesheet' href='style.css'");
        }
        BODY("") {
            MAIN() {
                DIV("class='nav-back top'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
                HR();

                H1("class='main-title'") { _("%s", titulo_categoria); }
                H2("class='main-subtitle'") { _("Selecione um termo para ver os itens correspondentes:"); }

                UL("class='sub-tree'") {
                    for (int i = 0; i < colecao->total; i++) {
                        char *valor_campo = NULL;
                        switch (tipo_campo) {
                            case 1: valor_campo = colecao->itens[i].tecnica; break;
                            case 2: valor_campo = colecao->itens[i].algoritmo; break;
                            case 3: valor_campo = colecao->itens[i].autores; break;
                            case 4: valor_campo = colecao->itens[i].data; break;
                            case 5: valor_campo = colecao->itens[i].conceitos; break;
                            case 6: valor_campo = colecao->itens[i].problema; break;
                        }

                        if (valor_campo != NULL && strlen(valor_campo) > 0) {
                            // Quebra os valores separados por vírgula (ex: "Divisão e conquista, recursão")
                            char copia_campo[256];
                            snprintf(copia_campo, sizeof(copia_campo), "%s", valor_campo);
                            char *token = strtok(copia_campo, ",");

                            while (token != NULL) {
                                // Limpa espaços no início
                                while (*token == ' ') token++;
                                
                                // Verifica se a tag já foi processada
                                int ja_existe = 0;
                                for (int k = 0; k < total_tags; k++) {
                                    if (strcasecmp(tags_processadas[k], token) == 0) {
                                        ja_existe = 1;
                                        break;
                                    }
                                }

                                // Se for uma tag nova, cria o HTML dela e o link na lista
                                if (!ja_existe && strlen(token) > 0) {
                                    strncpy(tags_processadas[total_tags++], token, 64);
                                    
                                    // Gera a página própria desta tag (ex: tag_recursaes.html)
                                    gerar_pagina_da_tag(token, colecao, tipo_campo);

                                    char arq_tag[128];
                                    sanitizar_nome_tag(token, arq_tag, sizeof(arq_tag));

                                    // Imprime APENAS a tag com o link para a página dela (sem o " - Merge Sort" atrás)
                                    LI("") {
                                        A(arq_tag, token);
                                    }
                                }
                                token = strtok(NULL, ",");
                            }
                        }
                    }
                }

                HR();
                DIV("class='nav-back bottom'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
            }
        }
    }
    fclose(fp);
}

void gerar_pagina_item(DadosTXT *item) {
    if (item->nome_arquivo_html[0] == '\0') return;

    char caminho[256];
    snprintf(caminho, sizeof(caminho), "generated/%s", item->nome_arquivo_html);

    FILE *fp = fopen(caminho, "w");
    if (fp == NULL) return;

    const char *titulo = item->algoritmo ? item->algoritmo : "Detalhes do Item";

    DOCTYPE;
    HTML("en") {
        HEAD() {
            META("charset='utf-8'");
            META("name='viewport' content='width=device-width, initial-scale=1'");
            fprintf(fp, "<title>MAC0122 - %s</title>", titulo);
            LINK("rel='stylesheet' href='style.css'");
        }
        BODY("") {
            MAIN() {
                DIV("class='nav-back top'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
                HR();

                H1("class='main-title'") { _("%s", titulo); }

                UL("class='sub-tree'") {
                    if (item->tecnica)   LI("") { _("<b>Técnica:</b> %s", item->tecnica); }
                    if (item->autores)   LI("") { _("<b>Autores:</b> %s", item->autores); }
                    if (item->data)      LI("") { _("<b>Data:</b> %s", item->data); }
                    if (item->conceitos) LI("") { _("<b>Conceitos:</b> %s", item->conceitos); }
                    if (item->problema)  LI("") { _("<b>Problema:</b> %s", item->problema); }
                }

                HR();

                H2("") { _("Explicação"); }
                if (item->solucao) {
                    P(item->solucao);
                }

                HR();
                DIV("class='nav-back bottom'") {
                    A("index.html", "← Voltar ao Índice Principal");
                }
            }
        }
    }
    fclose(fp);
}

void gerar_todas_as_paginas_indices(ColecaoDados *colecao) {
    // 1. Gera as páginas individuais de cada algoritmo/conceito
    for (int i = 0; i < colecao->total; i++) {
        gerar_pagina_item(&colecao->itens[i]);
    }

    // 2. Gera as 6 páginas de categorias (que agora geram também as sub-páginas de tags)
    gerar_pagina_categoria("tecnica.html", "Técnica", 1, colecao);
    gerar_pagina_categoria("algoritmo.html", "Algoritmo", 2, colecao);
    gerar_pagina_categoria("autores.html", "Autores", 3, colecao);
    gerar_pagina_categoria("data.html", "Data", 4, colecao);
    gerar_pagina_categoria("conceitos.html", "Conceitos", 5, colecao);
    gerar_pagina_categoria("problema.html", "Problema", 6, colecao);
}