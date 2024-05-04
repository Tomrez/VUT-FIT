<?php

namespace IPP\Student;

class Stack
{
    /** @var mixed[] */
    private array $stackArray;
    private int $stackTopPos;

    public function __construct()
    {
        $this->stackTopPos = 0;
    }

    public function stackPush(mixed $var): void
    {
        $this->stackArray[$this->stackTopPos] = $var;
        $this->stackTopPos++;
    }
    public function stackPop(): mixed
    {
        if($this->stackTopPos > 0)
        {
            $this->stackTopPos--;
            return $this->stackArray[$this->stackTopPos];
        }
        else
        {
            fwrite(STDERR, "Nelze popnout promennou, protoze se na zasobniku zadna nenachazi\n");
            exit(56);
        }
    }

    public function stackTop(): mixed
    {
        if($this->stackTopPos > 0)
        {
            return $this->stackArray[$this->stackTopPos - 1];
        }
        fwrite(STDERR, "Nelze ziskat vrchni promennou ze zasobniku\n");
        exit(54);
    }
}