#include "extrator_txt.h"

#include <ctype.h>
#include <dirent.h>
#include <strings.h>

static char *duplicar_aparado(const char *texto)
{
    const char *inicio = texto;
    size_t tamanho;
    char *copia;

    while (*inicio != '\0' && isspace((unsigned char)*inicio)) {
        inicio++;
    }

    tamanho = strlen(inicio);
    while (tamanho > 0 && isspace((unsigned char)inicio[tamanho - 1])) {
        tamanho--;
    }

    copia = malloc(tamanho + 1);
    if (copia == NULL) {
        return NULL;
    }

    memcpy(copia, inicio, tamanho);
    copia[tamanho] = '\0';
    return copia;
}

static void substituir_texto(char **destino, const char *origem)
{
    char *novo = duplicar_aparado(origem);
    if (novo == NULL) {
        return;
    }

    free(*destino);
    *destino = novo;
}

char *limpar_string(char *str)
{
    if (str != NULL) {
        str[strcspn(str, "\r\n")] = '\0';
    }
    return str;
}

void anexar_solucao(char **solucao_atual, const char *nova_linha)
{
    size_t tamanho_atual;
    size_t tamanho_novo;
    char *temporario;

    if (solucao_atual == NULL || nova_linha == NULL) {
        return;
    }

    if (*solucao_atual == NULL) {
        *solucao_atual = strdup(nova_linha);
        return;
    }

    tamanho_atual = strlen(*solucao_atual);
    tamanho_novo = strlen(nova_linha);
    temporario = realloc(*solucao_atual, tamanho_atual + tamanho_novo + 2);

    if (temporario != NULL) {
        *solucao_atual = temporario;
        (*solucao_atual)[tamanho_atual] = '\n';
        memcpy(*solucao_atual + tamanho_atual + 1, nova_linha, tamanho_novo + 1);
    }
}

static const char *inicio_linha(const char *linha)
{
    /* BOM UTF-8 (só aparece no começo do arquivo) */
    if ((unsigned char)linha[0] == 0xEF && linha[1] != '\0' && linha[2] != '\0' &&
        (unsigned char)linha[1] == 0xBB &&
        (unsigned char)linha[2] == 0xBF) {
        linha += 3;
    }

    while (*linha != '\0' && isspace((unsigned char)*linha)) {
        linha++;
    }

    return linha;
}

static int extrair_campo(const char *linha, const char *rotulo, const char **valor)
{
    size_t tamanho = strlen(rotulo);

    if (strncmp(linha, rotulo, tamanho) == 0) {
        *valor = linha + tamanho;
        return 1;
    }

    return 0;
}

void processar_linha_txt(const char *linha, DadosTXT *dados)
{
    const char *texto = inicio_linha(linha);
    const char *valor = NULL;

    if (extrair_campo(texto, "1. Técnica:", &valor) ||
        extrair_campo(texto, "1. Tecnica:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->tecnica, valor);
    }
    else if (extrair_campo(texto, "2. Algoritmo:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->algoritmo, valor);
    }
    else if (extrair_campo(texto, "3. Autores:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->autores, valor);
    }
    else if (extrair_campo(texto, "4. Data:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->data, valor);
    }
    else if (extrair_campo(texto, "5. Conceitos:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->conceitos, valor);
    }
    else if (extrair_campo(texto, "6. Problema:", &valor)) {
        dados->estado_leitura = 0;
        substituir_texto(&dados->problema, valor);
    }
    else if (extrair_campo(texto, "7. Explicação:", &valor) ||
             extrair_campo(texto, "7. Explicacao:", &valor) ||
             extrair_campo(texto, "7. Solução:", &valor) ||
             extrair_campo(texto, "7. Solucao:", &valor) ||
             extrair_campo(texto, "Explicação:", &valor) ||
             extrair_campo(texto, "Explicacao:", &valor) ||
             extrair_campo(texto, "Solução:", &valor) ||
             extrair_campo(texto, "Solucao:", &valor)) {
        dados->estado_leitura = 1;
        char *texto_aparado = duplicar_aparado(valor);
        if (texto_aparado != NULL) {
            anexar_solucao(&dados->solucao, texto_aparado);
            free(texto_aparado);
        }
    }
    else if (extrair_campo(texto, "8. Código:", &valor) || extrair_campo(texto, "8. Codigo:", &valor)) {
        dados->estado_leitura = 2; // Entramos no modo Código
        char *texto_aparado = duplicar_aparado(valor);
        if (texto_aparado != NULL) {
            anexar_solucao(&dados->codigo, texto_aparado);
            free(texto_aparado);
        }
    }
    else if (dados->estado_leitura == 2) {
        // Usamos a 'linha' original em vez do 'texto' para NÃO APAGAR a indentação do código
        anexar_solucao(&dados->codigo, limpar_string((char*)linha));
    }
    else if (dados->estado_leitura == 1 || dados->problema != NULL) {
        anexar_solucao(&dados->solucao, limpar_string((char *)texto));
    }
}

void processar_linha_conceito(const char *linha, DadosConceito *dados)
{
    const char *texto = inicio_linha(linha);
    const char *valor = NULL;

    if (extrair_campo(texto, "1. Nome do conceito:", &valor) ||
        extrair_campo(texto, "1. Nome do Conceito:", &valor) ||
        extrair_campo(texto, "1. Nome:", &valor)) {
        substituir_texto(&dados->nome, valor);
    }
    else if (extrair_campo(texto, "2. Explicação do conceito:", &valor) ||
             extrair_campo(texto, "2. Explicação do Conceito:", &valor) ||
             extrair_campo(texto, "2. Explicacao do conceito:", &valor) ||
             extrair_campo(texto, "2. Explicacao do Conceito:", &valor) ||
             extrair_campo(texto, "2. Explicação:", &valor) ||
             extrair_campo(texto, "2. Explicacao:", &valor)) {
        substituir_texto(&dados->explicacao, valor);
    }
    else if (dados->explicacao != NULL) {
        anexar_solucao(&dados->explicacao, limpar_string((char *)texto));
    }
}

static int eh_arquivo_txt(const char *nome)
{
    const char *extensao = strrchr(nome, '.');
    return extensao != NULL && strcasecmp(extensao, ".txt") == 0;
}

static int comparar_caminhos(const void *a, const void *b)
{
    const char *const *caminho_a = a;
    const char *const *caminho_b = b;
    return strcasecmp(*caminho_a, *caminho_b);
}

static int listar_arquivos_txt(const char *diretorio, char ***arquivos, size_t *quantidade)
{
    DIR *pasta = opendir(diretorio);
    struct dirent *entrada;
    char **lista = NULL;
    size_t total = 0;

    *arquivos = NULL;
    *quantidade = 0;

    if (pasta == NULL) {
        return -1;
    }

    while ((entrada = readdir(pasta)) != NULL) {
        char *caminho;
        char **temporario;
        size_t tamanho;

        if (entrada->d_name[0] == '.' || !eh_arquivo_txt(entrada->d_name)) {
            continue;
        }

        tamanho = strlen(diretorio) + strlen(entrada->d_name) + 2;
        caminho = malloc(tamanho);
        if (caminho == NULL) {
            closedir(pasta);
            for (size_t i = 0; i < total; i++) {
                free(lista[i]);
            }
            free(lista);
            return -1;
        }
        snprintf(caminho, tamanho, "%s/%s", diretorio, entrada->d_name);

        temporario = realloc(lista, (total + 1) * sizeof(*lista));
        if (temporario == NULL) {
            free(caminho);
            closedir(pasta);
            for (size_t i = 0; i < total; i++) {
                free(lista[i]);
            }
            free(lista);
            return -1;
        }

        lista = temporario;
        lista[total++] = caminho;
    }

    closedir(pasta);
    if (total > 1) {
        qsort(lista, total, sizeof(*lista), comparar_caminhos);
    }
    *arquivos = lista;
    *quantidade = total;
    return 0;
}

static char *nome_sem_extensao(const char *caminho)
{
    const char *nome = strrchr(caminho, '/');
    const char *ponto;
    size_t tamanho;
    char *resultado;

    nome = nome == NULL ? caminho : nome + 1;
    ponto = strrchr(nome, '.');
    tamanho = ponto == NULL ? strlen(nome) : (size_t)(ponto - nome);
    resultado = malloc(tamanho + 1);

    if (resultado != NULL) {
        memcpy(resultado, nome, tamanho);
        resultado[tamanho] = '\0';
    }

    return resultado;
}

static char *criar_slug(const char *texto)
{
    char *slug = malloc(strlen(texto) + 2);
    size_t i = 0;
    size_t j = 0;
    int separador = 0;

    if (slug == NULL) {
        return NULL;
    }

    while (texto[i] != '\0') {
        unsigned char caractere = (unsigned char)texto[i];
        char convertido = '\0';

        if (caractere < 128) {
            if (isalnum(caractere)) {
                convertido = (char)tolower(caractere);
            }
            i++;
        }
        else if (caractere == 0xC3 && texto[i + 1] != '\0') {
            unsigned char acento = (unsigned char)texto[i + 1];
            switch (acento) {
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84:
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4:
                    convertido = 'a'; break;
                case 0x87: case 0xA7:
                    convertido = 'c'; break;
                case 0x88: case 0x89: case 0x8A: case 0x8B:
                case 0xA8: case 0xA9: case 0xAA: case 0xAB:
                    convertido = 'e'; break;
                case 0x8C: case 0x8D: case 0x8E: case 0x8F:
                case 0xAC: case 0xAD: case 0xAE: case 0xAF:
                    convertido = 'i'; break;
                case 0x91: case 0xB1:
                    convertido = 'n'; break;
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96:
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6:
                    convertido = 'o'; break;
                case 0x99: case 0x9A: case 0x9B: case 0x9C:
                case 0xB9: case 0xBA: case 0xBB: case 0xBC:
                    convertido = 'u'; break;
                default: break;
            }
            i += 2;
        }
        else {
            i++;
        }

        if (convertido != '\0') {
            if (separador && j > 0) {
                slug[j++] = '-';
            }
            slug[j++] = convertido;
            separador = 0;
        }
        else {
            separador = 1;
        }
    }

    while (j > 0 && slug[j - 1] == '-') {
        j--;
    }
    slug[j] = '\0';

    if (j == 0) {
        strcpy(slug, "item");
    }

    return slug;
}

/* Verifica se o slug já foi usado por um item anterior (algoritmo ou conceito). */
static int slug_repetido(char *const *slugs, size_t quantidade, const char *slug)
{
    for (size_t i = 0; i < quantidade; i++) {
        if (slugs[i] != NULL && strcmp(slugs[i], slug) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Acrescenta -2, -3, ... até o slug ficar único. */
static void tornar_slug_unico(char **slug, char *const *anteriores, size_t quantidade)
{
    char *base;
    int sufixo = 2;

    if (*slug == NULL || !slug_repetido(anteriores, quantidade, *slug)) {
        return;
    }

    base = strdup(*slug);
    if (base == NULL) {
        return;
    }

    while (slug_repetido(anteriores, quantidade, *slug)) {
        size_t tamanho = strlen(base) + 24;
        char *novo = malloc(tamanho);
        if (novo == NULL) {
            break;
        }
        snprintf(novo, tamanho, "%s-%d", base, sufixo++);
        free(*slug);
        *slug = novo;
    }

    free(base);
}

static void liberar_lista(char **arquivos, size_t inicio, size_t total)
{
    for (size_t j = inicio; j < total; j++) {
        free(arquivos[j]);
    }
    free(arquivos);
}

int carregar_algoritmos(const char *diretorio, DadosTXT **algoritmos, size_t *quantidade)
{
    char **arquivos;
    size_t total_arquivos;

    *algoritmos = NULL;
    *quantidade = 0;

    if (listar_arquivos_txt(diretorio, &arquivos, &total_arquivos) != 0) {
        return -1;
    }

    for (size_t i = 0; i < total_arquivos; i++) {
        DadosTXT *temporario;
        FILE *arquivo;
        char *linha = NULL;
        size_t capacidade = 0;
        DadosTXT novo = {0};
        char **slugs;

        arquivo = fopen(arquivos[i], "r");
        if (arquivo == NULL) {
            perror(arquivos[i]);
            free(arquivos[i]);
            continue;
        }

        while (getline(&linha, &capacidade, arquivo) != -1) {
            processar_linha_txt(limpar_string(linha), &novo);
        }
        free(linha);
        fclose(arquivo);

        novo.arquivo = nome_sem_extensao(arquivos[i]);
        novo.slug = criar_slug(novo.algoritmo != NULL && novo.algoritmo[0] != '\0'
                                  ? novo.algoritmo
                                  : (novo.arquivo == NULL ? "algoritmo" : novo.arquivo));

        /* checagem de slug único usando os slugs já carregados */
        slugs = malloc((*quantidade + 1) * sizeof(*slugs));
        if (slugs != NULL) {
            for (size_t k = 0; k < *quantidade; k++) {
                slugs[k] = (*algoritmos)[k].slug;
            }
            tornar_slug_unico(&novo.slug, slugs, *quantidade);
            free(slugs);
        }

        temporario = realloc(*algoritmos, (*quantidade + 1) * sizeof(**algoritmos));
        if (temporario == NULL) {
            free(novo.tecnica); free(novo.algoritmo); free(novo.autores);
            free(novo.data); free(novo.conceitos); free(novo.problema);
            free(novo.solucao); free(novo.arquivo); free(novo.slug);
            liberar_lista(arquivos, i, total_arquivos);
            liberar_algoritmos(*algoritmos, *quantidade);
            *algoritmos = NULL;
            *quantidade = 0;
            return -1;
        }
        *algoritmos = temporario;
        (*algoritmos)[(*quantidade)++] = novo;
        free(arquivos[i]);
    }

    free(arquivos);
    return 0;
}

int carregar_conceitos(const char *diretorio, DadosConceito **conceitos, size_t *quantidade)
{
    char **arquivos;
    size_t total_arquivos;

    *conceitos = NULL;
    *quantidade = 0;

    if (listar_arquivos_txt(diretorio, &arquivos, &total_arquivos) != 0) {
        return -1;
    }

    for (size_t i = 0; i < total_arquivos; i++) {
        DadosConceito *temporario;
        FILE *arquivo;
        char *linha = NULL;
        size_t capacidade = 0;
        DadosConceito novo = {0};
        char **slugs;

        arquivo = fopen(arquivos[i], "r");
        if (arquivo == NULL) {
            perror(arquivos[i]);
            free(arquivos[i]);
            continue;
        }

        while (getline(&linha, &capacidade, arquivo) != -1) {
            processar_linha_conceito(limpar_string(linha), &novo);
        }
        free(linha);
        fclose(arquivo);

        novo.arquivo = nome_sem_extensao(arquivos[i]);
        novo.slug = criar_slug(novo.nome != NULL && novo.nome[0] != '\0'
                                  ? novo.nome
                                  : (novo.arquivo == NULL ? "conceito" : novo.arquivo));

        slugs = malloc((*quantidade + 1) * sizeof(*slugs));
        if (slugs != NULL) {
            for (size_t k = 0; k < *quantidade; k++) {
                slugs[k] = (*conceitos)[k].slug;
            }
            tornar_slug_unico(&novo.slug, slugs, *quantidade);
            free(slugs);
        }

        temporario = realloc(*conceitos, (*quantidade + 1) * sizeof(**conceitos));
        if (temporario == NULL) {
            free(novo.nome); free(novo.explicacao); free(novo.arquivo); free(novo.slug);
            liberar_lista(arquivos, i, total_arquivos);
            liberar_conceitos(*conceitos, *quantidade);
            *conceitos = NULL;
            *quantidade = 0;
            return -1;
        }
        *conceitos = temporario;
        (*conceitos)[(*quantidade)++] = novo;
        free(arquivos[i]);
    }

    free(arquivos);
    return 0;
}

void liberar_algoritmos(DadosTXT *algoritmos, size_t quantidade)
{
    for (size_t i = 0; i < quantidade; i++) {
        free(algoritmos[i].tecnica);
        free(algoritmos[i].algoritmo);
        free(algoritmos[i].autores);
        free(algoritmos[i].data);
        free(algoritmos[i].conceitos);
        free(algoritmos[i].problema);
        free(algoritmos[i].solucao);
        free(algoritmos[i].codigo);
        free(algoritmos[i].arquivo);
        free(algoritmos[i].slug);
    }
    free(algoritmos);
}

void liberar_conceitos(DadosConceito *conceitos, size_t quantidade)
{
    for (size_t i = 0; i < quantidade; i++) {
        free(conceitos[i].nome);
        free(conceitos[i].explicacao);
        free(conceitos[i].arquivo);
        free(conceitos[i].slug);
    }
    free(conceitos);
}
