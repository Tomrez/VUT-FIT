/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/


/*
    ----------------------------------
    PRIKLAD, JAK STACK POUZIVAT
    ----------------------------------


 data_type_token token_tmp;
    token_tmp.attribute.integer = 5;
    token_tmp.line_number = 55;
    token_tmp.type = TOKEN_NOT;

        fprintf(stderr, "ahoj\n");

    stack_t stack;
    stackInit(&stack);



    fprintf(stderr, "%d\n", stack.topIndex);
    stackPush(&stack, token_tmp);
    fprintf(stderr, "%d\n", stack.topIndex);

    data_type_token tmp;

    stackTop(&stack, &tmp);
    fprintf(stderr, "token line: %d\n", tmp.line_number);

    if(stack.array[stack.topIndex].line_number == 55)
    {
        fprintf(stderr, "ahoj\n");
    }

    //stackPop(&stack);

    stackDispose(&stack);
    fprintf(stderr, "%d\n", stack.topIndex);


*/

#include "stack.h"

void stackInit(stack_t *stack)
{
    if(stack == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stack->array = (stackItem_t *)malloc(sizeof(stackItem_t)*STACK_SIZE);
    if(stack->array == NULL)
    {
        exit(ERROR_INTERNAL);
    }
    stack->topIndex = -1; 
}


void stackPush(stack_t *stack, stackItem_t item)
{
    if(stack == NULL)
    {
        exit(ERROR_INTERNAL);
    }
    stack->topIndex++;
    stack->array[stack->topIndex] = item;
}


void stackPushAfterLastLiteral(stack_t *stack, stackItem_t item)
{
    if(stack == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stackItem_t tmpArr[6];
    int arrIndex = -1;

    while(stack->array[stack->topIndex].exprType != EXPR_DOLLAR)
    {
        if(stack->array[stack->topIndex].exprType != EXPR_NONTERM)
        {
            stackPush(stack, item);

            for(int i = arrIndex; i >= 0; i--)
            {
                stackPush(stack, tmpArr[i]);
            }
            return;
        }
        else
        {
            arrIndex++;
            stackTopAndPop(stack, &tmpArr[arrIndex]);
        }

    }
    //prvni terminal je $

    stackPush(stack, item); //vlozim za $ svuj vkladany znak

    //a vlozim zpatky vsechno ostatni
    for(int i = arrIndex; i >= 0; i--)
    {
        stackPush(stack, tmpArr[i]);
    }
}


void stackPop(stack_t *stack)
{
    if(stack == NULL)
    {
        exit(ERROR_INTERNAL);
    }
    
    stack->topIndex--;
}


void stackTopLastTerminal(stack_t *stack, stackItem_t *token)
{
    if(stack == NULL || token == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stackItem_t array[20];
    int array_index = -1;

    while(stack->array[stack->topIndex].exprType != EXPR_DOLLAR) //dokud nedojdu na dno zasobniku (v expr se tam uklada $)
    {
        if(stack->array[stack->topIndex].exprType != EXPR_NONTERM)
        {
            *token = stack->array[stack->topIndex]; //ulozim nalezeny terminal

            for(int i = array_index; i >= 0; i--) //vratim na zasobnik vsechny tokeny, ktere jsem musel popnout
            {
                stackPush(stack, array[i]);
            }

            return; //uspesne jsem nasel terminal a vratil zasobnik do puvodniho stavu

        }
        else //pokud je to neterminal
        {
            array_index++;
            stackTopAndPop(stack, &array[array_index]); //ulozim si ho do pole a popnu
        }
    }
    //pokud jsem zadny neterminal nenasel -> prvni nalezeny terminal bude '$'

    *token = stack->array[stack->topIndex]; //ulozim ho

    for(int i = array_index; i >= 0; i--) //vratim na zasobnik vsechny tokeny, ktere jsem musel popnout
    {
        stackPush(stack, array[i]);
    }
}


void stackTop(stack_t *stack, stackItem_t *token)
{
    if(stack == NULL || token == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stackItem_t tmp = stack->array[stack->topIndex];

    if (stack->topIndex == -1)
    {
        exit(ERROR_INTERNAL);
    }
    else
    {
        *token = tmp;
    }
}

void stackTopAndPop(stack_t *stack, stackItem_t *token)
{
    if(stack == NULL || token == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stackTop(stack, token);
    stackPop(stack);
}

void stackDispose(stack_t *stack)
{
    if(stack == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    stackItem_t *tmp;
    tmp = &stack->array[stack->topIndex];

    while(stack->topIndex > -1)
    {
        free(tmp);
        stack->topIndex--;
        if(stack->topIndex == -1)
        {
            break;
        }
        tmp = &stack->array[stack->topIndex];
    }
}