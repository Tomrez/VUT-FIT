<?php

namespace IPP\Student;

//Buffer slouzici pro ukladanich lokalnich ramcu
class FrameBuffer
{
    /** @var Frame[] */
    private array $buffer;

    private int $numOfFrames;

    public function __construct()
    {
        $this->numOfFrames = 0;
    }

    public function addFrameToBuffer(Frame $frame): void
    {
        $this->buffer[$this->numOfFrames] = $frame;
        $this->numOfFrames++;
    }

    public function getLatestFrame(): Frame
    {
        if($this->numOfFrames == 0)
        {
            fwrite (STDERR, "Zasobnik lokalnich ramcu je prazdny\n");
            exit(55);
        }
        return $this->buffer[($this->numOfFrames) - 1];
    }

    public function getLatestFrameIndex(): int
    {
        if($this->numOfFrames == 0)
        {
            fwrite (STDERR, "Zasobnik lokalnich ramcu je prazdny\n");
            exit(55);
        }
        return $this->numOfFrames - 1;
    }

    public function getFrameCount(): int
    {
        return $this->numOfFrames;
    }
    public function getNFrame(int $position): Frame
    {
        return $this->buffer[$position];
    }

    public function deleteLatestFrame(): void
    {
        $this->numOfFrames--;
        $this->buffer[$this->numOfFrames] = null;
    }

    public function findVariableInBuffer(string $name): int
    {
        if ($this->numOfFrames == 0)
        {
            fwrite(STDERR, "Neexistuje zadny LF\n");
            exit(55);
        }
        for ($i = $this->numOfFrames - 1; $i >= 0; $i--)
        {

            for ($j = 0; $j < $this->buffer[$i]->getVariableAmount(); $j++)
            {
                if ($this->buffer[$i]->getNVariable($j)->getName() == $name)
                {
                    return $i;
                }
            }
        }
        fwrite(STDERR, "Promenna se nenachazi v LF\n");
        exit(54);
    }
}