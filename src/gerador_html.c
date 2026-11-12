#include <stdio.h>
#include <stdlib.h>
#include "html_tags.h"

void style_css(FILE *fp)
{
    fputs("body { margin: 0; padding: 40px 20px; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; background-color: #f4f6f9; display: flex; justify-content: center; align-items: flex-start; min-height: 100vh; box-sizing: border-box; }\n", fp);
    fputs("main { background-color: #ffffff; width: 100%; max-width: 650px; padding: 35px 40px; border-radius: 8px; box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08); border: 1px solid #e1e4e8; box-sizing: border-box; }\n", fp);

    CSS("main-title")
    {
        fputs("margin-top: 0; color: #1a252f; font-size: 1.8rem; border-bottom: 2px solid #0f6cbf; padding-bottom: 10px;", fp);
    }

    CSS("main-subtitle")
    {
        fputs("color: #555; font-size: 1.1rem; margin-bottom: 25px; font-weight: 500;", fp);
    }

    fputs(".nav-tree, .sub-tree { list-style: none; padding-left: 24px; margin: 0; }\n", fp);
    fputs(".nav-tree { padding-left: 0; }\n", fp);
    fputs(".nav-tree li { margin: 10px 0; font-size: 16px; line-height: 1.5; }\n", fp);
    fputs(".nav-tree a { color: #0f6cbf; text-decoration: none; font-weight: 500; }\n", fp);
    fputs(".nav-tree a:hover { color: #004085; text-decoration: underline; }\n", fp);
    fputs(".contains-branch > span { font-weight: bold; color: #2c3e50; display: inline-block; margin-bottom: 8px; }\n", fp);

    CSS("nav-back")
    {
        fputs("margin: 15px 0; font-size: 14px;", fp);
    }

    fputs(".nav-back a { color: #0f6cbf; text-decoration: none; font-weight: 600; padding: 6px 12px; background-color: #e9ecef; border-radius: 4px; display: inline-block; transition: background-color 0.2s; }\n", fp);
    fputs(".nav-back a:hover { background-color: #0f6cbf; color: #ffffff; text-decoration: none; }\n", fp);
    fputs(".nav-back.bottom { margin-top: 30px; border-top: 1px solid #e1e4e8; padding-top: 15px; }\n", fp);
}

void render_nav_tree(FILE *fp) 
{
    NAV("class='full-tree-nav'") 
    {
        UL("class='nav-tree'") 
        {
            LI("class='tree-node'") 
            {
                A("index.html", "Início");
            }

            LI("class='tree-node contains-branch'") 
            {
                SPAN() { _("Índices do EP (MAC0122)"); }
                
                UL("class='sub-tree'") 
                {
                    LI("") { A("tecnica.html", "1. Técnica"); }
                    LI("") { A("algoritmo.html", "2. Algoritmo"); }
                    LI("") { A("autores.html", "3. Autores"); }
                    LI("") { A("data.html", "4. Data"); }
                    LI("") { A("conceitos.html", "5. Conceitos"); }
                    LI("") { A("problema.html", "6. Problema"); }
                }
            }
        }
    }
}

void main_page_html(FILE *fp) 
{
    DOCTYPE;
    HTML("en") 
    {
        HEAD() 
        {
            META("charset='utf-8'");
            META("name='viewport' content='width=device-width, initial-scale=1'");
            TITLE("MAC0122 - Índice Principal");
            LINK("rel='stylesheet' href='style.css'");
            META("name='description' content='Índice de navegação do EP'");
        }
        BODY("") 
        {
            MAIN()
            {
                H1("class='main-title'") B("MAC0122");
                H2("class='main-subtitle'") 
                {
                    _("Selecione uma categoria abaixo para navegar pelos algoritmos e conteúdos:");
                }
                render_nav_tree(fp);
            }
        }
    }
}