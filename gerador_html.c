#include <stdio.h>
#include <stdlib.h>

#include "html_tags.h"

void main_page_html(FILE *fp) 
{
    {
        DOCTYPE;
        HTML("pt-BR") 
        {
            HEAD() 
            {
                META("charset='utf-8'");
                META("name='viewport' "
                     "content='width=device-width, initial-scale=1'");
                TITLE("Main page");
                META("name='description' content='Description'");
                META("name='author' content='Author'");
                META("property='og:title' content='Title'");
            }
            BODY("") 
            {
                NAV()
                {
                    H1("style='color: blue'") _B() _("MAC0122") _B() _H1;
                }
                HEADER()
                {
                    H2("style='color: blue'")  _("") _H2;
                }
            }
        }
    }
}


int main() 
{
    char *html;
    size_t html_size;
    FILE *fp;
    fp = open_memstream(&html, &html_size);
    if (fp == NULL) 
    {
        return 1;
    }
    main_page_html(fp);
    fclose(fp);
    printf("%s\n", html);
    printf("%lu bytes\n", html_size);
    free(html);
    return 0;
}
