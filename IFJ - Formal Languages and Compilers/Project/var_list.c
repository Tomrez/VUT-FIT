/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#include "var_list.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool createFrame(var_list_t **list){
    // alokace prostoru
    var_list_t *new = malloc(sizeof(var_list_t));
    if(new == NULL){
        return false;
    }

    // nastavení hodnot
    new->list = NULL;
    new->next = *list;
    *list = new;
    return true;
}

bool addVar(var_list_t *list, char *name){
    // alokace prostoru
    var_listElem_t *new = malloc(sizeof(var_listElem_t));
    if(new == NULL){
        return false;
    }

    // nastavení hodnot
    new->name = name;
    new->next = list->list;
    list->list = new;
    return true;
}

bool searchVar(var_list_t *list, char *name){
    if(list != NULL){
        var_listElem_t *tmp = list->list;
        // průchod do konce seznamu identifikátorů v aktuálním bloku
        while(tmp != NULL){
            if(!strcmp(name, tmp->name)){
                // pokud se identifikátor rovná s hledaným -> true
                return true;
            }

            tmp = tmp->next;
        }
    }

    // pokud není nalezen identifikátor -> true
    return false;
}

void destroyFrame(var_list_t **list, htab_t *table){
    // průchod do konce seznamu identifikátorů v aktuálním bloku
    while((*list)->list != NULL){
        // mazání identifikatoru
        var_listElem_t *tmp = (*list)->list;
        (*list)->list = tmp->next;
        htabDelete(table, tmp->name);
        free(tmp->name);
        free(tmp);
    }

    var_list_t *tmp = (*list);
    (*list) = tmp->next;
    free(tmp);
}

void destroyList(var_list_t **list){
    if((*list) != NULL){
        // průchod všemi rámci
        while((*list) != NULL){
            // průchod do konce seznamu identifikátorů v aktuálním bloku
            while((*list)->list != NULL){
                // mazání identifikatoru
                var_listElem_t *tmp_elem = (*list)->list;
                (*list)->list = tmp_elem->next;
                free(tmp_elem->name);
                free(tmp_elem);
            }
            // mazání rámce
            var_list_t *tmp = (*list);
            (*list) = tmp->next;
            free(tmp);
        }
    }
}