<?php

namespace IPP\Student;
use IPP\Core\ReturnCode;

/**
 * Trida Arguments
 * Slouzi pro ukladani argumentu dane instrukce
 * Pri svem chodu vyuziva instance tridy Argument
 * @param $arg1 slouzi pro ulozeni informaci o prvnim argumentu instrukce
 * @param $arg2 slouzi pro ulozeni informaci o druhem argumentu instrukce
 * @param $arg3 slouzi pro ulozeni informaci o tretim argumentu instrukce
 * @param $numOfArgs slouzi pro ulozeni poctu argumentu, ktere se v dane instrukci nachazi
 */
class Arguments
{

    /**
     * Promenne pro ulozeni jednotlivych argumentu
     * Vzdy je vyuzit jen nutny pocet promennych, ostatni zustanou s hodnotou null
     */
    private ?Argument $arg1 = null;
    private ?Argument $arg2 = null;
    private ?Argument $arg3 = null;

    /**
     * Promenna pro uchovavani poctu ulozenych argumentu v dane instanci
     */
    private int $numberOfArgs = 0;

    /**
     * @param string[] $args pole argumentu
     * @param int $argNum pocet argumentu
     */
    public function __construct(array $args, int $argNum)
    {
        if ($argNum == 1)
        {
            $this->addOneArg($args[0], $args[1]);
        }
        else if ($argNum == 2)
        {
            $this->addTwoArgs($args[0], $args[1], $args[2], $args[3]);
        }
        else if ($argNum == 3)
        {
            $this->addThreeArgs($args[0], $args[1], $args[2], $args[3], $args[4], $args[5]);
        }

    }

    /**
     * Funkce addOneArg
     * Tato funkce muze byt zavolana pouze konstruktorem a to v pripade, kdy vytvarime instrukci s jednim argumentem
     * @param $arg1Type Obsahuje typ argumentu
     * @param $arg1Name Obsahuje nazev argumentu
     */
    public function addOneArg(string $arg1Type, string $arg1Name): void
    {
        $this->arg1 = new Argument($arg1Type, $arg1Name);

        $this->numberOfArgs = 1;
    }
    /**
     * Funkce addTwoArgs
     * Tato funkce muze byt zavolana pouze konstruktorem a to v pripade, kdy vytvarime instrukci se dvema argumenty
     * @param $arg1Type Obsahuje typ prvniho argumentu
     * @param $arg1Name Obsahuje nazev prvniho argumentu
     * @param $arg2Type Obsahuje typ druheho argumentu
     * @param $arg2Name Obsahuje nazev druheho argumentu
     */
    public function addTwoArgs(string $arg1Type, string $arg1Name, string $arg2Type, string $arg2Name): void
    {
        $this->arg1 = new Argument($arg1Type, $arg1Name);
        $this->arg2 = new Argument($arg2Type, $arg2Name);

        $this->numberOfArgs = 2;
    }
    /**
     * Funkce addThreeArgs
     * Tato funkce muze byt zavolana pouze konstruktorem a to v pripade, kdy vytvarime instrukci se tremi argumenty
     * @param $arg1Type Obsahuje typ prvniho argumentu
     * @param $arg1Name Obsahuje nazev prvniho argumentu
     * @param $arg2Type Obsahuje typ druheho argumentu
     * @param $arg2Name Obsahuje nazev druheho argumentu
     * @param $arg2Type Obsahuje typ tretiho argumentu
     * @param $arg2Name Obsahuje nazev tretiho argumentu
     */
    public function addThreeArgs(string $arg1Type, string $arg1Name, string $arg2Type, string $arg2Name, string $arg3Type, string $arg3Name): void
    {
        $this->arg1 = new Argument($arg1Type, $arg1Name);
        $this->arg2 = new Argument($arg2Type, $arg2Name);
        $this->arg3 = new Argument($arg3Type, $arg3Name);

        $this->numberOfArgs = 3;
    }
    /**
     * Funkce getNumberOfArgs
     * Tato funkce vraci pocet argumentu, ktere instrukce obsahuje
     * @return int Return je pocet argumentu ulozenych v teto instanci
     */
    public function getNumberOfArgs(): int
    {
        return $this->numberOfArgs;
    }


    /**
     * Funkce getFirstArg
     * Funkce vraci prvni argument dane instrukce
     * @return Argument je prvni argument
     */
    public function getFirstArg() : Argument
    {
        if ($this->arg1 === null)
        {
            fwrite(STDERR, "Nelze ziskat druhy argument\n");
            exit(ReturnCode::INTERNAL_ERROR);
        }
        return $this->arg1;
    }

    /**
     * Funkce getSecondArg
     * Funkce vraci druhy argument dane instrukce
     * @return Argument Return je druhy argument
     */
    public function getSecondArg(): Argument
    {
        if($this->arg2 == null)
        {
            fwrite(STDERR, "Nelze ziskat druhy argument\n");
            exit(ReturnCode::INTERNAL_ERROR);
        }
        return $this->arg2;
    }
    /**
    * Funkce getThirdArg
    * Funkce vraci treti argument dane instrukce
    * @return Argument Return je treti argument
    */
    public function getThirdArg(): Argument
    {
        if ($this->arg3 === null)
        {
            fwrite(STDERR, "Nelze ziskat druhy argument\n");
            exit(ReturnCode::INTERNAL_ERROR);
        }
        return $this->arg3;
    }
}