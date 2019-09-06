#include <stdio.h>
#include "tokeniser.h"
#include "parser.h"

int main(size_t argc, char **argv) {

    puts("This program parses and evaluate simple maths expressions.\n"
         "Supported operators: +-*\\()^\n"
         "eg. ( 2 * 2 ) / 4;\n"
         "Ctrl-D or blank line to quit.\n"
    );

    tokeniser_t *tok = get_tokeniser(stdin);
    parser_t *par = get_parser(tok);

    double d;
    while(get_next_parse_value(par, &d)){
        printf("%f\n", d);
    }

    free_parser(par);

    free_tokeniser(tok);
    return 0;
};