# Parser - IPPcode24 to XML
## Project evaluation
### 6.2/7 points
# Implementační dokumentace k 1. úloze do IPP 2023/2024
Jméno a přimení: Tomáš Řezníček
Login: xrezni33


## 1. Úvod
Úkolem první části projektu bylo zpracovat vstupní kód IPPcode24 na výstupní XML kód a provést nad kódem lexikální a syntaktickou analýzu 

## 2. Implementace
Script dostane na standartní __vstup kód ve formátu IPPcode24__, což je tříadresný kód. Po zpracování vygeneruje na standartní __výstup výsledný XML kód__. Script na začátku svého chodu zkontroluje přítomnost argumentů zadaných při spuštění programu. Jediný správný argument je argument -h nebo --help, který vyvolá nápovědu k programu a poté program ukončí s exit code 0 značícím úspěch. V případě výskytu jiných argumentů konční program exit code 10. Po kontrole argumentů přejde script k analýze kódu. Díky jednoduchosti vstupního kódu, kdy vždy platí, že se na jednom řádku nachází pouze jedna instrukce, jsem se rozhodl provádět lexikální a syntaktickou analýzu v jednom průchodu. Při vzniku scriptu jsem zvolil variantu, kdy porovnávám jednotlivé řádky kódu se všemi možnými patterny, které jsem si definoval pomocí __RegEx__ a hledá se shoda. V případě, že řádek na vstupu odpovídá některému z patternu, se přejde k syntaktické analýze daného řádku, v opačném případě program končí exit code 23. Syntaktická analýza dále zkontroluje správnost jednotlivých argumentů každé instrukce. V případě úspěchu vrátí funkce provádějící analýzu pole tokenů, které odtud putují do funkce, která zajišťuje výpis výstupního XML kódu na výstup. Každý token obsahuje informace o instrukci a všech jejích argumentech.