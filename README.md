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

## 🎯 O que o programa faz
O programa lê arquivos `.txt` com a descrição de algoritmos e de conceitos e gera um site estático em HTML para consultá-los. O catálogo pode ser navegado por seis categorias (Técnica, Algoritmo, Autores, Data, Conceitos e Problema) e também por um glossário de conceitos. Quando um algoritmo cita um conceito que possui explicação cadastrada, o nome do conceito vira um link para a página dele.

## ⚙️ Como compilar e rodar o gerador de HTML
Neste projeto, optou-se por utilizar o Makefile para facilitar a compilação e execução do programa, evitando-se, assim, digitar comandos extensos no terminal e outros erros que poderiam surgir ao digitar tudo manualmente.
- **Compilação do programa:** basta digitar `make page` e o arquivo executável gerado será direcionado a uma pasta chamada `output`. Caso essa pasta não exista, ela será criada durante o processo. Vale ressaltar que, neste repositório, o `.gitignore` impede o upload de arquivos de compilação (inclusive o da própria pasta `output`).
- **Execução do programa:** basta digitar `make run-page`. Nesta etapa, o site completo é gerado e direcionado à pasta `generated`. Para ver o resultado, abra o arquivo `generated/index.html` no navegador.
- **Pastas de entrada:** por padrão, o programa lê os algoritmos da pasta `algoritmos/` e os conceitos da pasta `conceitos/`, ambas na raiz do projeto. Também é possível indicar outras pastas ao executar diretamente o binário: `./output/gerador_html pasta_algoritmos pasta_conceitos`.

## 📁 Estrutura do projeto
```
.
├── algoritmos/      # arquivos .txt, um por algoritmo (entrada)
├── conceitos/       # arquivos .txt, um por conceito (entrada)
├── inc/             # cabeçalhos (.h)
├── src/             # código-fonte (.c)
├── output/          # objetos e executável (não vai para o repositório)
├── generated/       # site gerado (saída)
├── Makefile
└── .gitignore
```

| Arquivo | Responsabilidade |
|---|---|
| `src/main.c` | Lê as pastas de entrada, limpa as páginas antigas e chama os geradores |
| `src/extrator_txt.c` | Lê e interpreta os `.txt` (algoritmos e conceitos) |
| `src/gerador_html.c` | Gera o `style.css`, a barra de navegação e a página inicial |
| `src/gerador_indices.c` | Gera as páginas de categorias, tags, algoritmos, glossário e conceitos |
| `inc/html_tags.h` | Macros que facilitam a escrita das tags HTML |
| `inc/extrator_txt.h` | Estruturas de dados e funções de leitura dos `.txt` |
| `inc/indexador.h` | Declarações compartilhadas entre os geradores |

## 📝 Formato dos arquivos de entrada
**Algoritmos** (`algoritmos/*.txt`):
```
1. Técnica: Divisão e conquista, Recursão
2. Algoritmo: Merge Sort
3. Autores: John von Neumann
4. Data: 1945
5. Conceitos: Recursão, Estabilidade
6. Problema: Ordenar um vetor.
7. Explicação: Divide o vetor ao meio.
Depois intercala as metades.
```
- Técnicas e conceitos podem ser vários, separados por vírgula ou ponto e vírgula.
- Depois do campo `6. Problema:`, as linhas seguintes fazem parte da explicação, e o rótulo `7. Explicação:` é opcional.
- Os campos aceitam o rótulo com ou sem acento (por exemplo, `Técnica` e `Tecnica`).

**Conceitos** (`conceitos/*.txt`):
```
1. Nome do conceito: Recursão
2. Explicação do conceito: Função que chama a si mesma.
Precisa de um caso base.
```
- As linhas seguintes ao campo 2 continuam a explicação.
- Para o conceito ser linkado em um algoritmo, o nome precisa ser equivalente ao citado no campo `5. Conceitos:`. A comparação ignora acentos e maiúsculas e aceita o plural ("Recursão" e "Recursões" são tratados como o mesmo conceito).

## 🌐 Site gerado
Todas as páginas ficam na pasta `generated/`, em um mesmo nível:

| Arquivo | Conteúdo |
|---|---|
| `index.html` | Página inicial, com a lista de categorias e o acesso ao glossário |
| `style.css` | Estilo compartilhado por todas as páginas |
| `tecnica.html`, `algoritmo.html`, `autores.html`, `data.html`, `conceitos.html`, `problema.html` | Uma página por categoria, listando os termos |
| `tag_<categoria>_<termo>.html` | Algoritmos associados a um termo (por exemplo, uma técnica ou um autor) |
| `alg_<nome>.html` | Página de cada algoritmo, com técnica, autores, data, conceitos, problema e explicação |
| `glossario.html` | Lista de todos os conceitos |
| `conceito_<nome>.html` | Explicação de um conceito e os algoritmos que o utilizam |

Todas as páginas têm uma barra de navegação no topo, com links para a página inicial, para as **Técnicas** e para os **Conceitos**. A cada execução, os `.html` antigos de `generated/` são removidos, para não sobrar páginas de itens que já foram apagados.
