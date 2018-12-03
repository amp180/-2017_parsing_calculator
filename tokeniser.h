#ifndef TOKENISER_H
#define TOKENISER_H

#include "stdbool.h"
#include "stdio.h"


//Macro defining all recognised tokens, define INCLUDE_TOKENISER_H_MACROS to use.
#define TOKENS(TOKEN)\
	TOKEN(TOK_OP_PLUS, "\\+", {})\
	TOKEN(TOK_OP_MINUS, "-",  {})\
    TOKEN(TOK_OP_MULT, "\\*", {})\
    TOKEN(TOK_OP_DIV, "[/\\]", {})\
    TOKEN(TOK_OP_POW, "\\^",  {})\
    TOKEN(TOK_LEFT_BRACKET, "(", {})\
    TOKEN(TOK_RIGHT_BRACKET, ")", {})\
	TOKEN(TOK_NUM, "[0-9][0-9]*", {*(int*)(value) = atoi(tokeniser->curr_token); return true;} )\
    TOKEN(TOK_SEMIC, "[;\r\n][;\r\n]*", {})\
    TOKEN(TOK_NOTOKEN, "$$", {})


//Macros to extract token attributes, define INCLUDE_TOKENISER_H_MACROS to use.
#define TOKEN_NAME_COMMA(NAME, REGEX, VALUE) NAME,
#define TOKEN_NAME_STR_COMMA(NAME, REGEX, VALUE) #NAME,
#define TOKEN_REGEX_COMMA(NAME, REGEX, VALUE) REGEX,
#define TOKEN_COUNT(NAME, REGEX, VALUE) +1
#define TOKEN_VALUE_CASE(NAME, REGEX, VALUE) { case(NAME): (VALUE); break; };


//An enum of token types.
typedef enum token_t {
    TOKENS(TOKEN_NAME_COMMA)
} token_t;


//Constants defined in tokeniser.c
extern const char *TOKEN_REGEXES[]; //Associative array of token's regex strings.
extern const char *TOKEN_NAMES[]; //Associative array of token names.


//Struct to store tokeniser context.
typedef struct tokeniser_t tokeniser_t;


//Tokeniser interface.
tokeniser_t *get_tokeniser(FILE *fp); //Creates tokeniser context.
token_t get_curr_token(const tokeniser_t *tokeniser); //Gets the current token type.
bool curr_token_in(const tokeniser_t *tokeniser, const token_t *tokens, size_t size); //Checks the current token against an array of token types.
bool get_curr_token_value(const tokeniser_t *tokeniser, void* value); //Assigns the current token's value to a pointer.
size_t get_line_no(const tokeniser_t *tokeniser); //Returns the line currently being parsed.
bool get_next_token(tokeniser_t *tokeniser); //Advances the tokeniser.
void free_tokeniser(tokeniser_t *tokeniser); //Frees the tokeniser context.


//The macros are undef-ed if they're not needed to save global namespace.
#ifndef INCLUDE_TOKENISER_H_MACROS
#undef TOKENS
#undef TOKEN_NAME_COMMA
#undef TOKEN_NAME_STR_COMMA
#undef TOKEN_REGEX_COMMA
#undef TOKEN_COUNT
#undef TOKEN_VALUE_CASE
#endif //INCLUDE_TOKENISER_H_MACROS


#endif //TOKENISER_H
