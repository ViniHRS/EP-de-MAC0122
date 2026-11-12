#include <ctype.h>
#include <string.h>
#include <strings.h>
#include "indexador.h"
#include "html_tags.h"   /* sempre por último: define macros curtas (A, B, I, P...) */

/* Cabeçalho comum de todas as páginas (usar dentro de HTML()). */
#define CABECALHO_HEAD(titulo_pagina)                                              \
    HEAD() {                                                                       \
        META("charset='utf-8'");                                                   \
        META("name='viewport' content='width=device-width, initial-scale=1'");     \
        fputs("<title>MAC0122 - ", fp);                                            \
        escrever_html_escapado(fp, titulo_pagina);                                 \
        fputs("</title>", fp);                                                     \
        LINK("rel='stylesheet' href='style.css'");                                 \
    }

/* Link "voltar" no topo da página (usar dentro de MAIN()). */
#define VOLTAR(destino, texto) \
    fprintf(fp, "<a class='voltar' href='%s'>%s</a>\n", destino, texto)

typedef struct {
    char *nome;
    size_t *indices;
    size_t quantidade;
} Grupo;

/* ------------------------------------------------------------------ */
/* Utilitários de texto                                                */
/* ------------------------------------------------------------------ */

static char *aparar_texto(const char *texto)
{
    const char *inicio = texto;
    size_t tamanho;
    char *copia;

    while (isspace((unsigned char)*inicio)) inicio++;
    tamanho = strlen(inicio);
    while (tamanho > 0 && isspace((unsigned char)inicio[tamanho - 1])) tamanho--;

    copia = malloc(tamanho + 1);
    if (copia != NULL) {
        memcpy(copia, inicio, tamanho);
        copia[tamanho] = '\0';
    }
    return copia;
}

/* minúsculas, sem acentos, só letras/dígitos separados por um espaço */
static char *normalizar_chave(const char *texto)
{
    char *resultado = malloc(strlen(texto) + 2);
    size_t i = 0, j = 0;

    if (resultado == NULL) return NULL;

    while (texto[i] != '\0') {
        unsigned char c = (unsigned char)texto[i];
        char conv = ' ';

        if (c < 128) {
            if (isalnum(c)) conv = (char)tolower(c);
            i++;
        }
        else if (c == 0xC3 && texto[i + 1] != '\0') {
            switch ((unsigned char)texto[i + 1]) {
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84:
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: conv = 'a'; break;
                case 0x87: case 0xA7: conv = 'c'; break;
                case 0x88: case 0x89: case 0x8A: case 0x8B:
                case 0xA8: case 0xA9: case 0xAA: case 0xAB: conv = 'e'; break;
                case 0x8C: case 0x8D: case 0x8E: case 0x8F:
                case 0xAC: case 0xAD: case 0xAE: case 0xAF: conv = 'i'; break;
                case 0x91: case 0xB1: conv = 'n'; break;
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96:
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: conv = 'o'; break;
                case 0x99: case 0x9A: case 0x9B: case 0x9C:
                case 0xB9: case 0xBA: case 0xBB: case 0xBC: conv = 'u'; break;
                default: break;
            }
            i += 2;
        }
        else i++;

        if (conv == ' ') {
            if (j > 0 && resultado[j - 1] != ' ') resultado[j++] = ' ';
        } else {
            resultado[j++] = conv;
        }
    }

    if (j > 0 && resultado[j - 1] == ' ') j--;
    resultado[j] = '\0';
    return resultado;
}

static int palavra_equivalente(const char *a, size_t ta, const char *b, size_t tb)
{
    if (ta == tb && strncmp(a, b, ta) == 0) return 1;

    /* plural -ao / -oes (já normalizado, sem acento): recursao ~ recursoes */
    if (ta > 3 && tb == ta + 1 && strcmp(a + ta - 2, "ao") == 0 &&
        strcmp(b + tb - 3, "oes") == 0 && strncmp(a, b, ta - 2) == 0) return 1;
    if (tb > 3 && ta == tb + 1 && strcmp(b + tb - 2, "ao") == 0 &&
        strcmp(a + ta - 3, "oes") == 0 && strncmp(a, b, tb - 2) == 0) return 1;

    /* plural com "s" */
    if (ta > 3 && a[ta - 1] == 's' && ta - 1 == tb && strncmp(a, b, tb) == 0) return 1;
    if (tb > 3 && b[tb - 1] == 's' && tb - 1 == ta && strncmp(a, b, ta) == 0) return 1;

    /* plural com "es" */
    if (ta > 4 && ta - 2 == tb && a[ta - 2] == 'e' && a[ta - 1] == 's' &&
        strncmp(a, b, tb) == 0) return 1;
    if (tb > 4 && tb - 2 == ta && b[tb - 2] == 'e' && b[tb - 1] == 's' &&
        strncmp(a, b, ta) == 0) return 1;

    return 0;
}

static int nomes_equivalentes(const char *a, const char *b)
{
    char *na = normalizar_chave(a);
    char *nb = normalizar_chave(b);
    char *ca = NULL, *cb = NULL;
    char *pa, *pb;
    int iguais = 1;

    if (na == NULL || nb == NULL) {
        free(na);
        free(nb);
        return 0;
    }

    pa = strtok_r(na, " ", &ca);
    pb = strtok_r(nb, " ", &cb);

    while (pa != NULL && pb != NULL) {
        if (!palavra_equivalente(pa, strlen(pa), pb, strlen(pb))) {
            iguais = 0;
            break;
        }
        pa = strtok_r(NULL, " ", &ca);
        pb = strtok_r(NULL, " ", &cb);
    }
    if (pa != NULL || pb != NULL) iguais = 0;

    free(na);
    free(nb);
    return iguais;
}

/* "Divisão e Conquista" -> "divisao-e-conquista" (truncado em max-1). */
static void slug_de_texto(const char *texto, char *saida, size_t max, int *truncado)
{
    char *norm = normalizar_chave(texto);
    size_t j = 0;

    *truncado = 0;
    if (norm != NULL) {
        for (size_t i = 0; norm[i] != '\0'; i++) {
            if (j + 1 >= max) { *truncado = 1; break; }
            saida[j++] = norm[i] == ' ' ? '-' : norm[i];
        }
    }
    saida[j] = '\0';
    if (j == 0) snprintf(saida, max, "item");
    free(norm);
}

/* ------------------------------------------------------------------ */
/* Nomes de arquivos / títulos                                         */
/* ------------------------------------------------------------------ */

static const char *titulo_algoritmo(const DadosTXT *a)
{
    if (a->algoritmo != NULL && a->algoritmo[0] != '\0') return a->algoritmo;
    return a->arquivo != NULL ? a->arquivo : "Algoritmo";
}

static const char *titulo_conceito(const DadosConceito *c)
{
    if (c->nome != NULL && c->nome[0] != '\0') return c->nome;
    return c->arquivo != NULL ? c->arquivo : "Conceito";
}

static void arquivo_algoritmo(char *saida, size_t n, const DadosTXT *a)
{
    snprintf(saida, n, "alg_%s.html", a->slug != NULL ? a->slug : "item");
}

static void arquivo_conceito(char *saida, size_t n, const DadosConceito *c)
{
    snprintf(saida, n, "conceito_%s.html", c->slug != NULL ? c->slug : "item");
}

static void arquivo_tag(char *saida, size_t n, int categoria, const char *nome, size_t indice)
{
    char slug[64];
    int truncado;

    slug_de_texto(nome, slug, sizeof(slug), &truncado);
    if (truncado) snprintf(saida, n, "tag_%s_%s-%zu.html", categorias[categoria].prefixo, slug, indice);
    else          snprintf(saida, n, "tag_%s_%s.html", categorias[categoria].prefixo, slug);
}

/* ------------------------------------------------------------------ */
/* Conceitos                                                           */
/* ------------------------------------------------------------------ */

static DadosConceito *buscar_conceito(DadosConceito *conceitos, size_t quantidade,
                                      const char *nome)
{
    for (size_t i = 0; i < quantidade; i++) {
        if (conceitos[i].nome != NULL && nomes_equivalentes(conceitos[i].nome, nome)) {
            return &conceitos[i];
        }
    }
    return NULL;
}

static void escrever_conceitos_com_links(FILE *fp, const char *lista,
                                         DadosConceito *conceitos, size_t quantidade)
{
    char *copia, *parte, *contexto = NULL;
    int primeiro = 1;

    if (lista == NULL || lista[0] == '\0') return;

    copia = strdup(lista);
    if (copia == NULL) {
        escrever_html_escapado(fp, lista);
        return;
    }

    for (parte = strtok_r(copia, ",;", &contexto); parte != NULL;
         parte = strtok_r(NULL, ",;", &contexto)) {
        char *nome = aparar_texto(parte);
        DadosConceito *c = nome == NULL ? NULL : buscar_conceito(conceitos, quantidade, nome);

        if (!primeiro) fputs(", ", fp);
        primeiro = 0;

        if (c != NULL) {
            char arq[256];
            arquivo_conceito(arq, sizeof(arq), c);
            fprintf(fp, "<a href='%s'>", arq);
            escrever_html_escapado(fp, nome);
            fputs("</a>", fp);
        } else {
            escrever_html_escapado(fp, nome == NULL ? parte : nome);
        }
        free(nome);
    }
    free(copia);
}

static int algoritmo_tem_conceito(const DadosTXT *algoritmo, const char *nome_conceito)
{
    char *copia, *parte, *contexto = NULL;
    int achou = 0;

    if (algoritmo->conceitos == NULL) return 0;
    copia = strdup(algoritmo->conceitos);
    if (copia == NULL) return 0;

    for (parte = strtok_r(copia, ",;", &contexto); parte != NULL && !achou;
         parte = strtok_r(NULL, ",;", &contexto)) {
        char *nome = aparar_texto(parte);
        if (nome != NULL && nomes_equivalentes(nome, nome_conceito)) achou = 1;
        free(nome);
    }
    free(copia);
    return achou;
}

/* ------------------------------------------------------------------ */
/* Grupos (tags)                                                       */
/* ------------------------------------------------------------------ */

static const char *campo_algoritmo(const DadosTXT *a, int indice)
{
    switch (indice) {
        case 0: return a->tecnica;
        case 1: return a->algoritmo;
        case 2: return a->autores;
        case 3: return a->data;
        case 4: return a->conceitos;
        case 5: return a->problema;
        default: return NULL;
    }
}

static int comparar_grupos(const void *a, const void *b)
{
    const Grupo *ga = a, *gb = b;
    int r = strcasecmp(ga->nome, gb->nome);
    return r == 0 ? strcmp(ga->nome, gb->nome) : r;
}

static int adicionar_indice(Grupo *g, size_t indice)
{
    size_t *tmp = realloc(g->indices, (g->quantidade + 1) * sizeof(*tmp));
    if (tmp == NULL) return -1;
    g->indices = tmp;
    g->indices[g->quantidade++] = indice;
    return 0;
}

static int adicionar_ao_grupo(Grupo **grupos, size_t *quantidade, const char *nome,
                              size_t indice, int flexivel)
{
    char *limpo = aparar_texto(nome);
    Grupo *tmp;

    if (limpo == NULL) return -1;
    if (limpo[0] == '\0') {
        free(limpo);
        limpo = strdup("Não informado");
        if (limpo == NULL) return -1;
    }

    for (size_t i = 0; i < *quantidade; i++) {
        if ((flexivel && nomes_equivalentes((*grupos)[i].nome, limpo)) ||
            (!flexivel && strcmp((*grupos)[i].nome, limpo) == 0)) {
            free(limpo);
            /* evita repetir o mesmo algoritmo duas vezes no mesmo grupo */
            if ((*grupos)[i].quantidade > 0 &&
                (*grupos)[i].indices[(*grupos)[i].quantidade - 1] == indice) {
                return 0;
            }
            return adicionar_indice(&(*grupos)[i], indice);
        }
    }

    tmp = realloc(*grupos, (*quantidade + 1) * sizeof(**grupos));
    if (tmp == NULL) {
        free(limpo);
        return -1;
    }
    *grupos = tmp;
    (*grupos)[*quantidade].nome = limpo;
    (*grupos)[*quantidade].indices = NULL;
    (*grupos)[*quantidade].quantidade = 0;
    if (adicionar_indice(&(*grupos)[*quantidade], indice) != 0) {
        free(limpo);
        return -1;
    }
    (*quantidade)++;
    return 0;
}

static void liberar_grupos(Grupo *grupos, size_t quantidade)
{
    for (size_t i = 0; i < quantidade; i++) {
        free(grupos[i].nome);
        free(grupos[i].indices);
    }
    free(grupos);
}

/* Técnica (0) e Conceitos (4) aceitam vários termos separados por , ou ; */
static int montar_grupos(DadosTXT *algoritmos, size_t qa, int categoria,
                         Grupo **grupos, size_t *qg)
{
    *grupos = NULL;
    *qg = 0;

    for (size_t i = 0; i < qa; i++) {
        const char *valor = campo_algoritmo(&algoritmos[i], categoria);
        int multivalorado = (categoria == 0 || categoria == 4);

        if (multivalorado && valor != NULL && valor[0] != '\0') {
            char *copia = strdup(valor);
            char *parte, *contexto = NULL;
            if (copia == NULL) return -1;
            for (parte = strtok_r(copia, ",;", &contexto); parte != NULL;
                 parte = strtok_r(NULL, ",;", &contexto)) {
                if (adicionar_ao_grupo(grupos, qg, parte, i, 1) != 0) {
                    free(copia);
                    return -1;
                }
            }
            free(copia);
        }
        else if (adicionar_ao_grupo(grupos, qg,
                                    (valor == NULL || valor[0] == '\0') ? "Não informado" : valor,
                                    i, multivalorado) != 0) {
            return -1;
        }
    }

    if (*qg > 1) qsort(*grupos, *qg, sizeof(**grupos), comparar_grupos);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Páginas                                                             */
/* ------------------------------------------------------------------ */

static void escrever_link_algoritmo(FILE *fp, const DadosTXT *a)
{
    char arq[256];
    arquivo_algoritmo(arq, sizeof(arq), a);
    fprintf(fp, "<li><a href='%s'>", arq);
    escrever_html_escapado(fp, titulo_algoritmo(a));
    fputs("</a></li>\n", fp);
}

/* Bloco de título: sobretítulo, h1 e (opcional) frase de apoio. */
static void titulo_pagina(FILE *fp, const char *sobre, const char *titulo, const char *sub)
{
    DIV("class='titulo-pagina'") {
        _("<p class='sobretitulo'>");
        escrever_html_escapado(fp, sobre);
        _("</p>");
        H1("") { escrever_html_escapado(fp, titulo); }
        if (sub != NULL) {
            _("<p>");
            escrever_html_escapado(fp, sub);
            _("</p>");
        }
    }
}

static void metadado_inicio(FILE *fp, const char *rotulo)
{
    fprintf(fp, "<div class='metadado'><dt>%s</dt><dd>", rotulo);
}

static void metadado_fim(FILE *fp)
{
    fputs("</dd></div>\n", fp);
}

static void metadado_texto(FILE *fp, const char *rotulo, const char *valor)
{
    metadado_inicio(fp, rotulo);
    escrever_texto_com_quebras(fp, valor);
    metadado_fim(fp);
}

/* tag_<categoria>_<termo>.html: lista os algoritmos que têm esse termo */
static int gerar_pagina_tag(int categoria, const Grupo *grupo, size_t indice_grupo,
                            DadosTXT *algoritmos, DadosConceito *conceitos, size_t qc)
{
    char arq[256], caminho[300], titulo[300];
    DadosConceito *conceito = NULL;
    FILE *fp;

    arquivo_tag(arq, sizeof(arq), categoria, grupo->nome, indice_grupo);
    snprintf(caminho, sizeof(caminho), DIR_SAIDA "/%s", arq);
    snprintf(titulo, sizeof(titulo), "%s: %s", categorias[categoria].titulo, grupo->nome);

    fp = fopen(caminho, "w");
    if (fp == NULL) { perror(caminho); return -1; }

    if (categoria == 4) conceito = buscar_conceito(conceitos, qc, grupo->nome);

    DOCTYPE;
    HTML("pt-BR") {
        CABECALHO_HEAD(titulo)
        BODY("") {
            render_navegacao(fp);
            MAIN() {
                VOLTAR(categorias[categoria].arquivo, "← Voltar para a categoria");
                titulo_pagina(fp, categorias[categoria].titulo, grupo->nome,
                              "Algoritmos associados a este termo.");

                if (conceito != NULL) {
                    char ac[256];
                    arquivo_conceito(ac, sizeof(ac), conceito);
                    _("<p><a href='%s'>Ver explicação do conceito →</a></p>", ac);
                }

                UL("class='lista-itens'") {
                    for (size_t i = 0; i < grupo->quantidade; i++) {
                        escrever_link_algoritmo(fp, &algoritmos[grupo->indices[i]]);
                    }
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

/* tecnica.html, algoritmo.html, ... */
static int gerar_pagina_categoria(int categoria, DadosTXT *algoritmos, size_t qa,
                                  DadosConceito *conceitos, size_t qc)
{
    char caminho[300];
    Grupo *grupos = NULL;
    size_t qg = 0;
    int erro = 0;
    FILE *fp;

    snprintf(caminho, sizeof(caminho), DIR_SAIDA "/%s", categorias[categoria].arquivo);

    if (montar_grupos(algoritmos, qa, categoria, &grupos, &qg) != 0) {
        liberar_grupos(grupos, qg);
        return -1;
    }

    /* páginas de tag primeiro (fora dos blocos de macros) */
    if (categoria != 1) {
        for (size_t i = 0; i < qg && !erro; i++) {
            if (gerar_pagina_tag(categoria, &grupos[i], i, algoritmos, conceitos, qc) != 0) erro = 1;
        }
    }
    if (erro) {
        liberar_grupos(grupos, qg);
        return -1;
    }

    fp = fopen(caminho, "w");
    if (fp == NULL) {
        perror(caminho);
        liberar_grupos(grupos, qg);
        return -1;
    }

    DOCTYPE;
    HTML("pt-BR") {
        CABECALHO_HEAD(categorias[categoria].titulo)
        BODY("") {
            render_navegacao(fp);
            MAIN() {
                VOLTAR("index.html", "← Página inicial");
                titulo_pagina(fp, "Catálogo de MAC0122", categorias[categoria].titulo,
                              categorias[categoria].descricao);

                if (qa == 0) {
                    DIV("class='aviso'") {
                        _("Nenhum algoritmo foi encontrado. Coloque os arquivos TXT na pasta <code>algoritmos</code> e execute o gerador novamente.");
                    }
                }
                else if (categoria == 1) {
                    /* Algoritmos: link direto para a página do item */
                    UL("class='lista-itens'") {
                        for (size_t i = 0; i < qg; i++) {
                            for (size_t k = 0; k < grupos[i].quantidade; k++) {
                                escrever_link_algoritmo(fp, &algoritmos[grupos[i].indices[k]]);
                            }
                        }
                    }
                }
                else {
                    UL("class='lista-itens'") {
                        for (size_t i = 0; i < qg; i++) {
                            char arq[256];
                            arquivo_tag(arq, sizeof(arq), categoria, grupos[i].nome, i);
                            LI("") {
                                _("<a href='%s'>", arq);
                                escrever_html_escapado(fp, grupos[i].nome);
                                _("</a>");
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(fp);
    liberar_grupos(grupos, qg);
    return 0;
}

/* alg_<slug>.html */
static int gerar_pagina_algoritmo(DadosTXT *a, DadosConceito *conceitos, size_t qc)
{
    char arq[256], caminho[300];
    const char *titulo = titulo_algoritmo(a);
    FILE *fp;

    arquivo_algoritmo(arq, sizeof(arq), a);
    snprintf(caminho, sizeof(caminho), DIR_SAIDA "/%s", arq);
    fp = fopen(caminho, "w");
    if (fp == NULL) { perror(caminho); return -1; }

    DOCTYPE;
    HTML("pt-BR") {
        CABECALHO_HEAD(titulo)
        BODY("") {
            render_navegacao(fp);
            MAIN() {
                VOLTAR("algoritmo.html", "← Todos os algoritmos");
                titulo_pagina(fp, "Algoritmo", titulo, NULL);

                _("<dl class='metadados'>\n");
                metadado_texto(fp, "Técnica", a->tecnica);
                metadado_texto(fp, "Autores", a->autores);
                metadado_texto(fp, "Data", a->data);
                metadado_inicio(fp, "Conceitos");
                if (a->conceitos != NULL && a->conceitos[0] != '\0') {
                    escrever_conceitos_com_links(fp, a->conceitos, conceitos, qc);
                } else {
                    escrever_texto_com_quebras(fp, NULL);
                }
                metadado_fim(fp);
                _("</dl>\n");

                SECTION() {
                    H2("") { _("Problema"); }
                    _("<p>");
                    escrever_texto_com_quebras(fp, a->problema);
                    _("</p>");
                }

                SECTION() {
                    H2("") { _("Explicação"); }
                    DIV("class='solucao'") { escrever_texto_com_quebras(fp, a->solucao); }
                }
                //Renderização do Código Colorido
                if (a->codigo != NULL && a->codigo[0] != '\0') {
                    SECTION() {
                        H2("") { _("Implementação em C"); }
                        escrever_codigo_colorido(fp, a->codigo); 
                    }
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

/* glossario.html */
static int gerar_glossario(DadosConceito *conceitos, size_t qc)
{
    FILE *fp = fopen(DIR_SAIDA "/glossario.html", "w");
    if (fp == NULL) { perror(DIR_SAIDA "/glossario.html"); return -1; }

    DOCTYPE;
    HTML("pt-BR") {
        CABECALHO_HEAD("Conceitos")
        BODY("") {
            render_navegacao(fp);
            MAIN() {
                VOLTAR("index.html", "← Página inicial");
                titulo_pagina(fp, "Glossário", "Conceitos",
                              "Consulte a explicação de cada conceito.");

                if (qc == 0) {
                    DIV("class='aviso'") {
                        _("Nenhum conceito foi encontrado. Coloque os arquivos TXT na pasta <code>conceitos</code> e execute o gerador novamente.");
                    }
                } else {
                    UL("class='lista-itens'") {
                        for (size_t i = 0; i < qc; i++) {
                            char arq[256];
                            arquivo_conceito(arq, sizeof(arq), &conceitos[i]);
                            LI("") {
                                _("<a href='%s'>", arq);
                                escrever_html_escapado(fp, titulo_conceito(&conceitos[i]));
                                _("</a>");
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

/* conceito_<slug>.html */
static int gerar_pagina_conceito(DadosConceito *c, DadosTXT *algoritmos, size_t qa)
{
    char arq[256], caminho[300];
    const char *titulo = titulo_conceito(c);
    size_t relacionados = 0;
    FILE *fp;

    arquivo_conceito(arq, sizeof(arq), c);
    snprintf(caminho, sizeof(caminho), DIR_SAIDA "/%s", arq);
    fp = fopen(caminho, "w");
    if (fp == NULL) { perror(caminho); return -1; }

    for (size_t i = 0; i < qa; i++) {
        if (algoritmo_tem_conceito(&algoritmos[i], titulo)) relacionados++;
    }

    DOCTYPE;
    HTML("pt-BR") {
        CABECALHO_HEAD(titulo)
        BODY("") {
            render_navegacao(fp);
            MAIN() {
                VOLTAR("glossario.html", "← Todos os conceitos");
                titulo_pagina(fp, "Glossário", titulo, NULL);

                SECTION() {
                    H2("") { _("Explicação"); }
                    DIV("class='solucao'") { escrever_texto_com_quebras(fp, c->explicacao); }
                }

                if (relacionados > 0) {
                    SECTION() {
                        H2("") { _("Algoritmos relacionados"); }
                        UL("class='lista-itens'") {
                            for (size_t i = 0; i < qa; i++) {
                                if (algoritmo_tem_conceito(&algoritmos[i], titulo)) {
                                    escrever_link_algoritmo(fp, &algoritmos[i]);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    fclose(fp);
    return 0;
}

int gerar_paginas_indices(DadosTXT *algoritmos, size_t qtd_algoritmos,
                          DadosConceito *conceitos, size_t qtd_conceitos)
{
    int erro = 0;

    for (int i = 0; i < TOTAL_CATEGORIAS && !erro; i++) {
        if (gerar_pagina_categoria(i, algoritmos, qtd_algoritmos, conceitos, qtd_conceitos) != 0) erro = 1;
    }
    for (size_t i = 0; i < qtd_algoritmos && !erro; i++) {
        if (gerar_pagina_algoritmo(&algoritmos[i], conceitos, qtd_conceitos) != 0) erro = 1;
    }
    if (!erro && gerar_glossario(conceitos, qtd_conceitos) != 0) erro = 1;
    for (size_t i = 0; i < qtd_conceitos && !erro; i++) {
        if (gerar_pagina_conceito(&conceitos[i], algoritmos, qtd_algoritmos) != 0) erro = 1;
    }
    return erro;
}