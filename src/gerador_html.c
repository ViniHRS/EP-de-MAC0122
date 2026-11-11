#include <stdio.h>
#include <stdlib.h>

#include "html_tags.h"

void style_css(FILE *fp)
{
    {
        CSS("body")
        {
            _("background-color: #f2f2f2");
        }
    }
}

void main_page_html(FILE *fp) 
{
    {
        DOCTYPE;
        HTML("en") 
        {
            HEAD() 
            {
                META("charset='utf-8'");
                META("name='viewport' "
                     "content='width=device-width, initial-scale=1'");
                TITLE("Main page");
                LINK("rel='stylesheet' href='style.css'");
                META("name='description' content='Description'");
                META("name='author' content='Author'");
                META("property='og:title' content='Title'");
            }
            BODY("") 
            {
                NAV()
                {
                    H1("") B("MAC0122");
                }
                HEADER()
                {
                    H2("style='color: blue'") 
                    {
                        _("Texto para virar h2");
                    }
                }
                MAIN()
                {
                    H2("")
                    {
                        _("Vou Escrever aqui");
                    }
                    P("Mais um pouco");
                }
            }
        }
    }
}


int main(void) {
    FILE *css_fp = fopen("../generated/style.css", "w");
    if (css_fp == NULL) {
        perror("Erro ao criar o style.css");
        return 1;
    }

    FILE *html_fp = fopen("../generated/main.html", "w");
    if (html_fp == NULL) {
        perror("Erro ao criar o main.html");
        fclose(css_fp);
        return 1;
    }

    
    style_css(css_fp);
    main_page_html(html_fp);

    fclose(css_fp);
    fclose(html_fp);

    printf("Ficheiros 'style.css' e 'main.html' gerados com sucesso diretamente no disco!\n");

    return 0;
}