# ***************************************************************
#
# Makefile para o projeto Gerador de Páginas HTML de MAC0122
#
# ***************************************************************

#Variável de diretório
SRCDIR = src
INCDIR = inc
OUTDIR = output
GENDIR = generated

#Nome do executável final:
EXEC = gerador_html

#Busca todos os arquivos .c na pasta src e define os correspondentes .o na pasta output
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OUTDIR)/%.o, $(SRCS))

#Impede que o Make apague arquivos .o quando os trata como temporários
.PRECIOUS: $(OUTDIR)/%

#Coleta todos os arquivos .h da pasta inc automaticamente
HEADERS = $(wildcard $(INCDIR)/*.h)

#Compilador e Flags
RM = /bin/rm -f
CC = gcc
CFLAGS = -std=c99 -Wall -pedantic -O2 -lm -g -D_GNU_SOURCE -I$(INCDIR)

# ***************************************************************

#Declara alvos que não são arquivos reais
.PHONY: all page run-page clean clean_all

# Alvo padrão executado ao digitar apenas "make"
all: page

# ***************************************************************
# Regra de Compilação
# ***************************************************************
$(OUTDIR)/% : $(SRCDIR)/%.c $(HEADERS)
	@mkdir -p $(OUTDIR)
	@mkdir -p $(GENDIR)
	$(CC) $< -o $@ $(CFLAGS)

# ***************************************************************
# Alvos de compilação e execução
# ***************************************************************

# make page: compila o projeto
page: $(OUTDIR)/$(EXEC)

# make run-page: compila (se necessário) e executa
run-page: page
	./$(OUTDIR)/$(EXEC)

# ***************************************************************
# Regras de Limpeza
# ***************************************************************

clean:
	#Limpa arquivos temporários de editores
	$(RM) -r *~ \#* .\#* core a.out
	$(RM) -r $(SRCDIR)/*~ $(SRCDIR)/\#* $(SRCDIR)/.\#*
	$(RM) -r $(INCDIR)/*~ $(INCDIR)/\#* $(INCDIR)/.\#*
	$(RM) -rf *.dSYM
	# Limpa os arquivos objeto (.o) gerados na pasta output
	$(RM) -f $(OUTDIR)/*.o

clean_all: clean
		   #Limpeza total (apaga a pasta output)
	       $(RM) -rf $(OUTDIR)