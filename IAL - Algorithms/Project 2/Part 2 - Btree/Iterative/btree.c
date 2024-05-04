/*
 * Binární vyhledávací strom — iterativní varianta
 *
 * S využitím datových typů ze souboru btree.h, zásobníku ze souboru stack.h 
 * a připravených koster funkcí implementujte binární vyhledávací 
 * strom bez použití rekurze.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci. 
 */
void bst_init(bst_node_t **tree) {

    (* tree) = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 * 
 * Funkci implementujte iterativně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {

    //nasel jsem prvek?
    bool found = false;

    //dokud jsem neprosel cely strom nebo nenasel prvek
    while(tree != NULL && found == false)
    {
        //pokud jsem nasel hledany prvek, ulozim si jeho hodnotu a zapisu si, ze jsem ho nalezl
        if(tree->key == key)
        {
            *value = tree->value;
            found = true;
        }
        else
        {
            //pokud je klic mensi nez aktualni -> pujdu doleva
            if(key < tree->key)
            {
                tree = tree->left;
            }
            //jinak pujdu doprava
            else if(key > tree->key)
            {
                tree = tree->right;
            }
        }
    }

    return found;
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší. 
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {

    //pokud je strom prazdny
    if((* tree) == NULL)
    {
        //alokujeme pamet
        (* tree) = malloc(sizeof(bst_node_t));
        //kontrola, jestli byl malloc uspesny
        if((* tree) == NULL)
        {
            return;
        }
        //a ulozime do nej hodnoty
        (* tree)->key = key;
        (* tree)->value = value;
        (* tree)->left = NULL;
        (* tree)->right = NULL;
        return;
    }

    bst_node_t *tmp = (* tree);
    while(tmp != NULL)
    {
        //nasel jsi klic -> uloz do nej hodnotu a skonci
        if(tmp->key == key)
        {
            tmp->value = value;
            return;
        }
        //pokud je klic vetsi -> pujdu doprava
        else if(tmp->key < key)
        {
            //pokud jsem dosel na konec stromu a nenasel jsem prvek, ve kterem bych mel hodnotu aktualizovat(neexistuje) -> vytvorim novy prvek
            if(tmp->right == NULL)
            {
                //alokuji pamet a ulozim hodnoty
                bst_node_t *tmp2 = malloc(sizeof(bst_node_t));
                if(tmp2 == NULL)
                {
                    return;
                }
                tmp2->key = key;
                tmp2->value = value;
                tmp2->left = NULL;
                tmp2->right = NULL;
                //ulozim do predposledniho prvku ukazatel na nami nove pridany prvek
                tmp->right = tmp2;
                return;
            }
            tmp = tmp->right;
        }
        //pokud je klic mensi -> pujdu doleva
        else if(tmp->key > key)
        {
            if(tmp->left == NULL)
            {
                bst_node_t *tmp2 = malloc(sizeof(bst_node_t));
                if(tmp2 == NULL)
                {
                    return;
                }
                tmp2->left = NULL;
                tmp2->right = NULL;
                tmp2->key = key;
                tmp2->value = value;

                tmp->left = tmp2;
                return;
            }
            tmp = tmp->left;
        }
    }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 * 
 * Klíč a hodnota uzlu target budou nahrazené klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 * 
 * Tato pomocná funkce bude využita při implementaci funkce bst_delete.
 *
 * Funkci implementujte iterativně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {

    bst_node_t *tmp = (* tree);
    bst_node_t *above = NULL;

    //prochazej, dokud nedojdes na konec stromu - jdi doprava
    while(tmp != NULL && tmp->right != NULL)
    {
        above = tmp;
        tmp = tmp->right;
    }
    //ulozim si nalezene hodnoty do promenne
    target->key = tmp->key;
    target->value = tmp->value;

    //pokud nema posledni prvek zadny prvek nad sebou
    if(above != NULL)
    {
        if(tmp->left != NULL)
        {
            above->right = tmp->left;
        }
        else
        {
            above->right = NULL;
        }
    }
    //jinak prepis target na levy prvek
    else
    {
        (* tree) = tmp->left;
    }
    free(tmp);
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 * 
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 * 
 * Funkci implementujte iterativně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {

    bst_node_t *tmp = (* tree);
    bst_node_t *tmp2 = NULL;

    if((* tree) == NULL)
    {
        return;
    }

   while(tmp != NULL)
    {
        //pokud jsem nasel prvek urceny k odstraneni
        if(tmp->key == key)
        {
            //pokud nema ani jednoho potomka
            if(tmp->left == NULL && tmp->right == NULL)
            {
                if(tmp2 == NULL)
                {
                    (* tree) = NULL;
                }
                else if(tmp2->left == tmp)
                {
                    tmp2->left = NULL;
                }
                else
                {
                    tmp2->right = NULL;
                }
            }
            //pokud ma jenom leveho potomka
            else if(tmp->right == NULL)
            {
                if(tmp2 == NULL)
                {
                    (* tree) = tmp->left;
                }
                else if(tmp2->left == tmp)
                {
                    tmp2->left = tmp->left;
                }
                else
                {
                    tmp2->right = tmp->left;
                }
            }
            //pokud ma jenom praveho potomka
            else if(tmp->left == NULL)
            {
                if(tmp2 == NULL)
                {
                    (* tree) = tmp->right;
                }
                else if(tmp2->right == tmp)
                {
                    tmp2->left = tmp->right;
                }
                else
                {
                    tmp2->right = tmp->left;
                }
            }
            //pokud ma oba potomky -> pouzijeme vyse definovanou funkci replace rightmost
            else
            {
                    bst_replace_by_rightmost(tmp, &(tmp->left));
                    return;
            }
            //uvolnime odstranovany prvek
            free(tmp);
            return;
        }
        //pokud je klic mensi nez klic aktualniho prvku
        else if(tmp->key > key)
        {
            //pokud neni levy prvek prazdny
            if(tmp->left != NULL)
            {
                //podivam se, jestli se shoduje jeho klic s hledanym klicem
                if(tmp->left->key == key)
                {
                    //pokud ano, odstranim ho
                    tmp2 = tmp->left;

                    if(tmp2->left == NULL && tmp2->right == NULL)
                    {
                        tmp->left = NULL;
                    }
                    else if(tmp2->right == NULL)
                    {
                        tmp->left = tmp2->left;
                    }
                    else if(tmp2->left == NULL)
                    {
                        tmp->left = tmp2->right;
                    }
                    else
                    {
                        bst_replace_by_rightmost(tmp2, &(tmp2->left));
                        return;
                    }

                    free(tmp2);
                    return;
                }
                else
                {
                    tmp = tmp->left;
                }
            }
            else
            {
                return;
            }
        }
        //pokud je klic vetsi nez klic aktualniho prvku
        else if(tmp->key < key)
        {
            //zkontroluji existenci a pote klic praveho prvku
            if(tmp->right != NULL)
            {
                if(tmp->right->key == key)
                {
                    //pokud se shoduji, odstranim ho
                    tmp2 = tmp->right;

                    if(tmp2->left == NULL && tmp2->right == NULL)
                    {
                        tmp->right = NULL;
                    }
                    else if(tmp2->right == NULL)
                    {
                        tmp->right = tmp2->left;
                    }
                    else if(tmp2->left == NULL)
                    {
                        tmp->right = tmp2->right;
                    }
                    else
                    {
                        bst_replace_by_rightmost(tmp2, &(tmp2->left));
                        return;
                    }

                    free(tmp2);
                    return;
                }
                else
                {
                    tmp = tmp->right;
                }
            }
            else
            {
                return;
            }
        }
    }
}

/*
 * Zrušení celého stromu.
 * 
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po 
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených 
 * uzlů.
 * 
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {

    //vytvorim a inicializuji stack
    stack_bst_t stack;
    stack_bst_init(&stack);

    //dokud jsem nedosel na konec stromu a nevyprazdnil zasobnik
    while((* tree) != NULL || !stack_bst_empty(&stack))
    {
        //kdyz je hodnota tree NULL, vezmeme si vrchni hodnotu ze zasobniku a udelame pop
        if((* tree) == NULL)
        {
            (* tree) = stack_bst_top(&stack);
            stack_bst_pop(&stack);
        }
        //pokud mám pravý podstrom -> ulozim na stack
        if((* tree)->right != NULL)
        {
            stack_bst_push(&stack, (* tree)->right);
        }
        //ulozim si node, kterou chci uvolnit
        bst_node_t *tmp = (* tree);
        //novy strom bude levy podstrom aktualniho
        (* tree) = (* tree)->left;
        //uvolnim uzel
        free(tmp);
   }


}

/*
 * Pomocná funkce pro iterativní preorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu.
 * Nad zpracovanými uzly zavolá bst_add_node_to_items a uloží je do zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit, bst_items_t *items) {

bst_node_t *tmp = tree;
while(tmp != NULL)
{
    stack_bst_push(to_visit, tmp);
    bst_add_node_to_items(tmp, items);
    tmp = tmp->left;
}

}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_preorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {

    //vytvorime a inicializujeme si stack, na ktery budeme ukladat jednotlive node
    stack_bst_t stack;
    stack_bst_init(&stack);

    bst_leftmost_preorder(tree, &stack, items);

    while(!(stack_bst_empty(&stack)))
    {
        bst_node_t *tmp = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_leftmost_preorder(tmp->right, &stack, items);
    }
}

/*
 * Pomocná funkce pro iterativní inorder.
 * 
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů.
 *
 * Funkci implementujte iterativně s pomocí zásobníku a bez použití 
 * vlastních pomocných funkcí.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {

    bst_node_t *tmp = tree;
    while(tmp != NULL)
    {
        stack_bst_push(to_visit, tmp);
        tmp = tmp->left;
    }

}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_inorder a
 * zásobníku uzlů a bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {

    stack_bst_t stack;
    stack_bst_init(&stack);

    bst_leftmost_inorder(tree, &stack);

    while(!(stack_bst_empty(&stack)))
    {
        bst_node_t *tmp = stack_bst_top(&stack);
        stack_bst_pop(&stack);
        bst_add_node_to_items(tmp, items);
        bst_leftmost_inorder(tmp->right, &stack);
    }

}

/*
 * Pomocná funkce pro iterativní postorder.
 *
 * Prochází po levé větvi k nejlevějšímu uzlu podstromu a ukládá uzly do
 * zásobníku uzlů. Do zásobníku bool hodnot ukládá informaci, že uzel
 * byl navštíven poprvé.
 *
 * Funkci implementujte iterativně pomocí zásobníku uzlů a bool hodnot a bez použití
 * vlastních pomocných funkcí.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {

    while(tree != NULL)
    {
        stack_bst_push(to_visit, tree);
        stack_bool_push(first_visit, true);
        tree = tree->left;
    }

}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte iterativně pomocí funkce bst_leftmost_postorder a
 * zásobníku uzlů a bool hodnot a bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {

    stack_bst_t stack;
    stack_bst_init(&stack);

    stack_bool_t bool_stack;
    stack_bool_init(&bool_stack);

    bst_leftmost_postorder(tree, &stack, &bool_stack);

    bool fromLeft;

    while(!(stack_bst_empty(&stack)))
    {
        tree = stack_bst_top(&stack);
        fromLeft = stack_bool_top(&bool_stack);
        stack_bool_pop(&bool_stack);
        if(fromLeft == true)
        {
           stack_bool_push(&bool_stack, false);
           bst_leftmost_postorder(tree->right, &stack, &bool_stack);
        }
        else
        {
            stack_bst_pop(&stack);
            bst_add_node_to_items(tree, items);
        }

    }

}
