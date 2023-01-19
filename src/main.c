#include "include/io.h"
#include "include/lexer.h"
#include "include/parser.h"

void print_help()
{
    printf("Usage: \ndotk.out <filename.k>\n");
    exit(1);
}
int main(int argc, char *argv[])
{
    if (argc < 2)
        print_help();

    char *file_contents = get_file_contents(argv[1]);

    lexer_T *lexer = init_lexer(file_contents, argv[1]);

    parser_T *parser = init_parser(lexer);

    AST_T *root = parser_parse(parser, parser->scope);

#if DEBUG
    printf(KCYN);
    printf("-- DEBUG ENABLED --\n");
    printf(KNRM);
#endif
    ast_set_file_path(argv[1]);
    ast_visit(root);
    free(file_contents);
    return 0;
}
