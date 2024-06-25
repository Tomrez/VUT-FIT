/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/

#ifndef _STACKH_
#define _STACKH_

#include "scanner.h"
#include "error.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 400

typedef struct stackItem
{
    data_type_token token;
    exprTypes exprType;

}stackItem_t;


typedef struct stack
{
    stackItem_t *array;
    int topIndex;
}stack_t;

//*Hlavicky funkci//

void stackInit(stack_t *stack);

void stackPush(stack_t *stack, stackItem_t token);

void stackPushAfterLastLiteral(stack_t *stack, stackItem_t item);

void stackPop(stack_t *stack);

void stackTop(stack_t *stack, stackItem_t *token);

void stackTopLastTerminal(stack_t *stack, stackItem_t *token);

void stackTopAndPop(stack_t *stack, stackItem_t *token);

void stackDispose(stack_t *stack);


#endif