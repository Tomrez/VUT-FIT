/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
 * @author Matěj Lepeška, xlepes00
 * 
*/


#ifndef EXPR_H
#define EXPR_H

#include "analysis.h"

typedef enum{
    N_PLUS_N,       //E -> E+E
    N_MINUS_N,      //E -> E-E
    N_MULT_N,       //E -> E*E
    N_DIVID_N,      //E -> E/E
    N_LESS_N,       //E -> E<E
    N_GREATER_N,    //E -> E>E
    N_LEQUAL_N,     //E -> E<=E
    N_GEQUAL_N,     //E -> E>=E
    N_EQUAL_N,      //E -> E==E
    N_NEQUAL_N,     //E -> E!=E
    N_NOT,          //E -> E!
    N_DEFAULT_N,    //E -> E??E
    LB_N_RB,        //E -> (E)
    OPERAND,        //E -> i
    IS_NOT_RULE,
}rules;

typedef enum{
    EXPR_PLUS, // +
    EXPR_MINUS, // -
    EXPR_MUL, // *
    EXPR_DIV, // /

    EXPR_LOGIC_EQUAL, // ==
    EXPR_NOT_EQUAL, // !=
    EXPR_LESSER, // <
    EXPR_GREATER, // >
    EXPR_LESSER_EQUAL, // <=
    EXPR_GREATER_EQUAL, //>=

    EXPR_DEFAULT_VALUE, // ??
    EXPR_NOT,
    
    EXPR_LEFT_BRACKET, // (
    EXPR_RIGHT_BRACKET, // )

    EXPR_I, // i
    EXPR_DOLLAR, // $

    EXPR_SHIFT_SIGN, // <

    EXPR_NONTERM, // E
} exprTypes;

/**
 * @brief zpracuje výrazy
 * 
 * @param data interni data
 * @return int příslušnou hodnotu
 */
int expr(parser_data *data);

#endif