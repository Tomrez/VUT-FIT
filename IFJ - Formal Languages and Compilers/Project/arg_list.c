/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#include "arg_list.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


void initArgList(arg_list_t *list){
    list->active = NULL;
    list->first = NULL;
}

bool isActiveArg(arg_list_t *list){
    return list->active != NULL;
}

void FirstArg(arg_list_t *list){
    list->active = list->first;
}

void NextArg(arg_list_t *list){
    if(list->active != NULL){
        list->active = list->active->next;
    }
}

bool insertFirstArg(arg_list_t *list, char *name, tab_data_type type){
    // alokace prostoru
    arg_listElem_t *new = malloc(sizeof(arg_listElem_t));
    if(new == NULL){
        return false;
    }

    // nastavení hodnot
    new->name = name;
    new->type = type;
    new->next = list->first;
    list->first = new;
    return true;
}

bool insertAfterArg(arg_list_t *list, char *name, tab_data_type type){
    // alokace prostoru
    arg_listElem_t *new = malloc(sizeof(arg_listElem_t));
    if(new == NULL){
        return false;
    }

    // nastavení hodnot
    new->name = name;
    new->type = type;
    new->next = list->active->next;
    list->active->next = new;
    return true;
}

void deleteArg(arg_list_t *list, char *name){
    if(list != NULL){
        if(list->first == NULL) return;
        arg_listElem_t *curr = (list->first);
        if(!strcmp(list->first->name, name)){
            // prvni je hledany
            list->first = curr->next;
            free(curr);
        }else{
            arg_listElem_t *next = curr->next;
            while(next != NULL){
                if(!strcmp(next->name, name)){
                    curr->next = next->next;
                    free(next);
                    next = curr->next;
                }else{
                    curr = next;
                    next = curr->next;
                }
            }
        }
    }
}

void destroyArgList(arg_list_t *list){
    if(list != NULL){
        // dokud mám co tak mažu
        while(list->first != NULL){
            arg_listElem_t *tmp = list->first;
            list->first = tmp->next;
            free(tmp);
        }

        list->active = NULL;
    }
}