/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Petr Plíhal, xpliha02
*/

#include "scanner.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// Globální proměnné - pro zachování souboru a řádků i mezi voláními get_token
static FILE *input_file;
int file_line_number = 1;

/**
 * @brief Zkontroluje jestli je na vstupu ukazatel na soubor a nastaví tento soubor jako zdroj pro get_token
 *
 * @param *file_from_main ukazatel na soubor ze stdin
 * @return Návratovou hodnotu 0 (SCANNER_INITIALIZED_SUCCESSFULLY) v případě úspěchu, v případě nenalezení souboru na vstupu !=0 (ERROR_INTERNAL)
 */
int initialize_scanner (FILE *file_from_main) {

    input_file = file_from_main;
    
    if (input_file == NULL)
    {
        fprintf(stderr, FILE_POINTER_NULL);
        return ERROR_INTERNAL;
    }
 
    return SCANNER_INITIALIZED_SUCCESSFULLY;
}

/**
 * @brief Přidání znaku do string atributu tokenu - bez koncového '\0'
 *
 * @param *token ukazatel na strukturu tokenu
 * @param znak pro přidání
 */
void token_attribute_append (data_type_token *token, char input_char) {

    // Zvětší velikost alokované paměti pro atribut tokenu
    token->attribute.string = realloc(token->attribute.string, token->attribute.string_size + 1);

    // Kontrola alokace paměti
    if (token->attribute.string == NULL)
    {
        fprintf(stderr, TOKEN_ATTRIBUTE_APPEND_FAILED);
        exit(ERROR_INTERNAL);
    }

    // Přidání znaku na konec atributu tokenu
    token->attribute.string[token->attribute.string_size] = input_char;

    // Zvětšení velikosti atributu tokenu
    token->attribute.string_size++;
}

/**
 * @brief Smaže string z atributu tokenu - pro keywords, identifiers, a int/float s exponentem
 *
 * @param *token ukazatel na strukturu tokenu
 */
void token_attribute_string_delete (data_type_token *token) {

    free(token->attribute.string);
    token->attribute.string = NULL;
    token->attribute.string_size = 0;
}

/**
 * @brief Dealokuje paměť pro token
 *
 * @param **token ukazatel na ukazatel na strukturu tokenu
 */
void token_destroy(data_type_token **token) {
    
    if (*token != NULL) {

        if ( (*token)->attribute.string != NULL) {
            // Uvolnění paměti pro string
            token_attribute_string_delete(*token);
        }

        // Uvolnění samotného tokenu
        free(*token);
        *token = NULL;
    }
}

/**
 * @brief Přepíše exponent z atributu string do decimal
 *
 * @param *token ukazatel na strukturu tokenu
 * @return Návratovou hodnotu 0 () v případě úspěšného přepsání, v případě pře/pod tečení hodnotu != 0
 */
int token_attribute_exp_num_to_float (data_type_token *token) {

    char *error_pointer;

    double exp_number = strtod(token->attribute.string, &error_pointer);

    if ( error_pointer == token->attribute.string )
    {
        // Nalezení ne-numerické části -> string už byl kontrolován, nemělo by nastat
        fprintf(stderr, FSM_FLOAT_INTEGER_EXPONENT_INVALID_CHAR);
        return ERROR_LEXICAL_ANALYSIS;
    }
    else if ( *error_pointer != '\0' ) 
    {
        fprintf(stderr, FSM_FLOAT_INTEGER_EXPONENT_UNDER_OR_OVERFLOW);
        return ERROR_LEXICAL_ANALYSIS;
    }
    else 
    {
        // Zapsání čísla s exponentem do atributu tokenu
        token->attribute.decimal = exp_number;

        // Uvolnění paměti pro string
        token_attribute_string_delete(token);

        return 0;
    }
}

/**
 * @brief Zkontroluje a zapíše exponent pro token typu integer nebo float do stringu
 *
 * @param *token ukazatel na strukturu tokenu
 * @return Návratovou hodnotu funkce token_attribute_exp_num_to_float
 */
int token_with_exponent_append (data_type_token *token) {

    // Atribut všech tokenů s exponentem bude vždy se znakem malého 'e'
    token_attribute_append(token, 'e');
    char input_char = fgetc(input_file);

    if ( input_char == '+')
    {
        //exponent_sign_plus = true;
        token_attribute_append(token, input_char);
    }
    else if ( input_char == '-')
    {
        //exponent_sign_plus = false;
        token_attribute_append(token, input_char);
    }
    else if ( isdigit(input_char) )
    {
        //exponent_sign_plus = true;
        token_attribute_append(token, '+');
        // Vrátí číslo pro příští kontrolu
        ungetc(input_char, input_file);
    }
    else
    {
        // Za <int>e nebo <float>e musí být + / - / číslo
        fprintf(stderr, FSM_INTEGER_OR_FLOAT_EXPONENT_START_CHAR_INVALID_SIGN);
        return ERROR_LEXICAL_ANALYSIS;
    }

    input_char = fgetc(input_file);

    if ( isdigit(input_char) )
    {
        token_attribute_append(token, input_char);
        input_char = fgetc(input_file);
    }
    else
    {
        // Za <int>e- nebo <float>e+ musí být alespoň jedno číslo
        fprintf(stderr, FSM_INTEGER_OR_FLOAT_EXPONENT_START_CHAR_INVALID_LACKS_NUMBER);
        return ERROR_LEXICAL_ANALYSIS;
    }

    while ( isdigit(input_char) )
    {
        token_attribute_append(token, input_char);
        input_char = fgetc(input_file);
    }

    ungetc(input_char, input_file);

    // Zakončení znakem '\0'
    token_attribute_append(token, '\0');

    // Převede ze stringu do floatu a smaže string
    return token_attribute_exp_num_to_float(token);

}

/**
 * @brief Zkontroluje a zapíše escape sekvenci pro token typu string
 *
 * @param *token ukazatel na strukturu tokenu
 * @return Návratovou hodnotu 0 (FSM_ESCAPE_SEQ_SUCCESS) v případě úspěchu, jinak chybovou hodnotu !=0
 */
int token_with_escape_sequence_append (data_type_token *token) {

    char input_char = fgetc(input_file);

    if ( (input_char=='\"') || (input_char=='\\') || (input_char=='n') || (input_char=='r') || (input_char=='t') )
    {
        token_attribute_append(token, input_char);
        return FSM_ESCAPE_SEQ_SUCCESS;
    }
    else if ( input_char == 'u' )
    {
        token_attribute_append(token, input_char);

        input_char = fgetc(input_file);
        if ( input_char == '{' )
        {
            token_attribute_append(token, input_char);

            for (int eight_hex_chars = 0; eight_hex_chars < 8; eight_hex_chars++)
            {
                input_char = fgetc(input_file);

                if ( isxdigit(input_char) )
                {
                    token_attribute_append(token, input_char);
                }
                else
                {
                    fprintf(stderr, FSM_ESCAPE_SEQ_HEX_INVALID_CHAR);
                    return ERROR_LEXICAL_ANALYSIS;
                }
            }
            
            input_char = fgetc(input_file);
            if ( input_char == '}' )
            {
                token_attribute_append(token, input_char);
                return FSM_ESCAPE_SEQ_SUCCESS;
            }
            else
            {
                fprintf(stderr, FSM_ESCAPE_SEQ_HEX_CLOSING_PAR);
                return ERROR_LEXICAL_ANALYSIS;
            }
        }
        else
        {
            fprintf(stderr, FSM_ESCAPE_SEQ_HEX_OPENING_PAR);
            return ERROR_LEXICAL_ANALYSIS;
        }
    }
    else
    {
        fprintf(stderr, FSM_ESCAPE_SEQ_UNDEFINED_CHAR);
        return ERROR_LEXICAL_ANALYSIS;
    }

    fprintf(stderr, FSM_ESCAPE_SEQ_UNDEFINED_CHAR);
    return ERROR_INTERNAL;
}

/**
 * @brief Podle stringu v atributu tokenu určí typ tokenu (KEYWORD nebo IDENTIFIER)
 *
 * @param *token ukazatel na strukturu tokenu
 */
void determine_token_type (data_type_token *token) {

    if      ((strcmp(token->attribute.string, "Double"))==0)     {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_DOUBLE;}
    else if ((strcmp(token->attribute.string, "Double?"))==0)    {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_DOUBLE_NIL;}
    else if ((strcmp(token->attribute.string, "Int"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_INT;}
    else if ((strcmp(token->attribute.string, "Int?"))==0)       {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_INT_NIL;}
    else if ((strcmp(token->attribute.string, "String"))==0)     {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_STRING;}
    else if ((strcmp(token->attribute.string, "String?"))==0)    {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_STRING_NIL;}

    else if ((strcmp(token->attribute.string, "return"))==0)     {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_RETURN;}
    else if ((strcmp(token->attribute.string, "nil"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_NIL;}
    else if ((strcmp(token->attribute.string, "var"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_VAR;}
    else if ((strcmp(token->attribute.string, "let"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_LET;}
    else if ((strcmp(token->attribute.string, "func"))==0)       {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_FUNC;}
    else if ((strcmp(token->attribute.string, "if"))==0)         {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_IF;}
    else if ((strcmp(token->attribute.string, "else"))==0)       {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_ELSE;}
    else if ((strcmp(token->attribute.string, "while"))==0)      {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_WHILE;}

    else if ((strcmp(token->attribute.string, "readString"))==0) {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_READSTRING;}
    else if ((strcmp(token->attribute.string, "readInt"))==0)    {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_READINT;}
    else if ((strcmp(token->attribute.string, "readDouble"))==0) {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_READDOUBLE;}

    else if ((strcmp(token->attribute.string, "write"))==0)      {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_WRITE;}

    else if ((strcmp(token->attribute.string, "Int2Double"))==0) {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_INT2DOUBLE;}
    else if ((strcmp(token->attribute.string, "Double2Int"))==0) {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_DOUBLE2INT;}

    else if ((strcmp(token->attribute.string, "length"))==0)     {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_LENGTH;}
    else if ((strcmp(token->attribute.string, "substring"))==0)  {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_SUBSTRING;}
    else if ((strcmp(token->attribute.string, "ord"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_ORD;}
    else if ((strcmp(token->attribute.string, "chr"))==0)        {token->type = TOKEN_KEYWORD; token->attribute.keyword = KEYWORD_CHR;}

    else                                                  {token->type = TOKEN_IDENTIFIER;}

    if (token->type == TOKEN_KEYWORD)
    {
        token_attribute_string_delete(token);
    }
}

/**
 * @brief Vypíše typ tokenu a užitečné informace atributu
 *
 * @param *token ukazatel na strukturu tokenu
 * @param print_details 1 pro výpis detailů, 0 pro výpis jen typu tokenu
 */
void print_token_type_name (data_type_token *token, int print_details) {
    switch (token->type) {
        case TOKEN_IDENTIFIER:
            PRINT_TOKEN("TOKEN_IDENTIFIER\n");
            break;
        case TOKEN_KEYWORD:
            PRINT_TOKEN("TOKEN_KEYWORD\n");
            break;
        case TOKEN_INTEGER:
            PRINT_TOKEN("TOKEN_INTEGER\n");
            break;
        case TOKEN_FLOAT:
            PRINT_TOKEN("TOKEN_FLOAT\n");
            break;
        case TOKEN_STRING:
            PRINT_TOKEN("TOKEN_STRING\n");
            break;
        case TOKEN_DIVIDE:
            PRINT_TOKEN("TOKEN_DIVIDE\n");
            break;
        case TOKEN_PLUS:
            PRINT_TOKEN("TOKEN_PLUS\n");
            break;
        case TOKEN_MINUS:
            PRINT_TOKEN("TOKEN_MINUS\n");
            break;
        case TOKEN_RETURN_TYPE_SYMBOL:
            PRINT_TOKEN("TOKEN_RETURN_TYPE_SYMBOL\n");
            break;
        case TOKEN_MULTIPLY:
            PRINT_TOKEN("TOKEN_MULTIPLY\n");
            break;
        case TOKEN_NOT:
            PRINT_TOKEN("TOKEN_NOT\n");
            break;
        case TOKEN_NOT_EQUAL:
            PRINT_TOKEN("TOKEN_NOT_EQUAL\n");
            break;
        case TOKEN_ASSIGN_EQUAL:
            PRINT_TOKEN("TOKEN_ASSIGN_EQUAL\n");
            break;
        case TOKEN_LOGICAL_EQUAL:
            PRINT_TOKEN("TOKEN_LOGICAL_EQUAL\n");
            break;
        case TOKEN_LESSER:
            PRINT_TOKEN("TOKEN_LESSER\n");
            break;
        case TOKEN_LESSER_OR_EQUAL:
            PRINT_TOKEN("TOKEN_LESSER_OR_EQUAL\n");
            break;
        case TOKEN_GREATER:
            PRINT_TOKEN("TOKEN_GREATER\n");
            break;
        case TOKEN_GREATER_OR_EQUAL:
            PRINT_TOKEN("TOKEN_GREATER_OR_EQUAL\n");
            break;
        case TOKEN_DEFAULT_VALUE:
            PRINT_TOKEN("TOKEN_DEFAULT_VALUE\n");
            break;
        case TOKEN_LEFT_PARENTHESIS:
            PRINT_TOKEN("TOKEN_LEFT_PARENTHESIS\n");
            break;
        case TOKEN_RIGHT_PARENTHESIS:
            PRINT_TOKEN("TOKEN_RIGHT_PARENTHESIS\n");
            break;
        case TOKEN_LEFT_CURLY_BRACE:
            PRINT_TOKEN("TOKEN_LEFT_CURLY_BRACE\n");
            break;
        case TOKEN_RIGHT_CURLY_BRACE:
            PRINT_TOKEN("TOKEN_RIGHT_CURLY_BRACE\n");
            break;
        case TOKEN_COMMA:
            PRINT_TOKEN("TOKEN_COMMA\n");
            break;
        case TOKEN_COLON:
            PRINT_TOKEN("TOKEN_COLON\n");
            break;
        case TOKEN_EOF:
            PRINT_TOKEN("TOKEN_EOF\n");
            break;
        default:
            printf("\033[1;31mNeznámý typ tokenu na řádku: %i\033[0m\n", token->line_number);
            break;
    }

    if (print_details == 1)
    {

        if ( token->type == TOKEN_KEYWORD )
        {
            printf("  Atribut keyword: %i\n", token->attribute.keyword);
        }

        if ( (token->type == TOKEN_IDENTIFIER) || (token->type == TOKEN_STRING) )
        {
            printf("  Atribut string: %s\n", token->attribute.string);
            printf("    Délka atributu: %i\n", token->attribute.string_size);
        }
        
        if (token->type == TOKEN_INTEGER)
        {
            printf("  Atribut integer: %i\n", token->attribute.integer);
        }

        if (token->type == TOKEN_FLOAT)
        {
            printf("  Atribut decimal: %f\n", token->attribute.decimal);
        }

        printf("  Řádek: %i.\n", token->line_number);
        
        if (token->attribute.newline_before == true)
        {
            printf("    Token je první na řádku\n");
        }
        
        printf("\n");
    }
}


/**
 * @brief Zapíše další token do struktury na místo ukazatele
 *
 * @param *token ukazatel na strukturu tokenu
 * @return Návratovou hodnotu 0 (SCANNER_TOKEN_SUCCESS) v případě úspěchu, jinak chybovou hodnotu !=0
 */
int get_token (data_type_token *token) {

    int input_char;

    fsm_scanner_state state = FSM_START;

    token->attribute.newline_before = false;
    token->attribute.string_size = 0;

    // Hlavní cyklus pro průchody konečným automatem
    do
    {
        input_char = fgetc(input_file);
        
        switch (state) 
        {

        case FSM_START: {

            if ( isspace(input_char) )
            {
                state = FSM_START;
                if ( input_char == '\n' )
                {
                    file_line_number++;
                    token->attribute.newline_before = true;
                }
            }

            else if (  (isalpha(input_char)) || (input_char == '_')  )
            {
                token->line_number = file_line_number;
                state = FSM_IDENTIFIER;
                ungetc(input_char, input_file);
            }

            else if ( isdigit(input_char) )
            {
                token->line_number = file_line_number;
                state = FSM_INTEGER;
                ungetc(input_char, input_file);

                // TODO: je třeba inicializovat tady, protože stav INT a FLOAT + EXP cyklí pomocí FSM, ne vnitřního cyklu
                
            }

            else if ( input_char == '"' )
            {
                token->line_number = file_line_number;
                state = FSM_STRING_START;
            }

            else if ( input_char == '/' )
            {
                token->line_number = file_line_number;
                state = FSM_COMMENT;
            }

            else if ( input_char == '+' )
            {
                token->line_number = file_line_number;
                state = FSM_PLUS;
            }
            else if ( input_char == '-' )
            {
                token->line_number = file_line_number;
                state = FSM_MINUS;
            }
            else if ( input_char == '*' )
            {
                token->line_number = file_line_number;
                state = FSM_MULTIPLY;
            }
            else if ( input_char == '!' )
            {
                token->line_number = file_line_number;
                state = FSM_NOT;
            }
            else if ( input_char == '=' )
            {
                token->line_number = file_line_number;
                state = FSM_ASSIGN_EQUAL;
            }
            else if ( input_char == '<' )
            {
                token->line_number = file_line_number;
                state = FSM_LESSER;
            }
            else if ( input_char == '>' )
            {
                token->line_number = file_line_number;
                state = FSM_GREATER;
            }

            else if ( input_char == '(' )
            {
                token->line_number = file_line_number;
                state = FSM_LEFT_PARENTHESIS;
            }
            else if ( input_char == ')' )
            {
                token->line_number = file_line_number;
                state = FSM_RIGHT_PARENTHESIS;
            }
            else if ( input_char == '{' )
            {
                token->line_number = file_line_number;
                state = FSM_LEFT_CURLY_BRACE;
            }
            else if ( input_char == '}' )
            {
                token->line_number = file_line_number;
                state = FSM_RIGHT_CURLY_BRACE;
            }
            else if ( input_char == ',' )
            {
                token->line_number = file_line_number;
                state = FSM_COMMA;
            }
            else if ( input_char == ':' )
            {
                token->line_number = file_line_number;
                state = FSM_COLON;
            }
            else if ( input_char == '?' )
            {
                token->line_number = file_line_number;
                state = FSM_DEFAULT_VALUE;
            }
            
            else if ( input_char == EOF ) // fgetc vrací -1, v případě znaku EOF, nebo CHYBY
            {
                // ferror vrací nenulovou hodnotu, v případě chyby při čtení ze souboru
                if ( ferror(input_file) )
                {
                    fprintf(stderr, FSM_START_INPUT_CHAR_INVALID);
                    return ERROR_LEXICAL_ANALYSIS;
                }
                // feof vrací true v případě, že načtený znak je EOF
                else if ( !feof(input_file) )
                {
                    fprintf(stderr, FSM_START_INPUT_CHAR_INVALID);
                    return ERROR_LEXICAL_ANALYSIS;
                }

                token->line_number = file_line_number;
                state = FSM_EOF;

                // Aby se v dalším cyklu/přechodu do dalšího stavu nečetlo mimo soubor
                ungetc(input_char, input_file);
            }
            else 
            {
                fprintf(stderr, FSM_START_INPUT_CHAR_INVALID);
                return ERROR_LEXICAL_ANALYSIS;
            }
            break;

        }

        case FSM_IDENTIFIER: {

            // Načte sekvenci znaků
            while ( isalnum(input_char) || (input_char == '_') )
            {
                token_attribute_append(token, input_char);
                input_char = fgetc(input_file);
            }

            // Zjistí, jestli je korektně zapsán '?' na konci klíčového slova, pro které je nil definován
            if ( input_char == '?' )
            {
                token_attribute_append(token, input_char);

                token_attribute_append(token, '\0');

                determine_token_type(token);

                // ? se může vyskytovat pouze u klíčových slov, které mají definovaný nil typ
                if ((token->attribute.keyword != KEYWORD_DOUBLE_NIL)  &&
                    (token->attribute.keyword != KEYWORD_INT_NIL)     &&
                    (token->attribute.keyword != KEYWORD_STRING_NIL))
                {
                    fprintf(stderr, FSM_IDENTIFIER_INVALID_NIL_TYPE);
                    return ERROR_LEXICAL_ANALYSIS;
                }
                
            }
            // V případě žádného výskytu '?', je ukončující znak vrácen pro příští cyklus a funkce determine_token_type zjistí typ tokenu
            else
            {
                ungetc(input_char, input_file);

                token_attribute_append(token, '\0');

                determine_token_type(token);
            }
            
            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_INTEGER: {
            if ( isdigit(input_char) )
            {
                token_attribute_append(token, input_char);

                state = FSM_INTEGER;
            }
            else if ( input_char == 'e' || input_char == 'E' )
            {
                
                token->type = TOKEN_FLOAT;

                // Převedení ze stringu, do čísla
                return token_with_exponent_append(token);
            }
            else if ( input_char == '.' )
            {
                token_attribute_append(token, input_char);
                state = FSM_FLOAT_START;
            }
            // Znak který není 0-9 / e / E / . nalezen -> konec int čísla
            else
            {
                ungetc(input_char, input_file);

                token_attribute_append(token, '\0');

                // TODO: Chyby při převodu stringu na číslo
                token->attribute.integer = atoi(token->attribute.string);
                token_attribute_string_delete(token);

                token->type = TOKEN_INTEGER;
                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }
 
            case FSM_FLOAT_START: {

                if ( isdigit(input_char) )
                {
                    token_attribute_append(token, input_char);
                    state = FSM_FLOAT;
                }
                else
                {
                    // Za 0-9. musí být alespoň jedno číslo
                    fprintf(stderr, FSM_FLOAT_START_CHAR_INVALID);
                    return ERROR_LEXICAL_ANALYSIS;
                }
                break;
            }

            case FSM_FLOAT: {
                if ( isdigit(input_char) )
                {
                    token_attribute_append(token, input_char);
                    state = FSM_FLOAT;
                }
                else if ( input_char == 'e' || input_char == 'E' )
                {

                    token->type = TOKEN_FLOAT;

                    return token_with_exponent_append(token);

                }
                else
                {
                    ungetc(input_char, input_file);

                    token->type = TOKEN_FLOAT;

                    token_attribute_append(token, '\0');

                    // TODO: Chyby při převodu stringu na číslo
                    token->attribute.decimal = atof(token->attribute.string);
                    token_attribute_string_delete(token);

                    return SCANNER_TOKEN_SUCCESS;
                }
                break;
            }

        case FSM_STRING_START: {

            if ( ((input_char >= ' ') && (input_char <= '~'))  &&! (input_char == '"') )
            {
                ungetc(input_char, input_file);

                state = FSM_STRING_ONE_LINE;
            }

            // Nalezení sekvence ""
            else if ( input_char == '"' )
            {
                input_char = fgetc(input_file);
                // Nalezení sekvence """
                if ( input_char == '"')
                {
                    input_char = fgetc(input_file);
                    // Multiline string musí obsah od znaků """ oddělit znakem nového řádku '\n'
                    if ( input_char == '\n' )
                    {
                        file_line_number++;

                        state = FSM_STRING_MULTIPLE_LINES;
                    }
                    else
                    {
                        fprintf(stderr, FSM_STRING_START_MULTILINE_NEWLINE_NOT_FOUND);
                        return ERROR_LEXICAL_ANALYSIS;
                    }
                }
                // ""x -> prázdný string
                else
                {
                    ungetc(input_char, input_file);

                    token->type = TOKEN_STRING;

                    token_attribute_append(token, '\0');

                    return SCANNER_TOKEN_SUCCESS;
                }   
            }
            else
            {
                fprintf(stderr, FSM_STRING_START_CHAR_INVALID);
                return ERROR_LEXICAL_ANALYSIS;
            }
            break;
        }

            case FSM_STRING_ONE_LINE: {
                
                while (input_char != '"')
                {
                    if ( !((input_char >= ' ') && (input_char <= '~')) )
                    {
                        if ( input_char == '\n' )
                        {
                            file_line_number++;

                            fprintf(stderr, FSM_STRING_ONE_LINE_UNEXPECTED_EOL);
                        }
                        else if ( input_char == EOF)
                        {
                            fprintf(stderr, FSM_STRING_ONE_LINE_UNEXPECTED_EOF);
                        }
                        else
                        {
                            fprintf(stderr, FSM_STRING_ONE_LINE_CHAR_INVALID);
                        }
                        return ERROR_LEXICAL_ANALYSIS;
                    }
                    else if ( input_char == '\\' )
                    {

                        token_attribute_append(token, input_char);

                        int check_escape_sequence = token_with_escape_sequence_append(token);
                        if ( check_escape_sequence != FSM_ESCAPE_SEQ_SUCCESS )
                        {
                            return ERROR_LEXICAL_ANALYSIS;
                        }
                    }

                    // token_with_escape_sequence_append zapíše celou escape sekvenci, bez této podmínky by znovu připojil znak '\'
                    if ( input_char != '\\' )
                    {
                        token_attribute_append(token, input_char);
                    }
                    
                    input_char = fgetc(input_file);

                }

                token->type = TOKEN_STRING;

                token_attribute_append(token, '\0');

                return SCANNER_TOKEN_SUCCESS;
            }

            case FSM_STRING_MULTIPLE_LINES: {
                // Může obsahovat oproti oneline stringu navíc : ["] ; [""] ; carrige return(s) [\r] ; line feed(s) [\n]
                
                // Nelze se zastavit při nalezení jen jedné "
                while (input_char != EOF)
                {
                    if (   !( ((input_char>=' ')&&(input_char<='~')) || (input_char=='\r') || (input_char=='\n') )   )
                    {
                        fprintf(stderr, FSM_STRING_MULTIPLE_LINES_CHAR_INVALID);
                        return ERROR_LEXICAL_ANALYSIS;
                    }
                    else if ( input_char == '\\' )
                    {

                        token_attribute_append(token, input_char);

                        int check_escape_sequence = token_with_escape_sequence_append(token);
                        if ( check_escape_sequence != FSM_ESCAPE_SEQ_SUCCESS )
                        {
                            return ERROR_LEXICAL_ANALYSIS;
                        }
                    }
                    else if ( input_char == '\n' )
                    {
                        input_char = fgetc(input_file);

                        file_line_number++;

                        // Vstup '\n'"
                        if ( input_char == '"' )
                        {
                            input_char = fgetc(input_file);

                            // Vstup '\n'""
                            if ( input_char == '"' )
                            {
                                input_char = fgetc(input_file);

                                // Vstup '\n'""" -> konec komentáře
                                if ( input_char == '"' )
                                {
                                    token->type = TOKEN_STRING;
                                    token_attribute_append(token, '\0');
                                    return SCANNER_TOKEN_SUCCESS;
                                }
                                // Vstup '\n'""x -> chyba, sekvence "" nesmí být
                                else
                                {
                                    fprintf(stderr, FSM_STRING_MULTIPLE_LINES_TWO_DOUBLE_QUOTES);
                                    return ERROR_LEXICAL_ANALYSIS;
                                }
                                
                            }
                            // Vstup '\n'"x -> korektní, jeden znak " se může v bloku vyskytnout
                            else
                            {
                                // Kontrola neznámého znaku před vrácením
                                if (   !( ((input_char>=' ')&&(input_char<='~')) || (input_char=='\r') || (input_char=='\n') )   )
                                {
                                    fprintf(stderr, FSM_STRING_MULTIPLE_LINES_CHAR_INVALID);
                                    return ERROR_LEXICAL_ANALYSIS;
                                }

                                token_attribute_append(token, '\n');
                                token_attribute_append(token, '"');

                                ungetc(input_char, input_file);
                            }

                        }
                        // Vstup: '\n'x
                        else
                        {
                            // Přidám nalezenou a zpracovanou značku newline
                            token_attribute_append(token, '\n');

                            // Přeskočí přidání neznámého znaku a bude tak vyhodnocen v další iteraci zvlášť
                            ungetc(input_char, input_file);
                            input_char = '\\';
                        }
                    }
                    else if ( input_char == '"')
                    {
                        input_char = fgetc(input_file);
                        // Vstup : ""
                        if ( input_char == '"')
                        {
                            input_char = fgetc(input_file);
                            // Vstup : """ -> nekorektní ukončení komentáře, pokud jsme již nějaký znak načetli
                            if ( (input_char == '"') && (token->attribute.string_size != 0) )
                            {
                                fprintf(stderr, FSM_STRING_MULTIPLE_LINES_NEWLINE_NOT_FOUND);
                                return ERROR_LEXICAL_ANALYSIS;
                            }
                            // Vstup : """
                            // String může být prázdný -> první načtené znaky by byly POUZE """
                            //xxx"""
                            //"""
                            // Zatím není v token->attribute žádný znak a máme sekvenci """ -> konec prázdného stringu
                            else if ( (input_char == '"') && (token->attribute.string_size == 0) )
                            {
                                token->type = TOKEN_STRING;
                                token_attribute_append(token, '\0');
                                return SCANNER_TOKEN_SUCCESS;
                            }
                            // Vstup : ""x -> sekvence "" je povolená, připne se a neznámý znak se zkontroluje v příští iteraci
                            else
                            {
                                token_attribute_append(token, '"');
                                token_attribute_append(token, '"');
                                // Přeskočí přidání neznámého znaku a bude tak vyhodnocen v další iteraci zvlášť
                                ungetc(input_char, input_file);
                                input_char = '\\';
                            }
                        }
                        // Vstup : "x
                        else
                        {
                            token_attribute_append(token, '"');
                            // Přeskočí přidání neznámého znaku a bude tak vyhodnocen v další iteraci zvlášť
                            ungetc(input_char, input_file);
                            input_char = '\\';
                        }
                    }

                    // token_with_escape_sequence_append zapíše celou escape sekvenci, bez této podmínky by znovu připojil znak '\'
                    if (input_char != '\\')
                    {
                        token_attribute_append(token, input_char);
                    }
                    input_char = fgetc(input_file);
                }

                // Z cyklu vyplývá že jsme našli EOF před nalezením '\n'"""
                fprintf(stderr, FSM_STRING_MULTIPLE_LINES_UNEXPECTED_EOF);
                return ERROR_LEXICAL_ANALYSIS;

                break;
            }

        case FSM_COMMENT: {
            if ( input_char == '*' )
            {
                state = FSM_COMMENT_BLOCK_START;
            }
            else if ( input_char == '/' )
            {
                state = FSM_COMMENT_LINE_START;
            }
            else
            {
                // Za znakem '/' MUSÍ následovat '*' nebo '/'
                //fprintf(stderr, FSM_COMMENT_INPUT_CHAR_INVALID);
                //return ERROR_LEXICAL_ANALYSIS;
                ungetc(input_char, input_file);
                token->type = TOKEN_DIVIDE;
                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_COMMENT_BLOCK_START: {

                int start_or_stop_comment_block = 1;

                while ( start_or_stop_comment_block > 0 )
                {
                    // Po '/*' nebyl nalezen '*/' ale EOF
                    if ( input_char == EOF )
                    {
                        fprintf(stderr, FSM_COMMENT_BLOCK_UNEXPECTED_EOF);
                        return ERROR_LEXICAL_ANALYSIS;
                    } 

                    if ( input_char == '/' )
                    {
                        input_char = fgetc(input_file);

                        if ( input_char == '*')
                        {
                            start_or_stop_comment_block++;
                        }
                        else
                        {
                            ungetc(input_char, input_file);
                        }
                    }
                    else if ( input_char == '*' )
                    {
                        input_char = fgetc(input_file);
                        
                        if ( input_char == '/')
                        {
                            start_or_stop_comment_block--;
                        }
                        else
                        {
                            ungetc(input_char, input_file);
                        }
                    }

                    input_char = fgetc(input_file);
                }

                // Cyklus i po nalezení '*/' načetl další znak, před kontrolou počtů vnoření start_or_stop_comment_block -> vrátí ho
                ungetc(input_char, input_file);
                
                // Byl nalezen správný počet vnořených komentářů -> přechod do stavu pro vrácení tokenu
                state = FSM_COMMENT_BLOCK;
                
                break;
            }

            case FSM_COMMENT_BLOCK: {

                ungetc(input_char, input_file);

                state = FSM_START;
                break;
            }

            case FSM_COMMENT_LINE_START: {
                if ( (input_char == '\n') || (input_char == EOF) )
                {

                    ungetc(input_char, input_file);

                    state = FSM_COMMENT_LINE;
                }
                else
                {
                    state = FSM_COMMENT_LINE_START;
                }
                break;
            }

            case FSM_COMMENT_LINE: {

                ungetc(input_char, input_file);

                state = FSM_START;
                break;
            }

        case FSM_PLUS: {

            ungetc(input_char, input_file);

            token->type = TOKEN_PLUS;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_MINUS: {
            if ( input_char == '>' )
            {
                ungetc(input_char, input_file);

                state = FSM_RETURN_TYPE_SYMBOL;
            }
            else 
            {
                ungetc(input_char, input_file);

                token->type = TOKEN_MINUS;

                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_RETURN_TYPE_SYMBOL: {

                token->type = TOKEN_RETURN_TYPE_SYMBOL;
                
                return SCANNER_TOKEN_SUCCESS;
                break;
            }

        case FSM_MULTIPLY: {

            ungetc(input_char, input_file);

            token->type = TOKEN_MULTIPLY;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_NOT: {
            if ( input_char == '=' )
            {
                ungetc(input_char, input_file);

                state = FSM_NOT_EQUAL;
            }
            else
            {
                ungetc(input_char, input_file);

                token->type = TOKEN_NOT;

                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_NOT_EQUAL: {

                token->type = TOKEN_NOT_EQUAL;
                
                return SCANNER_TOKEN_SUCCESS;
                break;
            }

        case FSM_ASSIGN_EQUAL: {
            if ( input_char == '=' )
            {
                ungetc(input_char, input_file);

                state = FSM_LOGICAL_EQUAL;
            }
            else
            {
                ungetc(input_char, input_file);

                token->type = TOKEN_ASSIGN_EQUAL;

                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_LOGICAL_EQUAL: {

                token->type = TOKEN_LOGICAL_EQUAL;
                
                return SCANNER_TOKEN_SUCCESS;
                break;
            }

        case FSM_LESSER: {
            if ( input_char == '=' )
            {
                ungetc(input_char, input_file);

                state = FSM_LESSER_OR_EQUAL;
            }
            else
            {
                ungetc(input_char, input_file);

                token->type = TOKEN_LESSER;

                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_LESSER_OR_EQUAL: {

                token->type = TOKEN_LESSER_OR_EQUAL;
                
                return SCANNER_TOKEN_SUCCESS;
                break;
            }

        case FSM_GREATER: {
            if ( input_char == '=' )
            {
                ungetc(input_char, input_file);

                state = FSM_GREATER_OR_EQUAL;
            }
            else
            {
                ungetc(input_char, input_file);

                token->type = TOKEN_GREATER;

                return SCANNER_TOKEN_SUCCESS;
            }
            break;
        }

            case FSM_GREATER_OR_EQUAL: {

                token->type = TOKEN_GREATER_OR_EQUAL;
                
                return SCANNER_TOKEN_SUCCESS;
                break;
            }

        case FSM_DEFAULT_VALUE: {
            if ( input_char == '?' )
            {
                token->type = TOKEN_DEFAULT_VALUE;

                return SCANNER_TOKEN_SUCCESS;
            }
            else
            {
                fprintf(stderr, FSM_DEFAULT_VALUE_CHAR_INVALID);

                return ERROR_LEXICAL_ANALYSIS;
            }
            break;
        }

        case FSM_LEFT_PARENTHESIS: {

            ungetc(input_char, input_file);
            
            token->type = TOKEN_LEFT_PARENTHESIS;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_RIGHT_PARENTHESIS: {

            ungetc(input_char, input_file);

            token->type = TOKEN_RIGHT_PARENTHESIS;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_LEFT_CURLY_BRACE: {

            ungetc(input_char, input_file);

            token->type = TOKEN_LEFT_CURLY_BRACE;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_RIGHT_CURLY_BRACE: {

            ungetc(input_char, input_file);

            token->type = TOKEN_RIGHT_CURLY_BRACE;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_COMMA: {

            ungetc(input_char, input_file);

            token->type = TOKEN_COMMA;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_COLON: {

            ungetc(input_char, input_file);

            token->type = TOKEN_COLON;

            return SCANNER_TOKEN_SUCCESS;
            break;
        }

        case FSM_EOF: {

            // Pro případ že by bylo třeba později ověřit, že je následující token EOF
            ungetc(input_char, input_file);

            token->type = TOKEN_EOF;

            return SCANNER_TOKEN_SUCCESS;

            break;
        }

        default: {
            fprintf(stderr, FSM_UNDEFINED_STATE);
            return ERROR_INTERNAL;
        }

        }
    } while ( true );


fprintf(stderr, FSM_EXITED_MAIN_LOOP);
return ERROR_INTERNAL;

}