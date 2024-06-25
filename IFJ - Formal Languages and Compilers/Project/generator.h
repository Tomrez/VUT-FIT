/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
 * @author Petr Plíhal, xpliha02
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdbool.h>

/**
 * @brief vygeneruje zacatek programu
 * 
 */
void start_generator();

/**
 * @brief vygeneruje konec programu
 * 
 */
void end_generator();

/**
 * @brief vygeneruje vypis promenne
 * 
 * @param name jemeno promenne pro vytisknuti
 */
void write_id(char *name, bool is_local);

/**
 * @brief vygeneruje vypis celeho cisla
 * 
 * @param num literal pro vypis
 */
void write_int(int num);

/**
 * @brief vygeneruje vypis desetineho cisla
 * 
 * @param num literal pro vypis
 */
void write_double(float num);

/**
 * @brief vygeneruje vypis retezce
 * 
 * @param str literal pro vypis
 */
void write_string(char *str);

/**
 * @brief vezme promennou ze zasobniku, jako argument
 * 
 * @param name jmeno pro ulozeni promenne
 */
void pop_arg(char *name);

/**
 * @brief vlozi promennou na zasobnik, jako argument
 * 
 * @param name jmeno vkladane promenne
 * @param is_local indikuje, zda se vklada lokalni, nebo globalni promenna
 */
void push_id(char *name, bool is_local);

/**
 * @brief vlozi retezec na zasobnik, jako argument
 * 
 * @param str literal pro vlozeni
 */
void push_string(char *str);

/**
 * @brief vlozi cele cislo na zasobnik, jako argument
 * 
 * @param num literal pro vlozeni
 */
void push_int(int num);

/**
 * @brief vlozi desetine cislo na zasobnik, jako argument
 * 
 * @param num literal pro vlozeni
 */
void push_double(float num);

/**
 * @brief vlozi nil na zasobnik, jako argument
 * 
 */
void push_nil();

/**
 * @brief vygeneruje zacatek funkce
 * 
 * @param name nazev funkce
 */
void generate_func_start(char *name);

/**
 * @brief vygeneruje konec funkce
 * 
 * @param name nazev funkce
 * @param is_void indikuje zda je funkce bez navratove hodnoty
 */
void generate_func_end(char *name, bool is_void);

/**
 * @brief vygeneruje volani funkce
 * 
 * @param name jmeno volane funkce
 */
void generate_func_call(char *name);

/**
 * @brief generuje vycisteni zasobniku
 * 
 */
void clear_stack();

/**
 * @brief vygeneruje zacatek if konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_if_start(int num);

/**
 * @brief vygeneruje stred if konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_if_middle(int num);

/**
 * @brief vygeneruje konec if konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_if_end(int num);

/**
 * @brief vygeneruje zacatek while konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_while_start(int num);

/**
 * @brief vygeneruje stred while konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_while_middle(int num);

/**
 * @brief vygeneruje konec while konstrukce
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_while_end(int num);

/**
 * @brief vytvori promennou v aktualnim ramci
 * 
 * @param name jmeno vytvarene promenne
 * @param is_local indikuje, zda se vklada lokalni, nebo globalni promenna
 */
void generate_variable(char *name, bool is_local);

/**
 * @brief vygeneruje navrat z funkce
 * 
 */
void generate_return();

/**
 * @brief vygeneruje prirazeni hodnoty ze zasobniku do promenne
 * 
 * @param name jmeno promenne do ktere se prirazuje
 * @param is_local indikuje, zda se uklada do lokalni, nebo globalni promenne
 */
void generate_assign(char *name, bool is_local);

/**
 * @brief secte hodnoty na vrcholu zasobniku
 * 
 */
void generate_addition();

/**
 * @brief odecte hodnoty na vrcholu zasobniku
 * 
 */
void generate_subtraction();

/**
 * @brief vynasobi hodnoty na vrcholu zasobniku
 * 
 */
void generate_multiplication();

/**
 * @brief vydeli (celociselne) hodnoty na vrcholu zasobniku
 * 
 */
void generate_division_int();

/**
 * @brief vydeli (i s desetinou casti) hodnoty na vrcholu zasobniku
 * 
 */
void generate_division_double();

/**
 * @brief konkatenuje hodnoty na vrcholu zasobniku
 * 
 */
void generate_concatenation();

/**
 * @brief generuje ?? operator z hodnot na vrcholu zasobniku
 * 
 * @param num cislo pro vytvoreni unikatniho navesti
 */
void generate_nil_dif(int num);

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_equal();

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_not_equal();

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_lesser_equal();

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_greater_equal();

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_lesser();

/**
 * @brief porovna hodnoty na vrcholu zasobniku
 * 
 */
void generate_greater();

/**
 * @brief pretypuje promennou na vrcholu zasobniku na float
 * 
 */
void generate_to_float_top();

/**
 * @brief pretypuje promennou na druhem miste zasobniku na float
 * 
 */
void generate_to_float_second_top();

#endif