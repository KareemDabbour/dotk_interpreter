#include "include/lexer.h"
#include "include/parser.h"
#include "include/io.h"

void print_help()
{
    printf("Usage: \ndotk.out <filename.k>\n");
    exit(1);
}
int main(int argc, char *argv[])
{
    if (argc < 2)
        print_help();

    lexer_T *lexer = init_lexer(get_file_contents(argv[1]));

    parser_T *parser = init_parser(lexer);

    AST_T *root = parser_parse(parser, parser->scope);
    ast_visit(root);

    return 0;
}
