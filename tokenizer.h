/*
 * Copyright (c) 2006, Adam Dunkels
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

enum {
  TOKENIZER_ERROR,
  TOKENIZER_ENDOFINPUT,
  TOKENIZER_NUMBER,
  TOKENIZER_STRING,
  TOKENIZER_VARIABLE,
  TOKENIZER_LET,
  TOKENIZER_PRINT,
  TOKENIZER_IF,
  TOKENIZER_ELSE,
  TOKENIZER_FOR,
  TOKENIZER_TO,  //10
  TOKENIZER_RETURN,
  TOKENIZER_END,
  TOKENIZER_COMMA,
  TOKENIZER_SEMICOLON,
  TOKENIZER_PLUS,
  TOKENIZER_MINUS,
  TOKENIZER_AND,
  TOKENIZER_OR,
  TOKENIZER_ASTR,
  TOKENIZER_SLASH, //20
  TOKENIZER_MOD,
  TOKENIZER_LEFTPAREN,
  TOKENIZER_RIGHTPAREN,
  TOKENIZER_LEFTBRACK,   // Bruce land
  TOKENIZER_RIGHTBRACK,  // Bruce land
  TOKENIZER_LT,
  TOKENIZER_GT,
  TOKENIZER_EQ,
  TOKENIZER_CR,
  TOKENIZER_WHILE,       // 30 Bruce land
  TOKENIZER_REM,         // Bruce land
  TOKENIZER_DELAY,
  /* TOKENIZER_CLEARDMX,      // jth
  TOKENIZER_SETDMXCHAN,    // jth
  TOKENIZER_SENDR,         // jth 
  TOKENIZER_SENDG,         // jth 
  TOKENIZER_SENDB,         // jth 
  TOKENIZER_SENDRGB,       // jth 
  TOKENIZER_SETCOLOURRGB,  // jth
  TOKENIZER_SETLIGHT,      // 40 jth
  TOKENIZER_SETLIGHTGROUP, // jth   
  TOKENIZER_RUNSEQ,        // 42 jth */
};

//static char const *ptr, *nextptr;

struct keyword_token {
    char  *keyword;
    int   token;
};

const struct keyword_token keywords[] = {
    {"print", TOKENIZER_PRINT},
    {"if", TOKENIZER_IF},
    {"else", TOKENIZER_ELSE},
    {"for", TOKENIZER_FOR},
    {"to", TOKENIZER_TO},
    {"return", TOKENIZER_RETURN},
    {"end", TOKENIZER_END},
    {"delay", TOKENIZER_DELAY},    //brl4
    {"while", TOKENIZER_WHILE},    //brl4
    {"#", TOKENIZER_REM},          //brl4
    
  /*  {"clearDMX", TOKENIZER_CLEARDMX},          // jth
    {"setDMXchan", TOKENIZER_SETDMXCHAN},      // jth
    {"sendR", TOKENIZER_SENDR},                // jth 
    {"sendG", TOKENIZER_SENDG},                // jth 
    {"sendB", TOKENIZER_SENDB},                // jth 
    {"sendRGB", TOKENIZER_SENDRGB},            // jth     
    {"setcolourRGB", TOKENIZER_SETCOLOURRGB},  // jth 
    {"setlight", TOKENIZER_SETLIGHT},          // jth
    {"setgroup", TOKENIZER_SETLIGHTGROUP},     // jth
    {"runseq", TOKENIZER_RUNSEQ},              // jth (not a ubasic command) */
    
    {"\0", TOKENIZER_ERROR}
}; 

void tokenizer_init(/* const */ char *program);
void tokenizer_next(void);
int tokenizer_token(void);
int tokenizer_num(void);
int tokenizer_variable_num(void);
char* tokenizer_ptr(void);
void tokenizer_ptr_set(char *p);
void tokenizer_string(char *dest, int len);

int tokenizer_finished(void);
void tokenizer_error_print(void);

#endif /* __TOKENIZER_H__ */
