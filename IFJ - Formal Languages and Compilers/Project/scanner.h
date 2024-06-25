/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Petr Plíhal, xpliha02
*/

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>

// Error kódy pro fprintf
#define FILE_POINTER_NULL "Předání neplatného ukazatele na soubor\n"

#define TOKEN_ATTRIBUTE_APPEND_FAILED "Nepodařilo se alokovat paměť pro atribut tokenu\n"

#define FSM_EXITED_MAIN_LOOP "Automat se nedostal do žádného z koncových stavů -> chyba v kódu FSM\n"
#define FSM_UNDEFINED_STATE "Automat se dostal do nedefinovaného stavu -> chyba v kódu FSM\n"

#define FSM_START_INPUT_CHAR_INVALID "Stav: START ; načten nedefinovaný znak\n"

#define FSM_IDENTIFIER_INVALID_NIL_TYPE "Stav: IDENTIFIER se vyskytl '?' ale předchozí řetězec neodpovídá žádnému nil typu\n"

#define FSM_DEFAULT_VALUE_CHAR_INVALID "Stav: DEFAULT_VALUE ; po jednom znaku '?' musí následovat další znak '?'\n"

#define FSM_INTEGER_OR_FLOAT_EXPONENT_START_CHAR_INVALID_SIGN "Stav: INTEGER_EXPONENT_START nebo FLOAT_E... ; načten nedefinovaný znak, za <int>e nebo <float>e musí být + / - / číslo\n"
#define FSM_INTEGER_OR_FLOAT_EXPONENT_START_CHAR_INVALID_LACKS_NUMBER "Stav: INTEGER_EXPONENT_START nebo FLOAT_E... ; načten nedefinovaný znak, za <int>e+ nebo <float>e- musí být alespoň jedno číslo\n"
#define FSM_INTEGER_EXPONENT_START_CHAR_INVALID "Stav: INTEGER_EXPONENT_START ; načten nedefinovaný znak, za 0-9e musí být 0-9 / - / +\n"

#define FSM_FLOAT_START_CHAR_INVALID "Stav: FLOAT_START ; načten nedefinovaný znak, za 0-9.0-9e musí být 0-9 / - / +\n"

#define FSM_FLOAT_INTEGER_EXPONENT_INVALID_CHAR "Stav: FLOAT nebo INTEGER s exponenty ; nalezen ne-numerický znak"
#define FSM_FLOAT_INTEGER_EXPONENT_UNDER_OR_OVERFLOW "Stav: FLOAT nebo INTEGER s exponenty ; při konverzi na double došlo k pod nebo přetečení\n"
#define FSM_COMMENT_INPUT_CHAR_INVALID "Stav: COMMENT ; načten nedefinovaný znak\n"
#define FSM_COMMENT_BLOCK_UNEXPECTED_EOF "Stav: COMMENT_BLOCK_START ; načten nedefinovaný znak nebo EOF\n"

#define FSM_STRING_START_CHAR_INVALID "Stav: STRING_START, za začátkem komentáře \" nebyl nalezen ani \", ani povolený znak\n"
#define FSM_STRING_START_MULTILINE_NEWLINE_NOT_FOUND "Stav: STRING_START ; za sekvencí \"\"\" musí být znak odřádkování\n"
#define FSM_STRING_ONE_LINE_CHAR_INVALID "Stav: STRING_ONE_LINE ; načten nedefinovaný znak\n"
#define FSM_STRING_ONE_LINE_UNEXPECTED_EOF "Stav: STRING_ONE_LINE ; načten znak EOF před znakem \"\n"
#define FSM_STRING_ONE_LINE_UNEXPECTED_EOL "Stav: STRING_ONE_LINE ; načten znak konce řádku před znakem \"\n"

#define FSM_STRING_MULTIPLE_LINES_CHAR_INVALID "Stav: STRING_MULTIPLE_LINES ; načten nedefinovaný znak\n"
#define FSM_STRING_MULTIPLE_LINES_UNEXPECTED_EOF "Stav: STRING_MULTIPLE_LINES ; načten znak EOF před znakem '\n'\n"
#define FSM_STRING_MULTIPLE_LINES_TWO_DOUBLE_QUOTES "Stav: STRING_MULTIPLE_LINES ; načteny dva znaky \"\" za sebou bez třetího\n"
#define FSM_STRING_MULTIPLE_LINES_NEWLINE_NOT_FOUND "Stav: STRING_MULTIPLE_LINES ; před sekvencí \"\"\" musí být znak odřádkování, nebo musí být string prázdný\n"

#define FSM_ESCAPE_SEQ_UNDEFINED_CHAR "Nedefinovaný znak v escape sekvenci\n"
#define FSM_ESCAPE_SEQ_HEX_OPENING_PAR "Hexa číslo je zapsané bez '{' znaku\n"
#define FSM_ESCAPE_SEQ_HEX_CLOSING_PAR "Hexa číslo je zapsané bez '}' znaku, nebo je delší než 8 znaků\n"
#define FSM_ESCAPE_SEQ_HEX_INVALID_CHAR "Hexa číslo obsahuje ne-hexa znak\n"
#define FSM_ESCAPE_SEQ "Funkce pro zpracování escape sekvence neskončila úspěšně, chyba ve zdrojovém kódu\n"

// Návratové kódy
#define SCANNER_INITIALIZED_SUCCESSFULLY 0
#define SCANNER_TOKEN_SUCCESS 0
#define FSM_ESCAPE_SEQ_SUCCESS 0

// Barvy pro funkci print_token_type_name
#define COLOR_TEXT "\x1B[1;34m"// modrá
//#define COLOR_TEXT "\x1B[31m"// červená
//#define COLOR_TEXT "\x1B[1;32m"// zelená
#define RESET_COLOR_TEXT "\x1B[0m"
#define PRINT_TOKEN(token_name) printf(COLOR_TEXT token_name RESET_COLOR_TEXT)

// Výčet stavů automatu
typedef enum {
    // Počáteční stav
    FSM_START,

    // Stavy pro identifikátory a klíčová slova
    FSM_IDENTIFIER,
    FSM_TYPE_DOUBLE,
    FSM_NIL_TYPE_DOUBLE,
    FSM_TYPE_INTEGER,
    FSM_NIL_TYPE_INTEGER,
    FSM_TYPE_STRING,
    FSM_NIL_TYPE_STRING,
    FSM_RETURN,
    FSM_NIL,
    FSM_VAR,
    FSM_LET,
    FSM_FUNC,
    FSM_IF,
    FSM_ELSE,
    FSM_WHILE,

    // Stavy pro celá a desetinná čísla s i bez exponentu
    FSM_INTEGER,
    FSM_INTEGER_EXPONENT_START,
    FSM_INTEGER_EXPONENT_PLUS_START,
    FSM_INTEGER_EXPONENT_PLUS,
    FSM_INTEGER_EXPONENT_MINUS_START,
    FSM_INTEGER_EXPONENT_MINUS,
    FSM_FLOAT_START,
    FSM_FLOAT,
    FSM_FLOAT_EXPONENT_START,
    FSM_FLOAT_EXPONENT_PLUS_START,
    FSM_FLOAT_EXPONENT_PLUS,
    FSM_FLOAT_EXPONENT_MINUS_START,
    FSM_FLOAT_EXPONENT_MINUS,

    // Stavy pro řetězec
    FSM_STRING_START,
    FSM_STRING_ONE_LINE,
    FSM_STRING,
    FSM_STRING_ESCAPE,
    FSM_STRING_ESCAPE_HEX,
    FSM_STRING_MULTIPLE_LINES,

    // Stavy pro komentáře
    FSM_COMMENT,
    FSM_COMMENT_BLOCK_START,
    FSM_COMMENT_BLOCK,
    FSM_COMMENT_LINE_START,
    FSM_COMMENT_LINE,
    
    // Stavy pro operátory
    FSM_PLUS,
    FSM_MINUS,
    FSM_RETURN_TYPE_SYMBOL,
    FSM_MULTIPLY,
    FSM_NOT,
    FSM_NOT_EQUAL,
    FSM_ASSIGN_EQUAL,
    FSM_LOGICAL_EQUAL,
    FSM_LESSER,
    FSM_LESSER_OR_EQUAL,
    FSM_GREATER,
    FSM_GREATER_OR_EQUAL,
    FSM_DEFAULT_VALUE,// == ??

    // Stavy pro oddělovače
    FSM_LEFT_PARENTHESIS,
    FSM_RIGHT_PARENTHESIS,
    FSM_LEFT_CURLY_BRACE,
    FSM_RIGHT_CURLY_BRACE,
    FSM_COMMA,
    FSM_COLON,

    // Stav pro konec souboru
    FSM_EOF,
} fsm_scanner_state;



typedef enum {
    // Hodnota inicializovaného tokenu, hodnotu nabývá pouze při inicializaci FSM
    TOKEN_UNKNOWN,

    // Tokeny pro identifikátory a klíčová slova
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,

    // Tokeny pro celá a desetinná čísla s i bez exponentu
    TOKEN_INTEGER,
    TOKEN_INTEGER_EXPONENT_PLUS,
    TOKEN_INTEGER_EXPONENT_MINUS,
    TOKEN_FLOAT,
    TOKEN_FLOAT_EXPONENT_PLUS,
    TOKEN_FLOAT_EXPONENT_MINUS,

    // Token pro řetězec
    TOKEN_STRING,
  
    // Tokeny pro operátory
    TOKEN_DIVIDE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_RETURN_TYPE_SYMBOL,
    TOKEN_MULTIPLY,
    TOKEN_NOT,
    TOKEN_NOT_EQUAL,
    TOKEN_ASSIGN_EQUAL,
    TOKEN_LOGICAL_EQUAL,
    TOKEN_LESSER,
    TOKEN_LESSER_OR_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_OR_EQUAL,
    TOKEN_DEFAULT_VALUE, // == ??

    // Tokeny pro oddělovače
    TOKEN_LEFT_PARENTHESIS,
    TOKEN_RIGHT_PARENTHESIS,
    TOKEN_LEFT_CURLY_BRACE,
    TOKEN_RIGHT_CURLY_BRACE,
    TOKEN_COMMA,
    TOKEN_COLON,

    //Token potrebny ve vyrazech
    TOKEN_BOOL,
    TOKEN_DOLLAR,

    // Token pro konec souboru
    TOKEN_EOF,
    
} token_type;

typedef enum {
    // Hodnota po inicializaci
    KEYWORD_UNKNOWN,

    KEYWORD_DOUBLE,
    KEYWORD_DOUBLE_NIL,
    KEYWORD_INT,
    KEYWORD_INT_NIL,
    KEYWORD_STRING,
    KEYWORD_STRING_NIL,

    KEYWORD_RETURN,
    KEYWORD_NIL,
    KEYWORD_VAR,
    KEYWORD_LET,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_WHILE,

    KEYWORD_READSTRING,
    KEYWORD_READINT,
    KEYWORD_READDOUBLE,

    KEYWORD_WRITE,

    KEYWORD_INT2DOUBLE,
    KEYWORD_DOUBLE2INT,

    KEYWORD_LENGTH,
    KEYWORD_SUBSTRING,
    KEYWORD_ORD,
    KEYWORD_CHR,
} keyword;

typedef struct attribute {
    keyword keyword;
    char *string;
    int string_size;
    int integer;
    double decimal;
    bool newline_before;
} data_type_attribute;

typedef struct token {
    token_type type;
    data_type_attribute attribute;
    int line_number;
} data_type_token;

int initialize_scanner (FILE *file_from_main);

int get_token (data_type_token *token);

void token_destroy(data_type_token **token);

void print_token_type_name (data_type_token *token, int print_details);

#endif //SCANNER_H