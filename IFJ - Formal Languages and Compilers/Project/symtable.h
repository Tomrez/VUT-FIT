/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/


/** kontrola, ze se toto provede pouze jednou */
#ifndef __HFILE__ 
#define __HFILE__

/** pripojeni potrebnych knihoven */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "arg_list.h"
#include "error.h"
#include "hfile.h"

/** Max velikost pole pro implementaci tabulky 32003*/
#define MAX_HT_SIZE 32003

/** Max velikost kroku pri praci se synonymy*/
#define MAX_STEP_SIZE 17

/** Chyba pri init */
#define SERR_INIT -1
/** Chyba pri ulozeni prvku */
#define SERR_INSERT -2
/** Chyba pri cteni prvku */
#define SERR_READ -3
/** Chyba pri mazani prvku */
#define SERR_DELETE -4

/** datove typy a struktury */

typedef enum{
    TYPE_VARIABLE,
    TYPE_CONSTANT,
    TYPE_FUNC,
}tab_type;

typedef struct{
    char *name;                 // identifikator
    tab_type type;              // typ struktury
    tab_data_type data_type;    // datový typ proměnné/navratové hodnoty
    arg_list_t *args;            // seznam argumentů funkce
    bool defined;               // proměnná/funkce byla inicializována/definována
}TData;

typedef struct htItem
{
    char *key;
    bool active;    //přesunuto
    TData data;
}htabItem_t;

typedef struct hTab
{
    htabItem_t **array;
    int arraySize;
    int actualSize;

}htab_t;




/** HLAVICKY FUNKCI */

/** Inicializace htable */
htab_t *htabInit();

/** Ziskani hashe */
size_t getHash(char *key, htab_t *table);

/** Ziskani stepu pro ukladani/hledani synonym */
size_t getStep(char *key);

/** Nalezeni prvku v tabulce */
htabItem_t *htabSearch(htab_t *table, char *key);

/** Vlozeni prvku */
void htabInsert(htab_t *table, char *key, char *name, tab_type type, tab_data_type data_type, arg_list_t *args, bool defined);

/** Odstraneni prvku */
void htabDelete(htab_t *table, char *key);

/** Odstraneni vseho a uvolneni pameti */
void htabDeleteAll(htab_t *table);

#endif