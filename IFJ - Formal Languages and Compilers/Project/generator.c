/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
 * @author Petr Plíhal, xpliha02
*/

#include "generator.h"
#include "error.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Upravi vstupni retezec na fromat prijimany IFJcode23
 * 
 * @param string_in retezec na upraveni
 * @return char* ukazatel na upraveny retezec
 */
static char* string_prep(char *string_old){
    //maximalni delka je 4 * puvodni + 1 pro konec retezce
    char *string_new = malloc(strlen(string_old)*4+1);
    if(string_new == NULL) exit(ERROR_INTERNAL);

    int old = 0;
    int new = 0;
    char c = string_old[old];

    while(c != '\0'){
        if(((c >= 0) && (c <= 32)) || (c == 35) || (c == 92)){
            string_new[new] = '\\';
            new++;

            int tmp = c;
            string_new[new] = tmp / 100 + '0';
            new++;

            tmp %= 100;
            string_new[new] = tmp / 10 + '0';
            new++;
            
            tmp %= 10;
            string_new[new] = tmp + '0';
            new++;
        
        }else{
            string_new[new] = string_old[old];
            new++;
        }

        old++;
        c = string_old[old];
    }
    string_new[new] = '\0';
    return string_new;

}

void start_generator(){
    printf(".IFJcode23\n");
    printf("JUMP start&$?!\n");

    // vestavene funkce
    printf("#readString\n");
    printf("    label readString\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@string\n");
    printf("            read LF@string string\n");
    printf("            pushs LF@string\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#readInt\n");
    printf("    label readInt\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@int\n");
    printf("            read LF@int int\n");
    printf("            pushs LF@int\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#readDouble\n");
    printf("    label readDouble\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@double\n");
    printf("            read LF@double float\n");
    printf("            pushs LF@double\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#Int2Double\n");
    printf("    label Int2Double\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@int\n");
    printf("            pops LF@int\n");
    printf("            defvar LF@double\n");
    printf("            int2float LF@double LF@int\n");
    printf("            pushs LF@double\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#Double2Int\n");
    printf("    label Double2Int\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@double\n");
    printf("            pops LF@double\n");
    printf("            defvar LF@int\n");
    printf("            float2int LF@int LF@double\n");
    printf("            pushs LF@int\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#length\n");
    printf("    label length\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@string\n");
    printf("            pops LF@string\n");
    printf("            defvar LF@length\n");
    printf("            strlen LF@length LF@string\n");
    printf("            pushs LF@length\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#substring\n");
    printf("    label substring\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("            defvar LF@end_index\n");
    printf("            pops LF@end_index\n");
    printf("\n");
    printf("            defvar LF@start_index\n");
    printf("            pops LF@start_index\n");
    printf("\n");
    printf("            defvar LF@string\n");
    printf("            pops LF@string\n");
    printf("\n");
    printf("            defvar LF@substring\n");
    printf("            move LF@substring string@\n");
    printf("            defvar LF@substring_char\n");
    printf("            move LF@substring_char string@\n");
    printf("\n");
    printf("            defvar LF@length\n");
    printf("            pushs LF@string\n");
    printf("            call length\n");
    printf("            pops LF@length\n");
    printf("\n");
    printf("            # Dvě proměnné pro možnost porovnávání >=\n");
    printf("            defvar LF@comparison1\n");
    printf("            defvar LF@comparison2\n");
    printf("            defvar LF@comparison3\n");
    printf("\n");
    printf("            # Podmínky pro ukončení cyklu -> substring = nil\n");
    printf("                # i < 0\n");
    printf("                lt LF@comparison1 LF@start_index int@0\n");
    printf("                jumpifeq set_substring_to_nil LF@comparison1 bool@true\n");
    printf("\n");
    printf("                # j < 0\n");
    printf("                lt LF@comparison1 LF@end_index int@0\n");
    printf("                jumpifeq set_substring_to_nil LF@comparison1 bool@true\n");
    printf("\n");
    printf("               # i > j\n");
    printf("                gt LF@comparison1 LF@start_index LF@end_index\n");
    printf("                jumpifeq set_substring_to_nil LF@comparison1 bool@true\n");
    printf("\n");
    printf("                # i >= length(s)\n");
    printf("                gt LF@comparison1 LF@start_index LF@length\n");
    printf("                eq LF@comparison2 LF@start_index LF@length\n");
    printf("                or LF@comparison3 LF@comparison1 LF@comparison2\n");
    printf("                jumpifeq set_substring_to_nil LF@comparison3 bool@true\n");
    printf("\n");
    printf("                # j > length(s)\n");
    printf("                gt LF@comparison1 LF@end_index LF@length\n");
    printf("                jumpifeq set_substring_to_nil LF@comparison1 bool@true\n");
    printf("\n");
    printf("            label substring_cycle\n");
    printf("                jumpifeq substring_cycle_end LF@start_index LF@end_index\n");
    printf("\n");
    printf("                getchar LF@substring_char LF@string LF@start_index\n");
    printf("                concat LF@substring LF@substring LF@substring_char\n");
    printf("\n");
    printf("                add LF@start_index LF@start_index int@1\n");
    printf("                jump substring_cycle\n");
    printf("\n");
    printf("            label set_substring_to_nil\n");
    printf("                move LF@substring nil@nil\n");
    printf("\n");
    printf("            label substring_cycle_end\n");
    printf("\n");
    printf("            pushs LF@substring\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#ord\n");
    printf("    label ord\n");
    printf("        createframe\n");
    printf("        pushframe\n");
    printf("\n");
    printf("            defvar LF@string\n");
    printf("            pops LF@string\n");
    printf("\n");
    printf("            defvar LF@first_char_value\n");
    printf("\n");
    printf("            # Pokud je string neprázdný, vrátíme první znak v ascii\n");
    printf("            jumpifeq is_empty LF@string string@\n");
    printf("\n");
    printf("                defvar LF@first_char\n");
    printf("\n");
    printf("                getchar LF@first_char LF@string int@0\n");
    printf("\n");
    printf("                stri2int LF@first_char_value LF@first_char int@0\n");
    printf("\n");
    printf("            jump ord_end\n");
    printf("\n");
    printf("                # Pokud je string prázdný, vrátíme nil\n");
    printf("                label is_empty\n");
    printf("                move LF@first_char_value int@0\n");
    printf("\n");
    printf("            label ord_end\n");
    printf("\n");
    printf("            pushs LF@first_char_value\n");
    printf("        popframe\n");
    printf("    return\n");
    printf("\n");
    printf("#chr\n");
    printf("    label chr\n");
    printf("        pushframe\n");
    printf("            defvar LF@char_value\n");
    printf("            pops LF@char_value\n");
    printf("\n");
    printf("            defvar LF@char\n");
    printf("\n");
    printf("            int2char LF@char LF@char_value\n");
    printf("\n");
    printf("            pushs LF@char\n");
    printf("        popframe\n");
    printf("    return\n");

    printf("LABEL start&$?!\n");
    printf("DEFVAR GF@aux1&$?!\n");
    printf("DEFVAR GF@aux2&$?!\n");
    printf("DEFVAR GF@aux3&$?!\n");
}

void end_generator(){
    printf("EXIT int@0\n");
}

void write_id(char *name, bool is_local){
    if(is_local){
        printf("WRITE LF@%s\n", name);
    }else{
        printf("WRITE GF@%s\n", name);
    }
}

void write_int(int num){
    printf("WRITE int@%i\n", num);
}

void write_double(float num){
    printf("WRITE float@%f\n", num);
}

void write_string(char *str){
    char *tmp = string_prep(str);
    printf("WRITE string@%s\n", tmp);
    free(tmp);
}

void pop_arg(char *name){
    printf("DEFVAR LF@%s\n", name);
    printf("POPS LF@%s\n", name);
}

void push_id(char *name, bool is_local){
    if(is_local){
        printf("PUSHS LF@%s\n", name);
    }else{
        printf("PUSHS GF@%s\n", name);
    }
}

void push_string(char *str){
    char *tmp = string_prep(str);
    printf("PUSHS string@%s\n", tmp);
    free(tmp);
}

void push_int(int num){
    printf("PUSHS int@%i\n", num);
}

void push_double(float num){
    printf("PUSHS float@%f\n", num);
}

void push_nil(){
    printf("PUSHS nil@nil\n");void generate_func_start(char *name);
}

void generate_func_start(char *name){
    printf("JUMP %s_end\n", name);
    printf("LABEL %s\n", name);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generate_func_end(char *name, bool is_void){
    printf("POPFRAME\n");
    if(is_void){
        printf("RETURN\n");
    }else{
        printf("EXIT int@1\n");
    }
    printf("LABEL %s_end\n", name);
}

void generate_func_call(char *name){
    printf("CALL %s\n", name);
}

void clear_stack(){
    printf("CLEARS\n");
}

void generate_if_start(int num){
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS &%i_false\n", num);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generate_if_middle(int num){
    printf("POPFRAME\n");
    printf("JUMP &%i_end\n", num);
    printf("LABEL &%i_false\n", num);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generate_if_end(int num){
    printf("POPFRAME\n");
    printf("LABEL &%i_end\n", num);
}

void generate_while_start(int num){
    printf("LABEL &%i_start\n", num);
}

void generate_while_middle(int num){
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS &%i_end\n", num);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generate_while_end(int num){
    printf("POPFRAME\n");
    printf("JUMP &%i_start\n", num);
    printf("LABEL &%i_end\n", num);
}

void generate_variable(char *name, bool is_local){
    if(is_local){
        printf("DEFVAR LF@%s\n", name);
    }else{
        printf("DEFVAR GF@%s\n", name);
    }
}

void generate_return(){
    printf("RETURN\n");
}

void generate_assign(char *name, bool is_local){
    if(is_local){
        printf("POPS LF@%s\n", name);
    }else{
        printf("POPS GF@%s\n", name);
    }
}

void generate_addition(){
    printf("ADDS\n");
}

void generate_subtraction(){
    printf("SUBS\n");
}

void generate_multiplication(){
    printf("MULS\n");
}

void generate_division_int(){
    printf("IDIVS\n");
}

void generate_division_double(){
    printf("DIVS\n");
}

void generate_concatenation(){
    printf("POPS GF@aux1&$?!\n");
    printf("POPS GF@aux2&$?!\n");
    printf("CONCAT GF@aux3&$?! GF@aux2&$?! GF@aux1&$?!\n");
    printf("PUSHS GF@aux3&$?!\n");
}

void generate_nil_dif(int num){
    printf("POPS GF@aux1&$?!\n");
    printf("POPS GF@aux2&$?!\n");
    printf("JUMPIFEQ &%i_false GF@aux2&$?! nil@nil\n", num);
    printf("PUSHS GF@aux2&$?!\n");
    printf("JUMP &%i_end\n", num);
    printf("LABEL &%i_false\n", num);
    printf("PUSHS GF@aux1&$?!\n");
    printf("LABEL &%i_end\n", num);
}

void generate_equal(){
    printf("EQS\n");
}

void generate_not_equal(){
    printf("EQS\n");
    printf("NOTS\n");
}

void generate_lesser_equal(){
    printf("GTS\n");
    printf("NOTS\n");
}

void generate_greater_equal(){
    printf("LTS\n");
    printf("NOTS\n");
}

void generate_lesser(){
    printf("LTS\n");
}

void generate_greater(){
    printf("GTS\n");
}

void generate_to_float_top(){
    printf("INT2FLOATS\n");
}

void generate_to_float_second_top(){
    printf("POPS GF@aux1&$?!\n");
    printf("INT2FLOATS\n");
    printf("PUSHS GF@aux1&$?!\n");
}