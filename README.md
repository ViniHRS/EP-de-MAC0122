# EP-de-MAC0122

## 📄 Informações Gerais
- **Grupo:**  
  - Ian Gabriel Yamauti Fernandes  
  - Marcos Hideki Nagamine  
  - Vinícius Hiroshi Rodrigues Sakuma  
- **Disciplina:** MAC0122 - Princípios de Desenvolvimento de Algoritmos    
- **Professor:** Leônidas de Oliveira Brandão  
- **Curso:** Engenharia de Computação  
- **Instituição:** Escola Politécnica da Universidade de São Paulo

## ⚙️ Como compilar e rodar o gerador de HTML
Neste projeto, optou-se por utilizar o Makefile para facilitar a compilação e execução do programa, evitando-se, assim, digitar comandos extensos no terminal e outros erros que poderiam surgir ao digitar tudo manualmente.
- **Compilação do programa:** basta digitar `make gerador_html` e o arquivo executável gerado será direcionado a uma pasta chamada `output`. Caso essa pasta não exista, ela será criada durante o processo. Vale ressaltar que, neste repositório, o `.gitignore` impede o upload de arquivos de compilação (inclusive o da própria pasta `output`).
- **Execução do programa:** basta digitar `make run-gerador_html`. Nesta etapa, dois arquivos devem ser gerados (o `main.html` e o `style.css`) e direcionados a pasta `generated`.