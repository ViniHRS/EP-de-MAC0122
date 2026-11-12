# ***************************************************************
#
# Makefile para o projeto Gerador de Páginas HTML de MAC0122
#
# ***************************************************************

# Variáveis de diretório
SRCDIR = src
INCDIR = inc
OUTDIR = output
GENDIR = generated

# Nome do executável final:
EXEC = gerador_html

# Lista todos os ficheiros fonte e cabeçalhos
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))
HEADERS = $(wildcard $(INCDIR)/*.h)

# Compilador e Flags
RM = /bin/rm -f
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -O2 -lm -g -D_GNU_SOURCE -I$(INCDIR)

# ***************************************************************

# Declara alvos que não são ficheiros reais
.PHONY: all page run-page clean clean_all

# Alvo padrão executado ao digitar apenas "make"
all: page

# ***************************************************************
# Regra de Compilação de Ficheiros Objeto (.o)
# ***************************************************************
$(OUTDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(OUTDIR)
	@mkdir -p $(GENDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ***************************************************************
# Regra de Linkagem do Executável Final (Junta todos os .o)
# ***************************************************************
$(OUTDIR)/$(EXEC): $(OBJS)
	@mkdir -p $(GENDIR)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

# ***************************************************************
# Alvos de compilação e execução
# ***************************************************************

# make page: compila todo o projeto
page: $(OUTDIR)/$(EXEC)

# make run-page: compila (se necessário) e executa o gerador
run-page: page
	./$(OUTDIR)/$(EXEC)

# ***************************************************************
# Regras de Limpeza
# ***************************************************************

clean:
	# Limpa ficheiros temporários de editores
	$(RM) -r *~ \#* .\#* core a.out
	$(RM) -r $(SRCDIR)/*~ $(SRCDIR)/\#* $(SRCDIR)/.\#*
	$(RM) -r $(INCDIR)/*~ $(INCDIR)/\#* $(INCDIR)/.\#*
	$(RM) -rf *.dSYM
	# Limpa os ficheiros objeto (.o) e o executável gerados na pasta output
	$(RM) -f $(OUTDIR)/*.o $(OUTDIR)/$(EXEC)

clean-all: clean
	# Limpeza total (apaga as pastas output e generated)
	$(RM) -rf $(OUTDIR)
	$(RM) -rf $(GENDIR)