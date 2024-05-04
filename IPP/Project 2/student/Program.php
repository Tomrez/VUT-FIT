<?php

namespace IPP\Student;

class Program {


    /** $instructions je pole instanci tridy Instuction a je v nem ulozen cely program 
     *  @var Instruction[]
    */
    private array $instructions;

    /** Promenna $numOfInstructions uchovava informaci o celkovem poctu instrukci v $instructions*/
    private int $numOfInstructions = 0;

    /**
     * @param Instruction[] $instructions
     * @param int $numberOfInstructions
     */
    public function __construct(array $instructions, int $numberOfInstructions) 
    {
        $this->instructions = $instructions;
        $this->numOfInstructions = $numberOfInstructions;
    }

    public function getNumOfInstructions():int
    {
        return $this->numOfInstructions;
    }

    /**
     * @return Instruction[]
     */
    public function getInstructions(): array
    {
        return $this->instructions;
    }

    public function getNInstruction(int $index): Instruction
    {
        return  $this->instructions[$index];
    }
}