<?php

namespace IPP\Student;

class Frame
{
    private string $frameVisibility; //GF, LF, TF
    private mixed $variableArray;
    private int $variableAmount = 0;


    public function __construct(string $visibility)
    {
        if ($visibility == "GF" || $visibility == "LF" || $visibility == "TF")
        {
            $this->frameVisibility = $visibility;
        }
        else
        {
            fwrite(STDERR, "Wrong frame\n");
            exit(54);
        }        
    }

    public function getFrameType(): string
    {
        return $this->frameVisibility;
    }

    public function getVariableAmount(): int
    {
        return $this->variableAmount;
    }

    public function addVariable(Variable $variable): void
    {
        for ($i = 0; $i < $this->variableAmount; $i++)
        {
            if($this->variableArray[$i]->getName() == $variable->getName())
            {
                fwrite(STDERR, "Redefinice promenne\n");
                exit(52);
            }
        }

        $this->variableArray[$this->variableAmount] = $variable;
        $this->variableAmount++;
    }

    public function variableExistsInFrame(Variable $variable): bool
    {
        foreach ($this->variableArray as $varInArray)
        {
            if ($varInArray->getName() == $variable)
            {
                return true;
            }
        }
        return false;
    }

    public function updateVariableValue(string $variableName, mixed $value): void
    {
        foreach ($this->variableArray as $varibleFromArray)
        {
            if ($varibleFromArray->getName() == $variableName)
            {
                $varibleFromArray->updateValue($value);
            }
        }
    }

    public function updateVariableType(string $variableName, string $type): void
    {
        foreach ($this->variableArray as $varibleFromArray)
        {
            if ($varibleFromArray->getName() == $variableName)
            {
                $varibleFromArray->updateType($type);
            }
        }
    }

    public function getVariable(string $name): ?Variable
    {
        foreach ($this->variableArray as $variableFromArray)
        {
            if( $variableFromArray->getName() == $name)
            {
                return $variableFromArray;
            }
        }
        return null;
    }

    public function updateVisibility(string $newVisibility): void
    {
        if ($newVisibility == "TF" || $newVisibility == "LF")
        {
            $this->frameVisibility = $newVisibility;
        }
    }

    public function getNVariable(int $index) : ?Variable
    {
        return $this->variableArray[$index];
    }
}