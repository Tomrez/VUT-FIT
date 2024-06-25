/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#ifndef VAR_LIST_H
#define VAR_LIST_H

#include "symtable.h"

/**
 * @brief Prvek seznamu
 */
typedef struct var_listElem{
    
    char *name;
    struct var_listElem *next;

} var_listElem_t;

/**
 * @brief Prvek seznamu seznamů
 */
typedef struct var_list{
    
    var_listElem_t *list;
    struct var_list *next;

} var_list_t;

/**
 * @brief vytvoří novou položku (rámec)
 * 
 * @param list ukazatel na ukazatel v parser_data
 * @return true pokud se vložení podaří
 * @return false pokud se vložení nepodaří
 */
bool createFrame(var_list_t **list);

/**
 * @brief vytvoří novou položku (identifikator)
 * 
 * @param list ukazatel v parser_data
 * @param name vládaný identifikator
 * @return true pokud se vložení podaří
 * @return false pokud se vložení nepodaří
 */
bool addVar(var_list_t *list, char *name);

/**
 * @brief prohledá aktuální rámec
 * 
 * @param list ukazatel v parser_data
 * @param name hledaný identifikátor
 * @return true pokud se identifikator nachází v aktuálním rámci
 * @return false pokud se identifikator nenachází v aktuálním rámci
 */
bool searchVar(var_list_t *list, char *name);

/**
 * @brief smaže aktualní položku (rámec) a odstraní příslušné identifikatory z tabulky symbolů
 * 
 * @param list ukazatel na ukazatel v parser_data 
 * @param table ukazatel na tabulku symbolů
 */
void destroyFrame(var_list_t **list, htab_t *table);

/**
 * @brief při chybě uvolní celou strukturu
 * 
 * @param list ukazatel na ukazatel v parser_data 
 */
void destroyList(var_list_t **list);

#endif