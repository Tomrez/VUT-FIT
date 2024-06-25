/**
 * IFJ projekt 2023
 * Varianta TRP - Hash table
 * @author Marek Antoňů, xanton07
*/

#include "analysis.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int ret = 0;
    if((ret = initialize_scanner(stdin))){
        return ret;
    }
    
    return analyse();
}