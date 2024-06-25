/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Tomáš Řezníček, xrezni33
*/

#include "symtable.h"

/**
 * Fce init inicializuje tabulku
 * @param table Ukazatel na strukturu tabulky
*/
htab_t *htabInit()
{
    //alokujeme pamet pro hashovaci tabulku
    htab_t *table = malloc(sizeof(htab_t));
    if(table == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //alokujeme pamet pro pole prvku v hashovaci tabulce
    table->array = malloc(MAX_HT_SIZE * sizeof(htabItem_t *));
    if(table->array == NULL)
    {
        free(table);
        exit(ERROR_INTERNAL);
    }

    //do kazdeho prvku pole ulozime NULL
    for(int i = 0; i < MAX_HT_SIZE; i++)
    {
        table->array[i] = NULL;
    }

    //ulozime si k tabulce potrebna data
    table->actualSize = 0;
    table->arraySize = MAX_HT_SIZE;

    //vratime inicializovanou tabulku
    return table;
}


/**
 * Fce getHash je hashovaci funkce, ktera z klice predaneho parametrem vypocita index polozky v tabulce
 * @param key Klic pro prevedeni na prislusny index
 * @param table Udaj o velikosti tabulky pro ziskani indexu ve spravnych mezich
*/
size_t getHash(char *key, htab_t *table)
{
    //kontrola spravnosti klice
    if(key == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    uint32_t hash = 0;

    //hashovaci funkce pro vypocet prislusneho indexu v tabulce
    for(char *p = (char *)key; *p != '\0'; p++)
    {
        hash = 55557 * hash + *p; //55557 je prvocislo -> lepsi rozptyleni po cele tabulce
    }
    //zajisteni, ze bude index v mezich tabulky
    hash = hash % table->arraySize;

    return hash;
}


/**
 * Fce getStep je funkce, ktera vypocita "krok" po kterem se budeme pohybovat v tabulce pri hledani synonym
 * @param key Klic pro prevedeni na prislusny index
*/
size_t getStep(char *key)
{
    //kontrola spravnosti klice
    if(key == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    uint32_t step = 0;

    //funkce pro vypocet kroku
    for(char *p = (char *)key; *p != '\0'; p++)
    {
        step = 55557 * step + *p;
    }
    step += *key;
    step = step % MAX_STEP_SIZE;
    
    //krok nemuze byt 0 -> pokud je krok 0, prepiseme ho rucne na 1
    if(!step)
    {
        step += 1;
    }

    return step;
}


/**
 *Fce htabSearch provadi vyhledani prvku, specifikovaneho klicem, v tabulce
 * @param key Klic pro vyhledani prvku v poli
 * @param table Inicializovana tabulka s jednotlivymi prvky
**/
htabItem_t *htabSearch(htab_t *table, char *key)
{
    //kontrola spravnosti parametru
    if(table == NULL || key == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //ziskame potrebny index a velikost kroku
    uint32_t index = getHash(key, table);
    uint32_t step = getStep(key);

    //ulozime si hodnotu na danem indexu a pripravime si promennou, do ktere budeme ukladat posledni nalezeny prvek
    htabItem_t *tmp = (table->array)[index];
    htabItem_t *last_found = NULL;

    //pokud se na indexu nic nenachazi, vratime adresu NULL
    if(tmp == NULL)
    {
        return NULL;
    }
    //jinak prochazime vsechny synonyma, hledame posledni aktivni vyskyt a ten vratime
    else
    {
        while(tmp != NULL)
        {
            if(!strcmp(tmp->key, key) && tmp->active == true)
            {
                last_found = tmp;
            }
            index = (index + step) % table->arraySize;
            tmp = (table->array)[index];
        }
        return last_found;
    }
}


/**
 * Fce htabInsert vlozi do tabulky novy prvek na index, ktery vypocita fce getHash a ulozi do ni prislusne hodnoty predane parametry
 * @param table Inicializovana tabulka s jednotlivymi prvky
 * @param key Klic pro vypocet indexu pro ulozeni novych dat
 * @param data Data pro ulozeni do tabulky
 * @param active Informace o tom, zda je aktualne pridavany prvek aktivni, kterou je potreba taktez ulozit k informacim o prvku
*/
void htabInsert(htab_t *table, char *key, char *name, tab_type type, tab_data_type data_type, arg_list_t *args, bool defined)
{
    if(table == NULL || key == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //ziskame potrebny index a velikost kroku
    uint32_t index = getHash(key, table);
    uint32_t step = getStep(key);

    //ulozime si hodnotu na danem indexu
    htabItem_t *tmp = (table->array)[index];

    while(tmp != NULL)
    {
        index = (index + step) % table->arraySize;
        tmp = (table->array)[index];
    }

    //alokace pameti pro novy prvek a kontrola uspesnosti mallocu
    htabItem_t *toInsert = (htabItem_t *) malloc(sizeof(htabItem_t));
    if(toInsert == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    toInsert->key = key;
    toInsert->active = true;

    toInsert->data.name = name;
    toInsert->data.type = type;
    toInsert->data.data_type = data_type;
    toInsert->data.args = args;
    toInsert->data.defined = defined;

    //vlozeni prvku do tabulky a zvyseni cnt vlozenych prvku
    (table->array)[index] = toInsert;
    table->actualSize++;

}


/**
 * Fce htabDelete slouzi pro odstraneni prvku, specifikovaneho klicem, z tabulky
 * @param table Inicializovana tabulka s jednotlivymi prvky
 * @param key Klice pro vypocet indexu prvku, ktery chceme odstranit
*/
void htabDelete(htab_t *table, char *key)
{
    //kontrola spravnosti parametru
    if(table == NULL || key == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //ziskame potrebny index a velikost kroku
    uint32_t index = getHash(key, table);
    uint32_t step = getStep(key);

    //ulozime si hodnotu na danem indexu a pripravime si promennou, do ktere budeme ukladat posledni nalezeny prvek, ktery bude urcen k odstraneni
    htabItem_t *tmp = (table->array)[index];
    htabItem_t *toDelete = NULL;

    //pokud mam odstranit prvek, ktery neexistuje -> error
    if(tmp == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //projdu vsechna synonyma a hledam posledni aktivni vyskyt
    while(tmp != NULL)
    {
        if(!strcmp(tmp->key, key) && tmp->active == true)
        {
            toDelete = tmp;
        }

        index = (index + step) % table->arraySize;
        tmp = (table->array)[index];
    }

    //pokud jsem nenasel prvek, ktery mam odstranit -> error
    if(toDelete == NULL)
    {
        exit(ERROR_INTERNAL);
    }
    //odstranim prvek tim, ze ho nastavim jako neaktivni
    else
    {
        toDelete->active = false;
    }

}


/**
 * Fce htabDeleteAll odstrani vsechny prvky z tabulky, uvolni veskerou alokovanou pamet a znovu tabulku inicializuje jakozto prazdnou
 * @param table Inicializovana tabulka s jednotlivymi prvky
*/
void htabDeleteAll(htab_t *table)
{
    //kontrola spravnosti parametru
    if(table == NULL)
    {
        exit(ERROR_INTERNAL);
    }

    //pripravim si promennou, do ktere budu postupne ukladat data z kazdeho indexu tabulky
    htabItem_t *delete = NULL;

    //projdu kazdy jeden index tabulky
    for(int i = 0; i < table->arraySize; i++)
    {
        //ulozim si hodnotu na indexu
        delete = (table->array)[i];

        //pokud jsou zde ulozena nejaka data -> uvolnim je a snizim counter ulozenych prvku
        if(delete != NULL)
        {
            free(delete);
            table->actualSize--;
        }
        //ulozim na kazdy index NULL
        (table->array)[i] = NULL;
    }
    //kontrola, jestli jsem opravdu smazal vsechny vlozene prvky
    if(table->actualSize != 0)
    {
        exit(ERROR_INTERNAL);
    }
}