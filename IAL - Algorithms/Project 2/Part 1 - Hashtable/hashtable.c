/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table) {

	if(table == NULL)
	{
		return;
	}
	else
	{
		for(int i = 0; i < HT_SIZE; i++)
		{
			(* table)[i] = NULL; //vynulujeme kazdy prvek tabulky
		}
	}
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
	
	//TODO kontrola, jestli mame funkcni parametry (u vetsiny funkci udelat)
	if(table != NULL && key != NULL)
	{
		int index = get_hash(key);
		ht_item_t *tmp = (* table)[index];

		if(tmp == NULL)
		{
			return NULL;
		}
		else
		{
			if(strcmp(key, tmp->key) == 0) //pokud jen klic prvku v tabulce stejne jako klic, ktery jsme dostali v parametru funkce
			{
				return tmp; //vrat nalezeny prvek
			}
			else
			{
				tmp = tmp->next; //nacti si dalsi synonymum
				while(tmp != NULL) //dokud nedojdes na konec
				{
					if(strcmp(key, tmp->key) == 0) //hledej pozadovany prvek
					{
						return tmp; //pokud jsi ho nasel, tak ho vrat
					}
					else
					{
						tmp = tmp->next; //pokud ne, posun se na dalsi prvek
					}
				}
			}
		}
	}
  	return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {

	if(table != NULL && key != NULL)
	{
		ht_item_t *tmp = ht_search(table, key);
		int index = get_hash(key);

		if(tmp != NULL) //pokud search nasel pozadovany prvek
		{
			tmp ->value = value; //prepisu v nem hodnotu
		}
		//jinak vytvorim novy prvek a vlozim ho do tabulky
		else
		{	
			//alokuji pamet pro novy prvek
			ht_item_t *toInsert = (ht_item_t *) malloc(sizeof(ht_item_t));
			//kontrola, jestli se podarila alokace
			if(toInsert == NULL)
			{
				return;
			}
			//ulozim si klic a hodnotu
			toInsert->value = value;
			toInsert->key = key;
			//pokud uz se na danem indexu nachazi nejake synonymum -> vlozim novy prvek na zacatek seznamu a do next ulozim ukazatel na aktualni prvni prvek v seznamu
			if(((*table)[index]) != NULL)
			{
				toInsert->next = (*table)[index];
			}
			//pokud se zde jeste zadny prvek nenachazi -> do ukazatele next ulozim NULL
			else
			{
				toInsert->next = NULL;					
			}
			//vlozim nas vytvoreny prvek na zacatek seznamu
			(*table)[index] = toInsert;
		}
	}
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  
	if(table != NULL && key != NULL)
	{
		//ulozim si prvek vraceny funkci search
		ht_item_t *tmp = ht_search(table, key);
		//pokud search nic nenasel -> nebudeme mit zadnou hodnotu
		if(tmp == NULL)
		{
			return NULL;
		}
		else
		{
			//budu postupne prochazet vsechna synonyma a porovnavat jejich klic s mym klicem
			while(tmp != NULL)
			{
				//pokud se shoduji -> vratim value nalezeneho prvku
				if(strcmp(key, tmp->key) == 0)
				{
					return &tmp->value;
				}
				//jinak jdu na dalsi synomymum
				else
				{
					tmp = tmp->next;
				}
			}
		}
	}
  
  return NULL;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {

	if(table != NULL && key != NULL)
	{
		int index = get_hash(key);
		ht_item_t *tmp = (* table)[index];
		ht_item_t *tmp2 = NULL;
		if(tmp != NULL)
		{
			//dokud mam synonyma
			while(tmp != NULL)
			{
				//porovnavam jejich klice -> pokud se shoduji
				if(strcmp(tmp->key, key) == 0)
				{
					//pokud neni pred nalezenym prvek zadny predchozi synonymum
					if(tmp2 == NULL)
					{
						//reknu table ze ma ukazovat na nasledujici
						(* table)[index] = tmp->next;
					}
					else
					{
						//jinak reknu predchozimu prvku, at ukazuje na nasledujici smazaneho
						tmp2->next = tmp->next;
					}
					free(tmp);
					return;
				}
				//ulozim si predchozi prvek pro dalsi pruchod
				tmp2 = tmp;
				//jdu na dalsi synonymum
				tmp = tmp->next;
			}
		}
	}
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po 
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table) {

	if(table != NULL)
	{
		ht_item_t *tmp;
		ht_item_t *tmp2;

		//projdu postupne kazdy prvek tabulky
		for(int i = 0; i < HT_SIZE; i++)
		{
			tmp = (* table)[i];

			//prochazim vsechna synonyma na danem indexu tabulky
			while(tmp != NULL)
			{
				//a uvolnuji je
				tmp2 = tmp->next;
				free(tmp);
				tmp = tmp2;
			}
			//nakonec nastavim kazdy index tabulky na NULL, aby se po dokonceni funkce nachazela ve stejnem stavu, jako po initu
			(*table)[i] = NULL;
		}
	}

}
