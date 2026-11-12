#include <stdio.h>
#include <stdlib.h>
#include "indexador.h"
#include "html_tags.h"   /* sempre por último: define macros curtas (A, B, I, P...) */

const Categoria categorias[TOTAL_CATEGORIAS] = {
    {"tecnica.html",   "tecnica",   "Técnica",   "Técnicas utilizadas pelos algoritmos."},
    {"algoritmo.html", "algoritmo", "Algoritmo", "Lista dos algoritmos cadastrados."},
    {"autores.html",   "autores",   "Autores",   "Autores associados aos algoritmos."},
    {"data.html",      "data",      "Data",      "Datas de criação ou publicação dos algoritmos."},
    {"conceitos.html", "conceitos", "Conceitos", "Conceitos associados a cada algoritmo."},
    {"problema.html",  "problema",  "Problema",  "Problemas resolvidos pelos algoritmos."}
};

void escrever_html_escapado(FILE *fp, const char *texto)
{
    if (texto == NULL) return;
    for (; *texto != '\0'; texto++) {
        switch (*texto) {
            case '&':  fputs("&amp;", fp);  break;
            case '<':  fputs("&lt;", fp);   break;
            case '>':  fputs("&gt;", fp);   break;
            case '"':  fputs("&quot;", fp); break;
            case '\'': fputs("&#39;", fp);  break;
            default:   fputc(*texto, fp);   break;
        }
    }
}

void escrever_texto_com_quebras(FILE *fp, const char *texto)
{
    if (texto == NULL || texto[0] == '\0') {
        fputs("<span class='nao-informado'>Não informado.</span>", fp);
        return;
    }
    for (; *texto != '\0'; texto++) {
        if (*texto == '\n') {
            BR();
            fputc('\n', fp);
        } else {
            char um[2] = {*texto, '\0'};
            escrever_html_escapado(fp, um);
        }
    }
}

/* ------------------------------------------------------------------ */
/* CSS: sem cards; barra escura no topo, conteúdo direto no fundo claro */
/* ------------------------------------------------------------------ */

void style_css(FILE *fp)
{
    fputs(":root { --fundo: #f4f6f9; --texto: #202938; --secundario: #667085; --azul: #2457a7; --borda: #dde3ec; --nav: #17243a; }\n", fp);
    fputs("* { box-sizing: border-box; }\n", fp);
    fputs("body { margin: 0; background: var(--fundo); color: var(--texto); font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; line-height: 1.6; }\n", fp);
    fputs("a { color: var(--azul); text-decoration: none; font-weight: 500; }\n", fp);
    fputs("a:hover { text-decoration: underline; }\n", fp);

    /* barra de navegação */
    CSS(".navegacao")
    {
        fputs("min-height: 64px; padding: 0 max(5vw, calc((100% - 1100px) / 2)); display: flex; align-items: center; justify-content: space-between; gap: 24px; background: var(--nav);", fp);
    }
    fputs("\n", fp);
    fputs(".navegacao a { color: #ffffff; }\n", fp);
    fputs(".navegacao .marca { font-size: 1.15rem; font-weight: 700; letter-spacing: .04em; }\n", fp);
    fputs(".links-nav { display: flex; gap: 22px; font-size: .92rem; }\n", fp);

    /* corpo */
    fputs("main { width: min(1100px, 90%); margin: 0 auto; padding: 38px 0 72px; }\n", fp);
    fputs(".sobretitulo { margin: 0 0 8px; color: var(--azul); font-size: .74rem; font-weight: 700; letter-spacing: .16em; text-transform: uppercase; }\n", fp);

    /* página inicial */
    fputs(".hero { padding: 40px 0 36px; }\n", fp);
    fputs(".hero h1 { margin: 4px 0 0; font-size: clamp(2.8rem, 7vw, 5rem); line-height: 1.05; letter-spacing: -.05em; }\n", fp);
    fputs(".hero p:not(.sobretitulo) { margin: 8px 0; color: var(--secundario); font-size: 1.15rem; }\n", fp);
    fputs(".hero .introducao { max-width: 650px; margin-top: 24px; font-size: 1rem; }\n", fp);
    fputs("main > section { margin-top: 36px; }\n", fp);
    fputs(".cabecalho-secao { display: flex; align-items: flex-end; justify-content: space-between; margin-bottom: 8px; }\n", fp);
    fputs("main > section h2 { margin: 0 0 6px; font-size: 1.5rem; line-height: 1.25; letter-spacing: -.01em; }\n", fp);
    fputs(".contador { color: var(--secundario); font-size: .9rem; }\n", fp);
    fputs("section > p { color: var(--secundario); margin: 4px 0 10px; }\n", fp);

    /* lista de categorias (sem cartões, só linhas divisórias) */
    fputs(".lista-categorias { list-style: none; margin: 0; padding: 0; border-bottom: 1px solid var(--borda); }\n", fp);
    fputs(".lista-categorias li { display: grid; grid-template-columns: 2.4rem 1fr; gap: 6px; padding: 16px 0; border-top: 1px solid var(--borda); }\n", fp);
    fputs(".lista-categorias .numero { color: var(--azul); font-size: .8rem; font-weight: 700; padding-top: 4px; }\n", fp);
    fputs(".lista-categorias a { font-size: 1.15rem; font-weight: 700; color: var(--texto); }\n", fp);
    fputs(".lista-categorias a:hover { color: var(--azul); }\n", fp);
    fputs(".lista-categorias p { margin: 2px 0 0; color: var(--secundario); font-size: .92rem; }\n", fp);

    /* páginas internas */
    fputs(".voltar { display: inline-block; margin-bottom: 22px; font-size: .92rem; }\n", fp);
    fputs(".titulo-pagina { margin-bottom: 28px; }\n", fp);
    fputs(".titulo-pagina h1 { margin: 0; font-size: clamp(2rem, 5vw, 3.3rem); line-height: 1.15; letter-spacing: -.035em; }\n", fp);
    fputs(".titulo-pagina p:not(.sobretitulo) { margin: 8px 0 0; color: var(--secundario); }\n", fp);
    fputs(".lista-itens { list-style: none; margin: 0; padding: 0; border-bottom: 1px solid var(--borda); }\n", fp);
    fputs(".lista-itens li { padding: 12px 0; border-top: 1px solid var(--borda); }\n", fp);

    fputs(".metadados { display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: 0 32px; margin: 0 0 8px; }\n", fp);
    fputs(".metadado { padding: 14px 0; border-top: 1px solid var(--borda); }\n", fp);
    fputs(".metadado dt { margin-bottom: 2px; color: var(--secundario); font-size: .74rem; font-weight: 700; letter-spacing: .12em; text-transform: uppercase; }\n", fp);
    fputs(".metadado dd { margin: 0; }\n", fp);
    fputs(".solucao { overflow-wrap: anywhere; }\n", fp);
    fputs("main > section > p { color: var(--texto); }\n", fp);
    fputs(".nao-informado { color: var(--secundario); font-style: italic; }\n", fp);
    fputs(".aviso { padding: 14px 18px; border-left: 3px solid #d9b44a; background: #fff9e8; color: #684f15; }\n", fp);
    fputs("code { padding: 2px 5px; border-radius: 4px; background: #e9edf3; }\n", fp);
    fputs("@media (max-width: 760px) { .metadados { grid-template-columns: 1fr; } }\n", fp);
    fputs("@media (max-width: 520px) { .navegacao { padding: 0 5%; } .links-nav { gap: 14px; } .hero { padding-top: 24px; } }\n", fp);
}

/* ------------------------------------------------------------------ */
/* Barra de navegação (presente em todas as páginas)                   */
/* ------------------------------------------------------------------ */

void render_navegacao(FILE *fp)
{
    NAV("class='navegacao'")
    {
        A("index.html", "MAC0122");
        DIV("class='links-nav'")
        {
            A("tecnica.html", "Técnicas");
            A("glossario.html", "Conceitos");
        }
    }
    fputc('\n', fp);
}

/* ------------------------------------------------------------------ */
/* Página inicial                                                      */
/* ------------------------------------------------------------------ */

void main_page_html(FILE *fp, size_t qtd_algoritmos, size_t qtd_conceitos)
{
    DOCTYPE;
    HTML("pt-BR")
    {
        HEAD()
        {
            META("charset='utf-8'");
            META("name='viewport' content='width=device-width, initial-scale=1'");
            TITLE("MAC0122 - Índice Principal");
            LINK("rel='stylesheet' href='style.css'");
            META("name='description' content='Catálogo de algoritmos e conceitos de MAC0122'");
        }
        BODY("")
        {
            render_navegacao(fp);
            MAIN()
            {
                DIV("class='hero'")
                {
                    _("<p class='sobretitulo'>Escola Politécnica · USP</p>");
                    H1("") { _("MAC0122"); }
                    _("<p>Princípios de Desenvolvimento de Algoritmos</p>");
                    _("<p class='introducao'>Explore os algoritmos por diferentes categorias ou consulte o glossário de conceitos.</p>");
                }

                SECTION()
                {
                    DIV("class='cabecalho-secao'")
                    {
                        DIV("")
                        {
                            _("<p class='sobretitulo'>Navegação</p>");
                            H2("") { _("Categorias"); }
                        }
                        _("<span class='contador'>%zu algoritmos</span>", qtd_algoritmos);
                    }

                    UL("class='lista-categorias'")
                    {
                        for (int i = 0; i < TOTAL_CATEGORIAS; i++) {
                            LI("")
                            {
                                _("<span class='numero'>%d</span>", i + 1);
                                DIV("")
                                {
                                    A(categorias[i].arquivo, categorias[i].titulo);
                                    _("<p>%s</p>", categorias[i].descricao);
                                }
                            }
                        }
                    }
                }

                SECTION()
                {
                    _("<p class='sobretitulo'>Glossário</p>");
                    H2("") { _("Conceitos"); }
                    _("<p>Veja as definições dos conceitos utilizados nos algoritmos.</p>");
                    A("glossario.html", "Consultar conceitos ↗");
                    _("<p class='contador'>%zu conceitos cadastrados</p>", qtd_conceitos);
                }
            }
        }
    }
}