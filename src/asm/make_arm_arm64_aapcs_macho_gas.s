/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

// Stub file for universal ARM binary

#if defined(__arm__)
    #include "make_arm_aapcs_macho_gas.s"
#elif defined(__arm64__)
    #include "make_arm_aapcs_macho_gas.s"
#else
    #error "No arch's"
#endif
