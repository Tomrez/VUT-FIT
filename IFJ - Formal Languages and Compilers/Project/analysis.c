/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#include "analysis.h"
#include "error.h"
#include "expr.h"
#include "hfile.h"
#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// deklarace funkcí pro jednotlivé neterminály
static int prog(parser_data *data);
static int command(parser_data *data);
static int param(parser_data *data);
static int param_more(parser_data *data);
static int param_val(parser_data *data);
static int arg(parser_data *data);
static int arg_more(parser_data *data);
static int arg_val(parser_data *data);
static int func_call(parser_data *data);
static int assign(parser_data *data);
static int init(parser_data *data);
static int rtype(parser_data *data);
static int dtype(parser_data *data);
static int type(parser_data *data);
static int val(parser_data *data);

// pomocná makra
#define GET_TOKEN()     \
    data->prev_token = data->token.type;    \
    if((result = get_token(&data->token)) != SCANNER_TOKEN_SUCCESS) return result;    \
    if(IS_TYPE(TOKEN_IDENTIFIER)) data->loaded_id = htabSearch(data->sym_table, data->token.attribute.string)

#define CHECK_COMMAND_VIABILITY()   \
    if(!(data->token.attribute.newline_before) && (data->prev_token != TOKEN_LEFT_CURLY_BRACE) &&  \
    (data->prev_token != TOKEN_UNKNOWN))return ERROR_SYNTACTIC_ANALYSIS

// makra pro podmínky
#define IS_TYPE(compare)    \
    (data->token.type == compare)

#define IS_KEYWORD(compare)  \
    ((data->token.type == TOKEN_KEYWORD) &&  \
    (data->token.attribute.keyword == compare))

#define IS_ID_VAR()     \
    ((IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id != NULL)) && (IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id->data.type == TYPE_VARIABLE)))

#define IS_ID_CONST()     \
    ((IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id != NULL)) && (IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id->data.type == TYPE_CONSTANT)))

#define IS_ID_FUNC()    \
    ((IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id == NULL)) || (IS_TYPE(TOKEN_IDENTIFIER) && (data->loaded_id->data.type == TYPE_FUNC)))

#define IS_FUNC()   \
    (IS_KEYWORD(KEYWORD_READSTRING) || IS_KEYWORD(KEYWORD_READINT) || IS_KEYWORD(KEYWORD_READDOUBLE) || \
    IS_KEYWORD(KEYWORD_WRITE) || IS_KEYWORD(KEYWORD_INT2DOUBLE) || IS_KEYWORD(KEYWORD_DOUBLE2INT) || \
    IS_KEYWORD(KEYWORD_LENGTH) || IS_KEYWORD(KEYWORD_SUBSTRING) || IS_KEYWORD(KEYWORD_ORD) || \
    IS_KEYWORD(KEYWORD_CHR) || IS_ID_FUNC())

// kontrola pravidel a tokenů
#define CHECK_RULE(rule)    \
    if((result = rule(data))) return result

#define CHECK_TYPE(compare)    \
    if(!IS_TYPE(compare)) return ERROR_SYNTACTIC_ANALYSIS

#define CHECK_KEYWORD(compare)  \
    if(!IS_KEYWORD(compare)) return ERROR_SYNTACTIC_ANALYSIS

// kombinovaná makra
#define GET_TOKEN_AND_CHECK_RULE(rule)    \
    do{                     \
        GET_TOKEN();        \
        CHECK_RULE(rule);   \
    }while(0)

#define GET_TOKEN_AND_CHECK_TYPE(compare)    \
    do{                     \
        GET_TOKEN();        \
        CHECK_TYPE(compare);   \
    }while(0)

#define GET_TOKEN_AND_CHECK_KEYWORD(compare)  \
    do{                             \
        GET_TOKEN();                \
        CHECK_KEYWORD(compare);     \
    }while(0)

/**
 * @brief funkce pro vyhodnoceni neterminálu prog
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int prog(parser_data *data){
    int result;

    // zpracování příslušného pravidla gramatiky
    if(IS_KEYWORD(KEYWORD_FUNC)){
        //<prog> -> FUNC ID ( <param> ) <rtype> { <command_func> } <prog>
        
        printf("%s\n", data->token.attribute.newline_before?"true":"false");
        printf("%i\n", data->prev_token);
        CHECK_COMMAND_VIABILITY();
        printf("po\n");
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_IDENTIFIER);

        if(data->loaded_id == NULL){
            // identifikátor nebyl v tabulce symbolů
            char *name = malloc(data->token.attribute.string_size);
            data->curr_args = malloc(sizeof(arg_list_t));
            if((name == NULL) || (data->curr_args == NULL)){
                return ERROR_INTERNAL;
            }
            strcpy(name, data->token.attribute.string);
            initArgList(data->curr_args);
            data->curr_func_name = name;
            data->curr_func = NULL;

        }else{
            // identifikátor je v tabulce symbolů
            if(data->loaded_id->data.type == TYPE_FUNC){
                // identifikator byl funkce, kontrola

                if(data->loaded_id->data.defined == true){
                    return ERROR_SEMANTIC_DEFINING;
                }
                data->curr_func = data->loaded_id;
                data->curr_args = data->curr_func->data.args;
                FirstArg(data->curr_args);

            }else{
                // identifikator nebyl funkce
                return ERROR_SEMANTIC_DEFINING;
            }
        }
        
        generate_func_start(data->token.attribute.string);

        if(!createFrame(&(data->var_frame)))return ERROR_INTERNAL;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(param);

        if(data->curr_func != NULL){
            if(data->curr_args->active != NULL){
                // bylo volano s vice argumenty
                return ERROR_SEMANTIC_FUNCTION;
            }
        }

        // generovani vyndani parametru ze zasobniku
        var_listElem_t *tmp = data->var_frame->list;
        while(tmp != NULL){
            pop_arg(tmp->name);
            tmp = tmp->next;
        }


        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(rtype);

        if(data->curr_func == NULL){
            htabInsert(data->sym_table, data->curr_func_name, data->curr_func_name, TYPE_FUNC, data->data_type, data->curr_args, true);
            data->curr_func = htabSearch(data->sym_table, data->curr_func_name);
            if(data->curr_func == NULL) return ERROR_INTERNAL;
        }else{
            if(data->curr_func->data.data_type != data->data_type){
                if(data->curr_func->data.data_type == DATA_TYPE_VOID){
                    // funkce byla volana bez prirazeni navratove hodnoty
                    data->curr_func->data.data_type = data->data_type;
                }else{
                    return ERROR_SEMANTIC_FUNCTION;
                }
            }
            data->curr_func->data.defined = true;
            deleteArg(data->undefined_func, data->curr_func->data.name);
        }

        CHECK_TYPE(TOKEN_LEFT_CURLY_BRACE);
        data->in_function = true;
        data->function_return = false;
        GET_TOKEN_AND_CHECK_RULE(command);
        data->in_function = false;
        CHECK_TYPE(TOKEN_RIGHT_CURLY_BRACE);
        
        if((data->curr_func->data.data_type != DATA_TYPE_VOID) && !(data->function_return))return ERROR_SEMANTIC_NO_RETURN;
        destroyFrame(&(data->var_frame), data->sym_table);

        generate_func_end(data->curr_func->data.name, (data->curr_func->data.data_type == DATA_TYPE_VOID)?true:false);

        GET_TOKEN_AND_CHECK_RULE(prog);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_EOF)){
        //<prog> -> EOF
        
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_VAR) || IS_KEYWORD(KEYWORD_LET) || IS_KEYWORD(KEYWORD_IF) || 
            IS_KEYWORD(KEYWORD_WHILE) || IS_ID_CONST() || IS_ID_VAR() || IS_FUNC()){
        //<prog> -> <command><prog>
        
        CHECK_RULE(command);
        GET_TOKEN_AND_CHECK_RULE(prog);
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec prog

/**
 * @brief funkce pro vyhodnoceni neterminálu command a command_func
 * command a command_func jsou identické až na pravidlo s RETURN,
 * proto jsou implementovány jako jedna funcce a pomoci in_function v parser_data
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int command(parser_data *data){
    int result;

    if(IS_TYPE(TOKEN_IDENTIFIER)) data->loaded_id = htabSearch(data->sym_table, data->token.attribute.string);

    // zpracování příslušného pravidla gramatiky
    if(IS_KEYWORD(KEYWORD_VAR)){
        //<command> -> VAR ID <dtype> <init> <command>

        CHECK_COMMAND_VIABILITY();
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_IDENTIFIER);

        // kontrola kolize
        if(data->in_function || data->in_block){
            if(searchVar(data->var_frame, data->token.attribute.string)) return ERROR_SEMANTIC_DEFINING;
            generate_variable(data->token.attribute.string, true);
        }else{
            if(data->loaded_id != NULL) return ERROR_SEMANTIC_DEFINING;
            generate_variable(data->token.attribute.string, false);
        }

        data->is_defined = false;
        data->curr_func_name = malloc(data->token.attribute.string_size);
        if(data->curr_func_name == NULL) return ERROR_INTERNAL;
        strcpy(data->curr_func_name, data->token.attribute.string);

        GET_TOKEN_AND_CHECK_RULE(dtype);
        tab_data_type defined = data->data_type;
        CHECK_RULE(init);
        
        if(data->data_type == defined){
            if(defined == DATA_TYPE_UNDEFINED)return ERROR_SEMANTIC_TYPE_UNKNOWN;
        }else if(defined == DATA_TYPE_UNDEFINED){
            if(data->data_type == DATA_TYPE_BOOL)return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
            defined = data->data_type;
        }else{
            return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
        }
        if(data->in_function || data->in_block){
            // pridat do lokalnich promennych
            if(!addVar(data->var_frame, data->curr_func_name)) return ERROR_INTERNAL;
        }

        htabInsert(data->sym_table, data->curr_func_name, data->curr_func_name, TYPE_VARIABLE, defined, NULL, data->is_defined);

        CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_LET)){
        //<command> -> LET ID <dtype> <init> <command>

        CHECK_COMMAND_VIABILITY();
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_IDENTIFIER);

        // kontrola kolize
        if(data->in_function || data->in_block){
            if(searchVar(data->var_frame, data->token.attribute.string)) return ERROR_SEMANTIC_DEFINING;
            generate_variable(data->token.attribute.string, true);
        }else{
            if(data->loaded_id != NULL) return ERROR_SEMANTIC_DEFINING;
            generate_variable(data->token.attribute.string, false);
        }

        data->is_defined = false;
        data->curr_func_name = malloc(data->token.attribute.string_size);
        if(data->curr_func_name == NULL) return ERROR_INTERNAL;
        strcpy(data->curr_func_name, data->token.attribute.string);

        GET_TOKEN_AND_CHECK_RULE(dtype);
        tab_data_type defined = data->data_type;
        CHECK_RULE(init);
        
        if(data->data_type == defined){
            if(defined == DATA_TYPE_UNDEFINED)return ERROR_SEMANTIC_TYPE_UNKNOWN;
        }else if(defined == DATA_TYPE_UNDEFINED){
            if(data->data_type == DATA_TYPE_BOOL)return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
            defined = data->data_type;
        }else{
            return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
        }
        if(data->in_function || data->in_block){
            // pridat do lokalnich promennych
            if(!addVar(data->var_frame, data->curr_func_name)) return ERROR_INTERNAL;
        }

        htabInsert(data->sym_table, data->curr_func_name, data->curr_func_name, TYPE_CONSTANT, defined, NULL, data->is_defined);
        
        CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_IF)){
        //<command> -> IF <expr> { <command> } ELSE { <command> } <command>
        
        CHECK_COMMAND_VIABILITY();

        int label_num = data->label_num;
        data->label_num++;
        
        clear_stack();
        GET_TOKEN_AND_CHECK_RULE(expr);

        if(data->data_type != DATA_TYPE_BOOL)return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
        CHECK_TYPE(TOKEN_LEFT_CURLY_BRACE);
        data->in_block = true;

        generate_if_start(label_num);

        if(!createFrame(&(data->var_frame)))return ERROR_INTERNAL;
        GET_TOKEN_AND_CHECK_RULE(command);
        destroyFrame(&(data->var_frame), data->sym_table);

        generate_if_middle(label_num);

        CHECK_TYPE(TOKEN_RIGHT_CURLY_BRACE);
        GET_TOKEN_AND_CHECK_KEYWORD(KEYWORD_ELSE);
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_CURLY_BRACE);

        if(!createFrame(&(data->var_frame)))return ERROR_INTERNAL;
        GET_TOKEN_AND_CHECK_RULE(command);
        destroyFrame(&(data->var_frame), data->sym_table);

        generate_if_end(label_num);

        data->in_block = false;
        CHECK_TYPE(TOKEN_RIGHT_CURLY_BRACE);
        GET_TOKEN_AND_CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_WHILE)){
        //<command> -> WHILE <expr> { <command> } <command>
        
        CHECK_COMMAND_VIABILITY();
        
        int label_num = data->label_num;
        data->label_num++;

        generate_while_start(label_num);

        clear_stack();
        GET_TOKEN_AND_CHECK_RULE(expr);

        if(data->data_type != DATA_TYPE_BOOL)return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
        CHECK_TYPE(TOKEN_LEFT_CURLY_BRACE);
        data->in_block = true;

        generate_while_middle(label_num);

        if(!createFrame(&(data->var_frame)))return ERROR_INTERNAL;
        GET_TOKEN_AND_CHECK_RULE(command);
        destroyFrame(&(data->var_frame), data->sym_table);

        generate_while_end(label_num);

        data->in_block = false;
        CHECK_TYPE(TOKEN_RIGHT_CURLY_BRACE);
        GET_TOKEN_AND_CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_ID_CONST() || IS_ID_VAR()){
        //<command> -> ID = <assign> <command>

        CHECK_COMMAND_VIABILITY();
        if(data->loaded_id == NULL) return ERROR_SEMANTIC_VARIABLE;
        data->curr_id = data->loaded_id;
        if((data->curr_id->data.type == TYPE_CONSTANT) && (data->curr_id->data.defined)) return ERROR_SEMANTIC_DEFINING;
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_ASSIGN_EQUAL);
        data->data_type = data->curr_id->data.data_type;
        GET_TOKEN_AND_CHECK_RULE(assign);
        data->curr_id->data.defined = true;

        if(data->data_type != data->curr_id->data.data_type){
            return ERROR_SEMANTIC_TYPE_UNCOMPATIBLE;
        }

        generate_assign(data->curr_id->data.name, searchVar(data->var_frame, data->curr_id->data.name));

        CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_FUNC()){
        //<command> -> <func_call> <command>
        
        CHECK_COMMAND_VIABILITY();
        data->data_type = DATA_TYPE_UNDEFINED;
        htabItem_t *func = data->curr_func;
        CHECK_RULE(func_call);
        data->curr_func = func;
        GET_TOKEN_AND_CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_FUNC) || IS_TYPE(TOKEN_EOF) || IS_TYPE(TOKEN_RIGHT_CURLY_BRACE)){
        //<command> -> epsilon
        
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_RETURN) && data->in_function){
        //<command> -> RETURN <expr> <command_func>
        
        CHECK_COMMAND_VIABILITY();
        GET_TOKEN();
        if(IS_ID_CONST() || IS_ID_VAR() || IS_TYPE(TOKEN_FLOAT) || IS_TYPE(TOKEN_STRING) || IS_TYPE(TOKEN_INTEGER)){
            // za RETURN se nachazi nejaka hodnota -> vyhodnotit vyraz

            clear_stack();
            GET_TOKEN_AND_CHECK_RULE(expr);
            if(data->curr_func->data.data_type != data->data_type)return ERROR_SEMANTIC_FUNCTION;

        }else{
            // za RETURN se nenachazi nejaka hodnota -> nic nevracet
            if(data->curr_func->data.data_type != DATA_TYPE_VOID)return ERROR_SEMANTIC_FUNCTION;
        }

        generate_return();

        data->function_return = true;
        CHECK_RULE(command);
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec command

/**
 * @brief funkce pro vyhodnoceni neterminálu param
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int param(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_IDENTIFIER)){
        //<param> -> <param_val> <param_more>
        
        CHECK_RULE(param_val);
        GET_TOKEN_AND_CHECK_RULE(param_more);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_RIGHT_PARENTHESIS)){
        //<param> -> epsilon
        
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec param

/**
 * @brief funkce pro vyhodnoceni neterminálu param_more
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int param_more(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_COMMA)){
        //<param_more> -> , <param_val> <param_more>
        
        GET_TOKEN_AND_CHECK_RULE(param_val);
        GET_TOKEN_AND_CHECK_RULE(param_more);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_RIGHT_PARENTHESIS)){
        //<param_more> -> epsilon
        
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec param_more

/**
 * @brief funkce pro vyhodnoceni neterminálu param_val
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int param_val(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_IDENTIFIER)){
        //<param_val> -> ID ID : <type>
        
        char *arg_name = NULL;
        if(data->curr_func != NULL){
            // funkce je definována, alokuji prostor pro jmeno
            if(strcmp(data->token.attribute.string, "_")){
                //řetězec je rozdílný od "_"
                arg_name = malloc(data->token.attribute.string_size);
                if(arg_name == NULL){
                    return ERROR_INTERNAL;
                }
                strcpy(arg_name, data->token.attribute.string);
            }
        }else{
            // funkce již byla volána, kontrola
            if(data->curr_args->active == NULL){
                // bylo volano s mene argumenty
                return ERROR_SEMANTIC_FUNCTION;
            }

            if(strcmp(data->token.attribute.string, "_")){
                if(data->curr_args->active->name != NULL){
                    return ERROR_SEMANTIC_FUNCTION;
                }
            }else{
                if(!strcmp(data->curr_args->active->name, data->token.attribute.string)){
                    return ERROR_SEMANTIC_FUNCTION;
                }
            }
        }

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_IDENTIFIER);

        char *arg_var = NULL;
        if(data->curr_func != NULL){
            // funkce je definována, alokuji prostor pro jmeno
            if(strcmp(data->token.attribute.string, data->curr_func_name)){
                return ERROR_SEMANTIC_DEFINING;
            }
            
            arg_var = malloc(data->token.attribute.string_size);
            if(arg_var == NULL){
                return ERROR_INTERNAL;
            }
            strcpy(arg_var, data->token.attribute.string);
        }else{
            // funkce již byla volána, kontrola
        }

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_AND_CHECK_RULE(type);

        if(data->curr_func != NULL){
            // funkce je definována vytvorim potrebne záznamy
            if(data->curr_args->first == NULL){
                if(!insertFirstArg(data->curr_args, arg_name, data->data_type))return ERROR_INTERNAL;
                FirstArg(data->curr_args);
            }else{
                if(!insertAfterArg(data->curr_args, arg_name, data->data_type))return ERROR_INTERNAL;
                NextArg(data->curr_args);
            }
        }else{
            // funkce již byla volána, kontrola
            if(data->curr_args->active->type != data->data_type){
                if(data->curr_args->active->type != DATA_TYPE_UNDEFINED)return ERROR_SEMANTIC_FUNCTION;
                switch(data->curr_args->active->type){
                    case DATA_TYPE_STRING_NIL:
                        data->curr_args->active->type = DATA_TYPE_STRING_NIL;
                        break;
                    case DATA_TYPE_INT_NIL:
                        data->curr_args->active->type = DATA_TYPE_INT_NIL;
                        break;
                    case DATA_TYPE_DOUBLE_NIL:
                        data->curr_args->active->type = DATA_TYPE_DOUBLE_NIL;
                        break;
                    default:
                        return ERROR_SEMANTIC_FUNCTION;
                }

            }
            NextArg(data->curr_args);
        }

        if(!addVar(data->var_frame, arg_var))return ERROR_INTERNAL;
        htabInsert(data->sym_table, arg_var, arg_var, TYPE_VARIABLE, data->data_type, NULL, true);

        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec param_val

/**
 * @brief funkce pro vyhodnoceni neterminálu arg
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int arg(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_IDENTIFIER) || IS_TYPE(TOKEN_STRING) || IS_TYPE(TOKEN_INTEGER) || IS_TYPE(TOKEN_FLOAT) || IS_KEYWORD(KEYWORD_NIL)){
        //<arg> -> <arg_val> <arg_more>
        
        CHECK_RULE(arg_val);
        GET_TOKEN_AND_CHECK_RULE(arg_more);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_RIGHT_PARENTHESIS)){
        //<arg> -> epsilon
        
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec arg

/**
 * @brief funkce pro vyhodnoceni neterminálu arg_more
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int arg_more(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_COMMA)){
        //<param_more> -> , <arg_val> <arg_more>

        GET_TOKEN_AND_CHECK_RULE(arg_val);
        GET_TOKEN_AND_CHECK_RULE(arg_more);
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_RIGHT_PARENTHESIS)){
        //<param_more> -> epsilon
        
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec arg_more

/**
 * @brief funkce pro vyhodnoceni neterminálu arg_val
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int arg_val(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_ID_CONST() || IS_ID_VAR() || IS_TYPE(TOKEN_STRING) || IS_TYPE(TOKEN_INTEGER) || IS_TYPE(TOKEN_FLOAT) || IS_KEYWORD(KEYWORD_NIL)){
        //<arg_val> -> <val>
        
        CHECK_RULE(val);
        if(data->is_write){
            // volana funkce je write
            if(data->loaded_id == NULL){
                // je literal
                switch(data->arg_type){
                    case(DATA_TYPE_STRING):
                        write_string(data->token.attribute.string);
                        break;
                    case(DATA_TYPE_INT):
                        write_int(data->token.attribute.integer);
                        break;
                    case(DATA_TYPE_DOUBLE):
                        write_double(data->token.attribute.decimal);
                        break;
                    default:
                        break;
                }
            }else{
                // je identifikator
                write_id(data->token.attribute.string, searchVar(data->var_frame, data->token.attribute.string));
            }

        }else{
            // volana funkce neni write
            if(data->is_defined){
                if(data->curr_args->first == NULL){
                    if(!insertFirstArg(data->curr_args, NULL, data->arg_type)) return ERROR_INTERNAL;
                    FirstArg(data->curr_args);
                }else{
                    if(!insertAfterArg(data->curr_args, NULL, data->arg_type)) return ERROR_INTERNAL;
                    NextArg(data->curr_args);
                }
            }else{
                if(data->curr_args->active == NULL)return ERROR_SEMANTIC_FUNCTION;
                if(data->arg_type != data->curr_args->active->type) return ERROR_SEMANTIC_FUNCTION;
                NextArg(data->curr_args);
            }
        }
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_IDENTIFIER) && !data->is_write){
        //<arg_val> -> ID : <val>
        char* name = NULL;
        if(data->is_defined){
            name = malloc(data->token.attribute.string_size);
            if(name == NULL)return ERROR_INTERNAL;
            strcpy(name, data->token.attribute.string);
        }else{
            if(data->curr_args->active == NULL)return ERROR_SEMANTIC_FUNCTION;
            if(!strcmp(data->curr_args->active->name, data->token.attribute.string))return ERROR_SEMANTIC_FUNCTION;
        }

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_COLON);
        GET_TOKEN_AND_CHECK_RULE(val);

        if(data->is_defined){
            if(data->curr_args->first == NULL){
                if(!insertFirstArg(data->curr_args, name, data->arg_type)) return ERROR_INTERNAL;
                FirstArg(data->curr_args);
            }else{
                if(!insertAfterArg(data->curr_args, name, data->arg_type)) return ERROR_INTERNAL;
                NextArg(data->curr_args);
            }
        }else{
            if(data->arg_type != data->curr_args->active->type) return ERROR_SEMANTIC_FUNCTION;
            NextArg(data->curr_args);
        }

        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec arg_val

/**
 * @brief funkce pro vyhodnoceni neterminálu func_call
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int func_call(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_ID_FUNC()){
        //<func_call> -> ID (<arg>)
        
        if(data->loaded_id == NULL){
            // funkce neni definovana, vytvoreni nove funkce
            data->curr_func_name = malloc(data->token.attribute.string_size);
            if(data->curr_func_name == NULL) return ERROR_INTERNAL;
            strcpy(data->curr_func_name, data->token.attribute.string);
            data->curr_args = malloc(sizeof(arg_list_t));
            if(data->curr_args == NULL) return ERROR_INTERNAL;
            initArgList(data->curr_args);
            if(!insertFirstArg(data->undefined_func, data->curr_func_name, DATA_TYPE_UNDEFINED))return ERROR_INTERNAL;
            data->is_defined = false;
        }else{
            if(data->loaded_id->data.type == TYPE_FUNC){
                // funkce byla definovana nebo volana, kontrola
                data->curr_func = data->loaded_id;
                data->curr_args = data->curr_func->data.args;
                data->curr_func_name = data->curr_func->data.name;
                FirstArg(data->curr_args);
                data->is_defined = true;
            }else{
                return ERROR_SEMANTIC_OTHER;
            }
        }

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func_name);

        if(data->curr_func == NULL){
            if(data->data_type == DATA_TYPE_UNDEFINED){
                data->data_type = DATA_TYPE_VOID;
            }

            htabInsert(data->sym_table, data->curr_func_name, data->curr_func_name, TYPE_FUNC, data->data_type, data->curr_args, false);

        }else{
            if(data->curr_args->active != NULL) return ERROR_SEMANTIC_FUNCTION;
            if(data->curr_func->data.data_type == DATA_TYPE_UNDEFINED){
                data->data_type = data->curr_func->data.data_type;
            }else if(data->data_type != data->curr_func->data.data_type){
                if((!data->curr_func->data.defined) && (data->curr_func->data.data_type == DATA_TYPE_VOID)){
                    data->curr_func->data.data_type = data->data_type;
                }else{
                    return ERROR_SEMANTIC_FUNCTION;
                }
            }
            data->curr_func = NULL;
            data->curr_args = NULL;
        }

        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_READSTRING)){
        //<func_call> -> READSTRING (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "readString");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_READINT)){
        //<func_call> -> READINT (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "readInt");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_READDOUBLE)){
        //<func_call> -> READDOUBLE (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "readDouble");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_WRITE)){
        //<func_call> -> WRITE (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "write");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;
        
        data->is_write = true;
        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);
        data->is_write = false;

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_INT2DOUBLE)){
        //<func_call> -> INT2DOUBLE (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "Int2Double");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_DOUBLE2INT)){
        //<func_call> -> DOUBLE2INT (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "Double2Int");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_LENGTH)){
        //<func_call> -> LENGTH (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "length");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_SUBSTRING)){
        //<func_call> -> SUBSTRING (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "substring");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_ORD)){
        //<func_call> -> ORD (<arg>)
        
        data->curr_func = htabSearch(data->sym_table, "ord");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_CHR)){
        //<func_call> -> CHR (<arg>)

        data->curr_func = htabSearch(data->sym_table, "chr");
        if(data->curr_func == NULL)return ERROR_INTERNAL;
        data->curr_args = data->curr_func->data.args;
        FirstArg(data->curr_args);
        data->is_defined = true;

        GET_TOKEN_AND_CHECK_TYPE(TOKEN_LEFT_PARENTHESIS);
        GET_TOKEN_AND_CHECK_RULE(arg);
        CHECK_TYPE(TOKEN_RIGHT_PARENTHESIS);

        generate_func_call(data->curr_func->data.name);

        if(data->data_type == DATA_TYPE_UNDEFINED){
            data->data_type = data->curr_func->data.data_type;
        }else if(data->data_type != data->curr_func->data.data_type){
            return ERROR_SEMANTIC_FUNCTION;
        }
        data->curr_func = NULL;
        data->curr_args = NULL;
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec func_call

/**
 * @brief funkce pro vyhodnoceni neterminálu assign
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int assign(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_FUNC()){
        //<assign> -> <func_call>

        htabItem_t *func = data->curr_func;
        CHECK_RULE(func_call);
        data->curr_func = func;
        GET_TOKEN();
        if(data->data_type == DATA_TYPE_VOID)return ERROR_SEMANTIC_FUNCTION;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_FLOAT) || IS_TYPE(TOKEN_STRING) || IS_TYPE(TOKEN_INTEGER) || IS_TYPE(TOKEN_IDENTIFIER)){
        //<assign> -> <expr>
        clear_stack();
        CHECK_RULE(expr);
        return COMPILATION_SUCCESSFUL;

    }else{
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec assign

/**
 * @brief funkce pro vyhodnoceni neterminálu init
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int init(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_ASSIGN_EQUAL)){
        //<init> -> = <assign>
        
        GET_TOKEN_AND_CHECK_RULE(assign);

        generate_assign(data->curr_func_name, (data->in_function || data->in_block));

        data->is_defined = true;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_FUNC() || IS_TYPE(TOKEN_RIGHT_CURLY_BRACE) || IS_TYPE(TOKEN_EOF) || 
            IS_TYPE(TOKEN_IDENTIFIER) || IS_KEYWORD(KEYWORD_FUNC) || IS_KEYWORD(KEYWORD_VAR) || IS_KEYWORD(KEYWORD_LET) || 
            IS_KEYWORD(KEYWORD_RETURN) || IS_KEYWORD(KEYWORD_IF) || IS_KEYWORD(KEYWORD_WHILE)){
        //<init> -> epsilon
        
        if(data->data_type == DATA_TYPE_UNDEFINED){
            //nelze odvodit datovy typ
            return ERROR_SEMANTIC_TYPE_UNKNOWN;
        }
        // pokud chybí inicializace, pokud je datový typ s NIL bude inicializován
        if((data->data_type == DATA_TYPE_STRING) || (data->data_type == DATA_TYPE_INT) || (data->data_type == DATA_TYPE_DOUBLE)){
            data->is_defined = false;
        }else{
            data->is_defined = true;
            
            push_nil();
            generate_assign(data->curr_func_name, (data->in_function || data->in_block));
        }
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec init

/**
 * @brief funkce pro vyhodnoceni neterminálu rtype
 * function_return
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int rtype(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_RETURN_TYPE_SYMBOL)){
        //<rtype> -> -> <type>

        GET_TOKEN_AND_CHECK_RULE(type);
        data->curr_func->data.data_type = data->data_type;
        GET_TOKEN();
        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_LEFT_CURLY_BRACE)){
        //<rtype> -> epsilon

        data->curr_func->data.data_type = DATA_TYPE_VOID;
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec rtype

/**
 * @brief funkce pro vyhodnoceni neterminálu dtype
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int dtype(parser_data *data){
    int result;
    
    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_COLON)){
        //<dtype> -> : <type>

        GET_TOKEN_AND_CHECK_RULE(type);
        GET_TOKEN();
        return COMPILATION_SUCCESSFUL;

    }else if(IS_FUNC() || IS_TYPE(TOKEN_ASSIGN_EQUAL) || IS_TYPE(TOKEN_RIGHT_CURLY_BRACE) || IS_TYPE(TOKEN_EOF) || 
            IS_TYPE(TOKEN_IDENTIFIER) || IS_KEYWORD(KEYWORD_FUNC) || IS_KEYWORD(KEYWORD_VAR) || IS_KEYWORD(KEYWORD_LET) || 
            IS_KEYWORD(KEYWORD_RETURN) || IS_KEYWORD(KEYWORD_IF) || IS_KEYWORD(KEYWORD_WHILE)){
        //<dtype> -> epsilon

        data->data_type = DATA_TYPE_UNDEFINED;
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec dtype

/**
 * @brief funkce pro vyhodnoceni neterminálu type
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int type(parser_data *data){
    
    // zpracování příslušného pravidla gramatiky
    if(IS_KEYWORD(KEYWORD_STRING)){
        //<type> -> STRING

        data->data_type = DATA_TYPE_STRING;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_INT)){
        //<type> -> INT

        data->data_type = DATA_TYPE_INT;
        return COMPILATION_SUCCESSFUL;        
    
    }else if(IS_KEYWORD(KEYWORD_DOUBLE)){
        //<type> -> DOUBLE

        data->data_type = DATA_TYPE_DOUBLE;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_STRING_NIL)){
        //<type> -> STRING?

        data->data_type = DATA_TYPE_STRING_NIL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_INT_NIL)){
        //<type> -> INT?

        data->data_type = DATA_TYPE_INT_NIL;
        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_DOUBLE_NIL)){
        //<type> -> DOUBLE?

        data->data_type = DATA_TYPE_DOUBLE_NIL;
        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec type

/**
 * @brief funkce pro vyhodnoceni neterminálu val
 * 
 * @param data interni data
 * @return příslušnou návratovou hodnotu
 */
static int val(parser_data *data){

    // zpracování příslušného pravidla gramatiky
    if(IS_TYPE(TOKEN_STRING)){
        //<val> -> STRING_VAL

        data->loaded_id = NULL;
        data->arg_type = DATA_TYPE_STRING;

        if(!data->is_write){
            push_string(data->token.attribute.string);
        }

        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_INTEGER)){
        //<val> -> INT_VAL

        data->loaded_id = NULL;
        data->arg_type = DATA_TYPE_INT;
        
        if(!data->is_write){
            push_int(data->token.attribute.integer);
        }

        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_FLOAT)){
        //<val> -> DOUBLE_VAL

        data->loaded_id = NULL;
        data->arg_type = DATA_TYPE_DOUBLE;

        if(!data->is_write){
            push_double(data->token.attribute.decimal);
        }

        return COMPILATION_SUCCESSFUL;

    }else if(IS_TYPE(TOKEN_IDENTIFIER)){
        //<val> -> ID

        data->arg_type = data->loaded_id->data.data_type;

        if(!data->is_write){
            push_id(data->loaded_id->data.name, searchVar(data->var_frame, data->loaded_id->data.name));
        }

        return COMPILATION_SUCCESSFUL;

    }else if(IS_KEYWORD(KEYWORD_NIL)){
        //<val> -> NIL

        data->loaded_id = NULL;
        data->arg_type = DATA_TYPE_UNDEFINED;

        if(!data->is_write){
            push_nil();
        }

        return COMPILATION_SUCCESSFUL;

    }else{
        // pravidlo nenalezeno
        return ERROR_SYNTACTIC_ANALYSIS;
    }

}// konec val

/**
 * @brief alokuje prostor a inicializuje interní data
 * 
 * @return ukazatel na alokovanou a inicializovanou strukturu, pokud alokace selže NULL
 */
static parser_data* init_parser(){
    // alokace interní struktury
    parser_data *data = malloc(sizeof(parser_data));
    if(data == NULL){
        exit(ERROR_INTERNAL);
    }

    // alokace hashovací tabulky
    data->sym_table = htabInit();
    if(data->sym_table == NULL){
        exit(ERROR_INTERNAL);
    }

    // vložení vestavěných funkcí do hashovací tabulky
    // func readString()->String?
    char *func_name = malloc(strlen("readString")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "readString");
    arg_list_t *args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_STRING_NIL, args, true);

    // func readInt()->Int?
    func_name = malloc(strlen("readInt")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "readInt");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_INT_NIL, args, true);
    
    // func readDouble()->Double?
    func_name = malloc(strlen("readDouble")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "readDouble");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_DOUBLE_NIL, args, true);
    
    // func write(term, term, ..., term)
    func_name = malloc(strlen("write")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "write");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_VOID, args, true);
    
    // func Int2Double(_ term:Int)->Double
    func_name = malloc(strlen("Int2Double")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "Int2Double");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    if(!insertFirstArg(args, NULL, DATA_TYPE_INT))exit(ERROR_INTERNAL);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_DOUBLE, args, true);
    
    // func Double2Int(_ term:Double)->Int
    func_name = malloc(strlen("Double2Int")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "Double2Int");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    if(!insertFirstArg(args, NULL, DATA_TYPE_DOUBLE))exit(ERROR_INTERNAL);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_INT, args, true);
    
    // func length(_ s:String)->Int
    func_name = malloc(strlen("length")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "length");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    if(!insertFirstArg(args, NULL, DATA_TYPE_STRING))exit(ERROR_INTERNAL);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_INT, args, true);
    
    // func substring(of s:String, startingAt i:Int, endingBefore j:Int)->String?
    func_name = malloc(strlen("substring")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "substring");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);

    char *arg_name = malloc(strlen("of")+1);
    if(arg_name == NULL)exit(ERROR_INTERNAL);
    strcpy(arg_name, "of");
    if(!insertFirstArg(args, arg_name, DATA_TYPE_STRING))exit(ERROR_INTERNAL);
    FirstArg(args);
    arg_name = malloc(strlen("startingAt")+1);
    if(arg_name == NULL)exit(ERROR_INTERNAL);
    strcpy(arg_name, "startingAt");
    if(!insertAfterArg(args, arg_name, DATA_TYPE_INT))exit(ERROR_INTERNAL);
    NextArg(args);
    arg_name = malloc(strlen("endingBefore")+1);
    if(arg_name == NULL)exit(ERROR_INTERNAL);
    strcpy(arg_name, "endingBefore");
    if(!insertAfterArg(args, arg_name, DATA_TYPE_INT))exit(ERROR_INTERNAL);

    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_STRING_NIL, args, true);
    
    // func ord(_ c:String)->Int
    func_name = malloc(strlen("ord")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "ord");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    if(!insertFirstArg(args, NULL, DATA_TYPE_STRING))exit(ERROR_INTERNAL);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_INT, args, true);
    
    // func chr(_ i:Int)->String
    func_name = malloc(strlen("chr")+1);
    if(func_name == NULL)exit(ERROR_INTERNAL);
    strcpy(func_name, "chr");
    args = malloc(sizeof(arg_list_t));
    if(args == NULL)exit(ERROR_INTERNAL);
    initArgList(args);
    if(!insertFirstArg(args, NULL, DATA_TYPE_INT))exit(ERROR_INTERNAL);
    htabInsert(data->sym_table, func_name, func_name, TYPE_FUNC, DATA_TYPE_STRING, args, true);
    

    // inicializace ostatních proměnných
    data->loaded_id = NULL;
    data->curr_func = NULL;
    data->curr_id = NULL;
    data->var_frame = NULL;
    data->curr_args = NULL;
    data->curr_func_name = NULL;

    data->prev_token = TOKEN_UNKNOWN;
    data->data_type = DATA_TYPE_UNDEFINED;
    data->arg_type = DATA_TYPE_UNDEFINED;

    data->undefined_func = malloc(sizeof(arg_list_t));
    if(data->undefined_func == NULL) exit(ERROR_INTERNAL);
    initArgList(data->undefined_func);

    data->in_function = false;
    data->in_block = false;
    data->is_write = false;
    data->is_defined = false;
    data->function_return = false;
    data->label_num = 0;
    
    return data;
}

int analyse(){
    parser_data *data = init_parser();
    if(data == NULL){
        return ERROR_INTERNAL;
    }

    // načtení prvního tokenu a spuštění analýzy
    int result;
    if((result = get_token(&data->token)) == SCANNER_TOKEN_SUCCESS){
        start_generator();
        result = prog(data);
        end_generator();
    }

    // ukončení
    htabDeleteAll(data->sym_table);
    destroyList(&data->var_frame);
    destroyArgList(data->curr_args);
    destroyArgList(data->undefined_func);
    free(data->undefined_func);
    free(data->curr_args);
    free(data->curr_func_name);
    free(data);
    return result;
}