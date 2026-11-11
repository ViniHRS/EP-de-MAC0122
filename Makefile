# ***************************************************************
#
# Makefile genérico para compilar programas em C
# usando o compilador gcc
#
# ***************************************************************

# CFLAGS = -Wall -ansi -pedantic -O2 -lm
# CFLAGS = -Wall -std=c99 -pedantic -O2 -lm
# CFLAGS = -Wall -std=c99 -pedantic -O2 -lm -fsanitize=address,undefined
# CFLAGS = -std=c99 -g -Wall -pedantic -O2 -lm -fsanitize=address
# CFLAGS = -std=c99 -g -Wall -pedantic -lm -fsanitize=address
# CFLAGS = -std=c99 -g -Wall -pedantic -O2 -lm 
CFLAGS = -std=c99 -Wall -pedantic -O2 -lm -g -D_GNU_SOURCE

RM = /bin/rm -f
CC = gcc
# CC = clang

#Variável para o diretório dos arquivos fonte
SRCDIR = src
#Variável para os arquivos gerados na compilação
OUTDIR = output

# Impede que o Make apague os executáveis automaticamente (arquivos intermediários)
.PRECIOUS: $(OUTDIR)/%

# ***************************************************************

#Regra principal: O executável em output/ depende do .c em src/
$(OUTDIR)/% : $(SRCDIR)/%.c
	@mkdir -p $(OUTDIR)
	$(CC) $< -o $@ $(CFLAGS)

#Regra de atalho: Permite digitar apenas "make programa" em vez de "make output/programa"
% : $(OUTDIR)/%
	@
#Apenas força a dependência, não executa nada extra

# ***************************************************************

# Declara que clean não é um arquivo
.PHONY: clean clean_all

clean:
	$(RM) -r *~ \#* .\#* *.o core a.out
	$(RM) -r $(SRCDIR)/*~ $(SRCDIR)/\#* $(SRCDIR)/.\#* $(SRCDIR)/*.o
	$(RM) -rf *.dSYM

#Limpeza total: roda o "clean" e depois apaga a pasta output
clean_all: clean
	       $(RM) -rf $(OUTDIR)

# ***************************************************************

# Executa qualquer programa compilado
run-%: $(OUTDIR)/%
	./$<