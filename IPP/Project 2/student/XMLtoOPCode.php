<?php

namespace IPP\Student;
use IPP\Core\Interface\SourceReader;

/**
 * Funkce parse_xml
 * Funkce zpracuje vstupni xml kod a ulozi ho do jednotlivych instrukci, ktere pote vrati hlavnimu programu ke zpracovani
 * @param mixed $domDoc Vstupní soubor s XML daty
 * @return Program Navratova hodnota je instance tridy Program, ve ktere jsou ulozene vsechny instrukce a dalsi potrebne informace
 */
function parse_xml(mixed $domDoc) {
    
    $doc = $domDoc;

    //Kontrola spravnosti elementu program
    if($doc->documentElement->tagName != "program")
    {
        fwrite(STDERR, "Wrong input file format\n");
        exit(32);
    }
    
    $language = $doc->documentElement->getAttribute('language');

    //Kontrola spravnosti formatu instrukci
    foreach ($doc->documentElement->childNodes as $childNode)
    {
        if( $childNode->nodeType == XML_ELEMENT_NODE && $childNode->tagName !== 'instruction')
        {
            fwrite(STDERR,"Spatny format vstupniho souboru\n");
            exit(32);
        }
    }

    
    $instructions = [];
    $instructionsCount = 0;
    $realInstructionOrder = 1;
    $instruction_nodes = $doc->getElementsByTagName('instruction');

    //Serazeni instrukci do spravneho poradi pred dalsi praci s nimi
    $instruction_nodes_array = [];
    foreach ($instruction_nodes as $node)
    {
        
        $instruction_nodes_array[] = $node;
    }
    usort($instruction_nodes_array, function ($a, $b) {
        $orderA = (int)$a->getAttribute('order');
        $orderB = (int)$b->getAttribute('order');
        return $orderA <=> $orderB; // Compare the order attributes
    });
    

    //Zpracovani jednotlivych instrukci a ulozeni parametru order, opcode a args
    foreach ($instruction_nodes_array as $instruction_node) {


        $order = (int)$instruction_node->getAttribute('order');
        if ($order == null)
        {
            fwrite(STDERR, "Chybi instruction order\n");
            exit(32);
        }
        $opcode = $instruction_node->getAttribute('opcode');
        if ($opcode == null)
        {
            fwrite(STDERR, "Chybi opcode\n");
            exit(32);
        }
        $opcode = strtoupper($opcode);
        $args = [];
        $argsNum = 0; //Celkovy pocet argumentu

        $argExistArray = [false, false, false];

        //Zpracovani argumentu instrukce
        foreach ($instruction_node->childNodes as $arg) {
            if ($arg->nodeType == XML_ELEMENT_NODE) {
                
                //Kontrola spravnosti formatu elementu argument
                if (! preg_match('/^arg(\d+)$/', $arg->tagName, $matches))
                {
                    fwrite(STDERR,"Spatny format vstupniho souboru\n");
                    exit(32);
                }
                $argOrder = (int)substr($arg->tagName, 3); //TODO 1,2,3 ---> 0,1,2 2xOrder, 2xOrder - 1
                if ($argOrder < 1 || $argOrder > 3)
                {
                    fwrite(STDERR, "Spatny nazev argument node\n");
                    exit(32);
                }

                
                //Ziskani spravneho poradi argumentu
                $name = $arg->nodeValue;
                $name = trim($name);
                $type = $arg->getAttribute('type');
                if ($type == null)
                {
                    fwrite(STDERR, "Spatny format argumentu\n");
                    exit (32);
                }

                if ($argOrder == 1)
                {
                    $args[0] = $type;
                    $args[1] = $name;
                }
                else if ($argOrder == 2)
                {
                    $args[2] = $type;
                    $args[3] = $name;
                }
                else
                {
                    $args[4] = $type;
                    $args[5] = $name;
                }
                
                //Kontrola existence vice souboru se stejnym order
                if ($argExistArray[$argOrder - 1] == true)
                {
                    fwrite(STDERR, "Nelze mit dva argumenty se stejnym cislem\n");
                    exit(32);
                }
                else
                {
                    $argExistArray[$argOrder - 1] = true;
                }
                
                $argsNum++;

            }
        }
        //Kontrola, jestli byl zadan pozadovany pocet argumentu
        if ($argsNum == 3 && ($argExistArray[2] == false || $argExistArray[1] == false || $argExistArray[0] == false ))
        {
            fwrite(STDERR, "Spatne argumenty\n");
            exit(32);
        }
        else if ($argsNum == 2 && ($argExistArray[0] == false || $argExistArray[1] == false))
        {
            fwrite(STDERR, "Spatne argumenty\n");
            exit(32);
        }
        else if ($argsNum == 1 && $argExistArray[0] == false)
        {
            fwrite(STDERR, "Spatne argumenty\n");
            exit(32);
        }
        //Vytvoreni nove instance tridy Instruction a ulozeni do pole
        $instructions[] = new Instruction($order, $opcode, $args, $argsNum, $realInstructionOrder);
        $realInstructionOrder++;
        $instructionsCount++;
    }

    //Kontrola duplicitnich orders
    for ($i = 0; $i < $instructionsCount; $i++)
    {
        foreach($instructions as $instruction)
        {
            if ($instruction->getOrder() == $instructions[$i]->getOrder() && $instruction->getRealOrder() != $instructions[$i]->getRealOrder())
            {
                fwrite(STDERR, "Nesmi existovat dve instrukce se stejnym order\n");
                exit(32);
            }
            else if($instruction->getOrder() < 1)
            {
                fwrite(STDERR, "Order nesmi byt mensi nez 1\n");
                exit(32);
            }
        }
    }

    //Kontrola spravnosti jazyka vstupniho souboru
    if (strtolower($language) != "ippcode24")
    {
        fwrite(STDERR, "Spatny jazyk XML souboru\n");
        exit(32);
    }

    //Navraceni instance tridy Program pripravene pro zpracovani hlavnim programem
    return new Program($instructions, $instructionsCount);
}