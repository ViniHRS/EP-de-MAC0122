#include <stdio.h>
#include <stdlib.h>

#include "html_tags.h"

void style_css(FILE *fp)
{
    {
        CSS("main")
        {
            _("background-color: #e2e2e2");
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


int main(void) 
{
    char *css;
    char *html;
    size_t css_size;
    size_t html_size;
    FILE *css_fp;
    FILE *html_fp;
    css_fp = open_memstream(&css, &css_size);
    if (css_fp == NULL) 
    {
        return 1;
    }
    html_fp = open_memstream(&html, &html_size);
    if (html_fp == NULL) 
    {
        return 1;
    }
    style_css(css_fp);
    main_page_html(html_fp);
    fclose(css_fp);
    fclose(html_fp);
    FILE *css_f_out = fopen("../generated/style.css", "w");
    FILE *html_f_out = fopen("../generated/main.html", "w");
    if (css_f_out != NULL) {
        fwrite(css, 1, css_size, css_f_out);
        fclose(css_f_out);
        printf("Ficheiro 'style.css' gerado com sucesso (%lu bytes)!\n", css_size);
    } else {
        perror("Erro ao criar o style.css no disco");
    }
    if (html_f_out != NULL) {
        fwrite(html, 1, html_size, html_f_out);
        fclose(html_f_out);
        printf("Ficheiro 'main.html' gerado com sucesso (%lu bytes)!\n", html_size);
    } else {
        perror("Erro ao criar o main.html no disco");
    }
    printf("%s: %lu bytes\n", css, css_size);
    printf("%s: %lu bytes\n", html, html_size);
    free(css);
    free(html);
    return 0;
}
