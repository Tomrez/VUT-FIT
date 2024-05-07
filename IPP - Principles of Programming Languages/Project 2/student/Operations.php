<?php

namespace IPP\Student;


/**
 * @param Interpreter $that
 * @param Instruction $instruction
 * @param Frame $globalFrame
 * @param ?FrameBuffer $localFrameBuffer
 * @param ?Frame $temporaryFrame
 */
function arithmetic($that, $instruction, $globalFrame, $localFrameBuffer, $temporaryFrame): void
{
    $that->checkArgsAmount($instruction->getNumOfArgs(), 3);

    $arg1 = $instruction->getFirstArg();
    $arg2 = $instruction->getSecondArg();
    $arg3 = $instruction->getThirdArg();

    $numOne = $that->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
    $numTwo = $that->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

    if(is_numeric($numOne) && is_numeric($numTwo))
    {
        if ($instruction->getOpcode() == "ADD")
        {
            $result = $numOne + $numTwo;
        }
        else if ($instruction->getOpcode() == "SUB")
        {
            $result = $numOne - $numTwo;
        }
        else if ($instruction->getOpcode() == "MUL")
        {
            $result = $numOne * $numTwo;
        }
        else
        {
            if($numTwo == 0)
            {
                fwrite(STDERR, "Nelze delit nulou\n");
                exit(57);
            }

            $result = $numOne / $numTwo;
        }
    }
    else
    {
        fwrite(STDERR, "Obe promenne musi byt typu int\n");
        exit(53);
    }                   

    $that->updateVariableValueAndType($arg1->getName(), $result, "int", $globalFrame, $localFrameBuffer, $temporaryFrame);
}

/**
 * @param Interpreter $that
 * @param Instruction $instruction
 * @param Frame $globalFrame
 * @param ?FrameBuffer $localFrameBuffer
 * @param ?Frame $temporaryFrame
 */
function compare($that, $instruction, $globalFrame, $localFrameBuffer, $temporaryFrame): void
{
    $that->checkArgsAmount($instruction->getNumOfArgs(), 3);

    $arg1 = $instruction->getFirstArg();
    $arg2 = $instruction->getSecondArg();
    $arg3 = $instruction->getThirdArg();

    $first = null;
    $second = null;
    $firstType = null;
    $secondType = null;

    switch ($arg2->getType())
    {
        case "int":
            $first = $that->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
            $firstType = "int";
            break;
        case "var":
            $first = $that->getVarValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);

            if (is_numeric($first))
            {
                $first = intval($first);
                $firstType = "int";
            }
            else if ($first == "true" || $first == "false")
            {
                $firstType = "bool";
            }
            else if ($first == "nil")
            {
                if ($instruction->getOpcode() == "EQ")
                {
                    $firstType = "nil";
                }
                else
                {
                    fwrite(STDERR, "Nelze porovnavat s nil\n");
                    exit(53);
                }
            }
            else
            {
                $firstType = "string";
            }
            break;
            
        case "string":
            $first = $arg2->getName();
            $firstType = "string";
            break;
        case "bool":
            $first = $arg2->getName();
            $firstType = "bool";
            break;
        case "nil":
            if ($instruction->getOpcode() == "EQ")
            {
                $first = null;
                $firstType = "nil";
                break;
            }
            else
            {
                fwrite(STDERR, "Nelze porovnavat s nil\n");
                exit(53);
            }
    }

    switch ($arg3->getType())
    {
        case "int":
            $second = $that->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
            $secondType = "int";
            break;
        case "var":
            $second = $that->getVarValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

            if (is_numeric($second))
            {
                $second = intval($second);
                $secondType = "int";
            }
            else if ($second == "true" || $second == "false")
            {
                $secondType = "bool";
            }
            else if ($second == "nil")
            {
                if ($instruction->getOpcode() == "EQ")
                {
                    $secondType = "nil";
                }
                else
                {
                    fwrite(STDERR, "Nelze porovnavat s nil\n");
                    exit(53);
                }
            }
            else
            {
                $secondType = "string";
            }
            break;

        case "string":
            $second = $arg3->getName();
            $secondType = "string";
            break;
        case "bool":
            $second = $arg3->getName();
            $secondType = "bool";
            break;
        case "nil":
            if ($instruction->getOpcode() == "EQ")
            {
                $second = null;
                $secondType = "nil";
                break;
            }
            else
            {
                fwrite(STDERR, "Nelze porovnavat s nil\n");
                exit(53);
            }
    }
    if ($firstType == $secondType|| $firstType == "nil" || $secondType == "nil")
    {
        if ($firstType == "bool")
        {
            if ($firstType == "bool")
            {
                if ($first == "true")
                {
                    $first = true;
                }
                else
                {
                    $first = false;
                }
            }
            if ($secondType == "bool")
            {
                if ($second == "true")
                {
                    $second = true;
                }
                else
                {
                    $second = false;
                }
            }
        }
        if ($instruction->getOpcode() == "LT")
        {
            if ($first < $second)
            {
                //TRUE
                $that->updateVariableValueAndType($arg1->getName(), "true", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
            else
            {
                $that->updateVariableValueAndType($arg1->getName(), "false", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
        }
        else if ($instruction->getOpcode() == "GT")
        {
            if ($first > $second)
            {
                //TRUE
                $that->updateVariableValueAndType($arg1->getName(), "true", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
            else
            {
                $that->updateVariableValueAndType($arg1->getName(), "false", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
        }
        else
        {
            //EQ
            if ($first == $second)
            {
                //TRUE
                $that->updateVariableValueAndType($arg1->getName(), "true", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
            else
            {
                //FALSE
                $that->updateVariableValueAndType($arg1->getName(), "false", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
            }
        }
    }
    else
    {
        fwrite(STDERR, "Datove typy se neshoduji\n");
        exit(53);
    }
}


/**
 * @param Interpreter $that
 * @param Instruction $instruction
 * @param Frame $globalFrame
 * @param ?FrameBuffer $localFrameBuffer
 * @param ?Frame $temporaryFrame
 */
function logic($that, $instruction, $globalFrame, $localFrameBuffer, $temporaryFrame): void
{
    $that->checkArgsAmount($instruction->getNumOfArgs(), 3);

    $arg1 = $instruction->getFirstArg();
    $arg2 = $instruction->getSecondArg();
    $arg3 = $instruction->getThirdArg();

    $first = null;
    $second = null;

    if ($arg2->getType() == "var")
    {
        $first = $that->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

        if ($first != "true" && $first != "false")
        {
            fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool\n");
            exit(53);
        }
        if ($first == "true")
        {
            $first = true;
        }
        else
        {
            $first = false;
        }
    }
    else if($arg2->getType() == "bool")
    {
        $first = $arg2->getName();

        if ($first == "true")
        {
            $first = true;
        }
        else
        {
            $first = false;
        }
    }
    else if($arg2->getType() == "nil")
    {
        $first = null;
    }
    else
    {
        fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool");
        exit(53);
    }



    if ($arg3->getType() == "var")
    {
        $second = $that->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

        if ($second != "true" && $second != "false")
        {
            fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool\n");
            exit(53);
        }
        if ($second == "true")
        {
            $second = true;
        }
        else
        {
            $second = false;
        }
    }
    else if($arg3->getType() == "bool")
    {
        $second = $arg3->getName();

        if ($second == "true")
        {
            $second = true;
        }
        else
        {
            $second = false;
        }
    }
    else if($arg3->getType() == "nil")
    {
        $second = null;
    }
    else
    {
        fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool");
        exit(53);
    }
    if ($instruction->getOpcode() == "AND")
    {
        $result = $first && $second;
    }
    else
    {
        $result = $first || $second;
    }

    if ($result)
    {
        $that->updateVariableValueAndType($arg1->getName(), "true", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
    }
    else
    {
        $that->updateVariableValueAndType($arg1->getName(), "false", "bool", $globalFrame, $localFrameBuffer, $temporaryFrame);
    }
}