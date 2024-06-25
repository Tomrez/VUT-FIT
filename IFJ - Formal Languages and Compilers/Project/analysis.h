/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "symtable.h"
#include "scanner.h"
#include "arg_list.h"
#include "var_list.h"
#include "hfile.h"
#include <stdbool.h>

/**
 * @brief Datová struktura pro interní předáváni dat při analýze
 */
typedef struct{
    
    // ukazatel na tabulku symbolů
    htab_t *sym_table;

    // ukazatele na prvky v tabulce symbolů
    htabItem_t *loaded_id;
    htabItem_t *curr_func;
    htabItem_t *curr_id;

    // ukazatel na strukturu pro lokální proměnné
    var_list_t *var_frame;

    // ukazatel na aktuální argument
    arg_list_t *curr_args;

    // seznam nedefinovanych funkci
    arg_list_t *undefined_func;

    // pomocný datový typ načitané proměnné/funkce/parametru
    tab_data_type data_type;
    tab_data_type arg_type;

    // pomocný název aktualní proměnné/funkce/parametru
    char *curr_func_name;

    // načtený token
    data_type_token token;
    // typ predchoziho
    token_type prev_token;

    // indikace bloku prikazů uvnitř funkce/if/else/while
    bool in_function;
    bool in_block;

    // indikace volani write
    bool is_write;

    // indikace pro definici promenne
    bool is_defined;

    // indikace zda funkce obsahuje return
    bool function_return;

    // pomocna promena pro generovani unikatnich navesti
    int label_num;

} parser_data;

/**
 * @brief spustí syntaktickou a sémantickou analýzu
 * 
 * @return příslušnou návratovou hodnotu
 */
int analyse();

#endif