/* ---------------------------------------------------------------------------------
 *          GLOBAL Classes and Structures
 *  
 * See VirtualVAMeter.cpp for file and class hierarchy
 * Copyright (c) George Kontopidis 2019, All Rights Reserved
 * ----------------------------------------------------------------------------------
 */
#include <gkeL1io.h>
#include "globals.h"

    CPU cpu;        // defined in cpuClass.h, exported by this
    CLI cli;        // defined in cliClass.h, exported by this
    EXE exe;        // defined in cpuClass.h, exported by this
    EEP eep;        // defined in eepClass.h, exported by this

    GLOBALS myp( &eep );    // defined in global.cpp/.h in this directory
