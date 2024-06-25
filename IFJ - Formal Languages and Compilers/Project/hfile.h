/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/



//! DEFINICE VYJMUTE Z SYMTABLE.H A ARG_LIST.H
//! vznikala zde cyklicka zavislost, protoze oba hlavickove souboru potrebuji struktury definovane v tom druhem souboru

/** kontrola, ze se toto provede pouze jednou */
#ifndef __HFILEH__ 
#define __HFILEH__


typedef enum{
    DATA_TYPE_STRING,
    DATA_TYPE_INT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING_NIL,
    DATA_TYPE_INT_NIL,
    DATA_TYPE_DOUBLE_NIL,
    DATA_TYPE_VOID,
    DATA_TYPE_UNDEFINED,
    DATA_TYPE_BOOL,
}tab_data_type;



/**
 * @brief Prvek seznamu
 */
typedef struct arg_listElem{
    
    char *name;
    tab_data_type type;
    struct arg_listElem *next;

} arg_listElem_t;

/**
 * @brief Hlavička seznamu
 */
typedef struct{
    
    arg_listElem_t *first;
    arg_listElem_t *active;

} arg_list_t;



#endif