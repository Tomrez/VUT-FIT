/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

bool solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar( Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength ) {
	//solved = false; /* V případě řešení, smažte tento řádek! */
	char var;
	Stack_Top(stack, &var);
	while(var != '(' && Stack_IsEmpty(stack) == 0 ) //dokud nenajdes zavorku nebo nevyprazdnis cely stack
	{
		postfixExpression[(*postfixExpressionLength)++] = var; //ulozeni nacteneho znaku do retezce
		Stack_Pop(stack);
		Stack_Top(stack, &var); //nacteni znaku na vrcholu zasobniku
	}
	Stack_Pop(stack); //odstraneni leve zavorky
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation( Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength ) {

	char var;

	if(Stack_IsEmpty(stack)) //kdyz je zasobnik prazdny, nemusime nic resit a muzeme vlozit operator
	{
		Stack_Push(stack, c);
	}
	else //pokud ale neni prazdny
	{
		Stack_Top(stack, &var); //nacteme si znak z vrcholu zasobniku do pomocne promenne
		if(var == '(')
		{
			Stack_Push(stack, c);
		}
		else if((var == '+' || var == '-') && (c == '*' || c == '/')) //pokud ma znak ze zasobniku nizsi prioritu nez operator predany parametrem
		{
			Stack_Push(stack, c);
		}
		else //pokud nema nizsi prioritu
		{
			postfixExpression[(*postfixExpressionLength)++] = var; //ulozime nacteny znak do retezce
			Stack_Pop(stack);
			doOperation(stack, c, postfixExpression, postfixExpressionLength); //vyuzijeme rekurzi a zavolame znovu funkci doOperation cimz se nam podari odstranit dany operator ze zasobniku
		}
	}
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression vstupní znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix( const char *infixExpression ) {
	
	unsigned int length = 0;

	Stack stack;
	Stack_Init(&stack);

	char *postfix = (char *) malloc(MAX_LEN * sizeof(char)); //alokujeme pamet pro vystupni retezec
	if(!postfix)
	{
		Stack_Dispose(&stack);
		return NULL;
	} 

	int infix_count = 0;

	while(infixExpression[infix_count] != '\0')
	{
		if((infixExpression[infix_count] >= '0' && infixExpression[infix_count] <= '9') || (infixExpression[infix_count] >= 'a' && infixExpression[infix_count] <= 'z') || (infixExpression[infix_count] >= 'A' && infixExpression[infix_count] <= 'Z'))
		{
			postfix[length++] = infixExpression[infix_count]; //pokud se jedna o cislo nebo pismeno, nebudeme nic resit a pouze ho pridame do vystupniho retezce
		}
		else if (infixExpression[infix_count] == '(')
		{
			Stack_Push(&stack, infixExpression[infix_count]);
		}
		else if (infixExpression[infix_count] == ')')
		{
			untilLeftPar(&stack, postfix, &length); //pokud najdeme ')', vyuzijeme nasi funkci a zpracujeme vyraz az po levou zavorku
		}
		else if(infixExpression[infix_count] == '+' || infixExpression[infix_count] == '-' || infixExpression[infix_count] == '*' || infixExpression[infix_count] == '/')
		{
			doOperation(&stack, infixExpression[infix_count], postfix, &length); //pokud nalezneme operator, pouzijeme nasi pomocnou funkci na zjisteni jeho priority	
		}
		else if(infixExpression[infix_count] == '=') //mame vse co potrebujeme, vyprazdnime zasobnik
		{
			while(!Stack_IsEmpty(&stack))
			{
				Stack_Top(&stack, &(postfix[length++]));
				Stack_Pop(&stack);
			}
			
			//na konec naseho vystupniho retezce pridame znak '=' a \0 abychom poznali, ze se jedna o konec retezce
			postfix[length++] = '=';
			postfix[length] = '\0';

			break;
		}
		else
		{
			return NULL; //pokud to neni cislo, promenna a ani operand -> neco je spatne
		}
		infix_count++;
	}
	Stack_Dispose(&stack);

	return postfix;
}


/**
 * Pomocná metoda pro vložení celočíselné hodnoty na zásobník.
 *
 * Použitá implementace zásobníku aktuálně umožňuje vkládání pouze
 * hodnot o velikosti jednoho byte (char). Využijte této metody
 * k rozdělení a postupné vložení celočíselné (čtyřbytové) hodnoty
 * na vrchol poskytnutého zásobníku.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value hodnota k vložení na zásobník
 */
void expr_value_push( Stack *stack, int value ) {

	char byte[4]; //integer potrebujeme rozdelit na ctyri jedno bytove hodnoty

	//pomoci bitoveho posuvu si postupne udelame ctyri jednobytove hodnoty
	//diky operaci AND s hodnotou po posuvu zajistime, ze si vezmeme pouze osm potrebnych bitu (0xFF = 11111111)
	byte[0] = (value >> 24) & 0xFF;
	byte[1] = (value >> 16) & 0xFF;
	byte[2] = (value >> 8) & 0xFF;
	byte[3] = value & 0xFF;

	//postupne jednotlive byty pushneme na zasobnik
	for(int i = 3; i >= 0; i--)
	{
		Stack_Push(stack, byte[i]);
	}
}

/**
 * Pomocná metoda pro extrakci celočíselné hodnoty ze zásobníku.
 *
 * Využijte této metody k opětovnému načtení a složení celočíselné
 * hodnoty z aktuálního vrcholu poskytnutého zásobníku. Implementujte
 * tedy algoritmus opačný k algoritmu použitému v metodě
 * `expr_value_push`.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné celočíselné hodnoty z vrcholu zásobníku
 */
void expr_value_pop( Stack *stack, int *value ) {

	//opet potrebujeme misto, kam si ulozime ctyri hodnoty, ktere chceme spojit do jedne integerove hodnoty
	char byte[4];

	//nacteme si ctyri byty
	for(int i = 0; i < 4; i++)
	{
		Stack_Top(stack, &byte[i]);
		Stack_Pop(stack);
	}

	//spojime hodnoty do jedne
	int intNum = (byte[0] << 24) | (byte[1] << 16) | (byte[2] << 8) | byte[3];

	//ulozime vyslednou hodnotu do promenne predane parametrem
	*value = intNum;
}


/**
 * Tato metoda provede vyhodnocení výrazu zadaném v `infixExpression`,
 * kde hodnoty proměnných použitých v daném výrazu jsou definovány
 * v poli `variableValues`.
 *
 * K vyhodnocení vstupního výrazu využijte implementaci zásobníku
 * ze cvičení c202. Dále také využijte pomocných funkcí `expr_value_push`,
 * respektive `expr_value_pop`. Při řešení si můžete definovat libovolné
 * množství vlastních pomocných funkcí.
 *
 * Předpokládejte, že hodnoty budou vždy definovány
 * pro všechy proměnné použité ve vstupním výrazu.
 *
 * @param infixExpression vstpní infixový výraz s proměnnými
 * @param variableValues hodnoty proměnných ze vstupního výrazu
 * @param variableValueCount počet hodnot (unikátních proměnných
 *   ve vstupním výrazu)
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné hodnoty vyhodnocení vstupního výrazu
 *
 * @return výsledek vyhodnocení daného výrazu na základě poskytnutých hodnot proměnných
 */
bool eval( const char *infixExpression, VariableValue variableValues[], int variableValueCount, int *value ) {

	//vytvorime a inicializujeme stack
	Stack stack;
	Stack_Init(&stack);

	char * postfix = infix2postfix(infixExpression);

	//pomocne promenne pro ukladani hodnot ze zasobniku nad nimiz budeme provadet operace
	int value1;
	int value2;

	for(int i = 0; postfix[i] != '='; i++) //dokud nenarazis na rovna se, prochazej znak po znaku a delej prislusne operace
	{
		if(postfix[i] == '+')
		{
			expr_value_pop(&stack, &value1);
			expr_value_pop(&stack, &value2);
			expr_value_push(&stack, value1 + value2);
		}
		else if(postfix[i] == '-')
		{
			expr_value_pop(&stack, &value1);
			expr_value_pop(&stack, &value2);
			expr_value_push(&stack, value2 - value1);
		}
		else if(postfix[i] == '*')
		{
			expr_value_pop(&stack, &value1);
			expr_value_pop(&stack, &value2);
			expr_value_push(&stack, value1 * value2);
		}
		else if(postfix[i] == '/')
		{
			expr_value_pop(&stack, &value1);
			expr_value_pop(&stack, &value2);

			if(value1 == 0) //osetreni deleni nulou
			{
				return false;
			}

			expr_value_push(&stack, value2 / value1);
		}
		else
		{
			if(postfix[i] >= '0' && postfix[i] <= '9') //pokud je to cislo -> dej ho rovnou na stack
			{
				expr_value_push(&stack, postfix[i]);
			}
			else //pokud je to neco jineho (promenna) -> najdi jeji hodnotu a tu dej na stack
			{
				for(int j = 0; j < variableValueCount; j++)
				{
					if(variableValues[j].name == postfix[i])
					{
						expr_value_push(&stack, variableValues[j].value);
						break;
					}
					if(j == variableValueCount - 1) //pokud jsi nenasel prislusnou hodnotu k promenne -> vrat false
					{
						return false;
					}
				}
			}
		}
	}

	expr_value_pop(&stack, value); //ulozime vyslednou hodnotu do promenne predavane parametrem

	Stack_Dispose(&stack);
	free(postfix);

	return true;
}

/* Konec c204.c */
