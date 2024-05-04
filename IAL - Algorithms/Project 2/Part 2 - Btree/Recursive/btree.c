/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
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
  *tree = NULL;
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * hodnotu daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 * 
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
    
    if(tree == NULL)
    {
        return false;
    }

    else if(tree->key == key) //pokud je aktualni prvek ten hledany
    {
        *value = tree->value; //vezmeme hodnotu
        return true; //a vratime true -> uspesne nalezeno
    }
    else if(tree->key < key) //pokud je hodnota aktualniho prvku mensi
    {
        return bst_search(tree->right, key, value); //pujdeme hledat doprava
    }
    else if(tree->key > key)
    {
        return  bst_search(tree->left, key, value); //jinak pujdeme doleva
    }
    return false;
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
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
    if(tree == NULL || key == 0)
    {
        return;
    }
    else
    {
        //pokud je strom prazdny
        if((* tree) == NULL)
        {
            //alokuji pamet a ulozim do ni data
            *tree = malloc(sizeof(bst_node_t));
            if(*tree == NULL)
            {
                return;
            }
            (* tree)->key = key;
            (* tree)->value = value;
            (* tree)->left = NULL;
            (* tree)->right = NULL;
        }
        //pokud jsem nasel hledany prvek
        else if((* tree)->key == key)
        {
            //ulozim do nej hodnotu
            (* tree)->value = value;
        }
        //pokud je klic vetsi -> jdu doprava pomoci rekurzivniho volani funkce
        else if((* tree)->key < key)
        {
            bst_insert(&(* tree)->right, key, value);
        }
        //jinak jdu doleva
        else if((* tree)->key > key)
        {
            bst_insert(&(* tree)->left, key, value);
        }
    }

}


/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 * 
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 * 
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
    //pokud jeste nejsem na konci stromu -> znovu zavolam funkci na prvek o uroven vpravo
    if ((* tree)->right != NULL)
    {
        bst_replace_by_rightmost(target, &(* tree)->right);
    }
    //pokud jsem dosel nakonec -> prohodim hodnoty a uvolnim uzel
    else
    {
        bst_node_t *tmp = *tree;
        target->key = (* tree)->key;
        target->value = (* tree)->value;
        *tree = (* tree)->left;
        free(tmp);
        return;
    }
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
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key) {

    if( *tree == NULL)
    {
        return;
    }
    //jestli jsem nasel prvek s odpovidajicim klicem
    else if((* tree)->key == key)
    {
        //pokud ma odstranovany prvek jen jednoho potomka -> ulozim potomka na misto odstranovaneho prvku
        if((* tree)->left == NULL)
        {
            bst_node_t *tmp = *tree;
            *tree = (* tree)->right;
            free(tmp);
        }
        else if((* tree)->right == NULL)
        {
            bst_node_t *tmp = *tree;
            *tree = (* tree)->left;
            free(tmp);
        }
        //pokud ma odstranovany prvek oba potomky -> pouziji drive vytvorenou funkci replace_rightmost
        else
        {
            //funkci dam parametr tree->left, protoze algoritmus replace rightmost jde prvne o jeden doleva a az pote jde porad doprava
            bst_replace_by_rightmost(*tree, &(* tree)->left);
        }
    }
    //pokud je klic vetsi -> pujdu doprava
    else if((* tree)->key < key)
    {
        bst_delete(&(* tree)->right, key);
    }
    //jinak pujdu doleva
    else
    {
        bst_delete(&(* tree)->left, key);
    }
}

/*
 * Zrušení celého stromu.
 * 
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po 
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených 
 * uzlů.
 * 
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree) {

    if(*tree == NULL)
    {
        return;
    }
    else
    {
        //odstranim rekurzivne obe podvetve
        bst_dispose(&(* tree)->right);
        bst_dispose(&(* tree)->left);
        //uvolnim pamet daneho prvku
        free(* tree);
        //inicializuji strom na prazdny
        bst_init(tree);
    }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items) {

    if(tree == NULL || items == NULL)
    {
        return;
    }
    //zapisu prvek
    bst_add_node_to_items(tree, items);
    //zpracuji levou vetev
    bst_preorder(tree->left, items);
    //zpracuji pravou vetev
    bst_preorder(tree->right, items);
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items) {

    if(tree == NULL || items == NULL)
    {
        return;
    }
    //zpracuji levou vetev
    bst_inorder(tree->left, items);
    //zapisu prvek
    bst_add_node_to_items(tree, items);
    //zpracuji pravou vetev
    bst_inorder(tree->right, items);
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items) {

    if(tree == NULL || items == NULL)
    {
        return;
    }
    //zpracuji levou vetev
    bst_postorder(tree->left, items);
    //zpracuji pravou vetev
    bst_postorder(tree->right, items);
    //nakonec zapisu prvek
    bst_add_node_to_items(tree, items);
}
