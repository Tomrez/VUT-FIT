<?php

namespace IPP\Student;
use IPP\Core\FileSourceReader;
use IPP\Core\Interface\InputReader;
use IPP\Core\ReturnCode;

include("XMLtoOPCode.php");

use Exception;
use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\NotImplementedException;
use SymTable;

class Interpreter extends AbstractInterpreter
{
    /**
     * @param string $var Nazev promenne
     * @return string Nazev ramce, ve kterem se ma promenna nachazet
     */
    public function varFrame(string $var): string
    {
        $split = explode("@", $var);
        return $split[0];
    }

    /**
     * @param string $var Nazev promenne
     * @return string Nazev promenne bez specifikace ramce
     */
    public function varName(string $var): string
    {
        $split = explode("@", $var);
        array_shift($split);
        return implode("@", $split);
    }

    /**
     * @param Argument $arg Argument, jehoz ciselnou hodnotu pozadujeme
     * @param Frame $globalFrame Globalni ramec, ktery se vyuzije v pripade, ze se promenna nachazi v nem
     * @param ?FrameBuffer $localFrameBuffer Buffer lokalnich ramcu, nebo null pokud neexistuje
     * @param ?Frame $temporaryFrame Docasny ramec, v pripade neexistence null
     * @return ?int Navratova hodnota int nebo null v pripade neuspechu
     */
    public function getIntValue(Argument $arg, Frame $globalFrame, ?FrameBuffer $localFrameBuffer, ?Frame $temporaryFrame): ?int
    {
        if($arg->getType() == "int")
        {
            if (!is_numeric($arg->getName()))
            {
                fwrite(STDERR, "Spatna hodnota v datovem typu int\n");
                exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
            }
            return intval($arg->getName());
        }
        else if($arg->getType() == "var")
        {
                $number = $this->getVariableFromFrames($arg, $globalFrame, $localFrameBuffer, $temporaryFrame);

                if($number != null)
                {
                    if(! is_numeric($number->getValue()))
                    {
                        fwrite(STDERR, "Spatna hodnota v promenne\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    else
                    {
                        return intval($number->getValue());
                    }
                }
                else
                {
                    fwrite(STDERR, "nelze pristupovat k promenne bez hodnoty\n");
                    exit(ReturnCode::OPERAND_VALUE_ERROR);
                }
        }
        else
        {
            fwrite(STDERR, "Nelze provest aritmetickou operaci nad daty");
            exit(ReturnCode::OPERAND_TYPE_ERROR);
        }
    }

    /**
     * @param Argument $arg Argument, jehoz retezec pozadujeme
     * @param Frame $globalFrame Globalni ramec, ktery se vyuzije v pripade, ze se promenna nachazi v nem
     * @param ?FrameBuffer $localFrameBuffer Buffer lokalnich ramcu, nebo null pokud neexistuje
     * @param ?Frame $temporaryFrame Docasny ramec, v pripade neexistence null
     * @return ?string Navratova hodnota string nebo null v pripade neuspechu
     */
    public function getStringValue(Argument $arg, Frame $globalFrame, ?FrameBuffer $localFrameBuffer, ?Frame $temporaryFrame): ?string
    {
        if($arg->getType() == "string")
        {
            return $arg->getName();
        }
        else if($arg->getType() == "var")
        {
            $string = $this->getVariableFromFrames($arg, $globalFrame, $localFrameBuffer, $temporaryFrame);
            return $string->getValue();
        }
        else
        {
            fwrite(STDERR, "Nelze ziskat string hodnotu promenne\n");
            exit(ReturnCode::OPERAND_TYPE_ERROR);
        }
    }

    /**
     * @param Argument $arg Argument, ve kterem se nachazi nazev promenne
     * @param Frame $globalFrame Globalni ramec, ktery se vyuzije v pripade, ze se promenna nachazi v nem
     * @param ?FrameBuffer $localFrameBuffer Buffer lokalnich ramcu, nebo null pokud neexistuje
     * @param ?Frame $temporaryFrame Docasny ramec, v pripade neexistence null
     * @return ?string Navratova hodnota string nebo null v pripade neuspechu
     */
    public function getVarValue(Argument $arg, Frame $globalFrame, ?FrameBuffer $localFrameBuffer, ?Frame $temporaryFrame): ?string
    {
        $var = $this->getVariableFromFrames($arg, $globalFrame, $localFrameBuffer, $temporaryFrame);
        return $var->getValue();

    }

    /**
     * @param Instruction[] $instructions
     * @return mixed[] 
     */

    public function loadLabels(array $instructions): ?array
    {
        $labels = [];
        $numOfLabels = 0;
        $nameArray = [];

        foreach ($instructions as $instruction)
        {
            if ($instruction->getOpcode() == "LABEL")
            {
                if(! in_array($instruction->getFirstArg()->getName(), $nameArray))
                {
                    $labels[$numOfLabels] = new Label($instruction->getFirstArg()->getName(),($instruction->getRealOrder()) - 1);
                    $nameArray[$numOfLabels] = $instruction->getFirstArg()->getName();
                    $numOfLabels++;
                }
                else
                {
                    fwrite(STDERR, "Stejny label existuje dvakrat\n");
                    exit(ReturnCode::SEMANTIC_ERROR);
                } 
            }
        }
        return [$labels, $nameArray];
    }

    /**
     * @param Argument $arg Argument, jehoz datovy typ chceme zjistit
     * @param Frame $globalFrame Globalni ramec, ktery se vyuzije v pripade, ze se promenna nachazi v nem
     * @param ?FrameBuffer $localFrameBuffer Buffer lokalnich ramcu, nebo null pokud neexistuje
     * @param ?Frame $temporaryFrame Docasny ramec, v pripade neexistence null
     * @return string Navratova hodnota int nebo null v pripade neuspechu
     */
    public function getDataType(Argument $arg, Frame $globalFrame, ?FrameBuffer $localFrameBuffer, ?Frame $temporaryFrame): string
    {
        $varName = $this->varName($arg->getName());
        $varVal = null;

        $varVal = $this->getVariableFromFrames($arg, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

        if(is_numeric($varVal))
        {
            return "int";
        }
        else if($varVal == "true" || $varVal == "false")
        {
            return "bool";
        }
        else
        {
            return "string";
        }
    }

    /**
     * @param Argument $arg Argument s nazvem promenne, kterou chceme najit
     * @param Frame $globalFrame Globalni ramec, ktery se vyuzije v pripade, ze se promenna nachazi v nem
     * @param ?FrameBuffer $localFrameBuffer Buffer lokalnich ramcu, nebo null pokud neexistuje
     * @param ?Frame $temporaryFrame Docasny ramec, v pripade neexistence null
     * @return Variable Navratova hodnota int nebo null v pripade neuspechu
     */
    public function getVariableFromFrames(Argument $arg, Frame $globalFrame, ?FrameBuffer $localFrameBuffer, ?Frame $temporaryFrame): Variable
    {
        $varName = $this->varName($arg->getName());
        $varFrame = $this->varFrame($arg->getName());

        $var = null;

        if($varFrame == "GF")
        {
            if ($globalFrame->getVariableAmount() >0)
            {
                $var = $globalFrame->getVariable($varName);
            }
            else
            {
                fwrite(STDERR, "GF je prazdny\n");
                exit(ReturnCode::VARIABLE_ACCESS_ERROR);
            }
        }
        else if($varFrame == "LF")
        {
            $index = $localFrameBuffer->findVariableInBuffer($varName);
            $var = $localFrameBuffer->getNFrame($index)->getVariable($varName);
        }
        else if($varFrame == "TF")
        {
            if ($temporaryFrame != null)
            {
                $var = $temporaryFrame->getVariable($varName);
            }
            else
            {
                fwrite(STDERR, "TF neexistuje\n");
                exit(ReturnCode::FRAME_ACCESS_ERROR);
            }
        }
        else
        {
            fwrite(STDERR, "Nelze pouzit jiny nazev ramce nez GF/LF/TF\n");
            exit(ReturnCode::FRAME_ACCESS_ERROR);
        }


        if($var != null)
        {
            return $var;
        }
        else
        {
            fwrite(STDERR, "Promenna nebyla v danem ramci nalezena\n");
            exit(ReturnCode::VARIABLE_ACCESS_ERROR);
        }
    }


    public function execute(): int
    {
        // TDO: Start your code here
        // Check \IPP\Core\AbstractInterpreter for predefined I/O objects:
        $dom = $this->source->getDOMDocument();
        // $val = $this->input->readString();
        // $this->stdout->writeString("stdout");
        // $this->stderr->writeString("stderr");
        //throw new NotImplementedException;
        
        //TODO nil v equals a osetrit vsude jinde
        

        $program = parse_xml($dom);
        $globalFrame = new Frame("GF");
        $temporaryFrame = null;
        $localFrameBuffer = new FrameBuffer();
        $dataStack = new Stack();
        $returnStack = new Stack();

        [$labelArray, $labelNameArray] = $this->loadLabels($program->getInstructions());

        for( $i = 0; $i < $program->getNumOfInstructions(); $i++)
        {
            $instruction = $program->getNInstruction($i);

            switch ($instruction->getOpcode())
            {
                case "MOVE":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();

                    if($arg2->getType() == "var")
                    {
                        $moveFrom = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();
                    }
                    else if($arg2->getType() == "int")
                    {
                        $moveFrom = intval($arg2->getName());
                    }
                    else if($arg2->getType() == "bool")
                    {
                        if($arg2->getName() == "true")
                        {
                            $moveFrom = "true";
                        }
                        else if($arg2->getName() == "false")
                        {
                            $moveFrom = "false";
                        }
                        else
                        {
                            fwrite(STDERR, "Spatna promenna bool\n");
                            exit(ReturnCode::VALUE_ERROR);
                        }
                    }
                    else
                    {
                        $moveFrom = $arg2->getName();
                    }

                    if ($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $moveFrom);
                        
                        if ($arg2->getType() == "int")
                        {
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "int");
                        }
                        else if ($arg2->getType() == "bool")
                        {
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if ($arg2->getType() == "nil")
                        {
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "nil");
                        }
                        else
                        {
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                        break;
                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                        if ($temporaryFrame->getVariableAmount() == 0)
                        {
                            fwrite(STDERR, "Nelze presunout promennou do TF, protoze zde neni zadna promenna vytvorena\n");
                            exit(ReturnCode::VARIABLE_ACCESS_ERROR);
                        }

                        $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $moveFrom);
                        if ($arg2->getType() == "int")
                        {
                           $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "int");
                        }
                        else if ($arg2->getType() == "bool")
                        {
                           $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if ($arg2->getType() == "nil")
                        {
                           $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "nil");
                        }
                        else
                        {
                           $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($this->varName($arg1->getName()));
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $moveFrom);
                        
                        if ($arg2->getType() == "int")
                        {
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "int");
                        }
                        else if ($arg2->getType() == "bool")
                        {
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if ($arg2->getType() == "nil")
                        {
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "nil");
                        }
                        else
                        {
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Neexistujici frame\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }

                    break;
                
                case "CREATEFRAME":
                    if ($instruction->getNumOfArgs() != 0)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    if ($temporaryFrame != null)
                    {
                        $temporaryFrame == null;
                    }
                    $temporaryFrame = new Frame("TF");

                    break;
                
                case "PUSHFRAME":
                    if ($instruction->getNumOfArgs() != 0)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    if($temporaryFrame == null)
                    {
                        fwrite(STDERR,  "Nelze vytvorit lokalni ramec bez existujiciho docasneho\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }
                    $frameToPush = $temporaryFrame;
                    $frameToPush->updateVisibility("LF");
                    $temporaryFrame = null;

                    $localFrameBuffer->addFrameToBuffer($frameToPush);
                    break;
                
                case "POPFRAME":
                    if ($instruction->getNumOfArgs() != 0)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $temporaryFrame = null;
                    $temporaryFrame = $localFrameBuffer->getLatestFrame();
                    $temporaryFrame->updateVisibility("TF");
                    $localFrameBuffer->deleteLatestFrame();
                    break;

                case "DEFVAR":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();

                    $frame = $this->varFrame($arg1->getName());
                    $name = $this->varName($arg1->getName());

                    $variableToAdd = new Variable($name, null, null);
                    if($frame == "GF")
                    {
                        $globalFrame->addVariable($variableToAdd);
                    }
                    else if($frame == "LF")
                    {
                        $localFrameBuffer->getNFrame($localFrameBuffer->getLatestFrameIndex())->addVariable(($variableToAdd));
                    }
                    else if($frame == "TF")
                    {

                        if ($temporaryFrame != null)
                        {
                            $temporaryFrame->addVariable($variableToAdd);
                        }
                        else
                        {
                            fwrite(STDERR, "Docasny ramec neexistuje\n");
                            exit(ReturnCode::FRAME_ACCESS_ERROR);
                        }
                    }
                    break;

                case "CALL":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();

                    //Pri return se vratim na instruction[i], kde i = poradi predchozi instrukce, protoze se cisluje od 1
                    $returnStack->stackPush(($instruction->getRealOrder()) - 1);

                    $index = array_search($arg1->getName(), $labelNameArray);

                    if($index !== false)
                    {
                        $i = $labelArray[$index]->getPosition();
                    }
                    else
                    {
                        fwrite(STDERR, "Pozadovany label nebyl nalezen\n");
                        exit(ReturnCode::SEMANTIC_ERROR);
                    }
                    break; 

                case "RETURN":
                    if ($instruction->getNumOfArgs() != 0)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $i = $returnStack->stackPop();

                    break;

                case "PUSHS":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();

                    if ($arg1->getType() == "var")
                    {
                        $var = $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        $varToPush = new Variable($arg1->getName(), $var->getValue(), "var");
                    }
                    else
                    {
                        $varToPush = new Variable($arg1->getName(), null, $arg1->getType());
                    }

                    
                    $dataStack->stackPush($varToPush);

                    break;

                case "POPS":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                     
                    $popVar = $dataStack->stackPop();

                    if ($popVar->getType() == "var")
                    {

                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $popVar->getValue());
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), $popVar->getType());

                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                                $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $popVar->getValue());
                                $temporaryFrame->updateVariableType($this->varName($arg1->getName()), $popVar->getType());
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $popVar->getValue());
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), $popVar->getType());
                        }
                        else
                        {
                            fwrite(STDERR, "Nelze udelat pop jinam nez do existujicich ramcu\n");
                            exit(ReturnCode::FRAME_ACCESS_ERROR);
                        }
                    }
                    else
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $popVar->getName());
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), $popVar->getType());

                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                                $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $popVar->getName());
                                $temporaryFrame->updateVariableType($this->varName($arg1->getName()), $popVar->getType());
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $popVar->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), $popVar->getType());
                        }
                        else
                        {
                            fwrite(STDERR, "Nelze udelat pop jinam nez do existujicich ramcu\n");
                            exit(ReturnCode::FRAME_ACCESS_ERROR);
                        }
                    }

                    break;

                case "ADD":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $numOne = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                    $numTwo = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

                    if(is_numeric($numOne) && is_numeric($numTwo))
                    {
                        $add = $numOne + $numTwo;
                    }
                    else
                    {
                        fwrite(STDERR, "Obe promenne musi byt typu int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);                        
                    }

                    if($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $add);
                        $globalFrame->updateVariableType($this->varName($arg1->getName()), "int");

                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                         $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $add);
                         $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $add);
                        $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else
                    {
                        fwrite(STDERR, "Neexistujici ramec\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }

                    break;

                case "SUB":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $numOne = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                    $numTwo = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

                    if(is_numeric($numOne) && is_numeric($numTwo))
                    {
                        $sub = $numOne - $numTwo;
                    }
                    else
                    {
                        fwrite(STDERR, "Obe promenne musi byt typu int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $sub);
                        $globalFrame->updateVariableType($this->varName($arg1->getName()), "int");

                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                         $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $sub);
                         $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $sub);
                        $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else
                    {
                        fwrite(STDERR, "Neexistujici ramec\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }

                    break;

                case "MUL":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $numOne = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                    $numTwo = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

                    if(is_numeric($numOne) && is_numeric($numTwo))
                    {
                        $mul = $numOne * $numTwo;
                    }
                    else
                    {
                        fwrite(STDERR, "Obe promenne musi byt typu int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    
                    if($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $mul);
                        $globalFrame->updateVariableType($this->varName($arg1->getName()), "int");

                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                         $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $mul);
                         $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $mul);
                        $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else
                    {
                        fwrite(STDERR, "Neexistujici ramec\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }

                    break;
                    
                
                case "IDIV":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $numOne = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                    $numTwo = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

                    if($numTwo == 0)
                    {
                        fwrite(STDERR, "Nelze delit nulou\n");
                        exit(ReturnCode::OPERAND_VALUE_ERROR);
                    }

                    if(is_numeric($numOne) && is_numeric($numTwo))
                    {
                        $div = $numOne / $numTwo;
                    }
                    else
                    {
                        fwrite(STDERR, "Obe promenne musi byt typu int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    
                    if($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $div);
                        $globalFrame->updateVariableType($this->varName($arg1->getName()), "int");

                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                         $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $div);
                         $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $div);
                        $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "int");
                    }
                    else
                    {
                        fwrite(STDERR, "Neexistujici ramec\n");
                        exit(ReturnCode::FRAME_ACCESS_ERROR);
                    }

                    break;

                case "LT":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

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
                            $first = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $firstType = "int";
                            break;
                        case "var":
                            $first = $this->getVarValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);

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
                                fwrite(STDERR, "Nelze porovnavat s nil\n");
                                exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                            fwrite(STDERR, "Nelze porovnavat s nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    switch ($arg3->getType())
                    {
                        case "int":
                            $second = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $secondType = "int";
                            break;
                        case "var":
                            $second = $this->getVarValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

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
                                fwrite(STDERR, "Nelze porovnavat s nil\n");
                                exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                            fwrite(STDERR, "Nelze porovnavat s nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    if ($firstType == $secondType)
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
                            if ($second == "true")
                            {
                                $second = true;
                            }
                            else
                            {
                                $second = false;
                            }
                        }

                        if ($first < $second)
                        {
                            //TRUE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                        else
                        {
                            //FALSE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Datove typy se neshoduji\n");
                        exit(53);
                    }
                    break;

                case "GT":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

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
                            $first = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $firstType = "int";
                            break;
                        case "var":
                            $first = $this->getVarValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);

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
                                fwrite(STDERR, "Nelze porovnavat s nil\n");
                                exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                            fwrite(STDERR, "Nelze porovnavat s nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    switch ($arg3->getType())
                    {
                        case "int":
                            $second = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $secondType = "int";
                            break;
                        case "var":
                            $second = $this->getVarValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

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
                                fwrite(STDERR, "Nelze porovnavat s nil\n");
                                exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                            fwrite(STDERR, "Nelze porovnavat s nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);

                    }
                    if ($firstType == $secondType)
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
                            if ($second == "true")
                            {
                                $second = true;
                            }
                            else
                            {
                                $second = false;
                            }
                        }

                        if ($first > $second)
                        {
                            //TRUE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                        else
                        {
                            //FALSE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Datove typy se neshoduji\n");
                        exit(53);
                    }
                    break;

                case "EQ":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

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
                            $first = $this->getIntValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $firstType = "int";
                            break;
                        case "var":
                            $first = $this->getVarValue($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);

                            if (is_numeric($first))
                            {
                                $first = intval($first);
                                $firstType = "int";
                            }
                            else if ($first == "true" || $first == "false")
                            {
                                $firstType = "bool";
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
                            $first = null;
                            $firstType = "nil";
                            break;
                    }

                    switch ($arg3->getType())
                    {
                        case "int":
                            $second = $this->getIntValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                            $secondType = "int";
                            break;
                        case "var":
                            $second = $this->getVarValue($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);

                            if (is_numeric($second))
                            {
                                $second = intval($second);
                                $secondType = "int";
                            }
                            else if ($second == "true" || $second == "false")
                            {
                                $secondType = "bool";
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
                            $second = null;
                            $secondType = "nil";
                            break;
                    }
                    if ($firstType == $secondType || $firstType == "nil" || $secondType == "nil")
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

                        if ($first == $second)
                        {
                            //TRUE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                        else
                        {
                            //FALSE
                            if($this->varFrame($arg1->getName()) == "GF")
                            {
                                $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
        
                            }
                            else if($this->varFrame($arg1->getName()) == "TF")
                            {
                                    $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                                    $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else if($this->varFrame($arg1->getName()) == "LF")
                            {
                                $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                                $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                                $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                            }
                            else
                            {
                                fwrite(STDERR, "Neexistujici ramec\n");
                                exit(ReturnCode::FRAME_ACCESS_ERROR);
                            }
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Datove typy se neshoduji\n");
                        exit(53);
                    }
                    break;

                case "AND":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $first = null;
                    $second = null;

                    if ($arg2->getType() == "var")
                    {
                        $first = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($first != "true" && $first != "false")
                        {
                            fwrite(STDERR, "Funkce AND vyzaduje dve promenne typu bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                        fwrite(STDERR,  "Funkce AND vyzaduje dve promenne typu bool");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }



                    if ($arg3->getType() == "var")
                    {
                        $second = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($second != "true" && $second != "false")
                        {
                            fwrite(STDERR, "Funkce AND vyzaduje dve promenne typu bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                        fwrite(STDERR, "Funkce AND vyzaduje dve promenne typu bool");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                
                    $andResult = $first && $second;

                    if ($andResult)
                    {
                        //TRUE
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }
                    else
                    {
                        //FALSE
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }
                  

                    break;

                case "OR":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $first = null;
                    $second = null;

                    if ($arg2->getType() == "var")
                    {
                        $first = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($first != "true" && $first != "false")
                        {
                            fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }



                    if ($arg3->getType() == "var")
                    {
                        $second = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($second != "true" && $second != "false")
                        {
                            fwrite(STDERR, "Funkce OR vyzaduje dve promenne typu bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                
                    $orResult = $first || $second;

                    if ($orResult)
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }
                    else
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }
                    break;

                case "NOT":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();

                    $first = null;

                    if ($arg2->getType() == "var")
                    {
                        $first = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($first != "true" && $first != "false")
                        {
                            fwrite(STDERR, "Funkce NOT vyzaduje promennou typu bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
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
                    else
                    {
                        fwrite(STDERR, "Funkce NOT vyzaduje promennou typu bool");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                
                    if ($first == true)
                    {
                        $notResult = false;
                    }
                    else
                    {
                        $notResult = true;
                    }

                    if ($notResult)
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "true");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "true");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }
                    else
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "bool");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), "false");
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), "false");
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "bool");
                        }
                    }

                    break;

                
                case "INT2CHAR":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();

                    if ($arg2->getType() == "var")
                    {
                        $unicodeVal = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if (! is_numeric($unicodeVal))
                        {
                            fwrite(STDERR, "INT2CHAR vyzaduje ciselnou hodnotu\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        else
                        {
                            $unicodeVal = intval($unicodeVal);
                        }
                    }
                    else if($arg2->getType() == "int")
                    {
                        $unicodeVal = intval($arg2->getName());
                    }
                    else
                    {
                        fwrite(STDERR, "INT2CHAR vyzaduje ciselnou hodnotu\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    
                    $charVal = mb_chr($unicodeVal);
                    if ($charVal == false)
                    {
                        fwrite(STDERR, "Nebyl zadan validni unicode symbol\n");
                        exit(ReturnCode::STRING_OPERATION_ERROR);
                    }

                    if($arg1->getType() == "var")
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $charVal);
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "string");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                            $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $charVal);
                            $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $charVal);
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                    }
                    break;

                case "STRI2INT":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    if ($arg2->getType() == "var")
                    {
                        $string = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        if ($string->getType() == "bool" || $string->getType() == "int")
                        {
                            fwrite(STDERR, "Operace STRI2INT lze provest pouze nad retezcem\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $string = $string->getValue();
                    }
                    else if($arg2->getType() == "string")
                    {
                        $string = $arg2->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Operace STRI2INT lze provest pouze nad retezcem\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if ($arg3->getType() == "var")
                    {
                        $stringPos = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();
                        if (! is_numeric($stringPos))
                        {
                            fwrite(STDERR, "STRI2INT vyzaduje jako treti argument cislo\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $stringPos = intval($stringPos);
                    }
                    else if ($arg3->getType() == "int")
                    {
                        $stringPos = intval($arg3->getName());
                    }
                    else
                    {
                        fwrite(STDERR, "Ocekaval se inteeger jakozto treti argument\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $stringLength = strlen($string);
                    if ($stringPos >= $stringLength)
                    {
                        fwrite(STDERR, "Indexace mimo delku stringu\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $char = $string[$stringPos];
                    $unicode = ord($char);
                   
                    if ($arg1->getType() == "var")
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $unicode);
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "string");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $unicode);
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $unicode);
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "string");
                        }                        

                    }
                    else
                    {
                        fwrite(STDERR, "Prvni argument GETCHAR musi byt promenna\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    break;


                case "READ":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();

                    $readVal = null;
                    $readType = null;

                    if ($arg2->getType() != "type")
                    {
                        fwrite(STDERR, "Druhy argument read musi byt typu type\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    if ($arg2->getName() == "int")
                    {
                        $readVal = $this->input->readInt();
                        $readType = $arg2->getName();  

                        if ($readVal == null)
                        {
                            $readType = "nil";                      
                        }
                    }
                    else if ($arg2->getName() == "string")
                    {
                        $readVal = $this->input->readString();
                        $readType = $arg2->getName();
                    }
                    else if ($arg2->getName() == "bool")
                    {
                        $readVal = $this->input->readString();
                        $readType = $arg2->getName();

                        if ($readVal == "true")
                        {
                            $readVal = "true";
                        }
                        else if ($readVal == "false")
                        {
                            $readVal = "false";
                        }
                        else if ($readVal == null)
                        {
                            $readVal = null;
                            $readType = "nil";
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Argument type muze nabyvat pouze hodnot int/string/bool\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if($arg1->getType() == "var")
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $readVal);
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), $readType);
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $readVal);
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), $readType);
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $readVal);
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), $readType);
                        }
                        else
                        {
                            fwrite(STDERR, "Nelze ulozit promennou jinam nez do zakladnich ramcu\n");
                            exit(ReturnCode::FRAME_ACCESS_ERROR);
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Prvni argument read musi byt variable\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    break;


                case "WRITE":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    
                    if ($arg1->getType() == "var")
                    {
                        $toWrite = $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame);

                        if ($toWrite->hasValue() == false)
                        {
                            exit(ReturnCode::VALUE_ERROR);
                        }

                        if($toWrite->getType() == "int")
                        {
                            $this->stdout->writeInt(intval($toWrite->getValue()));
                        }
                        else if ($toWrite->getType() == "nil")
                        {
                            break;
                        }
                        else if($toWrite->getType() == "bool")
                        {
                            if($toWrite->getValue() == "true")
                            {
                                $this->stdout->writeBool(true);
                            }
                            else if($toWrite->getValue() == "false")
                            {
                                $this->stdout->writeBool(false);
                            }
                            else
                            {
                                fwrite(STDERR, "spatna hodnota ulozena v bool promenne\n");
                                exit(ReturnCode::VALUE_ERROR);
                            }
                        }
                        else if($toWrite->getType() == "string")
                        {
                            $specChars = $toWrite->getValue();

                            $specCharsReplaced = str_replace(
                                array('\\010', '\\032', '\\035', '\\092'),
                                array("\n", ' ', '#', "\\"),
                                $specChars
                            );

                            $toWrite->updateValue($specCharsReplaced);


                            $this->stdout->writeString($toWrite->getValue());
                        }
                        else
                        {
                            $specChars = $toWrite->getValue();

                            $specCharsReplaced = str_replace(
                                array('\\010', '\\032', '\\035', '\\092'),
                                array("\n", ' ', '#', "\\"),
                                $specChars
                            );

                            $toWrite->updateValue($specCharsReplaced);

                            echo $toWrite->getValue();
                        }
                    }
                    else if($arg1->getType() == "int")
                    {
                        $toWrite = $arg1->getName();
                        if (! is_numeric($toWrite))
                        {
                            fwrite(STDERR, "V datovem typu int se nenachazi cislo\n");
                            exit(ReturnCode::VALUE_ERROR);
                        }
                        $this->stdout->writeInt(intval($toWrite));
                    }
                    else if ($arg1->getType() == "nil")
                    {
                        break;
                    }
                    else if($arg1->getType() == "bool")
                    {
                        $toWrite = $arg1->getName();

                        if ($toWrite == "true")
                        {
                            $this->stdout->writeBool(true);
                        }
                        else if ($toWrite == "false")
                        {
                            $this->stdout->writeBool(false);
                        }
                        else
                        {
                            fwrite(STDERR, "V datovem typu bool se nachazi spatna hodnota\n");
                            exit(ReturnCode::VALUE_ERROR);
                        }
                    }
                    else
                    {
                        $toWrite = $arg1->getName();

                        $specChars = $toWrite;

                        $specCharsReplaced = str_replace(
                            array('\\010', '\\032', '\\035', '\\092'),
                            array("\n", ' ', '#', "\\"),
                            $specChars
                        );

                        $toWrite = $specCharsReplaced;

                        $this->stdout->writeString($toWrite);
                    }
                    break;

                case "CONCAT":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $firstString = null;
                    $secondString = null;
                    $concatString = null;

                    if ($arg2->getType() == "var")
                    {
                        $var = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        if ($var->getValue() == "true" || $var->getValue() == "false" || is_numeric($var->getValue()))
                        {
                            fwrite(STDERR, "Nelze pouzit concat nad daty typu int nebo bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        else if ($var->getValue() == "nil")
                        {
                            fwrite(STDERR, "Nelze pouzit concat nad daty typu nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $firstString = $var->getValue();
                    }
                    else if($arg2->getType() == "string")
                    {
                        $firstString = $arg2->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Funkce concat vyzaduje pouze data typu string\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if ($arg3->getType() == "var")
                    {
                        $var = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        if ($var->getValue() == "true" || $var->getValue() == "false" || is_numeric($var->getValue()))
                        {
                            fwrite(STDERR, "Nelze pouzit concat nad daty typu int nebo bool\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        else if ($var->getValue() == "nil")
                        {
                            fwrite(STDERR, "Nelze pouzit concat nad daty typu nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $secondString = $var->getValue();
                    }
                    else if($arg3->getType() == "string")
                    {
                        $secondString = $arg3->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Funkce concat vyzaduje pouze data typu string\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    $concatString = $firstString.$secondString;

                    if($arg1->getType() == "var")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue($concatString);
                    }
                    else
                    {
                        fwrite(STDERR, "Funkce concat uklada vysledek pouze do promenne\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    break;

                case "STRLEN":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();

                    if ($arg2->getType() == "var")
                    {
                        $var = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if ($var == "nil")
                        {
                            fwrite(STDERR,"Nelze zjistit delku nil\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        else if ($this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getType() == "bool")
                        {
                            fwrite(STDERR, "Nelze spocitat delku bool promenne\n");
                            exit(53);
                        }
                        else if (is_numeric($var))
                        {
                            fwrite(STDERR, "Nelze spocitat delku promenne int\n");
                            exit(53);
                        }
                    }
                    else if($arg2->getType() == "string")
                    {
                        $var = $arg2->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Vstup pro strlen musi byt string\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $strlen = strlen($var);

                    if($arg1->getType() == "var")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue($strlen);
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateType("int");

                    }
                    break;

                case "GETCHAR":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    if ($arg2->getType() == "var")
                    {
                        $string = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        if ($string->getType() == "bool" || $string->getType() == "int" || $string->getType() == "nil")
                        {
                            fwrite(STDERR, "Operace GETCHAR lze provest pouze nad retezcem\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $string = $string->getValue();
                    }
                    else if($arg2->getType() == "string")
                    {
                        $string = $arg2->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Operace GETCHAR lze provest pouze nad retezcem\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if ($arg3->getType() == "var")
                    {
                        $stringPos = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();
                        if (! is_numeric($stringPos))
                        {
                            fwrite(STDERR, "GETCHAR vyzaduje jako treti argument cislo\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }

                        $stringPos = intval($stringPos);
                    }
                    else if ($arg3->getType() == "int")
                    {
                        $stringPos = intval($arg3->getName());
                    }
                    else
                    {
                        fwrite(STDERR, "Treti argument musi byt int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $stringLength = strlen($string); //pocet znaku stringu - stringPos je index (od 0)
                    if ($stringPos >= $stringLength)
                    {
                        fwrite(STDERR, "Indexace mimo delku stringu\n");
                        exit(ReturnCode::STRING_OPERATION_ERROR);
                    }

                    $getChar = $string[$stringPos];
                   
                    if ($arg1->getType() == "var")
                    {
                        if($this->varFrame($arg1->getName()) == "GF")
                        {
                            $globalFrame->updateVariableValue($this->varName($arg1->getName()), $getChar);
                            $globalFrame->updateVariableType($this->varName($arg1->getName()), "string");
    
                        }
                        else if($this->varFrame($arg1->getName()) == "TF")
                        {
                             $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $getChar);
                             $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "string");
                        }
                        else if($this->varFrame($arg1->getName()) == "LF")
                        {
                            $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                            $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $getChar);
                            $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "string");
                        }                        

                    }
                    else
                    {
                        fwrite(STDERR, "Prvni argument GETCHAR musi byt promenna\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    break;

                case "SETCHAR":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    if ($arg1->getType() != "var")
                    {
                        fwrite(STDERR, "Prvni argument SETCHAR musi byt typu var\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $stringToModify = $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                    if ($arg2->getType() == "var")
                    {
                        $stringPos = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                        if (! is_numeric($stringPos))
                        {
                            fwrite(STDERR, "Druhy argument SETCHAR musi byt cislo\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        }
                        $stringPos = intval($stringPos);
                    }
                    else if($arg2->getType() == "int")
                    {
                        $stringPos = intval($arg2->getName());
                    }
                    else
                    {
                        fwrite(STDERR, "Treti argument musi byt int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $stringLength = strlen($stringToModify);
                    if ($stringPos >= $stringLength)
                    {
                        fwrite(STDERR, "Indexace mimo string\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if ($arg3->getType() == "var")
                    {
                        $charToReplace = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        if ($charToReplace->getType() == "int" || $charToReplace->getType() == "bool" || $charToReplace->getType() == "nil")
                        {
                            fwrite(STDERR, "SETCHAR pozaduje retezec\n");
                            exit(ReturnCode::OPERAND_TYPE_ERROR);
                        } 
                        $charToReplace = $charToReplace->getValue()[0];
                    }
                    else if ($arg3->getType() == "string")
                    {
                        $charToReplace = $arg3->getName();
                        $charToReplace = $charToReplace[0];
                    }
                    else
                    {
                        fwrite(STDERR, "Treti argument musi byt char\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    $newString = $stringToModify;
                    $newString[$stringPos] = $charToReplace;

                    if($this->varFrame($arg1->getName()) == "GF")
                    {
                        $globalFrame->updateVariableValue($this->varName($arg1->getName()), $newString);
                        $globalFrame->updateVariableType($this->varName($arg1->getName()), "string");

                    }
                    else if($this->varFrame($arg1->getName()) == "TF")
                    {
                         $temporaryFrame->updateVariableValue($this->varName($arg1->getName()), $newString);
                         $temporaryFrame->updateVariableType($this->varName($arg1->getName()), "string");
                    }
                    else if($this->varFrame($arg1->getName()) == "LF")
                    {
                        $index = $localFrameBuffer->findVariableInBuffer($arg1->getName());
                        $localFrameBuffer->getNFrame($index)->updateVariableValue($this->varName($arg1->getName()), $newString);
                        $localFrameBuffer->getNFrame($index)->updateVariableType($this->varName($arg1->getName()), "string");
                    }                                  

                    break;

                case "TYPE":
                    if ($instruction->getNumOfArgs() != 2)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();


                    if ($arg2->getType() == "int")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("int");
                    }
                    else if($arg2->getType() == "string")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("string");
                    }
                    else if($arg2->getType() == "bool")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("bool");
                    }
                    else if($arg2->getType() == "nil")
                    {
                        $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("nil");
                    }
                    else if($arg2->getType() == "var")
                    {

                        if(is_numeric($this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue()))
                        {
                            $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("int");
                        }
                        else if($this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue() == "true" || $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue() == "false")
                        {
                            $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("bool");
                        }
                        else if($this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue() == null)
                        {
                            $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("nil");
                        }
                        else
                        {
                            $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->updateValue("string");
                        }
                    }

                    break;


                case "LABEL":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }
                    break;


                case "JUMP":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();

                    $index = array_search($arg1->getName(), $labelNameArray);

                    if($index !== false)
                    {
                        $i = $labelArray[$index]->getPosition();
                    }
                    else
                    {
                        fwrite(STDERR, "Pozadovany label nebyl nalezen\n");
                        exit(ReturnCode::SEMANTIC_ERROR);
                    }
                    break;

                case "JUMPIFEQ":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $arg2Type = null;
                    $arg3Type = null;
                    $arg2Value = null;
                    $arg3Value = null;

                    $first = null;
                    $second = null;

                    if ($arg2->getType() == "var")
                    {
                    
                        $arg2Type = $this->getDataType($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        $arg2Value = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                    }
                    else if ($arg2->getType() == "int")
                    {
                        $arg2Type = "int";
                        if (! is_numeric($arg2->getName()))
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu int\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                        $arg2Value = intval($arg2->getName());
                    }
                    else if ($arg2->getType() == "bool")
                    {
                        $arg2Type = "bool";
                        if ($arg2->getName() == "true")
                        {
                            $arg2Value = true;
                        }
                        else if ($arg2->getName() == "false")
                        {
                            $arg2Value = false;
                        }
                        else
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu bool\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                    }
                    else if ($arg2->getType() == "nil")
                    {
                        $arg2Value = null;
                        $arg2Type = "nil";
                    }
                    else
                    {
                        $arg2Value = $arg2->getName();
                        $arg2Type = "string";
                    }



                    if ($arg3->getType() == "var")
                    {
                    
                        $arg3Type = $this->getDataType($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        $arg3Value = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                    }
                    else if ($arg3->getType() == "int")
                    {
                        $arg3Type = "int";
                        if (! is_numeric($arg3->getName()))
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu int\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                        $arg3Value = intval($arg3->getName());
                    }
                    else if ($arg3->getType() == "bool")
                    {
                        $arg3Type = "bool";
                        if ($arg3->getName() == "true")
                        {
                            $arg3Value = true;
                        }
                        else if ($arg3->getName() == "false")
                        {
                            $arg3Value = false;
                        }
                        else
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu bool\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                    }
                    else if ($arg3->getType() == "nil")
                    {
                        $arg3Value = null;
                        $arg3Type = "nil";
                    }
                    else
                    {
                        $arg3Value = $arg3->getName();
                        $arg3Type = "string";
                    }

                    $first = $arg2Value;
                    $second = $arg3Value;

                    if ($arg2Type == $arg3Type || $arg2Type == "nil" || $arg3Type == "nil")
                    {

                        if ($first == $second)
                        {
                            $index = array_search($arg1->getName(), $labelNameArray);

                            if($index !== false)
                            {
                                $i = $labelArray[$index]->getPosition();
                            }
                            else
                            {
                                fwrite(STDERR, "Pozadovany label nebyl nalezen\n");
                                exit(ReturnCode::SEMANTIC_ERROR);
                            }
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Nelze porovnavat dva ruzne datove typy v JUMPIFEQ\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    break;

                case "JUMPIFNEQ":
                    if ($instruction->getNumOfArgs() != 3)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();
                    $arg2 = $instruction->getSecondArg();
                    $arg3 = $instruction->getThirdArg();

                    $arg2Type = null;
                    $arg3Type = null;
                    $arg2Value = null;
                    $arg3Value = null;

                    $first = null;
                    $second = null;

                    if ($arg2->getType() == "var")
                    {
                    
                        $arg2Type = $this->getDataType($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        $arg2Value = $this->getVariableFromFrames($arg2, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                    }
                    else if ($arg2->getType() == "int")
                    {
                        $arg2Type = "int";
                        if (! is_numeric($arg2->getName()))
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu int\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                        $arg2Value = intval($arg2->getName());
                    }
                    else if ($arg2->getType() == "bool")
                    {
                        $arg2Type = "bool";
                        if ($arg2->getName() == "true")
                        {
                            $arg2Value = true;
                        }
                        else if ($arg2->getName() == "false")
                        {
                            $arg2Value = false;
                        }
                        else
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu bool\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                    }
                    else if ($arg2->getType() == "nil")
                    {
                        $arg2Value = null;
                        $arg2Type = "nil";
                    }
                    else
                    {
                        $arg2Value = $arg2->getName();
                        $arg2Type = "string";
                    }



                    if ($arg3->getType() == "var")
                    {
                    
                        $arg3Type = $this->getDataType($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame);
                        $arg3Value = $this->getVariableFromFrames($arg3, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();

                    }
                    else if ($arg3->getType() == "int")
                    {
                        $arg3Type = "int";
                        if (! is_numeric($arg3->getName()))
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu int\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                        $arg3Value = intval($arg3->getName());
                    }
                    else if ($arg3->getType() == "bool")
                    {
                        $arg3Type = "bool";
                        if ($arg3->getName() == "true")
                        {
                            $arg3Value = true;
                        }
                        else if ($arg3->getName() == "false")
                        {
                            $arg3Value = false;
                        }
                        else
                        {
                            fwrite(STDERR, "Spatna hodnota v datovem typu bool\n");
                            exit(ReturnCode::OPERAND_VALUE_ERROR);
                        }
                    }
                    else if ($arg3->getType() == "nil")
                    {
                        $arg3Value = null;
                        $arg3Type = "nil";
                    }
                    else
                    {
                        $arg3Value = $arg3->getName();
                        $arg3Type = "string";
                    }

                    $first = $arg2Value;
                    $second = $arg3Value;

                    if ($arg2Type == $arg3Type || $arg2Type == "nil" || $arg3Type == "nil")
                    {

                        if ($first != $second)
                        {
                            $index = array_search($arg1->getName(), $labelNameArray);

                            if($index !== false)
                            {
                                $i = $labelArray[$index]->getPosition();
                            }
                            else
                            {
                                fwrite(STDERR, "Pozadovany label nebyl nalezen\n");
                                exit(ReturnCode::SEMANTIC_ERROR);
                            }
                        }
                    }
                    else
                    {
                        fwrite(STDERR, "Nelze porovnavat dva ruzne datove typy v JUMPIFEQ\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }
                    break;

                case "EXIT":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }
            
                    $arg1 = $instruction->getFirstArg();

                    $exitCode = null;

                    if ($arg1->getType() == "var")
                    {
                        $exitCode = $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();
                    }
                    else if ($arg1->getType() == "int")
                    {
                        $exitCode = $arg1->getName();
                    }
                    else
                    {
                        fwrite(STDERR, "Exit code musi byt typu int\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if (! is_numeric($exitCode))
                    {
                        fwrite(STDERR, "Exit code musi byt celociselna hodnota\n");
                        exit(ReturnCode::OPERAND_TYPE_ERROR);
                    }

                    if($exitCode >= 0 && $exitCode <= 9)
                    {
                        exit(intVal($exitCode));
                    }
                    else
                    {
                        fwrite(STDERR, "Exit code musi byt celociselna hodnota mezi 0 - 9 vcetne\n");
                        exit(ReturnCode::OPERAND_VALUE_ERROR);
                    }
                    //break; Should be here i guess, but is flaged as dead code, what i dont like at all, cause 0 problems >>> 1 problem


                case "DPRINT":
                    if ($instruction->getNumOfArgs() != 1)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $arg1 = $instruction->getFirstArg();

                    if ($arg1->getType() == "var")
                    {
                        $print = $this->getVariableFromFrames($arg1, $globalFrame, $localFrameBuffer, $temporaryFrame)->getValue();
                        
                        $this->stderr->writeString($print);
                    }
                    else if($arg1->getType() == "string")
                    {
                        $this->stderr->writeString($arg1->getName());
                    }
                    else if($arg1->getType() == "int")
                    {
                        $this->stderr->writeInt(intVal($arg1->getName()));
                    }
                    else if($arg1->getType() == "bool")
                    {
                        $this->stderr->writeBool(boolval($arg1->getName()));
                    }
                    else
                    {
                        $this->stderr->writeString($arg1->getName());
                    }
                    break;

                case "BREAK":
                    if ($instruction->getNumOfArgs() != 0)
                    {
                        fwrite(STDERR, "Spatny pocet argumentu\n");
                        exit(ReturnCode::INVALID_SOURCE_STRUCTURE);
                    }

                    $instructionOrder = $instruction->getOrder();
                    $instructionOpcode = $instruction->getOpcode();

                    if ($temporaryFrame != null)
                    {
                        $temporaryFrameExist = true;
                    }
                    else
                    {
                        $temporaryFrameExist = false;
                    }

                    $globalFrameVarAmount = $globalFrame->getVariableAmount();
                    $localFramesAmount = $localFrameBuffer->getFrameCount();

                    $this->stderr->writeString("Instruction order: ");
                    $this->stderr->writeInt(intval($instructionOrder));
                    $this->stderr->writeString("\n");

                    $this->stderr->writeString("Instruction opcode: ");
                    $this->stderr->writeString($instructionOpcode);
                    $this->stderr->writeString("\n");

                    $this->stderr->writeString("Does temporary frame exists: ");
                    $this->stderr->writeBool($temporaryFrameExist);
                    $this->stderr->writeString("\n");

                    $this->stderr->writeString("Amount of variables in global frame: ");
                    $this->stderr->writeInt(intVal($globalFrameVarAmount));
                    $this->stderr->writeString("\n");

                    $this->stderr->writeString("Local frames amount: ");
                    $this->stderr->writeInt(intval($localFramesAmount));
                    $this->stderr->writeString("\n");
                    break;

                    
                default:
                    fwrite(STDERR, "Neznamy opcode\n");
                    exit(ReturnCode::INVALID_SOURCE_STRUCTURE);

                    //break; Should be here as well, but dead code is not good :) 
            }
        }
        return 0;
    }
}