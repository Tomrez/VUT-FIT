/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {

	list->activeElement = list->firstElement = list->lastElement = NULL; //nastavime vsechny prvky seznamu na NULL
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {

	DLLElementPtr tmp_first = list->firstElement;
	
	while (tmp_first != NULL) //dokud jsi neprosel cely seznam
	{
		list->firstElement = list->firstElement->nextElement; //posun prvni prvek o jeden dal
		free(tmp_first); //uvolni pamet predchoziho prvniho prvku
		tmp_first = list->firstElement; //uloz si aktualni prvni prvek
	}
	//DLL_Init(list);
	list->activeElement = list->firstElement = list->lastElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {

	DLLElementPtr tmp = (DLLElementPtr)malloc(sizeof(struct DLLElement));

	if(tmp == NULL)
	{
		DLL_Error();
	}

	//vytvorime si prvek, ktery pote vlozime na prvni pozici v seznamu
	tmp->data = data;
	tmp->nextElement = list->firstElement;
	tmp->previousElement = NULL;


	if(list->firstElement != NULL)//pokud neni seznam prazdny
	{
		list->firstElement->previousElement = tmp; //prvek pred aktualnim prvnim prvkem bude ten nami vlozeny
	}
	else
	{
		list->lastElement = tmp; //pokud je seznam prazdny -> prvni prvek je zaroven i posledni prvek
	}
	list->firstElement = tmp; //prvni prvek seznamu je nami vlozeny
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {
	
	DLLElementPtr tmp = (DLLElementPtr)malloc(sizeof(struct DLLElement));

	if(tmp == NULL)
	{
		DLL_Error();
	}

	//obdobne jako vlozeni prvniho prvku
	tmp->data = data;
	tmp->previousElement = list->lastElement;
	tmp->nextElement = NULL;

	if(list->lastElement != NULL)
	{
		list->lastElement->nextElement = tmp;
	}
	else
	{
		list->firstElement = tmp;
	}
	list->lastElement = tmp;

}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {

	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {

	if(list->firstElement != NULL)
	{
		*dataPtr = list->firstElement->data;
	}
	else
	{
		DLL_Error();
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {

	if(list->lastElement != NULL)
	{
		*dataPtr = list->lastElement->data;
	}
	else
	{
		DLL_Error();
	}
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {

	if(list->firstElement != NULL)
	{
		DLLElementPtr tmp = list->firstElement;

		if(list->firstElement == list->lastElement) //pokud ma seznam pouze jeden prvek
		{
			list->activeElement = NULL;
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		else
		{
			list->firstElement = list->firstElement->nextElement; //nastavime prvni prvek jako nasledujici
			list->firstElement->previousElement = NULL; //pred novym prvnim prvek uz nebude zadny dalsi prvek
		}
		free(tmp); //uvolnime pamet, kde byl ulozen prvni prvek
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {

	if(list->lastElement != NULL)
	{
		DLLElementPtr tmp = list->lastElement;

		//obdobne jako u odstraneni prvniho prvku
		if(list->lastElement == list->firstElement)
		{
			list->activeElement = NULL;
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		else
		{
			list->lastElement = list->lastElement->previousElement;
			list->lastElement->nextElement = NULL;
		}
		free(tmp);
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {

	if(list->activeElement != NULL)
	{
		if(list->activeElement != list->lastElement)
		{	
			DLLElementPtr tmp = list->activeElement->nextElement; //ulozime si prvek, ktery chceme odstranit

			if(tmp == list->lastElement) //pokud odstranovany prvek je poslednim prvkem seznamu
			{
				list->activeElement->nextElement = NULL; //aktivni prvek se stane poslednim
				list->lastElement = list->activeElement;
			}
			else
			{
				tmp->nextElement->previousElement = list->activeElement;
				list->activeElement->nextElement = tmp->nextElement;
			}
			free(tmp);
		}
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {

	if(list->activeElement != NULL)
	{
		if(list->activeElement->previousElement != NULL)
		{
			DLLElementPtr tmp = list->activeElement->previousElement;

			//obdobne jako u mazani nasledujiciho prvku
			if(list->activeElement->previousElement == list->firstElement)
			{
				list->activeElement->previousElement = NULL;
				list->firstElement = list->activeElement;
			}
			else
			{
				tmp->previousElement->nextElement = list->activeElement;
				list->activeElement->previousElement = tmp->previousElement;
			}
			free(tmp);
		}
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {

	DLLElementPtr tmp = malloc(sizeof(struct DLLElement));
	if(tmp == NULL)
	{
		DLL_Error();
	}

	if(list->activeElement != NULL)
	{
		tmp->data = data;

		if(list->activeElement == list->lastElement)
		{
			//lidske reseni by bylo pouzit funkci InsertLast, ale zadani rika neco jineho, unluko
			//DLL_InsertLast(list, data)
			DLLElementPtr tmp = (DLLElementPtr)malloc(sizeof(struct DLLElement));

			if(tmp == NULL)
			{
				DLL_Error();
			}

			//obdobne jako vlozeni prvniho prvku
			tmp->data = data;
			tmp->previousElement = list->lastElement;
			tmp->nextElement = NULL;

			if(list->lastElement != NULL)
			{
				list->lastElement->nextElement = tmp;
			}
			else
			{
				list->firstElement = tmp;
			}
			list->lastElement = tmp;
		}
		else
		{
			tmp->nextElement = list->activeElement->nextElement;
			tmp->previousElement = list->activeElement;
			list->activeElement->nextElement->previousElement = tmp;
			list->activeElement->nextElement = tmp;
		}
	}
	else
	{
		DLL_Error();
	}
	free(tmp); //uvolnim pamet
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {

	DLLElementPtr tmp = malloc(sizeof(struct DLLElement)); //alokujeme si pomocnou promennou typu ukazatel na seznam

	if(list->activeElement != NULL)
	{
		if(tmp == NULL)
		{
			DLL_Error(); //Pokud se nepodarilo mallocu alokovat pozadovanou pamet, ukoncime funkci a vypiseme error
		}

		tmp->data = data;

		if(list->activeElement == list->firstElement)
		{
			//DLL_InsertFirst(list, data);
			DLLElementPtr tmp = (DLLElementPtr)malloc(sizeof(struct DLLElement));

			if(tmp == NULL)
			{
				DLL_Error();
			}

			//vytvorime si prvek, ktery pote vlozime na prvni pozici v seznamu
			tmp->data = data;
			tmp->nextElement = list->firstElement;
			tmp->previousElement = NULL;


			if(list->firstElement != NULL)//pokud neni seznam prazdny
			{
				list->firstElement->previousElement = tmp; //prvek pred aktualnim prvnim prvkem bude ten nami vlozeny
			}
			else
			{
				list->lastElement = tmp; //pokud je seznam prazdny -> prvni prvek je zaroven i posledni prvek
			}
			list->firstElement = tmp; //prvni prvek seznamu je nami vlozeny
		}
		else
		{
			tmp->nextElement = list->activeElement;
			tmp->previousElement = list->activeElement->previousElement;
			list->activeElement->previousElement->nextElement = tmp;
			list->activeElement->previousElement = tmp;
		}
	}
	else
	{
		DLL_Error();
	}
	free(tmp);
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {

	if(list->activeElement != NULL)
	{
		*dataPtr = list->activeElement->data;//Do ukazatele predaneho parametrem ulozime hodnotu aktivniho prvku
	}
	else
	{
		DLL_Error();
	}
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {

	if(list->activeElement != NULL)
	{
		list->activeElement->data = data; //Do aktivniho prvku ulozime hodnotu predanou parametrem funkce
	}

}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {

	if(list->activeElement != NULL)
	{
		list->activeElement = list->activeElement->nextElement; //Pokud ne, tak pouze ulozime nasledujici prvek jakozo aktivni
	}
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {

	if(list->activeElement != NULL)
	{
		list->activeElement = list->activeElement->previousElement; //Pokud ne, tak pouze ulozime predchozi prvek jakozto aktivni
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {

return (list->activeElement == NULL)? 0 : 1;
}

/* Konec c206.c */
