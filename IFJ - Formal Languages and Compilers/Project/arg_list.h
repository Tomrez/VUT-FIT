/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/


#ifndef ARG_LIST_H
#define ARG_LIST_H


#include "hfile.h"
#include "symtable.h"

#include <stdbool.h>


/**
 * @brief inicializuje seznam
 * 
 * @param list hlavička seznamu
 */
void initArgList(arg_list_t *list);

/**
 * @brief zjistí zda je seznam aktivní
 * 
 * @param list hlavička seznamu
 * @return true pokud je aktivní
 * @return false pokud není aktivní
 */
bool isActiveArg(arg_list_t *list);

/**
 * @brief nastavi aktivní prvek na první prvek
 * 
 * @param list hlavička seznamu
 */
void FirstArg(arg_list_t *list);

/**
 * @brief nastavi aktivní prvek na nasledovníka aktivního prvku
 * 
 * @param list hlavička seznamu
 */
void NextArg(arg_list_t *list);

/**
 * @brief vytvoří první prvek
 * 
 * @param list hlavička seznamu
 * @param name pomocný název argumentu
 * @param type datový typ argumentu
 * @return true pokud je vložení úspěšné
 * @return false pokud je vložení neúspěšné
 */
bool insertFirstArg(arg_list_t *list, char *name, tab_data_type type);

/**
 * @brief vytvoří následovníka za aktivní prvek
 * 
 * @param list hlavička seznamu
 * @param name pomocný název argumentu
 * @param type datový typ argumentu
 * @return true pokud je vložení úspěšné
 * @return false pokud je vložení neúspěšné
 */
bool insertAfterArg(arg_list_t *list, char *name, tab_data_type type);

/**
 * @brief odstraní záznam ze seznamu pokud existuje
 * 
 * @param list hlavička seznamu
 * @param arg_name nazev hledaneho
 */
void deleteArg(arg_list_t *list, char *name);

/**
 * @brief vyprázdní seznam
 * 
 * @param list hlavička seznamu
 */
void destroyArgList(arg_list_t *list);

#endif