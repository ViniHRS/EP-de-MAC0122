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

int main() {
    FILE *ficheiro = fopen("BFS.txt", "r");
    if (!ficheiro) {
        printf("Erro ao abrir o ficheiro de entrada.\n");
        return 1;
    }

    DadosTXT meu_ep = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    char buffer[512];

    // Lê o ficheiro linha a linha
    while (fgets(buffer, sizeof(buffer), ficheiro) != NULL) {
        limpar_string(buffer);
        processar_linha_txt(buffer, &meu_ep);
    }

    fclose(ficheiro);

    // Exibindo os dados extraídos para confirmação
    printf("--- Dados Extraídos do EP ---\n");
    printf("Técnica:   %s\n", meu_ep.tecnica ? meu_ep.tecnica : "N/D");
    printf("Algoritmo: %s\n", meu_ep.algoritmo ? meu_ep.algoritmo : "N/D");
    printf("Autores:   %s\n", meu_ep.autores ? meu_ep.autores : "N/D");
    printf("Data:      %s\n", meu_ep.data ? meu_ep.data : "N/D");
    printf("Conceitos: %s\n", meu_ep.conceitos ? meu_ep.conceitos : "N/D");
    printf("Problema:  %s\n", meu_ep.problema ? meu_ep.problema : "N/D");
    printf("Solução:   \n%s\n", meu_ep.solucao ? meu_ep.solucao : "N/D");

    // Libertar a memória alocada pelo strdup
    free(meu_ep.tecnica);
    free(meu_ep.algoritmo);
    free(meu_ep.autores);
    free(meu_ep.data);
    free(meu_ep.conceitos);
    free(meu_ep.problema);
    free(meu_ep.solucao);

    return 0;
}