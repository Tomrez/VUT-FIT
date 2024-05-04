
#IPP Project - Parser
#Author - Tomas Reznicek
#February - March 2024


import argparse
import sys
import re

#objekt Token
class Token:
    def __init__(self, tokenType, arg1=None, arg1Type=None, arg2=None, arg2Type=None, arg3=None, arg3Type=None, numberOfLine=None):
        self.tokenType = tokenType
        self.arg1 = arg1
        self.arg1Type = arg1Type
        self.arg2 = arg2
        self.arg2Type = arg2Type
        self.arg3 = arg3
        self.arg3Type = arg3Type
        self.numberOfLine = numberOfLine



def args():

    parser = argparse.ArgumentParser(description='Parser of IPPcode code. Output is XML code')

    #kontrola pritomnosti dalsich argumentu pri pozadavku na napovedu
    if '--help' in sys.argv[1:] and len(sys.argv) > 2:
        print("Additional arguments are not allowed with --help.")
        sys.exit(10)

    args = parser.parse_args()



#Funkce kontrolujici validitu hodnoty ulozene v Inteegeru    
def intCheck(intVal):
    
    #Povolene znaky v jednotlivych typech
    decimalValues = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
    hexaValues = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd', 'e', 'f']
    octaValues = ['0', '1', '2', '3', '4', '5', '6', '7'] 
    
    intValStriped = intVal
    
    
    if intVal[0] == '-':
        intValStriped = intVal.split('-')[1]
    
    #zjisteni typu cisla a jeho nasledna kontrola    
    if len(intValStriped) > 1:    
        
        #hexa
        if intValStriped[0] == '0' and intValStriped[1] == 'x':
            WannabeHexaVal = intValStriped.split('x')
            for char in WannabeHexaVal[1]:
                            
                if char not in hexaValues:
                    sys.exit(23)
        #octa    
        elif intValStriped[0] == '0' and intValStriped[1] == 'o':
            WannabeOctaVal = intValStriped.split('o')
            for char in WannabeOctaVal[1]:
                if char not in octaValues:
                    sys.exit(23)
        
    else:
        #decimal
        for char in intValStriped:
            if char not in decimalValues:
                sys.exit(23)



#funkce kontrolujici validitu stringu, pripadne prepsani problematickych znaku v XML a escape sekvence    
def stringCheck(strVal):
        strVal = strVal.replace('&', '&amp;')
        strVal = strVal.replace('<', '&lt;')  
        strVal = strVal.replace('>', '&gt;')
        
        EscapeSeq = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
        isEscape = False
        numCount = 0
        
        for char in strVal:
            if char == '\\':
                if isEscape == True:
                    exit(23)
                isEscape = True
                
            elif isEscape == True:  

                if char in EscapeSeq:
                    if numCount == 2:
                        isEscape = False
                        numCount = 0
                        continue
                        
                    numCount += 1
                else:
                    sys.exit(23)
                    
        if numCount != 0 or isEscape == True:
            sys.exit(23)        
                
        return strVal
    
    
    
#funkce kontrolujici validitu promenne a specialni znaky
def varCheck(varVal):
    
    specChars = ['_', '-', '$', '&', '%', '*', '!', '?']
    
    for char in varVal:
        
        if char == '@':
            varVal = varVal.split('@', maxsplit=1)[1]
            break
    
    if varVal[0] not in specChars:
        if not (varVal[0].isalpha() or varVal[0].isdigit()):
            sys.exit(23)
    
    #kontrola specialnich znaku na zacatku a na konci promenne
    elif len(varVal)>2 and varVal[1] == varVal[0]:
        
        if varVal[len(varVal)-1] in specChars:
            if varVal[len(varVal)-2] in specChars:
                if varVal[len(varVal)-1] == varVal[len(varVal)-2]:
                    for char in varVal[2:len(varVal) - 2]:
                        if char in specChars:
                            sys.exit(23)
                else:
                    sys.exit(23)
            else:                
                for char in varVal[2:len(varVal) - 1]:
                    if char in specChars:
                        sys.exit(23)
        else:                
            for char in varVal[2:]:
                if char in specChars:
                    sys.exit(23)
        return
    
    if len(varVal) > 1:
        if varVal[len(varVal)-1] in specChars:
            if varVal[len(varVal)-2] in specChars:
                if varVal[len(varVal)-1] == varVal[len(varVal)-2]:
                    for char in varVal[1:len(varVal) - 2]:
                        if char in specChars:
                            sys.exit(23)
                else:
                    sys.exit(23)
            else:                
                for char in varVal[1:len(varVal) - 1]:
                    if char in specChars:
                        sys.exit(23)
        else:                
            for char in varVal[1:]:
                if char in specChars:
                    sys.exit(23)

            
            
                
#Funkce zjisti typ promenne a vrati ho            
def varType(var):
    
    argTypes = [
        (r'^GF@(?!@|\d).*', 'var'),
        (r'^LF@(?!@|\d).*', 'var'),
        (r'^TF@(?!@|\d).*', 'var'),
        
        (r'^int@(?!@).', 'int'),        
        
        (r'^string@(?!@)[A-Za-z0-9]*', 'string'),
        (r'^bool@true(?![\w])', 'bool'),
        (r'^bool@false(?![\w])', 'bool'),   
        (r'^nil@nil(?![\w])', 'nil'),
        
    ]
     
    for option, argType in argTypes:
        match = re.match(option, var)
        if match:
            return argType
    
    sys.exit(23)
    
    
    
#Lexikalni a syntakticka analyza - funkce vraci tokeny (zpracovane jednotlive radky), ktere jsou dale vyuzity pro tisk XML kodu
def makeTokens():
    
    tokens = []
    lineNum = 1
    IPPcodeFlag = False
    
    for line in sys.stdin:
        
        
        #odstraneni komentaru
        line = line.split('#')[0]
        
        line = line.strip()
        #pokud je prazdny radek -> jdu na dalsi
        if not line:
            lineNum += 1
            continue
        if line[0] == '.':
            if line == '.IPPcode24':
                lineNum += 1
                if IPPcodeFlag == True:
                    sys.exit(23)
                IPPcodeFlag = True
                continue
            else:
                sys.exit(21)
        elif line in ['IPPcode24', '.IPPcode23', 'IPPcode23']:
            sys.exit(21)
        
        options = [
            (r'^MOVE\s+([^ ]+)\s+([^ ]+)$', 'MOVE'),
            (r'^CREATEFRAME$', 'CREATEFRAME'),
            (r'^PUSHFRAME$', 'PUSHFRAME'),
            (r'^POPFRAME$', 'POPFRAME'),
            (r'^DEFVAR\s+([^ ]+)$', 'DEFVAR'),
            (r'^CALL\s+([^ ]+)$', 'CALL'),
            (r'^RETURN$', 'RETURN'),

            (r'^PUSHS\s+([^ ]+)$', 'PUSHS'),
            (r'^POPS\s+([^ ]+)$', 'POPS'),

            (r'^ADD\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'ADD'),
            (r'^SUB\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'SUB'),
            (r'^MUL\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'MUL'),
            (r'^IDIV\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'IDIV'),
            
            (r'^LT\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'LT'),
            (r'^GT\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'GT'),
            (r'^EQ\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'EQ'),
            
            (r'^AND\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'AND'),
            (r'^OR\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'OR'),
            (r'^NOT\s+([^ ]+)\s+([^ ]+)$', 'NOT'),
            
            (r'^INT2CHAR\s+([^ ]+)\s+([^ ]+)$', 'INT2CHAR'),
            (r'^STRI2INT\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'STRI2INT'),

            (r'^READ\s+([^ ]+)\s+([^ ]+)$', 'READ'),
            (r'^WRITE\s+([^ ]+)$', 'WRITE'),
            
            (r'^CONCAT\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'CONCAT'),
            (r'^STRLEN\s+([^ ]+)\s+([^ ]+)$', 'STRLEN'),
            (r'^GETCHAR\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'GETCHAR'),
            (r'^SETCHAR\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'SETCHAR'),

            (r'^TYPE\s+([^ ]+)\s+([^ ]+)$', 'TYPE'),

            (r'^LABEL\s+([^ ]+)$', 'LABEL'),
            (r'^JUMP\s+([^ ]+)$', 'JUMP'),
            (r'^JUMPIFEQ\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'JUMPIFEQ'),
            (r'^JUMPIFNEQ\s+([^ ]+)\s+([^ ]+)\s+([^ ]+)$', 'JUMPIFNEQ'),
            (r'^EXIT\s+([^ ]+)$', 'EXIT'),

            (r'^DPRINT\s+([^ ]+)$', 'DPRINT'),
            (r'^BREAK$', 'BREAK'),
        ]
        
        
        flagMatch = False
        
        #CASE INSENSITIVE
        lineSplit = line.split(' ')
        lineSplit[0] = lineSplit[0].upper()
        line = " ".join(lineSplit)
        
        for option, tokenType in options:
            match = re.match(option, line)
            if match:
                if IPPcodeFlag == False:
                    sys.exit(21)
                
                
                #Instrukce pozadujici 3 vstupni argumenty
                if tokenType in ['ADD', 'SUB', 'MUL', 'IDIV', 'CONCAT', 'GETCHAR', 'SETCHAR', 'STRI2INT', 'LT', 'GT', 'EQ', 'AND', 'OR', 'JUMPIFEQ', 'JUMPIFNEQ']:
                    
                    typeOfArg1 = None
                    typeOfArg2 = varType(match.group(2))
                    typeOfArg3 = varType(match.group(3))
                    
                    
                    
                    matchGroupToCrop2 = match.group(2)
                    matchGroupToCrop3 = match.group(3)
                    
                    if typeOfArg2 != 'var':
                        matchGroupToCrop2 = match.group(2).split('@', maxsplit=1)[1]
                        
                    if typeOfArg3 != 'var':
                        matchGroupToCrop3 = match.group(3).split('@', maxsplit=1)[1]
                    
                   
                    #var symb1 symb2 - ADD, SUB, MUL, IDIV, CONCAT, GETCHAR, SETCHAR, STRI2INT
                    #! LT, GT, EQ, AND, OR, NOT
                    if tokenType not in ['JUMPIFEQ', 'JUMPIFNEQ']:
                        
                        typeOfArg1 = varType(match.group(1))
                        if typeOfArg1 != 'var':
                            sys.exit(23)
                    
                    #label symb1 symb2 - JUMPIFEQ, JUMPIFNEQ
                    else:
                        typeOfArg1 = 'label'
                        for char in match.group(1):
                            if char in ['@', '/', '\\']:
                                sys.exit(23)
                        
                        
                            
                    if typeOfArg2 not in ['var', 'int', 'string', 'bool', 'nil']:
                        sys.exit(23)
                            
                    if typeOfArg2 not in ['var', 'int', 'string', 'bool', 'nil']:
                        sys.exit(23)    
                        
                    #kontrola validni hodnoty v int    
                    if typeOfArg2 == 'int':
                        intCheck(matchGroupToCrop2)
                    if typeOfArg3 == 'int':
                        intCheck(matchGroupToCrop3)
                        
                        
                    if typeOfArg2 == 'string':
                        matchGroupToCrop2 = stringCheck(matchGroupToCrop2)
                    if typeOfArg3 == 'string':
                        matchGroupToCrop3 = stringCheck(matchGroupToCrop3)
                        
                        
                    if typeOfArg1 == 'var':
                        varCheck(match.group(1))
                    if typeOfArg2 == 'var':
                        varCheck(matchGroupToCrop2)
                    if typeOfArg3 == 'var':
                        varCheck(matchGroupToCrop3)
                        
                    tokens.append(Token(tokenType, match.group(1), typeOfArg1, matchGroupToCrop2, typeOfArg2, matchGroupToCrop3, typeOfArg3, lineNum))
                    
                    
                #Funkce pozadujici 2 vstupni argumenty    
                elif tokenType in ['MOVE', 'INT2CHAR', 'READ', 'STRLEN', 'TYPE', 'NOT']:
                    
                    matchGroupToCrop2 = match.group(2)
                    
                    typeOfArg1 = varType(match.group(1))
                    typeOfArg2 = None
                    
                    if typeOfArg1 != 'var':
                        sys.exit(23)
                    
                    #var symb - MOVE, INT2CHAR, STRLEN, TYPE, NOT
                    if tokenType != 'READ':
                        
                        typeOfArg2 = varType(match.group(2))
                        if typeOfArg2 not in ['var', 'int', 'string', 'bool', 'nil']:
                            sys.exit(23)
                            
                        if typeOfArg2 != 'var':
                            matchGroupToCrop2 = match.group(2).split('@', maxsplit=1)[1]
                    
                    #var type - READ
                    else:
                        if match.group(2) in ['int', 'string', 'bool']:
                            typeOfArg2 = 'type'
                        else:
                            sys.exit(23)
                    
                    
                    #kontrola validni hodnoty v int   
                    if typeOfArg2 == 'int':
                        intCheck(matchGroupToCrop2)  
                    
                    if typeOfArg2 == 'string':
                        matchGroupToCrop = stringCheck(matchGroupToCrop2)     
                        
                    if typeOfArg1 == 'var':
                        matchGroupToCrop = match.group(1).split('@', maxsplit=1)[1]
                        varCheck(matchGroupToCrop)   
                    
                    if typeOfArg2 == 'var':
                        varCheck(matchGroupToCrop2)              
                    
                      
                    tokens.append(Token(tokenType, match.group(1), typeOfArg1, matchGroupToCrop2, typeOfArg2, None, None, lineNum))
                
                #Funkce pozadujici jeden vstupni parametr    
                elif tokenType in ['DEFVAR', 'CALL', 'PUSHS', 'POPS', 'WRITE', 'LABEL', 'JUMP', 'EXIT', 'DPRINT']:
                    
                    matchGroupToCrop = match.group(1)
                    
                    #var - DEFVAR, POPS
                    if tokenType in ['DEFVAR', 'POPS']:
                        typeOfArg1 = varType(match.group(1))
                        if typeOfArg1 != 'var':
                            sys.exit(23)
                    
                    
                    #label - CALL, LABEL, JUMP
                    if tokenType in ['CALL', 'LABEL', 'JUMP']:
                        typeOfArg1 = 'label'
                        
                        for char in match.group(1):
                            if char in ['@', '/', '\\']:
                                sys.exit(23)
                    
                    #symb - PUSHS, WRITE, EXIT, DPRINT
                    else:
                        typeOfArg1 = varType(match.group(1))
                        if typeOfArg1 not in ['var', 'int', 'string', 'bool', 'nil']:
                            sys.exit(23)
                        if typeOfArg1 != 'var':
                            matchGroupToCrop = match.group(1).split('@', maxsplit=1)[1]
                    
                    #kontrola validni hodnoty v int   
                    if typeOfArg1 == 'int':
                        intCheck(matchGroupToCrop) 
                        
                    if typeOfArg1 == 'string':
                        matchGroupToCrop = stringCheck(matchGroupToCrop)     
                        
                    if typeOfArg1 == 'var':
                        varCheck(matchGroupToCrop)    
                    
                    tokens.append(Token(tokenType, matchGroupToCrop, typeOfArg1, None, None, None, None, lineNum))
                    
                else:
                    tokens.append(Token(tokenType, None, None, None, lineNum))
                
                flagMatch = True
                break
        if flagMatch == False:
            print("Unexpected input on line: {}".format(lineNum))
            sys.exit(23)

        lineNum += 1            
    
    if not IPPcodeFlag:
        sys.exit(21)
        
    return tokens



#Funkce generujici vysledny XML kod
def codeGen(tokens):
    
    header = '<?xml version="1.0" encoding="UTF-8"?>\n<program language="IPPcode24">'
    footer = '</program>'
    order = 1
    
    print(header)
    
    for token in tokens:
        print('\t<instruction order="{}" opcode="{}">'.format(order, token.tokenType))
        if token.arg1 == None:
            pass
        elif token.arg2 == None:
            print('\t\t<arg1 type="{}">{}</arg1>'.format(token.arg1Type, token.arg1))
        elif token.arg3 == None:
            print('\t\t<arg1 type="{}">{}</arg1>'.format(token.arg1Type, token.arg1))
            print('\t\t<arg2 type="{}">{}</arg2>'.format(token.arg2Type, token.arg2))
        else:
            print('\t\t<arg1 type="{}">{}</arg1>'.format(token.arg1Type, token.arg1))
            print('\t\t<arg2 type="{}">{}</arg2>'.format(token.arg2Type, token.arg2))
            print('\t\t<arg3 type="{}">{}</arg3>'.format(token.arg3Type, token.arg3))             
        
        print('\t</instruction>')
        order += 1
        
    print(footer)

def main():
    
    #Kontrola spravnosti argumentu
    args()
    #Analyza a ulozeni do tokenu pro nasledny tisk XML kodu
    tokens = makeTokens()
    codeGen(tokens)
    
    sys.exit(0)


if __name__ == "__main__":
    main()