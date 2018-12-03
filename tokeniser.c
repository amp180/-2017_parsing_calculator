#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define INCLUDE_TOKENISER_H_MACROS
#include "tokeniser.h"


//An associative array of token names.
const char *TOKEN_NAMES[] = {
        TOKENS(TOKEN_NAME_STR_COMMA)
};


//An associative array of token regexes.
const char *TOKEN_REGEXES[] = {
        TOKENS(TOKEN_REGEX_COMMA)
};


//Structure to store tokeniser state.
struct tokeniser_t {
    FILE* fp;
    char* curr_line;
    char* curr_line_start;
    size_t line_num;
    enum token_t curr_token_type;
    char* curr_token;
};

#ifdef __MINGW32_MAJOR_VERSION
//Crummy getline polyfill, prolly bugged AF
ssize_t getline(char const **lineptr, size_t const *n, FILE const *stream){
    if((!*lineptr) && (*n==0)){
        *lineptr = malloc(120);
        *n = 120;
        if(!*lineptr){
            fputs("Failed to allocate memory for line", stderr);
            return -1;
        }
    };

    ssize_t count = 0;
    int c = fgetc(stream);

    while(c!=EOF && c!='\n'){
        if(count>=*n){
            *lineptr = realloc(*lineptr, (*n += 120));
        };
        (*lineptr)[count++] = (char)(c);
        c = fgetc(stream);
    };

    if(c!=EOF) {
        (*lineptr)[count++] = (char)(c);
    }
    (*lineptr)[count++] = (char)('\0');

    return count;
};
#endif


//Free the current line and set the token to TOK_NOTOKEN.
void set_default_condition(tokeniser_t *const tokeniser) {
    if(tokeniser->curr_line_start) {
        free(tokeniser->curr_line_start);
    };
    tokeniser->curr_token_type = TOK_NOTOKEN;
    tokeniser->curr_token = (char*) NULL;
    tokeniser->curr_line_start = (char*) NULL;
    tokeniser->curr_line = (char*) NULL;
};


//Advance the current line past any whitespace.
int swallow_whitespace(tokeniser_t * const tokeniser){
    int chars_swallowed = 0;

    while(tokeniser->curr_line && *(tokeniser->curr_line) && isblank(*(tokeniser->curr_line))){
        tokeniser->curr_line++;
	    chars_swallowed++;
    };

    return chars_swallowed;
};


//Create a new tokeniser struct.
tokeniser_t *get_tokeniser(FILE *const fp){
  tokeniser_t * const tokeniser = calloc(1, sizeof(tokeniser_t));
  tokeniser->fp = (FILE*)(fp);
  tokeniser->curr_token_type = TOK_NOTOKEN;
  tokeniser->curr_token = (char*) NULL;
  tokeniser->curr_line_start = (char*) NULL;
  tokeniser->curr_line = (char*) NULL;
  tokeniser->line_num = 0;
  return tokeniser;
};


//Gets the current stored token.
enum token_t get_curr_token(const tokeniser_t *const tokeniser){
  return tokeniser->curr_token_type;
};


//Checks if the current token type is a member of an array.
bool curr_token_in(const tokeniser_t *const tokeniser, const token_t *const tokens, const size_t size){
    for(size_t i=0; i<size; i++)
        if(tokens[i]==tokeniser->curr_token_type)
            return true;
    return false;
};


//Gets the value of the current token.
bool get_curr_token_value(const tokeniser_t *const tokeniser, void *const value){
    switch(tokeniser->curr_token_type) {
        TOKENS(TOKEN_VALUE_CASE)
        default:
            break;
    };

    return false;
};


//Get the current line number.
size_t get_line_no(const tokeniser_t *const tokeniser){
    return tokeniser->line_num;
};


//Advances the tokeniser to the next token.
bool get_next_token(tokeniser_t *const tokeniser){
    swallow_whitespace(tokeniser);
    if((tokeniser->curr_line)==NULL || !(*(tokeniser->curr_line)) ){ //current line is empty
        char *new_line = NULL;
        ssize_t new_line_len = 0;
        size_t new_line_buff_len = 0;

        //Read a new line.
        new_line_len = getline(&new_line, &new_line_buff_len, tokeniser->fp);
        if(!new_line | new_line_len<0){
            if(new_line) {
                free(new_line);
            };
            //The line is empty, stop the tokeniser.
            set_default_condition(tokeniser);
            return false;
        };
        tokeniser->line_num++;

        //Free the old line.
        if (tokeniser->curr_line) {
            free(tokeniser->curr_line_start);
        };

        //Set the state.
        tokeniser->curr_line = new_line;
        tokeniser->curr_line_start = new_line;

        //Stop producing tokens at the end of the file.
        if(!(*(tokeniser->curr_line_start))) {
            set_default_condition(tokeniser);
            return false;
        };

    };

    swallow_whitespace(tokeniser);

    for(size_t i=0; i<(TOKENS(TOKEN_COUNT)); i++){
        regex_t r;
        regmatch_t m;

        //Compile the regex. (Todo: These could probably be cached in the tokeniser struct, but aren't.)
        if (regcomp(&r, TOKEN_REGEXES[i], REG_NEWLINE))
        {
            fprintf(stderr, "Regcomp failed.\n%s: %s", TOKEN_NAMES[i], TOKEN_REGEXES[i]);
            set_default_condition(tokeniser);
            return false;
        };

        //Check for the largest substring match, and that it starts at 0 offset.
        int regexec_ret = regexec(&r, tokeniser->curr_line, 1, &m, 0);
        regoff_t match_pos = m.rm_so;
        if ((regexec_ret) | (match_pos != 0 && match_pos != -1)){
            regfree(&r);
            continue;
        };

        //Free the regex.
        regfree(&r);

        //Replace the current token in the context with a copy of the new token and return.
        if(tokeniser->curr_token){
            free(tokeniser->curr_token);
        };
        size_t match_len = (size_t)(m.rm_eo-m.rm_so);
        char* new_token = calloc(match_len, sizeof tokeniser->curr_line[0]);
        strncpy(new_token, tokeniser->curr_line, match_len);
        tokeniser->curr_token = new_token;
        tokeniser->curr_token_type = (token_t)(i);
        tokeniser->curr_line += match_len;
        swallow_whitespace(tokeniser);
        return true;
    };

    //There were no matches, stop the tokeniser.
    set_default_condition(tokeniser);
    return false;
};


//Free the tokeniser struct.
void free_tokeniser(tokeniser_t *tokeniser){
    set_default_condition(tokeniser);
    free(tokeniser);
};
