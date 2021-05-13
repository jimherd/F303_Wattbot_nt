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
 //////////////////////////////////////////////////////
 // modifed by Bruce Land, cornell university, may 2010
 // 
 // control structures changed to block-oriented
 ///////////////////////////////////////////////////////

//#define DEBUG 0

//#ifdef DEBUG
//#define DEBUG_PRINTF(...)  pc.printf(__VA_ARGS__)
//#else
//#define DEBUG_PRINTF(...)
//#endif  */

#include "globals.h"
#include "tokenizer.h"
#include "ubasic.h"

//static char *program_ptr;
#define MAX_STRINGLEN 40
static char string[MAX_STRINGLEN];

#define MAX_VARLEN 4
#define MAX_VARNUM 26
static int variables[MAX_VARNUM];       //char 

static int ended;

static int expr(void);
// static void line_statement(void);
static void statement(void);
/*---------------------------------------------------------------------------*/
void ubasic_init(char *program)
{
//    program_ptr = program;
//  for_stack_ptr = gosub_stack_ptr = 0;
    tokenizer_init(program);
    ended = 0;
}

/*---------------------------------------------------------------------------*/
static void accept(int token)
{
    if (token != tokenizer_token()) {
        DEBUG_PRINTF("Token not what was expected (expected %d, got %d)\n",
                     token, tokenizer_token());
        tokenizer_error_print();
        exit(1);
    }
    DEBUG_PRINTF("Expected %d, got it\n", token);
    tokenizer_next();
}

/*---------------------------------------------------------------------------*/
static int varfactor(void)
{
    int r;
    DEBUG_PRINTF("varfactor: obtaining %d from variable %d\n",
                 variables[tokenizer_variable_num()],
                 tokenizer_variable_num());
    r = ubasic_get_variable(tokenizer_variable_num());
    accept(TOKENIZER_VARIABLE);
    return r;
}

/*---------------------------------------------------------------------------*/
static int factor(void)
{
    int r;

    DEBUG_PRINTF("factor: token %d\n", tokenizer_token());
    switch (tokenizer_token()) {
    case TOKENIZER_NUMBER:
        r = tokenizer_num();
        DEBUG_PRINTF("factor: number %d\n", r);
        accept(TOKENIZER_NUMBER);
        break;
    case TOKENIZER_LEFTPAREN:
        accept(TOKENIZER_LEFTPAREN);
        r = expr();
        accept(TOKENIZER_RIGHTPAREN);
        break;
    default:
        r = varfactor();
        break;
    }
    return r;
}

/*---------------------------------------------------------------------------*/
static int term(void)
{
    int f1, f2;
    int op;

    f1 = factor();
    op = tokenizer_token();
    DEBUG_PRINTF("term: token %d\n", op);
    while (op == TOKENIZER_ASTR ||
           op == TOKENIZER_SLASH || op == TOKENIZER_MOD) {
        tokenizer_next();
        f2 = factor();
        DEBUG_PRINTF("term: %d %d %d\n", f1, op, f2);
        switch (op) {
        case TOKENIZER_ASTR:
            f1 = f1 * f2;
            break;
        case TOKENIZER_SLASH:
            f1 = f1 / f2;
            break;
        case TOKENIZER_MOD:
            f1 = f1 % f2;
            break;
        }
        op = tokenizer_token();
    }
    DEBUG_PRINTF("term: %d\n", f1);
    return f1;
}

/*---------------------------------------------------------------------------*/
static int expr(void)
{
    int t1, t2;
    int op;

    t1 = term();
    op = tokenizer_token();
    DEBUG_PRINTF("expr: token %d\n", op);
    while (op == TOKENIZER_PLUS ||
           op == TOKENIZER_MINUS ||
           op == TOKENIZER_AND || op == TOKENIZER_OR) {
        tokenizer_next();
        t2 = term();
        DEBUG_PRINTF("expr: %d %d %d\n", t1, op, t2);
        switch (op) {
        case TOKENIZER_PLUS:
            t1 = t1 + t2;
            break;
        case TOKENIZER_MINUS:
            t1 = t1 - t2;
            break;
        case TOKENIZER_AND:
            t1 = t1 & t2;
            break;
        case TOKENIZER_OR:
            t1 = t1 | t2;
            break;
        }
        op = tokenizer_token();
    }
    DEBUG_PRINTF("expr: %d\n", t1);
    return t1;
}

/*---------------------------------------------------------------------------*/
static int relation(void)
{
    int r1, r2;
    int op;

    r1 = expr();
    op = tokenizer_token();
    DEBUG_PRINTF("relation: token %d\n", op);
    while (op == TOKENIZER_LT || op == TOKENIZER_GT || op == TOKENIZER_EQ) {
        tokenizer_next();
        r2 = expr();
        DEBUG_PRINTF("relation: %d %d %d\n", r1, op, r2);
        switch (op) {
        case TOKENIZER_LT:
            r1 = r1 < r2;
            break;
        case TOKENIZER_GT:
            r1 = r1 > r2;
            break;
        case TOKENIZER_EQ:
            r1 = r1 == r2;
            break;
        }
        op = tokenizer_token();
    }
    return r1;
}


/*---------------------------------------------------------------------------*/
static void print_statement(void)
{
    accept(TOKENIZER_PRINT);
  /*  do {
        DEBUG_PRINTF("Print loop\n");
        if (tokenizer_token() == TOKENIZER_STRING) {
            tokenizer_string(string, sizeof(string));
            printf("%s", string);
            tokenizer_next();
        } else if (tokenizer_token() == TOKENIZER_COMMA) {
            printf(" ");
            tokenizer_next();
            //} else if(tokenizer_token() == TOKENIZER_SEMICOLON) {
            //  tokenizer_next();
        } else if ((tokenizer_token() == TOKENIZER_VARIABLE) ||
                   (tokenizer_token() == TOKENIZER_NUMBER)) {
            printf("%d", expr());
        } else {
            break;
        }
    } while ((tokenizer_token() != TOKENIZER_CR) &&
             (tokenizer_token() != TOKENIZER_ENDOFINPUT));
    printf("\n");
    DEBUG_PRINTF("End of print\n"); */
    tokenizer_next();
}

static void if_statement(void)
{
    int r;
    char bracket_depth;

    accept(TOKENIZER_IF);

    r = relation();
    DEBUG_PRINTF("if_statement: relation %d\n", r);
    accept(TOKENIZER_LEFTBRACK);
    bracket_depth = 1;
    if (r) {
        statement();    // statement() ends with new token

        // more than one statement?
        while (tokenizer_token() != TOKENIZER_RIGHTBRACK) {
            statement();
        }

        // is there an 'else' clause
        tokenizer_next();
        if (tokenizer_token() == TOKENIZER_ELSE) {
            // if so, skip over 'else' part of the 'if' statement
            // need to count brackets for nesting
            bracket_depth = 0;
            do {
                tokenizer_next();
                if (tokenizer_token() == TOKENIZER_RIGHTBRACK)
                    bracket_depth--;
                if (tokenizer_token() == TOKENIZER_LEFTBRACK)
                    bracket_depth++;
            } while (tokenizer_token() != TOKENIZER_RIGHTBRACK ||
                     bracket_depth > 0);

            do {
                tokenizer_next();
            } while (tokenizer_token() != TOKENIZER_CR);
        }
        // token for the next statement in the pgm    
        tokenizer_next();

    } else {
        // skip over 'then' part of 'if' statement.
        // need to count brackets for nesting
        do {
            tokenizer_next();
            if (tokenizer_token() == TOKENIZER_RIGHTBRACK)
                bracket_depth--;
            if (tokenizer_token() == TOKENIZER_LEFTBRACK)
                bracket_depth++;
        } while (tokenizer_token() != TOKENIZER_RIGHTBRACK ||
                 bracket_depth > 0);

        // find the else
        do {
            tokenizer_next();
        } while (tokenizer_token() != TOKENIZER_ELSE &&
                 tokenizer_token() != TOKENIZER_CR &&
                 tokenizer_token() != TOKENIZER_ENDOFINPUT);
        if (tokenizer_token() == TOKENIZER_ELSE) {
            tokenizer_next();
            accept(TOKENIZER_LEFTBRACK);
            statement();
            // more than one statement?
            while (tokenizer_token() != TOKENIZER_RIGHTBRACK) {
                statement();
            }
            accept(TOKENIZER_RIGHTBRACK);
            accept(TOKENIZER_CR);
        } else if (tokenizer_token() == TOKENIZER_CR) {
            tokenizer_next();
        }
    }
}

/*---------------------------------------------------------------------------*/
static void let_statement(void)
{
    int var;

    var = tokenizer_variable_num();

    accept(TOKENIZER_VARIABLE);
    accept(TOKENIZER_EQ);
    ubasic_set_variable(var, expr());
    DEBUG_PRINTF("let_statement: assign %d to %d\n", variables[var], var);
    accept(TOKENIZER_CR);

}

/*---------------------------------------------------------------------------*/
// syntax: delay expr
// added by brl4
static void delay_statement(void)
{
    int val;
    accept(TOKENIZER_DELAY);
    val = expr();
 //   wait_ms(val);
    accept(TOKENIZER_CR);
}

/*---------------------------------------------------------------------------*/
/*static void
return_statement(void)
{
  accept(TOKENIZER_RETURN);
  if(gosub_stack_ptr > 0) {
    gosub_stack_ptr--;
    jump_linenum(gosub_stack[gosub_stack_ptr]);
  } else {
    DEBUG_PRINTF("return_statement: non-matching return\n");
  }
}*/
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
// modified for {} delimited block -- BRL4
static void for_statement(void)
{
    int for_variable, to, for_value;
    /* int */ char *for_ptr;       //char

    accept(TOKENIZER_FOR);
    for_variable = tokenizer_variable_num();
    accept(TOKENIZER_VARIABLE);
    accept(TOKENIZER_EQ);
    ubasic_set_variable(for_variable, expr());
    accept(TOKENIZER_TO);
    to = expr();

    // record the next token search location
    for_ptr = tokenizer_ptr();

    while (variables[for_variable] <= to) {

        tokenizer_next();
        statement();
        // more than one statement?
        while (tokenizer_token() != TOKENIZER_RIGHTBRACK) {
            statement();
        }

        //variables[for_variable]++ ;
        for_value = ubasic_get_variable(for_variable);
        ubasic_set_variable(for_variable, ++for_value);

        if (variables[for_variable] <= to)
            tokenizer_ptr_set(for_ptr);
    }
    tokenizer_next();
    accept(TOKENIZER_CR);
}

/*---------------------------------------------------------------------------*/
// modified for {} delimited block -- BRL4
static void while_statement(void)
{
    int while_expr;
    /* int */ char *while_ptr, *end_ptr;
    char finished = 0, bracket_depth = 0;

    accept(TOKENIZER_WHILE);

    // record the next token search location
    while_ptr = tokenizer_ptr();
    while_expr = relation();

    // record the next pointer after the while loop ends
    // need to count brackets for nesting
    bracket_depth = 1;  //skip over LEFTBRACK
    do {
        tokenizer_next();
        if (tokenizer_token() == TOKENIZER_RIGHTBRACK)
            bracket_depth--;
        if (tokenizer_token() == TOKENIZER_LEFTBRACK)
            bracket_depth++;
    } while (tokenizer_token() != TOKENIZER_RIGHTBRACK ||
             bracket_depth > 0);
    end_ptr = tokenizer_ptr();

    // are we finished yet?
    if (while_expr) {
        finished = 1;
        tokenizer_ptr_set(while_ptr);
        relation();
    } else {
        finished = 0;
        tokenizer_ptr_set(end_ptr);
    }

    while (finished) {
        //printf("%d", tokenizer_token());
        accept(TOKENIZER_LEFTBRACK);
        //tokenizer_next();
        statement();
        // more than one statement?
        while (tokenizer_token() != TOKENIZER_RIGHTBRACK) {
            statement();
        }
        //end_ptr = tokenizer_ptr() ;
        tokenizer_ptr_set(while_ptr);
        while_expr = relation();
        if (!while_expr) {
            tokenizer_ptr_set(end_ptr);
            finished = 0;
        }
    }
    tokenizer_next();
    accept(TOKENIZER_CR);
}

/*---------------------------------------------------------------------------*/
/* static void rem_statement(void)
{
    accept(TOKENIZER_REM);
    while (tokenizer_token() != TOKENIZER_CR) {
        tokenizer_next();
    }

    tokenizer_next();
}  */

/*---------------------------------------------------------------------------*/
static void end_statement(void)
{
    accept(TOKENIZER_END);
    ended = 1;
}

/*
static void ClearDMX_statement(void)
{
    accept(TOKENIZER_CLEARDMX);
    accept(TOKENIZER_CR);
    ClearDMX();
}


static void SetDMXchan_statement(void)
{
int chan, value;

    accept(TOKENIZER_SETDMXCHAN);
    chan  = expr();
    value = expr();
    accept(TOKENIZER_CR);
    SetDMXchan(chan, value);
}


static void SendR_statement(void)
{
int chan, Rvalue;

    accept(TOKENIZER_SENDR);
    chan  = expr();
    Rvalue = expr();
    accept(TOKENIZER_CR);
    SendR(chan, Rvalue);
}

static void SendG_statement(void)
{
int chan, Gvalue;

    accept(TOKENIZER_SENDG);
    chan  = expr();
    Gvalue = expr();
    accept(TOKENIZER_CR);
    SendG(chan, Gvalue);
}


static void SendB_statement(void)
{
int chan, Bvalue;

    accept(TOKENIZER_SENDB);
    chan  = expr();
    Bvalue = expr();
    accept(TOKENIZER_CR);
    SendG(chan, Bvalue);
}


static void SendRGB_statement(void)
{
int chan, Rvalue, Gvalue, Bvalue;

    accept(TOKENIZER_SENDRGB);
    chan  = expr();
    Rvalue = expr();
    Gvalue = expr();
    Bvalue = expr();
    accept(TOKENIZER_CR);
    SendRGB(chan, Rvalue, Gvalue, Bvalue);
}



static void SetColourRGB_statement(void)
{
int index, Rvalue, Gvalue, Bvalue;

    accept(TOKENIZER_SETCOLOURRGB);
    index = expr();
    Rvalue = expr();
    Gvalue = expr();
    Bvalue = expr();
    accept(TOKENIZER_CR);
    SetColourRGB(index, Rvalue, Gvalue, Bvalue);
}


static void SetLight_statement(void)
{
    int spoke, light, value;

    accept(TOKENIZER_SETLIGHT);
    spoke = expr();
    light = expr();

    if (tokenizer_token() == TOKENIZER_STRING) {
        tokenizer_string(string, sizeof(string));
        DEBUG_PRINTF("%s", string);
        tokenizer_next();
        value = colour_string_to_index(string);
    } else {
        value = expr();
    }
    
    SetLight(spoke, light, value);
    accept(TOKENIZER_CR);
}


static void SetGroup_statement(void)
{
int group, value;

    accept(TOKENIZER_SETLIGHTGROUP);
    group  = expr();
    value = expr();
    accept(TOKENIZER_CR);
    SetLightGroup(group, value);
}
*/

static void statement(void)
{
    int token;

    token = tokenizer_token();

    switch (token) {
    case TOKENIZER_VARIABLE:
        let_statement();
        break;
    case TOKENIZER_PRINT:
        print_statement();
        break;
    case TOKENIZER_IF:
        if_statement();
        break;
    case TOKENIZER_FOR:
        for_statement();
        break;
    case TOKENIZER_END:
        end_statement();
        break;
    case TOKENIZER_DELAY:
        delay_statement();
        break;
    case TOKENIZER_WHILE:
        while_statement();
        break;
      
 /*    case TOKENIZER_CLEARDMX:      // jth
        ClearDMX_statement();
        break; 
    case TOKENIZER_SETDMXCHAN:     // jth
        SetDMXchan_statement();
        break; 
    case TOKENIZER_SENDR:          // jth
        SendR_statement();
        break; 
    case TOKENIZER_SENDG:          // jth
        SendG_statement();
        break;    
    case TOKENIZER_SENDB:          // jth
        SendB_statement();
        break;    
    case TOKENIZER_SENDRGB:        // jth
        SendRGB_statement();
        break; 
    case TOKENIZER_SETCOLOURRGB:   // jth
        SetColourRGB_statement();
        break;              
    case TOKENIZER_SETLIGHT:       // jth
        SetLight_statement();
        break; */
  //  case TOKENIZER_SETLIGHTGROUP:  // jth
   //     SetGroup_statement();
   //     break;
//    case TOKENIZER_REM:
//        rem_statement();
//        break;
//    case TOKENIZER_LET:
//        accept(TOKENIZER_LET);
        /* Fall through. */
    default:
        DEBUG_PRINTF("ubasic.c: statement(): not implemented %d\n", token);
        exit(1);
    }
}

/*---------------------------------------------------------------------------*/
/*static void
line_statement(void)
{
      DEBUG_PRINTF("----------- Line number %d ---------\n", tokenizer_num());
      current_linenum = tokenizer_num();
      accept(TOKENIZER_NUMBER);
      statement();
      return;
} */

/*---------------------------------------------------------------------------*/
void ubasic_run(void)
{
    if (tokenizer_finished()) {
        DEBUG_PRINTF("uBASIC program finished\n");
        return;
    }
    statement();        //modified to eliminate line numbers
    //line_statement();
}

//----------------------------------------------------------------------------
// run_ubasicp_program : run a ubasic test script
// ===================
//
// Notes
//
uint8_t run_ubasic_program(char *program) {

    ubasic_init(program);
    do{
        ubasic_run();
    } while(!ubasic_finished());
    return 0;
}

/*---------------------------------------------------------------------------*/
int ubasic_finished(void)
{
    return (ended || tokenizer_finished());
}

/*---------------------------------------------------------------------------*/
void ubasic_set_variable(int varnum, int value)
{
    if (varnum > 0 && varnum <= MAX_VARNUM) {
        variables[varnum] = value;
    }
}

/*---------------------------------------------------------------------------*/
int ubasic_get_variable(int varnum)
{
    if (varnum > 0 && varnum <= MAX_VARNUM) {
        return variables[varnum];
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
