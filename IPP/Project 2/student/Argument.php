<?php

namespace IPP\Student;


/**
 * Trida Argument
 * Reprezentuje a uklada data jednotlivych argumentu
 * 
 * @param $type slouzi k ulozeni typu argumentu obvykle zadaneho ve vstupnim XML souboru
 * @param $name slouzi k ulozeni nazvu argumentu zadaneho ve vstupnim XML souboru
 */

class Argument
{

    private string $type;
    private string $name;

    public function __construct(string $type, string $name)
    {
        $this->type = $type;
        $this->name = $name;
    }

    /**
     * Funkce slouzici k ziskani dat z daneho argumentu
     * @return Argument Funkce vraci Argument, nad kterym byla provedena
     */
    public function getArg()
    {
        return $this;
    }

    /**
     * Funkce slouzici pro ziskani typu daneho argumentu
     * @return string Funkce vraci typ argumentu, nad kterym byla provedena
     */
    public function getType(): string
    {
        return $this->type;
    }

    /**
     * Funkce slouzici pro ziskani nazvu daneho argumentu
     * @return  string Funkce vraci nazev argumentu, nad kterym byla provedena
     */
    public function getName(): string
    {
        return $this->name;
    }

    /**
     * Funkce nastavi typ argumentu na uzivatelem zadany
     */
    public function setType(string $type): void
    {
        $this->type = $type;
    }

    /**
     * Funkce nastavi nazev argumentu na uzivatelem zadany
     */
    public function setName(string $name) :void
    {
        $this->name = $name;
    }
}