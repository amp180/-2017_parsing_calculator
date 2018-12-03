//
// Created by amp180 on 20/01/18.
//

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include "tokeniser.h"


//Opaque struct to hold parser state.
typedef struct parser_t parser_t;


//Parser interface
parser_t *get_parser(tokeniser_t *tok);
bool get_next_parse_value(parser_t *parser, double *d);
void free_parser(parser_t *parser);


#endif //PARSER_PARSER_H
