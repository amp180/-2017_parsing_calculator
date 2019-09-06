//
// Created by amp180 on 20/01/18.
//

#include "stdlib.h"
#include "assert.h"
#include "math.h"
#include "parser.h"
#include "tokeniser.h"

struct parser_t {
    tokeniser_t *tokeniser;
};


double arith_expr(parser_t * parser);


parser_t *get_parser(tokeniser_t *const tokeniser){
    get_next_token(tokeniser);
    parser_t *parser = malloc(sizeof(parser_t));
    parser->tokeniser = tokeniser;
};


double atom_expr(parser_t *const parser) {

    if (get_curr_token(parser->tokeniser) == TOK_SEMIC ){
        puts(TOKEN_NAMES[get_curr_token(parser->tokeniser)]);
        assert(("You need at least one more number before the semicolon or end of line.", false));
    }

    if(get_curr_token(parser->tokeniser) == TOK_LEFT_BRACKET){
        get_next_token(parser->tokeniser);
        double d = arith_expr(parser);
        if(get_curr_token(parser->tokeniser) == TOK_RIGHT_BRACKET) {
            get_next_token(parser->tokeniser);
            return d;
        } else {
            puts(TOKEN_NAMES[get_curr_token(parser->tokeniser)]);
            assert(("Missing matching right bracket.", false));
        }
    }

    if(get_curr_token(parser->tokeniser) == TOK_NUM){
        int i;
        get_curr_token_value(parser->tokeniser, &i);
        get_next_token(parser->tokeniser);
        return i;
    }

    puts(TOKEN_NAMES[get_curr_token(parser->tokeniser)]);
    assert(("Atomic expression expected", false));
};


double pow_expr(parser_t *const parser) {
    double d1 = atom_expr(parser);

    if (get_curr_token(parser->tokeniser) == TOK_SEMIC){
        return d1;
    }

    if (get_curr_token(parser->tokeniser) == TOK_OP_POW) {
        get_next_token(parser->tokeniser);
        return pow(d1, pow_expr(parser));
    }

    return d1;
};


double mul_expr(parser_t *const parser) {
    double d1 = pow_expr(parser);

    if (get_curr_token(parser->tokeniser) == TOK_SEMIC){
        return d1;
    }

    if (get_curr_token(parser->tokeniser) == TOK_OP_DIV) {
        get_next_token(parser->tokeniser);
        return d1 / mul_expr(parser);
    }

    if (get_curr_token(parser->tokeniser) == TOK_OP_MULT) {
        get_next_token(parser->tokeniser);
        return d1 * mul_expr(parser);
    }

    return d1;
};


double arith_expr(parser_t *const parser) {
    double d1 = mul_expr(parser);

    if (get_curr_token(parser->tokeniser) == TOK_SEMIC){
        return d1;
    }

    if (get_curr_token(parser->tokeniser) == TOK_OP_PLUS) {
        get_next_token(parser->tokeniser);
        return d1 + arith_expr(parser);
    }

    if(get_curr_token(parser->tokeniser) == TOK_OP_MINUS){
        get_next_token(parser->tokeniser);
        return d1 - arith_expr(parser);
    }

    return d1;
};


bool get_next_parse_value(parser_t *const parser, double *d) {

    const token_t blanks[2] = {TOK_SEMIC, TOK_NOTOKEN};
    if (curr_token_in(parser->tokeniser, blanks, 2)){
        get_next_token(parser->tokeniser);
        if (curr_token_in(parser->tokeniser, blanks, 2)){
            return false;
        }
    }

    *d =  arith_expr(parser);

    return true;
};


void free_parser(parser_t *const parser){
    free(parser);
};