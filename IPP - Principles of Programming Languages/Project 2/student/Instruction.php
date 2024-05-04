<?php

namespace IPP\Student;

/**
 * Trida Instruction
 * Slouzi pro ukladani informaci o jednotlivych instrukci
 * @param $order uchovava informaci o order, kterym byla instrukce oznacena ve vstupnim XML kodu
 * @param $opcode obsahuje informaci o jakou instrukci se jedna
 * @param $args je instance tridy Arguments, ktera uchovava informace o argumentech dane instrukce
 * @param $realOrder obsahuje informaci o realnem poradi dane instrukce, kde se v XML kodu nachazela
 */

class Instruction {
    
    private int $order;
    private string $opcode;
    private Arguments $args;
    private int $realOrder;

    /**
     * @param int $order
     * @param string $opcode
     * @param string[] $args
     * @param int $argsNum
     * @param int $realOrder
     */
    public function __construct(int $order, string $opcode, array $args, int $argsNum, int $realOrder) {
        $this->order = $order;
        $this->opcode = $opcode;
        $this->args = new Arguments($args, $argsNum);
        $this->realOrder = $realOrder;
    }

    public function getFirstArg(): Argument 
    {
        return $this->args->getFirstArg();
    }

    public function getSecondArg(): Argument
    {
        return $this->args->getSecondArg();
    }

    public function getThirdArg(): Argument
    {
        return $this->args->getThirdArg();
    }

    public function getNumOfArgs(): int
    {
        return $this->args->getNumberOfArgs();
    }

    public function getOpcode(): string
    {
        return $this->opcode;
    }

    public function getRealOrder(): int
    {
        return $this->realOrder;
    }

    public function getOrder(): int
    {
        return $this->order;
    }
    
}