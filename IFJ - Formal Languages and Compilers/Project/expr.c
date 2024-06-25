/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/

#include "scanner.h"
#include "error.h"
#include "expr.h"
#include "stack.h"
#include "symtable.h"
#include "generator.h"
#include "var_list.h"

#define TAB_SIZE 9

typedef enum {
    OPEN,   // >
    CLOSE,  // <
    EQUAL,  // =
    ERROR   //
}preced_table_symbol;

unsigned int preced_table[TAB_SIZE][TAB_SIZE] = {
    /* ! */                 {ERROR, CLOSE, OPEN, OPEN, OPEN, CLOSE, OPEN, CLOSE, OPEN},
    /* * / */               {CLOSE, OPEN, OPEN, OPEN, OPEN, CLOSE, OPEN, CLOSE, OPEN},
    /* + - */               {CLOSE, CLOSE, OPEN, OPEN, OPEN, CLOSE, OPEN, CLOSE, OPEN},
    /* == != < > <= >= */   {CLOSE, CLOSE, CLOSE, ERROR, OPEN, CLOSE, OPEN, CLOSE, OPEN},
    /* ?? */                {CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN, CLOSE, OPEN},
    /* ( */                 {CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, OPEN, EQUAL, CLOSE, ERROR},
    /* ) */                 {OPEN, OPEN, OPEN, OPEN, OPEN, ERROR, CLOSE, ERROR, OPEN},
    /* i */                 {OPEN, OPEN, OPEN, OPEN, OPEN, ERROR, OPEN, ERROR, OPEN},
    /* $ */                 {CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, CLOSE, ERROR, CLOSE, ERROR}
};

/**
 * @brief 
 * @param token is pointer to last read token
 * @param index is index of row or column in preced table
*/
void preced_table_index (data_type_token *token, int *index){
    if(token == NULL){
        exit(ERROR_INTERNAL);
    }
    switch(token->type){
        case TOKEN_PLUS: case TOKEN_MINUS:
            *index = 2;
            break;
        case TOKEN_RETURN_TYPE_SYMBOL:
            *index = 8;
            break;
        case TOKEN_MULTIPLY: case TOKEN_DIVIDE:
            *index = 1;
            break;
        case TOKEN_NOT:
            *index = 0;
            break;
        case TOKEN_NOT_EQUAL: case TOKEN_LOGICAL_EQUAL: case TOKEN_LESSER: case TOKEN_LESSER_OR_EQUAL: case TOKEN_GREATER: case TOKEN_GREATER_OR_EQUAL:
            *index = 3;
            break;
        case TOKEN_LEFT_PARENTHESIS:
            *index = 5;
            break;
        case TOKEN_RIGHT_PARENTHESIS:
            *index = 6;
            break;
        case TOKEN_INTEGER: case TOKEN_INTEGER_EXPONENT_MINUS: case TOKEN_INTEGER_EXPONENT_PLUS: case TOKEN_FLOAT: case TOKEN_FLOAT_EXPONENT_MINUS: case TOKEN_FLOAT_EXPONENT_PLUS: case TOKEN_STRING:
            *index = 7;
            break;
        case TOKEN_DEFAULT_VALUE:
            *index = 4;
            break;
        case TOKEN_DOLLAR:
            *index = 8;
            break;
        default:
            *index = -1;
    }
}

token_type testPlus(stackItem_t *x1, stackItem_t *x2)
{
    if(x1->token.type == TOKEN_INTEGER && x2->token.type == TOKEN_INTEGER)
    {
        return TOKEN_INTEGER;
    }
    else if(x1->token.type == TOKEN_INTEGER && x2->token.type == TOKEN_FLOAT)
    {
        generate_to_float_top();
        return TOKEN_FLOAT;
    }   
    else if(x1->token.type == TOKEN_FLOAT && x2->token.type == TOKEN_INTEGER)
    {   
        generate_to_float_second_top();
        return TOKEN_FLOAT;
    }
    else if(x1->token.type == TOKEN_FLOAT && x2->token.type == TOKEN_FLOAT)
    {
        return TOKEN_FLOAT;
    }
    else if(x1->token.type == TOKEN_STRING && x2->token.type == TOKEN_STRING)
    {
        return TOKEN_STRING;
    }
    else
    {
        exit(ERROR_SEMANTIC_TYPE_UNCOMPATIBLE);
    }   
}


token_type testMinusMulDiv(stackItem_t *x1, stackItem_t *x2)
{
    if(x1->token.type == TOKEN_INTEGER && x2->token.type == TOKEN_INTEGER)
    {
        return TOKEN_INTEGER;
    }
    else if(x1->token.type == TOKEN_INTEGER && x2->token.type == TOKEN_FLOAT)
    {
        generate_to_float_top();
        return TOKEN_FLOAT;
    }
    else if(x1->token.type == TOKEN_FLOAT && x2->token.type == TOKEN_INTEGER)
    {
        generate_to_float_second_top();
        return TOKEN_FLOAT;
    }
    else if(x1->token.type == TOKEN_FLOAT && x2->token.type == TOKEN_FLOAT)
    {
        return TOKEN_FLOAT;
    }
    else
    {
        exit(ERROR_SEMANTIC_TYPE_UNCOMPATIBLE);
    }
}


token_type testEquality(stackItem_t *x1, stackItem_t *x2)
{
    if(x1->token.type == TOKEN_INTEGER && x2->token.type == TOKEN_INTEGER)
    {
        return TOKEN_BOOL;
    }
    else if(x1->token.type == TOKEN_FLOAT && x2->token.type == TOKEN_FLOAT)
    {
        return TOKEN_BOOL;
    }
    else
    {
        exit(ERROR_SEMANTIC_TYPE_UNCOMPATIBLE);
    }
}


token_type testDefVal(stackItem_t *x1, stackItem_t *x2)
{
    if(x1->token.type != TOKEN_UNKNOWN)
        return x1->token.type;
    else
        return x2->token.type;
    
    return TOKEN_BOOL;
}

token_type testNot(stackItem_t *x1)
{
    return x1->token.type;
}

int amountToReduce(stack_t *stack)
{
    int amount = 0;

    stackItem_t tmpArr[4];
    int tmpArrCount = -1;

    do
    {     
        tmpArrCount++;

        if(tmpArrCount == 4)
        {
            exit(ERROR_SYNTACTIC_ANALYSIS);
        }

        stackTopAndPop(stack, &tmpArr[tmpArrCount]);
        amount++;

        if(tmpArr[tmpArrCount].exprType == EXPR_DOLLAR)
        {
            exit(ERROR_SYNTACTIC_ANALYSIS);
        }

    }while(tmpArr[tmpArrCount].exprType != EXPR_SHIFT_SIGN);

    for(int i = tmpArrCount; i >= 0; i--)
    {
        stackPush(stack, tmpArr[i]);
    }


    return amount - 1; //vratim ho o jedna mensi, protoze jsem musel nacist i SHIFT_SIGN
}

rules reduceOneByRule(stack_t *stack, stackItem_t *x1)
{
    //pokud to je nejake 'i', je to pravidlo

    if(x1->exprType == EXPR_I)
    {

            stackPop(stack); //shift sign

            stackItem_t tmp;
            tmp.exprType = EXPR_NONTERM;
            tmp.token.type = x1->token.type;     
            stackPush(stack, tmp);

            return OPERAND;
    }
    else
    {
        exit(ERROR_SYNTACTIC_ANALYSIS);
    }


}

rules reduceTwoByRule(stack_t *stack, stackItem_t *x1, stackItem_t *x2)
{
    //napr E! pravidlo

    if(x1->exprType == EXPR_NONTERM)
    {
        if(x2->exprType == EXPR_NOT)
        {
            token_type dataType = testNot(x1); // int? -> int

            stackPop(stack); //shift sign

            stackItem_t tmp;
            tmp.exprType = EXPR_NONTERM;
            tmp.token.type = dataType;     
            stackPush(stack, tmp);

            return N_NOT;

        }
        else
        {
            exit(ERROR_SYNTACTIC_ANALYSIS);
        }
    }
    else
    {
        exit(ERROR_SYNTACTIC_ANALYSIS);
    }
}


rules reduceThreeByRule(stack_t *stack, stackItem_t *x1, stackItem_t *x2, stackItem_t *x3)
{
    //zjistit, jestli to odpovida nekteremu z pravidel v expr.h, pokud ano, je to v pohode, jinak chyba
    //E neco E
    //( E )

    if(x1->exprType == EXPR_NONTERM && x3->exprType == EXPR_NONTERM)
    {
        token_type dataType;
        stackItem_t tmp;
        switch (x2->exprType)
        {
            case EXPR_PLUS:
                dataType = testPlus(x1, x3); //otestuji, jestli jsou obe E ready na scitani a vrati datovy typ vysledku

                stackPop(stack); //shift sign

                tmp.exprType = EXPR_NONTERM;
                tmp.token.type = dataType;     
                stackPush(stack, tmp);

                if(dataType == TOKEN_STRING){
                    generate_concatenation();
                }else{
                    generate_addition();
                }

                return N_PLUS_N;

            case EXPR_MINUS:
            case EXPR_MUL:
            case EXPR_DIV:

                dataType = testMinusMulDiv(x1, x3);

                stackPop(stack);

                tmp.exprType = EXPR_NONTERM;
                tmp.token.type = dataType;
                stackPush(stack, tmp);

                if(x2->exprType == EXPR_MINUS){
                    generate_subtraction();
                    return N_MINUS_N;

                }else if(x2->exprType == EXPR_MUL){
                    generate_multiplication();
                    return N_MULT_N;

                }else{
                    if(dataType == TOKEN_FLOAT){
                        generate_division_double();
                    }else{
                        generate_division_int();
                    }
                    return N_DIVID_N;
                }

            case EXPR_LOGIC_EQUAL:
            case EXPR_NOT_EQUAL:
            case EXPR_LESSER:
            case EXPR_GREATER:
            case EXPR_LESSER_EQUAL:
            case EXPR_GREATER_EQUAL:

                dataType = testEquality(x1, x3);

                stackPop(stack);

                tmp.exprType = EXPR_NONTERM;
                tmp.token.type = dataType;     
                stackPush(stack, tmp);

                switch (x2->exprType)
                {
                    case EXPR_LOGIC_EQUAL:
                        generate_equal();
                        return N_EQUAL_N;

                    case EXPR_NOT_EQUAL:
                        generate_not_equal();
                        return N_NEQUAL_N;

                    case EXPR_LESSER:
                        generate_lesser();
                        return N_LESS_N;

                    case EXPR_GREATER:
                        generate_greater();
                        return N_GREATER_N;

                    case EXPR_LESSER_EQUAL:
                        generate_lesser_equal();
                        return N_LEQUAL_N;

                    case EXPR_GREATER_EQUAL:
                        generate_greater_equal();
                        return N_GEQUAL_N;
                        
                    default:
                        //! NOT SURE CO TO MA VRATIT
                        exit(ERROR_SEMANTIC_OTHER);
                }

            case EXPR_DEFAULT_VALUE:
                dataType = testDefVal(x1, x3); //! TODO

                stackPop(stack);

                tmp.exprType = EXPR_NONTERM;
                tmp.token.type = dataType;     
                stackPush(stack, tmp);

                return N_DEFAULT_N;

            default:
                exit(ERROR_SYNTACTIC_ANALYSIS);

        }
    }
    else if(x1->exprType == EXPR_LEFT_BRACKET && x3->exprType == EXPR_RIGHT_BRACKET)
    {
        if (x2->exprType == EXPR_NONTERM)
        {

            stackPop(stack);

            stackItem_t tmp;
            tmp.exprType = EXPR_NONTERM;
            tmp.token.type = x2->token.type;     
            stackPush(stack, tmp);

            return LB_N_RB;
        }
        else
        {
            exit(ERROR_SYNTACTIC_ANALYSIS);
        }

    }
    else
    {
        exit(ERROR_SYNTACTIC_ANALYSIS);
    }

}


int reduce(stack_t *stack)
{

    //potrebuji zjistit, kolik operandu mam redukovat
    int amount = amountToReduce(stack);
    
    stackItem_t x1;
    stackItem_t x2;
    stackItem_t x3;

    switch(amount)
    {
        case 1:
                
            stackTopAndPop(stack, &x1);
            
            reduceOneByRule(stack, &x1);

            break;

        case 2:

            stackTopAndPop(stack, &x1);
            stackTopAndPop(stack, &x2);

            reduceTwoByRule(stack, &x1, &x2);

            break;

        case 3:

            stackTopAndPop(stack, &x1);
            stackTopAndPop(stack, &x2);
            stackTopAndPop(stack, &x3);

            reduceThreeByRule(stack, &x1, &x2, &x3);
            
            break;

        default: //pravdepodobne moznost 0
            //I guess, ze nemuze byt jiny pocet operandu nez 1, 2 nebo 3 -> ERROR
            
            exit(ERROR_SYNTACTIC_ANALYSIS);
    }
    return 0;
}

exprTypes setType(stackItem_t item)
{
   switch(item.token.type)
   {
    case TOKEN_PLUS:
        return EXPR_PLUS;
    case TOKEN_MINUS:
        return EXPR_MINUS;
    case TOKEN_MULTIPLY:
        return EXPR_MUL;
    case TOKEN_DIVIDE:
        return EXPR_DIV;
    case TOKEN_NOT:
        return EXPR_NOT;
    case TOKEN_LOGICAL_EQUAL:
        return EXPR_LOGIC_EQUAL;
    case TOKEN_NOT_EQUAL:
        return EXPR_NOT_EQUAL;
    case TOKEN_LESSER:
        return EXPR_LESSER;
    case TOKEN_GREATER:
        return EXPR_GREATER;
    case TOKEN_GREATER_OR_EQUAL:
        return EXPR_GREATER_EQUAL;
    case TOKEN_LESSER_OR_EQUAL:
        return EXPR_LESSER_EQUAL;
    case TOKEN_LEFT_PARENTHESIS:
        return EXPR_LEFT_BRACKET;
    case TOKEN_RIGHT_PARENTHESIS:
        return EXPR_RIGHT_BRACKET;
    case TOKEN_DOLLAR:
        return EXPR_DOLLAR;
    case TOKEN_DEFAULT_VALUE:
        return EXPR_DEFAULT_VALUE;
    default:
        return EXPR_I;
        break;
   }
}



int expr(parser_data *data){
    stack_t stack;
    stackInit(&stack);
    
    stackItem_t stack_top;
    stackItem_t act_symbol;
    int top;
    int act;

    stackItem_t tmp;
    tmp.exprType = EXPR_DOLLAR;
    tmp.token.type = TOKEN_DOLLAR;
    stackPush(&stack, tmp);


    act_symbol.token = data->token;
    if(data->token.attribute.string != NULL)
    {
        htabItem_t *tabItem = htabSearch(data->sym_table, data->token.attribute.string);
        switch(tabItem->data.data_type)
        {
            case DATA_TYPE_STRING:
                act_symbol.token.type = TOKEN_STRING;
                break;
            case DATA_TYPE_INT:
                act_symbol.token.type = TOKEN_INTEGER;
                break;
            case DATA_TYPE_DOUBLE:
                act_symbol.token.type = TOKEN_FLOAT;
                break;
            case DATA_TYPE_BOOL:
                act_symbol.token.type = TOKEN_BOOL;
                break;
            default:
                act_symbol.token.type = TOKEN_UNKNOWN;
                break;
        }
    }

    while(true)
    {
        stackTopLastTerminal(&stack, &stack_top);  
        preced_table_index(&stack_top.token,&top);
        preced_table_index(&act_symbol.token, &act);

        if(act == -1) // kdyz dostanu neco, co precedencni tabulka neocekavala
        {
            //zpracovavat zasobnik, dokud tam nebude $E
            stackItem_t reduceUntilDollar;
            stackTopLastTerminal(&stack, &reduceUntilDollar);
            while(reduceUntilDollar.exprType != EXPR_DOLLAR)
            {
            reduce(&stack);
            stackTopLastTerminal(&stack, &reduceUntilDollar);
            }

            //? USPESNY PRUCHOD EXPR POKUD SE DOSTANU AZ SEM//

            //ulozime do data_type datovy typ vysledneho E
            stackTop(&stack, &tmp);

            data->token = act_symbol.token;

            switch (tmp.token.type)
            {
                case TOKEN_INTEGER:
                    data->data_type = DATA_TYPE_INT;
                    break;
                
                case TOKEN_FLOAT:
                    data->data_type = DATA_TYPE_DOUBLE;
                    break;

                case TOKEN_STRING:
                    data->data_type = DATA_TYPE_STRING;
                    break;

                case TOKEN_BOOL:
                    data->data_type = DATA_TYPE_BOOL;
                    break;
                default:
                    break;
            }
            return 0;
        }

        switch(preced_table[top][act]){
            case OPEN: //>
                reduce(&stack);

                tmp.exprType = EXPR_SHIFT_SIGN;
                stackPushAfterLastLiteral(&stack, tmp);
                act_symbol.exprType = setType(act_symbol);
                stackPush(&stack, act_symbol);
                get_token(&act_symbol.token);
                break;
            case CLOSE: //<
                tmp.exprType = EXPR_SHIFT_SIGN;
                stackPushAfterLastLiteral(&stack, tmp);
                act_symbol.exprType = setType(act_symbol);
                stackPush(&stack, act_symbol);

                // generovani ulozeni na zasobnik
                switch(act_symbol.token.type){
                    case(TOKEN_FLOAT):
                        push_double(act_symbol.token.attribute.decimal);
                        break;
                    case(TOKEN_INTEGER):
                        push_int(act_symbol.token.attribute.integer);
                        break;
                    case(TOKEN_STRING):
                        push_string(act_symbol.token.attribute.string);
                        break;
                    case(TOKEN_IDENTIFIER):
                        if(act_symbol.token.attribute.keyword == KEYWORD_NIL){
                            push_nil();
                        }else{
                            push_id(act_symbol.token.attribute.string, searchVar(data->var_frame, act_symbol.token.attribute.string));
                        }
                        break;
                    default:
                        break;
                }

                get_token(&act_symbol.token);
                break;
            case EQUAL: //=
                stackPush(&stack, act_symbol);
                get_token(&act_symbol.token);
                break;
            case ERROR:
                exit(ERROR_SYNTACTIC_ANALYSIS);
        }
    }

}
