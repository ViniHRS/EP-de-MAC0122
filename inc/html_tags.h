#ifndef HTML_TAGS_H_
#define HTML_TAGS_H_

#define SCOPE(atStart, atEnd) for (int _scope_break = ((atStart), 1); _scope_break; _scope_break = ((atEnd), 0))
#define _(...) fprintf(fp, __VA_ARGS__)

// HTML Document
#define DOCTYPE fputs("<!DOCTYPE html>", fp)
#define HTML(lang) SCOPE(fprintf(fp, "<html lang='%s'>", lang), fputs("</html>", fp))
#define HEAD() SCOPE(fputs("<head>", fp), fputs("</head>", fp))
#define BODY(attributes) SCOPE(fprintf(fp, "<body %s>", attributes), fputs("</body>", fp))

// Head
#define TITLE(text) fprintf(fp, "<title>%s</title>", text)
#define META(attributes) fprintf(fp, "<meta %s>", attributes)
#define BASE(attributes) fprintf(fp, "<base %s>", attributes)
#define LINK(attributes) fprintf(fp, "<link %s>", attributes)

// Webpage Layout
#define HEADER() SCOPE(fputs("<header>", fp), fputs("</header>", fp))
#define NAV() SCOPE(fputs("<nav>", fp), fputs("</nav>", fp))
#define MAIN() SCOPE(fputs("<main>", fp), fputs("</main>", fp))
#define SECTION() SCOPE(fputs("<section>", fp), fputs("</section>", fp))
#define ASIDE() SCOPE(fputs("<aside>", fp), fputs("</aside>", fp))
#define FOOTER() SCOPE(fputs("<footer>", fp), fputs("</footer>", fp))

// Content Formating
#define BR() fputs("<br>", fp)
#define HR() fputs("<hr>", fp)
#define DIV(attributes) SCOPE(fprintf(fp, "<div %s>", attributes), fputs("</div>", fp))
#define SPAN() SCOPE(fputs("<span>", fp), fputs("</span>", fp))

// Text
#define H1(attributes) SCOPE(fprintf(fp, "<h1 %s>", attributes), fputs("</h1>", fp))
#define H2(attributes) SCOPE(fprintf(fp, "<h2 %s>", attributes), fputs("</h2>", fp))
#define H3(attributes) SCOPE(fprintf(fp, "<h3 %s>", attributes), fputs("</h3>", fp))
#define H4(attributes) SCOPE(fprintf(fp, "<h4 %s>", attributes), fputs("</h4>", fp))
#define H5(attributes) SCOPE(fprintf(fp, "<h5 %s>", attributes), fputs("</h5>", fp))
#define H6(attributes) SCOPE(fprintf(fp, "<h6 %s>", attributes), fputs("</h6>", fp))
#define P(content) fprintf(fp, "<p>%s</p>", content)

// Text - Font
#define B(text) fprintf(fp, "<b>%s</b>", text)
#define I(text) fprintf(fp, "<i>%s</i>", text) 
#define U(text) fprintf(fp, "<u>%s</u>", text) 
#define S(text) fprintf(fp, "<s>%s</s>", text) 
#define TT(text) fprintf(fp, "<tt>%s</tt>", text)
#define SMALL(text) fprintf(fp, "<small>%s</small>", text) 
#define SUP(text) fprintf(fp, "<sup>%s</sup>", text) 
#define SUB(text) fprintf(fp, "<sub>%s</sub>", text)
#define MARK(text) fprintf(fp, "<mark>%s</mark>", text)

// List
#define OL(attributes) SCOPE(fprintf(fp, "<ol %s>", attributes), fputs("</ol>", fp))
#define UL(attributes) SCOPE(fprintf(fp, "<ul %s>", attributes), fputs("</ul>", fp))
#define LI(attributes) SCOPE(fprintf(fp, "<li %s>", attributes), fputs("</li>", fp))

// Table
#define TABLE(attributes) SCOPE(fprintf(fp, "<table %s>", attributes), fputs("</table>", fp))
#define CAPTION(text) fprintf(fp, "<caption>%s</caption>", text)
#define CALGROUP() SCOPE(fputs("<CALGROUP>", fp), fputs("</CALGROUP>", fp))
#define COL(attributes) fprintf(fp, "<col %s>", attributes)
#define TR(attributes) SCOPE(fprintf(fp, "<tr %s>", attributes), fputs("</tr>", fp))
#define TH(attributes) SCOPE(fprintf(fp, "<th %s>", attributes), fputs("</th>", fp))
#define TD(attributes) SCOPE(fprintf(fp, "<td %s>", attributes), fputs("</td>", fp))

// Media Tag
#define A(href, content) fprintf(fp, "<a href='%s'>%s</a>", href, content) 

// Hidden Content
#define DETAILS() SCOPE(fputs("<details>", fp), fputs("</details>", fp))
#define SUMMARY(attributes, content) fprintf(fp, "<summary %s>%s</summary>", attributes, content)

// CSS Generator
#define CSS(id) SCOPE(fprintf(fp, "%s {", id), fputs("}", fp))

#endif
