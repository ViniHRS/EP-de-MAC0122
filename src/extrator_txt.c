#include "extrator_txt.h"

char* limpar_string(char *str) {
    str[strcspn(str, "\r\n")] = '\0';
    return str;
}

void anexar_solucao(char **solucao_atual, const char *nova_linha) {
    if (*solucao_atual == NULL) {
        *solucao_atual = strdup(nova_linha);
    } else {
        size_t len_atual = strlen(*solucao_atual);
        size_t len_nova = strlen(nova_linha);
        
        char *temp = realloc(*solucao_atual, len_atual + len_nova + 2);
        if (temp != NULL) {
            *solucao_atual = temp;
            strcat(*solucao_atual, "\n");
            strcat(*solucao_atual, nova_linha);
        }
    }
}

void processar_linha_txt(const char *linha, DadosTXT *dados) {
    if (strncmp(linha, "1. Tecnica:", 11) == 0) {
        dados->tecnica = strdup(linha + 11);
    } 
    else if (strncmp(linha, "2. Algoritmo:", 13) == 0) {
        dados->algoritmo = strdup(linha + 13);
    } 
    else if (strncmp(linha, "3. Autores:", 11) == 0) {
        dados->autores = strdup(linha + 11);
    } 
    else if (strncmp(linha, "4. Data:", 8) == 0) {
        dados->data = strdup(linha + 8);
    } 
    else if (strncmp(linha, "5. Conceitos:", 13) == 0) {
        dados->conceitos = strdup(linha + 13);
    } 
    else if (strncmp(linha, "6. Problema:", 12) == 0) {
        dados->problema = strdup(linha + 12);
    }
    else {
        anexar_solucao(&(dados->solucao), linha);
    }
}